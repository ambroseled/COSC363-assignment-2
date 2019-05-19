


#include "Cone.h"
#include <math.h>


float Cone::intersect(glm::vec3 pos, glm::vec3 dir)
{
    glm::vec3 d = pos - center;
    float yd = height - pos.y + center.y;
    float rh = radius / height;
    float tan = powf(rh, 2);

    float a = powf(dir.x, 2) + powf(dir.z, 2) - (tan*powf(dir.y, 2));
    float b = 2*(d.x * dir.x + d.z * dir.z + tan * yd * dir.y);
    float c = powf(d.x, 2) + powf(d.z, 2) - (tan * powf(yd, 2));
    float delta = powf(b, 2) - 4*(a * c);

	if(fabs(delta) < 0.001 || delta < 0.0){
		return -1.0;
	}

    float t1 = (-b - sqrt(delta))/(2*a);
    float t2 = (-b + sqrt(delta))/(2*a);

    if(t1<0.01) t1=-1;
	if(t2<0.01) t2=-1;

	if (t1<t2){
		float temp = t2;
        t2 = t1;
        t1 = temp;
    }

    float ypos = pos.y + dir.y * t2;
    if ((ypos >= center.y) && (ypos <= center.y + height)){
		return t2;
	} else {
       float ypos = pos.y + dir.y * t1;
       if ((ypos >= center.y) && (ypos <= center.y + height)){
			return t1;
       } else {
			return -1.0;
       }
    }
}



glm::vec3 Cone::normal(glm::vec3 pnt) {
    glm::vec3 d = pnt - center;
    float r = sqrt(pow(d.x, 2) + pow(d.z, 2));
    glm::vec3 normal = glm::vec3(d.x, r * (radius / height), d.z);
    normal = glm::normalize(normal);
    return normal;
}
