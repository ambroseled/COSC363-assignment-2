


#ifndef H_cone
#define H_cone

#include <glm/glm.hpp>
#include "SceneObject.h"

/**
* Class that defines a cone object
*/
class Cone : public SceneObject
{
private:
    glm::vec3 center;
    float radius;
    float height;

public:
    Cone(void);

    Cone(glm::vec3 cen, float rad, float hgt, glm::vec3 col)
        : center(cen), radius(rad), height(hgt)
    {
        color = col;
    }

    float intersect(glm::vec3 posn, glm::vec3 dir);

	glm::vec3 normal(glm::vec3 pt);
};

#endif
