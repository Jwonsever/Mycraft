//Quaternion implementation.

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

typedef glm::mat3 mat3 ;
typedef glm::mat4 mat4 ; 
typedef glm::vec3 vec3 ; 
typedef glm::vec4 vec4 ; 

class Quaternion
{
public:
  float x, y, z, w;
  Quaternion();
  void FromAxis(const vec3 &v, float angle);
  mat4 getMatrix();
  Quaternion operator* (const Quaternion &rq);
  Quaternion getConjugate();
  vec3 operator* (const vec3 &vec);
};
