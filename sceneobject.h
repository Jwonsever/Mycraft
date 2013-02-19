/*Class describing a generalized object in the scene */

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <fstream>
#include <string>
#include <sstream>
#include <vector>

#define NumberOf(array) (sizeof(array)/sizeof(array[0])) 

class sceneObject {
 private:
  GLfloat * verticies;
  int sv, sc, sf;
  GLubyte * faces;
  GLfloat * colors;
  GLfloat * textures;
  bool textured;
  GLenum PrimType;
  const char* name;
 public:
  //Name, Verticies, Faces, Colors, Textures
  sceneObject();
  sceneObject(const char *, GLfloat*, int,  GLubyte*, int, GLfloat*, int, GLfloat*, int);
  const char *  getName() {return name;};
  GLfloat * getVerts() {return verticies;};
  int sizeVerts() {return sv;};
  GLubyte * getInds() {return faces;};
  int sizeInds() {return sf;};
  GLfloat * getColors() {return colors;};
  int sizeCols() {return sc;};
  bool isTextured() {return textured;};
  GLfloat * getTexture() {return textures;};
  GLenum primitive() {return PrimType;};

};

//null constructor
sceneObject::sceneObject() {
  verticies = NULL;
  faces = NULL;
  colors = NULL;
  textures = NULL;
  textured = false;
  sc = 0;
  sf = 0;
  sv = 0;
  PrimType = GL_POLYGON;
  name = "NULL";
}

sceneObject::sceneObject(const char * nm, GLfloat * ver, int sv1, GLubyte * fac, int sf1, GLfloat * col, int sc1, GLfloat * tex, int pr) {
  name = nm;
  sv = sv1;
  sc = sc1;
  sf = sf1;
  verticies = ver;
  faces = fac;
  colors = col;
  textures = tex;
  PrimType = pr;
  if (tex != NULL) {textured = true;} else {textured = false;}
}

//Make a few sample sceneobjects

//Make a cube
GLfloat wd = 1 ; 
GLfloat _cubecol[8][3] = { 
  {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0},
  {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}} ; 
GLfloat cubeverts[8][3] = {
  {-wd, -wd, 0.0}, {-wd, wd, 0.0}, {wd, wd, 0.0}, {wd, -wd, 0.0},
  {-wd, -wd, wd}, {wd, -wd, wd}, {wd, wd, wd}, {-wd, wd, wd}
} ; 
GLfloat cubecol[8][3] ;
GLubyte cubeinds[6][4] = { 
	{0, 1, 2, 3}, // BOTTOM 
	{4, 5, 6, 7}, // TOP 
	{0, 4, 7, 1}, // LEFT 
	{0, 3, 5, 4}, // FRONT
	{3, 2, 6, 5}, // RIGHT 
	{1, 7, 6, 2} // BACK
} ; 

sceneObject mycube ((const char *)"MYCUBE", (GLfloat*)cubeverts, sizeof(cubeverts), (GLubyte*)cubeinds, sizeof(cubeinds), (GLfloat*) _cubecol, sizeof(_cubecol), (GLfloat*)NULL, GL_QUADS);

//make a floor

int worldSize = 10;
GLfloat floorverts[4][3] = {
  {worldSize, worldSize, 0.0}, {-worldSize, worldSize, 0.0}, {-worldSize, -worldSize, 0.0}, {worldSize, -worldSize, 0.0}
} ;
GLfloat floorcol[4][3] = {
  {0.0, 0.3, 0.0}, {0.0, 0.3, 0.0}, {0.0, 0.3, 0.0}, {0.0, 0.3, 0.0}
} ; 
GLubyte floorinds[1][4] = { {0, 1, 2, 3} } ; 
GLfloat floortex[4][2] = { 
  {20.0, 20.0}, {0.0, 20.0}, {0.0, 0.0}, {20.0, 0.0}
} ;
sceneObject myfloor ((const char *)"FLOOR", (GLfloat*)floorverts, sizeof(floorverts), (GLubyte*)floorinds, sizeof(floorinds), (GLfloat*) floorcol, sizeof(floorcol), (GLfloat*)NULL, GL_POLYGON);

//make a dude
GLfloat dudelen = .3;
GLfloat dudewidth = .15;
GLfloat boardlength = 1;
GLfloat dudeverts[10][3] = {
  {-dudewidth, -dudelen/2, 0.0}, {-dudewidth, dudelen/2, 0.0}, {-dudewidth, 0.0, dudelen},
  {dudewidth, -dudelen/2, 0.0}, {dudewidth, dudelen/2, 0.0}, {dudewidth, 0.0, dudelen},
  {-dudewidth, -boardlength, 0.0}, {-dudewidth, boardlength, 2*dudelen},
  {dudewidth, -boardlength, 0.0}, {dudewidth, boardlength, 2*dudelen}
} ; 
GLfloat dudecol[10][3] = {
  {0.0, 0.0, 0.0},{0.0, 0.0, 0.0},{0.0, 0.0, 0.0},
  {0.0, 0.0, 0.0},{0.0, 0.0, 0.0},{0.0, 0.0, 0.0},
  {0.5, 0.0, 0.0},{0.5, 0.0, 0.0},{0.5, 0.0,0.0},{0.5, 0.0, 0.0}
} ; 
GLubyte dudeinds[10][3] = { {0, 1, 2}, {3, 4, 5}, {0, 1, 4}, {4, 3, 0}, {0, 3, 5} , {5, 2, 0} , {1, 2, 5} ,{5, 4, 1}, {6, 7, 8}, {8, 9, 7}} ; 

sceneObject dude ((const char *)"DUDE", (GLfloat*)dudeverts, sizeof(dudeverts), (GLubyte*)dudeinds, sizeof(dudeinds), (GLfloat*) dudecol, sizeof(dudecol), (GLfloat*)NULL, GL_TRIANGLES);

//ADDING AN IMPORTED OBJECT

std::vector<std::string> split(std::string const &input) { 
    std::stringstream buffer(input);
    std::vector<std::string> ret;

    std::copy(std::istream_iterator<std::string>(buffer), 
              std::istream_iterator<std::string>(),
              std::back_inserter(ret));
    return ret;
}

/*Read an object file*/
void loadObjFile(const char* filename, std::vector<vec3> &verts, std::vector<vec3> &faces) {
  
  std::string line;
  std::ifstream myfile (filename);
  if (myfile.is_open())
  {
    while ( myfile.good() )
    {
      getline (myfile,line);
      // std::cout << line<<std::endl; 
      if (line[0] == 'v' && line[1] == ' ') {
	std::vector<std::string> sline = split(line);
	GLfloat a = (GLfloat)atof(sline[1].c_str());
	GLfloat b = (GLfloat)atof(sline[2].c_str()); 
	GLfloat c = (GLfloat)atof(sline[3].c_str());
	verts.push_back(vec3(a,b,c));
      }
      if (line[0] == 'f') {
	std::vector<std::string> sline = split(line);
	GLuint d = (GLuint)(atoi(sline[1].c_str()) - 1); 
	for (int i = 3; i < sline.size(); i ++) {
	  GLuint e = (GLuint)(atoi(sline[i-1].c_str()) - 1);
	  GLuint f = (GLuint)(atoi(sline[i].c_str()) - 1);
	  faces.push_back(vec3(d,e,f));
	}
      }
    }
    myfile.close();
  }
}

//Used for imported objects
