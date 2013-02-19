//Basic Struct for triangles.
//All objects (Blocks) are made of these.
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct triobject {

  vec3 vertexA;//used by tris
  vec3 vertexB;
  vec3 vertexC;
  vec3 facenorm;
  int type;

};
