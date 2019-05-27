/*----------------------------------------------------------
* COSC363  Ray Tracer Assignment
*
*  Tetrahedron class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#include "Tetrahedron.h"
#include <math.h>

/**
* Check if a point is inside the triangle plane
*/
bool Tetrahedron::isInside(glm::vec3 point) {
    glm::vec3 norm = normal(point);

    glm::vec3 uA = b - a;
    glm::vec3 uB = c - b;
    glm::vec3 uC = a - c;

    glm::vec3 vA = point - a;
    glm::vec3 vB = point - b;
    glm::vec3 vC = point = c;

    return glm::dot(glm::cross(uA, vA), norm) > 0 &&
        glm::dot(glm::cross(uB, vB), norm) > 0 &&
        glm::dot(glm::cross(uC, vC), norm) > 0;
}

/**
* Intersection check for a point with the traingle plane
*/
float Tetrahedron::intersect(glm::vec3 pos, glm::vec3 dir) {
    glm::vec3 norm = normal(pos);
    glm::vec3 vDif = a - pos;

    float vDotN = glm::dot(dir, norm);
    if (fabs(vDotN) < 0.001) return -1;

    float t = glm::dot(vDif, norm) / vDotN;
    if (fabs(t) < 0.001) return -1;

    glm::vec3 q = pos + dir * t;
    if (isInside(q)) return t;
    else return -1;

}

/**
* Normal calculation for the plane
*/
glm::vec3 Tetrahedron::normal(glm::vec3 pos)
{
    glm::vec3 n = glm::cross(b - a, c - a);
    n = glm::normalize(n);
    return n;
}
