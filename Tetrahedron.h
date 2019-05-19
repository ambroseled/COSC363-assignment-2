


#ifndef H_TETRAHEDRON
#define H_TETRAHEDRON

#include <glm/glm.hpp>
#include "SceneObject.h"

class Tetrahedron : public SceneObject
{
private:
    glm::vec3 a, b, c;

public:
	Tetrahedron(void);

    Tetrahedron (glm::vec3 pa, glm::vec3 pb, glm::vec3 pc, glm::vec3 col)
		: a(pa), b(pb), c(pc)
	{
		color = col;
	};

	bool isInside(glm::vec3 pos);

	float intersect(glm::vec3 pos, glm::vec3 dir);

	glm::vec3 normal(glm::vec3 pos);

};
#endif
