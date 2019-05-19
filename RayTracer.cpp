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
using namespace std;

const float WIDTH = 20.0;
const float HEIGHT = 20.0;
const float EDIST = 40.0;
const int NUMDIV = 500;
const int MAX_STEPS = 5;
const float XMIN = -WIDTH * 0.5;
const float XMAX =  WIDTH * 0.5;
const float YMIN = -HEIGHT * 0.5;
const float YMAX =  HEIGHT * 0.5;
const float ETA = 1.01;
vector<SceneObject*> sceneObjects;  //A global list containing pointers to objects in the scene


//---The most important function in a ray tracer! ----------------------------------
//   Computes the colour value obtained by tracing a ray and finding its
//     closest point of intersection with objects in the scene.
//----------------------------------------------------------------------------------
glm::vec3 trace(Ray ray, int step)
{
	glm::vec3 backgroundCol(0);
	glm::vec3 light(10, 40, -3);
	glm::vec3 ambientCol(0.2);   //Ambient color of light
    glm::vec3 colorSum(0.0);
	float transVal = 0.2;

    ray.closestPt(sceneObjects);		//Compute the closest point of intersetion of objects with the ray

    if(ray.xindex == -1) return backgroundCol;      //If there is no intersection return background colour

    glm::vec3 materialCol = sceneObjects[ray.xindex]->getColor(); //else return object's colour
    glm::vec3 normalVector = sceneObjects[ray.xindex]->normal(ray.xpt);
    glm::vec3 lightVector = light - ray.xpt;
    glm::vec3 lightUnit = glm::normalize(lightVector);
    glm::vec3 reflVector = glm::reflect(-lightUnit, normalVector);


    Ray shadow(ray.xpt, lightUnit);
    shadow.closestPt(sceneObjects);

    float rV = glm::dot(reflVector, -ray.dir);

    float spec = 0.0;

    if (rV > 0) {
        spec = pow(rV, 10);
    }
	if (ray.xindex == 6) {
		spec = 0;
	}

    ambientCol += spec;

    float lDotn = glm::dot(lightUnit, normalVector);

    if (lDotn <= 0 ||
            (shadow.xindex > -1 && shadow.xdist < glm::length(lightVector))) {
        materialCol = ambientCol*materialCol;
    } else {
        materialCol = ambientCol*materialCol + (lDotn*materialCol + spec);
    }


    if(ray.xindex == 0 && step < MAX_STEPS)
     {
        glm::vec3 reflectedDir = glm::reflect(ray.dir, normalVector);
        Ray reflectedRay(ray.xpt, reflectedDir);
        glm::vec3 reflectedCol = trace(reflectedRay, step+1); //Recursion!
        colorSum = colorSum + (0.8f*reflectedCol);
     }

	 // Transparency of cone through refraction
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

	  // Patterned floor - chequers
	 if(ray.xindex == 3) {
		 int modx = (int)((ray.xpt.x + 50) / 8) % 2;
		 int modz = (int)((ray.xpt.z + 200) / 8) % 2;

		 if((modx && modz) || (!modx && !modz)) {
			 materialCol = glm::vec3(0, 0.2, 0);
		 } else {
			 materialCol = glm::vec3(1, 0, 1);
		 }
	 }

	 // Procedural pattern on sphere
	 if(ray.xindex == 7) {
		 int val = ((int) (ray.xpt.x + ray.xpt.y - 6)) % 3;
		 if (val == 0) {
			 materialCol = glm::vec3(1, 0.4, 0.27) + spec;
		 } else if (val == 1) {
			 materialCol = glm::vec3(0.8, 1, 0.6) + spec;
		 } else {
			 materialCol = glm::vec3(0.55, 0.15, 1) + spec;
		 }
	 }

    return materialCol + colorSum*0.8f;
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

//---The main display module -----------------------------------------------------------
// In a ray tracing application, it just displays the ray traced image by drawing
// each cell as a quad.
//---------------------------------------------------------------------------------------
void display()
{
	float xp, yp;  //grid point
	float cellX = (XMAX-XMIN)/NUMDIV;  //cell width
	float cellY = (YMAX-YMIN)/NUMDIV;  //cell height

	glm::vec3 eye(0., 0., 0.);  //The eye position (source of primary rays) is the origin

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
		    glm::vec3 col = trace (ray, 1); //Trace the primary ray and get the colour value

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

	//-- Create a pointer to a sphere object
    Sphere *sphere1 = new Sphere(glm::vec3(-5.0, -5.0, -110.0), 10.0, glm::vec3(0, 0, 1));
    Sphere *sphere2 = new Sphere(glm::vec3(3, 5, -90.0), 2.0, glm::vec3(1, 0, 1));
    Sphere *sphere3 = new Sphere(glm::vec3(-22, -17, -120), 2.5, glm::vec3(0, 1, 1));
	Sphere *sun = new Sphere(glm::vec3(27, 25, -130.0), 4, glm::vec3(0, 1, 1));

    Plane *floorPlane = new Plane( glm::vec3(-50., -20, -40),
                              glm::vec3(50., -20, -40),
                              glm::vec3(50., -20, -200),
                              glm::vec3(-50., -20, -200),
                              glm::vec3(0.5, 0.5, 0));

	Plane *wallPlane = new Plane( glm::vec3(-50., -20, -200),
	                        glm::vec3(50., -20, -200),
	                        glm::vec3(50., 50, -200),
	                        glm::vec3(-50., 500, -200),
	                        glm::vec3(0.5, 0.5, 0));


    Cone *coneTrans = new Cone(glm::vec3(-15, -15, -75), 4, 10, glm::vec3(1, 0, 1));
	Cone *coneNormal = new Cone(glm::vec3(12.5, -10, -92.5), 2, 6, glm::vec3(1, 0, 1));

	//--Add the above to the list of scene objects.
    sceneObjects.push_back(sphere1); // 0
    sceneObjects.push_back(sphere2); // 1
    sceneObjects.push_back(sphere3); // 2
    sceneObjects.push_back(floorPlane); // 3
	sceneObjects.push_back(wallPlane); // 4
	sceneObjects.push_back(coneTrans); // 5
	sceneObjects.push_back(coneNormal); // 6
	sceneObjects.push_back(sun); // 7


	drawBox(5, 5, 5, 10, -15, -90, glm::vec3(0.4648, 0, 0.996));

}



int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB );
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(20, 20);
    glutCreateWindow("Raytracer");

    glutDisplayFunc(display);
    initialize();

    glutMainLoop();
    return 0;
}
