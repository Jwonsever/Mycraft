/*****************************************************************************/
// This file is raytrace.h.
/*****************************************************************************/

#define BPP 24
#include <math.h>
#include <time.h>
#include <assert.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

//Temp def. (c flaws)
class Block;

float xGap, yGap, zGap;
float xMAX, yMAX, zMAX; 
float xMIN, yMIN, zMIN;

//Math
float magnitude (vec3 a) {
  return sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
}

mat4 translate(const float &tx, const float &ty, const float &tz) {
  mat4 M = mat4(1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		tx, ty, tz, 1);
  return M;
}
mat4 translate(vec3 pos) {
  mat4 M = mat4(1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
	       pos.x, pos.y, pos.z, 1);
  return M;
}

vec3 normalize (vec3 a) {
  a = a / magnitude(a);
  return a;
}
vec3 aToVec3(float a[3]) {
  return vec3(a[0],a[1],a[2]);
}

GLfloat max(GLfloat a, GLfloat b) {
  return (((a) > (b)) ? (a) : (b));
}
GLfloat min(GLfloat a, GLfloat b) {
  return (((a) < (b)) ? (a) : (b));
}

//temp fix to avoid redefinition in readfile
vec3 normalizeCross2 (vec3 a, vec3 b) {
  vec3 product = vec3(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x);
  return glm::normalize(product);
}

float dot (vec3 a, vec3 b) {
  float product = a.x*b.x + a.y*b.y + a.z*b.z;
  return product;
}

/*uniform grid intersection
void gridIntersections(vec3 posit, vec3 direction, float zFar) {

  //wipe old values
  for (int r = 0; r < numGrids; r++)
    for (int s = 0; s < numGrids; s++)
      for (int t = 0; t < numGrids; t++)
	hitGrids[r][s][t] = false;

  //compute t
  for (int r = 0; r < numGrids; r++) {
    float boxMin = xMIN + r * xGap; float boxMax = xMIN + (r + 1) * xGap;
    if (boxMin > boxMax) {
      float tmp = boxMax;
      boxMax = boxMin;
      boxMin = tmp;
    }
    if (abs(direction.x) < 0.0000001)
      if (posit.x > boxMax and posit.x < boxMin)
	continue;
    float inv = 1 / (float) direction.x;
    float tMin = inv * (boxMax - posit.x);
    float tMax = inv * (boxMin - posit.x);
    for (int s = 0; s < numGrids; s++) {
      boxMin = yMIN + s * yGap; boxMax = yMIN + (s + 1) * yGap;
      float hitMin = posit.y + direction.y * tMin; float hitMax = posit.y + direction.y * tMax;
      if (hitMin > hitMax) {
	float tmp = hitMax;
	hitMax = hitMin;
	hitMin = tmp;
      }
      //check y*t in box
      if (hitMax >= boxMin && hitMin <= boxMax)
	for (int t = 0; t < numGrids; t++) {
	  boxMin = zMIN + t * zGap; boxMax = zMIN + (t + 1) * zGap;
	  hitMin = posit.z + direction.z * tMin; hitMax = posit.z + direction.z * tMax;
	  if (hitMin > hitMax) {
	    float tmp = hitMax;
	    hitMax = hitMin;
	    hitMin = tmp;
	  } 
	  //check z*t in box
	  if (hitMax >= boxMin && hitMin <= boxMax)
	    hitGrids[r][s][t] = true;
	}
    }
  }
  // for (int r = 0; r < numGrids; r++) 
  //   for (int s = 0; s < numGrids; s++) 
  //     for (int t = 0; t < numGrids; t++) 
  // 	if(hitGrids[r][s][t])
  // 	  std::cout << r << "," << s << "," << t << "\n"; 
}
  */

//Gets rid of warning
Block hitblock;
//Run the ray to see if it hits a block
Block * runRay(vec3 root, vec3 direction, float maxDist, bool shadowing) {

  //Hit location and normal for shading
  vec3 hitPos;
  vec3 hitNorm;

  //Predefinitions
  vec3 position, normal;
  float t, dist = 0;

  float zHit = maxDist;
  bool hit = false;

  //Does this help???
  
  for (int k = 0; k < numBlocksUsed; k++) {
    
    Block nextObj = allBlocks[k];
    vec3 tmppos = nextObj.position - eye;
    if (tmppos.x * tmppos.x + tmppos.y * tmppos.y > selectDistance * selectDistance)
      continue;

    //Ray Triangle Intersections?
    mat4 M = translate(nextObj.position);

    for (int i = 0; i < 12; i++) {
      triobject nextTri = blockTriFrame[i];
      vec4 a4 = M * vec4(nextTri.vertexA, 1);
      vec3 a = vec3(a4.x, a4.y, a4.z) / (float) a4.w;
      vec4 b4 = M * vec4(nextTri.vertexB, 1);
      vec3 b = vec3(b4.x, b4.y, b4.z) / (float) b4.w;
      vec4 c4 = M * vec4(nextTri.vertexC, 1);
      vec3 c = vec3(c4.x, c4.y, c4.z) / (float) c4.w;

      normal = nextTri.facenorm * mat3(glm::transpose(glm::inverse(M)));
      t = (dot(a, normal) - dot(root, normal)) / dot(direction, normal);
      dist = t;

      //Make sure all the distances are correct for norms
      if (t < zHit && t > zNear) {
	
	//would be nearest
	position = root + direction * t;
	vec3 v0 = b - a, v1 = c - a, v2 = position - a;
	
	float dot00 = dot(v0, v0);
	float dot01 = dot(v0, v1);
	float dot11 = dot(v1, v1);
	float dot20 = dot(v2, v0);
	float dot21 = dot(v2, v1);
	float denom = dot00 * dot11 - dot01 * dot01;
	float v = (dot11 * dot20 - dot01 * dot21) / denom;
	float w = (dot00 * dot21 - dot01 * dot20) / denom;
	
	if (-0.0001 < v && -0.0001 < w && (v + w) < 1.0) {
	  //I hit
	  hit = true;
	  if (shadowing)
	    break;
	  
	  //Set the hit distance
	  zHit = t;
	  
	  //Set the postion && normal
	  hitPos = position;
	  hitNorm = glm::normalize(normal);
	  selectedNormal = hitNorm;
	  hitblock = nextObj;
	}
      }
    }
  }
  if (hit) {
    return &hitblock;
  } else {
    return (Block *) NULL;
  }
}

//General Raytrace, for selections
Block * raytrace() {
  vec3 wHat, uHat, vHat; //Coordinate system
  vec3 down = eye - center;
  wHat = down / sqrt(down.x*down.x + down.y*down.y + down.z*down.z);
  uHat = normalizeCross2(up, wHat);
  vHat = normalizeCross2(wHat, uHat);

  float alpha = tan(fovy/360.0 * 3.141592) * 0;
  float beta = tan(fovy/360.0 * 3.141592) * 0;

  //Initialize ray direction
  vec3 direction = alpha * uHat +  beta * vHat - wHat;
  direction = glm::normalize(direction);
  
  Block * hit = runRay(eye, direction, selectDistance, false);

}

//Raytrace for shadow mapping
bool shadowTrace(vec3 lightpos, vec3 vertexpos) {
  if (runRay(vertexpos, glm::normalize(lightpos - vertexpos), magnitude(vertexpos - lightpos), true) == NULL)
    return false;
  else
    return true;
}
