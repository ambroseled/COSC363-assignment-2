/*========================================================================
* COSC 363  Computer Graphics (2018)
* Ray tracer
* See Lab07.pdf for details.
*=========================================================================
*/
#include <iostream>
#include <cmath>
#include <vector>
#include <glm/glm.hpp>
#include "Sphere.h"
#include "SceneObject.h"
#include "Ray.h"
#include <GL/glut.h>
#include "Plane.h"
#include "Cone.h"
#include "TextureBMP.h"
#include "Cylinder.h"
#include "Tetrahedron.h"
using namespace std;

const float WIDTH = 20.0;
const float HEIGHT = 20.0;
float EDIST = 40.0;
const int NUMDIV = 500;
const int MAX_STEPS = 5;
const float XMIN = -WIDTH * 0.5;
const float XMAX =  WIDTH * 0.5;
const float YMIN = -HEIGHT * 0.5;
const float YMAX =  HEIGHT * 0.5;
const float ETA = 1.05;
float eye_x = 0.;
float eye_y = 0.;
float eye_z = 0.;
vector<SceneObject*> sceneObjects;  //A global list containing pointers to objects in the scene
TextureBMP textureSphere;
TextureBMP textureStars;


//---The most important function in a ray tracer! ----------------------------------
//   Computes the colour value obtained by tracing a ray and finding its
//     closest point of intersection with objects in the scene.
//----------------------------------------------------------------------------------
glm::vec3 trace(Ray ray, int step)
{
	glm::vec3 backgroundCol(0);
	glm::vec3 light(10, 40, -3);
	glm::vec3 light2(30, 40, 0);
	glm::vec3 ambientCol(0.2);   //Ambient color of light
    glm::vec3 colorSum(0.0);
	float transVal = 0.2;

    ray.closestPt(sceneObjects);		//Compute the closest point of intersetion of objects with the ray

    if(ray.xindex == -1) return backgroundCol;      //If there is no intersection return background colour

    glm::vec3 materialCol = sceneObjects[ray.xindex]->getColor(); //else return object's colour
    glm::vec3 normalVector = sceneObjects[ray.xindex]->normal(ray.xpt);
    glm::vec3 lightVector = light - ray.xpt;
	glm::vec3 lightVector2 = light2 - ray.xpt;
    glm::vec3 lightUnit = glm::normalize(lightVector);
	glm::vec3 lightUnit2 = glm::normalize(lightVector2);

    glm::vec3 reflVector = glm::reflect(-lightUnit, normalVector);
	glm::vec3 reflVector2 = glm::reflect(-lightUnit2, normalVector);

	float lDotn = glm::dot(lightUnit, normalVector);
	float lDotn2 = glm::dot(lightUnit2, normalVector);

	float rV = glm::dot(reflVector, -ray.dir);
	float rV2 = glm::dot(reflVector2, -ray.dir);

	//--------------------------
	// Patterned floor - chequers
	if(ray.xindex == 3) {
	   int modx = (int)((ray.xpt.x + 50) / 8) % 2;
	   int modz = (int)((ray.xpt.z + 200) / 8) % 2;

	   if((modx && modz) || (!modx && !modz)) {
		   materialCol = glm::vec3(0.25, 0.25, 0.25);
	   } else {
		   materialCol = glm::vec3(0, 0.632, 1);
	   }
	}
	//--------------------------

	//--------------------------
	// Procedural pattern on sphere
	if(ray.xindex == 7) {
	   int val = ((int) (ray.xpt.x + ray.xpt.y)) % 3;
	   if (val == 0) {
		   materialCol = glm::vec3(1, 0.4, 0.35);
	   } else if (val == 1) {
		   materialCol = glm::vec3(0.8, 1, 0.6);
	   } else {
		   materialCol = glm::vec3(0.55, 0.15, 1);
	   }
	}
	//--------------------------

	//--------------------------
	// Texturing sphere with planet texture
	if (ray.xindex == 1 && step < MAX_STEPS) {
	   glm::vec3 center(3, 3, -90.0);
	   glm::vec3 dirTex = glm::normalize(ray.xpt - center);
	   float s = (0.5 - atan2(dirTex.z, dirTex.x) + M_PI) / (2 * M_PI);
	   float t = 0.5 + asin(dirTex.y) / M_PI;
	   materialCol = textureSphere.getColorAt(s, t);
	}
	//--------------------------

	//--------------------------
	// Shadow ray for light source 1
    Ray shadow(ray.xpt, lightUnit);
    shadow.closestPt(sceneObjects);

    float spec = 0.0;
    if (rV > 0) {
        spec = pow(rV, 10);
    }
	if (ray.xindex == 6) {
		spec = 0;
	}
    ambientCol += spec;

    if (lDotn <= 0 ||
            (shadow.xindex > -1 && shadow.xdist < glm::length(lightVector)) || ray.xindex == 4) {
        materialCol = ambientCol*materialCol;
    } else {
        materialCol = ambientCol*materialCol + (lDotn*materialCol + spec);
    }
	//--------------------------

	//--------------------------
	// Shadow ray for light source 2
    Ray shadow2(ray.xpt, lightUnit2);
    shadow2.closestPt(sceneObjects);

    float spec2 = 0.0;
    if (rV2 > 0) {
        spec2 = pow(rV2, 10);
    }
	if (ray.xindex == 6) {
		spec2 = 0;
	}
    ambientCol += spec2;

    if (lDotn2 <= 0 ||
            (shadow2.xindex > -1 && shadow2.xdist < glm::length(lightVector2))) {
        materialCol = ambientCol*materialCol;
    } else {
        materialCol = ambientCol*materialCol + (lDotn2*materialCol + spec2);
    }
	//--------------------------

	//--------------------------
	// Reflection off of a sphere
	if((ray.xindex == 0) && step < MAX_STEPS)
	{
		glm::vec3 reflectedDir = glm::reflect(ray.dir, normalVector);
		Ray reflectedRay(ray.xpt, reflectedDir);
		glm::vec3 reflectedCol = trace(reflectedRay, step+1); //Recursion!
		colorSum = (colorSum*0.2f) + (0.8f*reflectedCol);
	}
	//--------------------------

	// Refraction through cone
	//--------------------------
	if(ray.xindex == 5 && step < MAX_STEPS)
	{
		glm::vec3 g = glm::refract(ray.dir, normalVector, ETA);
		Ray refractedRay(ray.xpt, g);
		refractedRay.closestPt(sceneObjects);
		if (refractedRay.xindex == -1) return backgroundCol;

		glm::vec3 m = sceneObjects[refractedRay.xindex] -> normal(refractedRay.xpt);
		glm::vec3 h = glm::refract(g, -m, 1.0f/ETA);
		Ray refractedRay2(ray.xpt, g);
		refractedRay2.closestPt(sceneObjects);
		if (refractedRay2.xindex == -1) return backgroundCol;

		glm::vec3 refColor = trace(refractedRay2, step+1);
		colorSum *= transVal;
		colorSum += refColor * (1 - transVal);

		return colorSum;
	}
	//--------------------------

	// Refraction through cone
	//--------------------------
	if(ray.xindex == 6 && step < MAX_STEPS)
	{

	}
	//--------------------------


    return materialCol + colorSum*0.8f;
}


glm::vec3 antiAlias(float x, float y, glm::vec3 eye, float pixelSize) {
	float quarterLower = pixelSize * 0.25;
	float quarterUpper = pixelSize * 0.75;
	glm::vec2 quarters[4] {
		glm::vec2(quarterLower, quarterLower),
		glm::vec2(quarterLower, quarterUpper),
		glm::vec2(quarterUpper, quarterLower),
		glm::vec2(quarterUpper, quarterUpper),
	};

	glm::vec3 colorSum = glm::vec3(0, 0, 0);

	for (int i = 0; i < 4; i++) {
		Ray ray = Ray(eye, glm::vec3(x + quarters[0].x, y + quarters[0].y, -EDIST));
		ray.normalize();
		colorSum += trace(ray, 1);
	}

	colorSum *= glm::vec3(0.25);

	return colorSum;
}

void drawBox(float length, float width, float height, float x, float y, float z, glm::vec3 col) {
	glm::vec3 a = glm::vec3(x, y, z);
	glm::vec3 b = glm::vec3(x + length, y, z);
	glm::vec3 c = glm::vec3(x + length, y + height, z);
	glm::vec3 d = glm::vec3(x, y + height, z);
	glm::vec3 e = glm::vec3(x + length, y, z - width);
	glm::vec3 f = glm::vec3(x + length, y + height, z - width);
	glm::vec3 g = glm::vec3(x, y + height, z - width);
	glm::vec3 h = glm::vec3(x, y, z - width);

	Plane *front = new Plane(a, b, c, d, col);
	Plane *right = new Plane(b, e, f, c, col);
	Plane *back = new Plane(e, h, g, f, col);
	Plane *sideFour = new Plane(d, g, h, a, col);
	Plane *bottom = new Plane(d, c, f, g, col);
	Plane *top = new Plane(h, e, b, a, col);


	sceneObjects.push_back(front);
	sceneObjects.push_back(right);
	sceneObjects.push_back(back);
	sceneObjects.push_back(sideFour);
	sceneObjects.push_back(bottom);
	sceneObjects.push_back(top);
}

void drawTetrahedron(float len, float x, float y, float z, glm::vec3 col) {


	glm::vec3 a = glm::vec3(x, y, z);
	glm::vec3 b = glm::vec3(x, y, z + len);
	glm::vec3 c = glm::vec3(x + len, y, z + len/2);
	glm::vec3 d = glm::vec3(x + len/2, y + len, z + len/2);

	Tetrahedron *left = new Tetrahedron(a, d, c, col);
	Tetrahedron *right = new Tetrahedron(c, d, b, col);
	Tetrahedron *back = new Tetrahedron(b, d, a, col);
	Tetrahedron *base = new Tetrahedron(a, b, c, col);

	sceneObjects.push_back(left);
	sceneObjects.push_back(right);
	sceneObjects.push_back(back);
	sceneObjects.push_back(base);

}

//---The main display module -----------------------------------------------------------
// In a ray tracing application, it just displays the ray traced image by drawing
// each cell as a quad.
//---------------------------------------------------------------------------------------
void display()
{
	float xp, yp;  //grid point
	float cellX = (XMAX-XMIN)/NUMDIV;  //cell width
	float cellY = (YMAX-YMIN)/NUMDIV;  //cell height

	glm::vec3 eye(eye_x, eye_y, eye_z);  //The eye position (source of primary rays) is the origin

	glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	glBegin(GL_QUADS);  //Each cell is a quad.

	for(int i = 0; i < NUMDIV; i++)  	//For each grid point xp, yp
	{
		xp = XMIN + i*cellX;
		for(int j = 0; j < NUMDIV; j++)
		{
			yp = YMIN + j*cellY;

		    glm::vec3 dir(xp+0.5*cellX, yp+0.5*cellY, -EDIST);	//direction of the primary ray

		    Ray ray = Ray(eye, dir);		//Create a ray originating from the camera in the direction 'dir'
			ray.normalize();				//Normalize the direction of the ray to a unit vector
		   // glm::vec3 col = antiAlias(xp, yp, eye, cellX); //Trace the primary ray and get the colour value
		    glm::vec3 col = trace(ray, 1);

			glColor3f(col.r, col.g, col.b);
			glVertex2f(xp, yp);				//Draw each cell with its color value
			glVertex2f(xp+cellX, yp);
			glVertex2f(xp+cellX, yp+cellY);
			glVertex2f(xp, yp+cellY);
        }
    }

    glEnd();
    glFlush();
}


void special(int key, int x, int y)
{
	//--------------------------
	// Camera motion
    if (key == GLUT_KEY_UP) EDIST += 5;
    else if (key == GLUT_KEY_DOWN) EDIST -= 5;
	else if (key == GLUT_KEY_LEFT) eye_x -= 5;
    else if (key == GLUT_KEY_RIGHT) eye_x += 5;

	// Bounding checks for camera motion
	if (EDIST <= -200) EDIST == -200;
	if (EDIST >= 0) EDIST == 0;
	//--------------------------

    glutPostRedisplay();
}


//---This function initializes the scene -------------------------------------------
//   Specifically, it creates scene objects (spheres, planes, cones, cylinders etc)
//     and add them to the list of scene objects.
//   It also initializes the OpenGL orthographc projection matrix for drawing the
//     the ray traced image.
//----------------------------------------------------------------------------------
void initialize()
{
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(XMIN, XMAX, YMIN, YMAX);
    glClearColor(0, 0, 0, 1);

	textureSphere = TextureBMP("Gaseous4.bmp");
	textureStars = TextureBMP("stars.bmp");

	//-- Create a pointer to a sphere object
    Sphere *sphere1 = new Sphere(glm::vec3(-5.0, -5.0, -110.0), 10.0, glm::vec3(0, 0, 1));
    Sphere *sphere2 = new Sphere(glm::vec3(3, 5, -90.0), 3, glm::vec3(1, 0, 1));
    Sphere *sphere3 = new Sphere(glm::vec3(-22, -17, -120), 2.5, glm::vec3(0, 1, 1));
	Sphere *sun = new Sphere(glm::vec3(27, 25, -130.0), 4, glm::vec3(0, 1, 1));

    Plane *floorPlane = new Plane( glm::vec3(-50., -20, -40),
                              glm::vec3(50., -20, -40),
                              glm::vec3(50., -20, -200),
                              glm::vec3(-50., -20, -200),
                              glm::vec3(0.5, 0.5, 0));

	Plane *wallPlane = new Plane( glm::vec3(-50., -25, -200),
	                        glm::vec3(50., -25, -200),
	                        glm::vec3(50., 50, -200),
	                        glm::vec3(-50., 500, -200),
	                        glm::vec3(0.4, 0.2, 0));


    Cone *coneTrans = new Cone(glm::vec3(-15, -20, -85), 4, 10, glm::vec3(1, 0, 1));
	Cone *coneNormal = new Cone(glm::vec3(12.5, -10, -92.5), 2, 6, glm::vec3(1, 0, 1));

	Cylinder *cylinder = new Cylinder(glm::vec3(25, -5, -130), 5, 10, glm::vec3(0, 1, 0));


	//--Add the above to the list of scene objects.
    sceneObjects.push_back(sphere1); // 0
    sceneObjects.push_back(sphere2); // 1
    sceneObjects.push_back(sphere3); // 2
    sceneObjects.push_back(floorPlane); // 3
	sceneObjects.push_back(wallPlane); // 4
	sceneObjects.push_back(coneTrans); // 5
	sceneObjects.push_back(coneNormal); // 6
	sceneObjects.push_back(sun); // 7
	sceneObjects.push_back(cylinder); // 8


	drawBox(5, 5, 5, 10, -15, -90, glm::vec3(0.4648, 0, 0.996));
	drawTetrahedron(0.5, 25, 0, -180, glm::vec3(0, 1, 1));

}



int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB );
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(20, 20);
    glutCreateWindow("Raytracer");

	glutSpecialFunc(special);
    glutDisplayFunc(display);
    initialize();

    glutMainLoop();
    return 0;
}
