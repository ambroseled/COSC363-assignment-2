/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The object class
*  This is a generic type for storing objects in the scene
*  Sphere, Plane etc. must be defined as subclasses of Object.
*  Being an abstract class, this class cannot be instantiated.
-------------------------------------------------------------*/

#include "SceneObject.h"
#include <math.h>

glm::vec3 SceneObject::getColor()
{
	return color;
}

void SceneObject::setColor(glm::vec3 col)
{
	color = col;
}

glm::vec3 SceneObject::rotateZ(glm::vec3 pos, float angle)
{
	glm::mat4 rotationMat =
	{
		{cos(angle), -sin(angle), 0, 0},
		{sin(angle), cos(angle), 0, 0},
		{0, 0, 1, 0},
		{0, 0, 0, 1}
	};
	glm::vec4 rotation = glm::vec4(pos, 0.0) * rotationMat;

	return glm::vec3(rotation);
}
