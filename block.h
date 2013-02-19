// Block header file

#include <vector>
#include "triobject.h"
//#include "raytrace.h"

using namespace std;

//All types of blocks
int lengthOfTypes = 15;
enum Type {GRASS, DIRT, STONE, BEDROCK, COAL, IRON, GOLD, DIAMOND, WOOD, TORCH, SAND, LEAF, PORT1, PORT2, GORDO};

float shadowStrength = 0.2;
class Block
{
 public:
  Type type;
  vec3 position; // 3 ints
  int id; //which block is this.
  vector<triobject> tris;
  GLuint initDraw();
  GLuint initTop();
  GLuint initSides();
};

GLuint Block::initDraw() {

  GLuint index = glGenLists(1);
  glNewList(index, GL_COMPILE);
  	//Testbox
	glBegin(GL_QUADS);
	//f1
	glNormal3f(0, 0, 1);
	glTexCoord2d(1.0,0.0); glVertex3d(0,0,1);
	glTexCoord2d(1.0,1.0); glVertex3d(1,0,1);
	glTexCoord2d(0.0,1.0); glVertex3d(1,1,1);
	glTexCoord2d(0.0, 0.0); glVertex3d(0,1,1);
	//f2
	glNormal3f(0, 0, -1);
	glTexCoord2d(1.0,0.0); glVertex3d(0,0,0);
	glTexCoord2d(1.0,1.0); glVertex3d(1,0,0);
	glTexCoord2d(0.0,1.0); glVertex3d(1,1,0);
	glTexCoord2d(0.0,0.0); glVertex3d(0,1,0);
	//f3
	glNormal3f(0, 1, 0);
	glTexCoord2d(1.0,0.0); glVertex3d(0,0,1);
	glTexCoord2d(1.0,1.0); glVertex3d(0,0,0);
	glTexCoord2d(0.0,1.0); glVertex3d(1,0,0);
	glTexCoord2d(0.0,0.0); glVertex3d(1,0,1);	
	//f4
	glNormal3f(1, 0, 0);
	glTexCoord2d(1.0,0.0); glVertex3d(0,0,1);
	glTexCoord2d(1.0,1.0); glVertex3d(0,0,0);
	glTexCoord2d(0.0,1.0); glVertex3d(0,1,0);
	glTexCoord2d(0.0,0.0); glVertex3d(0,1,1);
	//f5
	glNormal3f(0, -1, 0);
	glTexCoord2d(1.0,0.0); glVertex3d(1,1,1);
	glTexCoord2d(1.0,1.0); glVertex3d(1,1,0);
	glTexCoord2d(0.0,1.0); glVertex3d(0,1,0);
	glTexCoord2d(0.0,0.0); glVertex3d(0,1,1);
	//f6
	glNormal3f(-1, 0, 0);
	glTexCoord2d(1.0,0.0); glVertex3d(1,1,1);
	glTexCoord2d(1.0,1.0); glVertex3d(1,1,0);
	glTexCoord2d(0.0,1.0); glVertex3d(1,0,0);
	glTexCoord2d(0.0,0.0); glVertex3d(1,0,1);
	glEnd();
	glEndList();
	return index;
}

GLuint Block::initTop() {
  GLuint index = glGenLists(1);
  glNewList(index, GL_COMPILE);
	glBegin(GL_QUADS);
	//f1
	glNormal3f(0, 0, 1);
	glTexCoord2d(1.0,0.0); glVertex3d(0,0,1);
	glTexCoord2d(1.0,1.0); glVertex3d(1,0,1);
	glTexCoord2d(0.0,1.0); glVertex3d(1,1,1);
	glTexCoord2d(0.0, 0.0); glVertex3d(0,1,1);
	glEnd();
	glEndList();
	return index;
}

GLuint Block::initSides() {
  GLuint index = glGenLists(1);
  glNewList(index, GL_COMPILE);
	glBegin(GL_QUADS);
	//f2
	glNormal3f(0, 0, -1);
	glTexCoord2d(1.0,0.0); glVertex3d(0,0,0);
	glTexCoord2d(1.0,1.0); glVertex3d(1,0,0);
	glTexCoord2d(0.0,1.0); glVertex3d(1,1,0);
	glTexCoord2d(0.0, 0.0); glVertex3d(0,1,0);
	//f3
	glNormal3f(0, 1, 0);
	glTexCoord2d(1.0,0.0); glVertex3d(0,0,1);
	glTexCoord2d(1.0,1.0); glVertex3d(0,0,0);
	glTexCoord2d(0.0,1.0); glVertex3d(1,0,0);
	glTexCoord2d(0.0,0.0); glVertex3d(1,0,1);	
	//f4
	glNormal3f(1, 0, 0);
	glTexCoord2d(1.0,0.0); glVertex3d(0,0,1);
	glTexCoord2d(1.0,1.0); glVertex3d(0,0,0);
	glTexCoord2d(0.0,1.0); glVertex3d(0,1,0);
	glTexCoord2d(0.0,0.0); glVertex3d(0,1,1);
	//f5
	glNormal3f(0, -1, 0);
	glTexCoord2d(1.0,0.0); glVertex3d(1,1,1);
	glTexCoord2d(1.0,1.0); glVertex3d(1,1,0);
	glTexCoord2d(0.0,1.0); glVertex3d(0,1,0);
	glTexCoord2d(0.0,0.0); glVertex3d(0,1,1);
	//f6
	glNormal3f(-1, 0, 0);
	glTexCoord2d(1.0,0.0); glVertex3d(1,0,1);
	glTexCoord2d(1.0,1.0); glVertex3d(1,0,0);
	glTexCoord2d(0.0,1.0); glVertex3d(1,1,0);
	glTexCoord2d(0.0,0.0); glVertex3d(1,1,1);
	glEnd();
	glEndList();
	return index;
}
