/*----------------------------------------------------------
* COSC363  Ray Tracer Assignment
*
*  Cylinder class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#ifndef H_CYLINDER
#define H_CYLINDER

#include <glm/glm.hpp>
#include "SceneObject.h"

/**
 * Class that defines cylinder object
 */
class Cylinder : public SceneObject
{

private:
    glm::vec3 center;
    float radius;
    float height;

public:
	Cylinder(void);

    Cylinder(glm::vec3 cen, float rad, float hgt, glm::vec3 col)
		: center(cen), radius(rad), height(hgt)
	{
		color = col;
	};

	float intersect(glm::vec3 pos, glm::vec3 dir);

	glm::vec3 normal(glm::vec3 p);

};

#endif //!H_CYLINDER
