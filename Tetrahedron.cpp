

#include "Tetrahedron.h"
#include <math.h>

bool Tetrahedron::isInside(glm::vec3 point) {
    glm::vec3 normal = normal(point);

    glm::vec3 uA = b - a;
    glm::vec3 uB = c - b;
    glm::vec3 uC = a - c;

    glm::vec3 vA = point - a;
    glm::vec3 vB = point - b;
    glm::vec3 vC = point = c;

    return glm::dot(glm::cross(uA, vA), normal) > 0 &&
        glm::dot(glm::cross(uB, vB), normal) > 0 &&
        glm::dot(glm::cross(uC, vC), normal) > 0;
}

float Tetrahedron::intersect(glm::vec3 pos, glm::vec3 dir) {
    glm::vec3 normal = normal(pos);
    glm::vec3 vDif = a - pos;

    float vDotN = glm::dot(dir, normal);
    if (fabs(vDotN) < 0.001) return -1;

    float t = glm::dot(vDif, normal) / vDotN;
    if (fabs(t) < 0.001) return -1;

    glm::vec3 q = pos + dir * t;
    if (isInside(q)) return t;
    else return -1;

}


glm::vec3 Tetrahedron::normal(glm::vec3 pos)
{
    glm::vec3 n = glm::cross(b - a, c - a);
    n = glm::normalize(n);
    return n;
}
