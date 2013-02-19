//Variables for HW4

// This is the basic include file for the global variables in the program.  
// Since all files need access to it, we define EXTERN as either blank or 
// extern, depending on if included in the main program or not.  

#ifdef MAINPROGRAM 
#define EXTERN 
#else 
#define EXTERN extern 
#endif 

#include "block.h"
#include <iostream>
#include <sys/time.h>

float fovy = 70; //Field of Vision

bool wireframe = false, 
  fullscreen = false, 
  cursorflag = false,
  portal = false;//Flags for toggles

bool keydown[4];
int mouseoldx, mouseoldy ; // For mouse motion
float playerHeight = 1.73;
float eyeHeight = 1.62;
int worldDimX = 20, worldDimY = 20, worldDimZ = 10;
int fogDistance = 10;
vec3 const eyeinit(0.0,0.0,worldDimZ - 1) ; // Initial eye position, also for resets
vec3 const upinit(0.0,0.0,1.0) ; // Initial up position, also for resets
vec3 const centerinit(0.0,5.0,0.25) ;

vec3 eye = eyeinit;
vec3 up = upinit;
vec3 center = centerinit, centerRot;
vec3 lookVec, rightdir, leftdir, displacement;

float fakeGravity = -.05; // pusedo gravity
float jumpHeight = 0.35;
int zNear = 0.001, zFar = 40; //How far can you see
int selectDistance = 6; //How far can you click
float moveSpeed = 0.125; //How fast do you move
time_t lastTick;
bool fpsCount;

//Triangle for smoke
struct smokeTri{
  vec3 motion;
  vec3 location;
  vec3 verts[3];
  vec3 normal;
};
int smokiness = 100;
vector<smokeTri*> smokeParticles;

float sunx = eyeinit.x, suny = eyeinit.y; // Where is the sun (area light?)
float sunDist = 40;
float brightness = 1.0;

int windowWidth = 500, windowHeight = 500; //Width/Height of OpenGL window
float centery = (float) windowHeight / 2.0;
float centerx = (float) windowWidth / 2.0;

GLuint vertexshader, fragmentshader, shaderprogram ; // shaders
GLuint index, top, sides; //display list

// Lighting parameter array, similar to that in the fragment shader
const int numLights = 50 ; 
 GLfloat lightposn [4*numLights] ; // Light Positions
 GLfloat lightcolor[4*numLights] ; // Light Colors
 GLfloat lightransf[4*numLights] ; // Lights transformed by modelview
 GLuint numused;                     // How many lights are used 

// Object array
const int maxNumBlocks = 50; 
int numBlocksUsed = 0;// How many objects are used 
int currId = 0;// What id is next

//Blocks for usage
vector<Block> allBlocks;
vec3 selectedNormal;
Type activeType = DIRT;
Type portal1Type, portal2Type;
int portal1id, portal2id;
vec3 portal1Pos, portal2Pos;
bool portal1Exists, portal2Exists;
//int activeType = 0;

vector<triobject> blockTriFrame;
triobject initobj;
void initBlockFrame() {
  initobj.type = activeType;
  initobj.vertexA = vec3(0,0,0);
  initobj.vertexB = vec3(0,0,1);
  initobj.vertexC = vec3(0,1,1);
  initobj.facenorm = vec3(1,0,0);
  blockTriFrame.push_back(initobj);
  initobj.vertexA = vec3(0,0,0);
  initobj.vertexB = vec3(0,1,1);
  initobj.vertexC = vec3(0,1,0);
  initobj.facenorm = vec3(1,0,0);
  blockTriFrame.push_back(initobj);
  initobj.vertexA = vec3(1,0,0);
  initobj.vertexB = vec3(1,1,0);
  initobj.vertexC = vec3(1,1,1);
  initobj.facenorm = vec3(-1, 0, 0);
  blockTriFrame.push_back(initobj);
  initobj.vertexA = vec3(1,0,0);
  initobj.vertexB = vec3(1,1,1);
  initobj.vertexC = vec3(1,0,1);
  initobj.facenorm = vec3(-1, 0, 0);
  blockTriFrame.push_back(initobj);
  initobj.vertexA = vec3(0,0,1);
  initobj.vertexB = vec3(1,0,1);
  initobj.vertexC = vec3(1,1,1);
  initobj.facenorm = vec3(0, 0, -1);
  blockTriFrame.push_back(initobj);
  initobj.vertexA = vec3(0,0,1);
  initobj.vertexB = vec3(1,1,1);
  initobj.vertexC = vec3(0,1,1);
  initobj.facenorm = vec3(0, 0, -1);
  blockTriFrame.push_back(initobj);
  initobj.vertexA = vec3(1,0,0);
  initobj.vertexB = vec3(1,0,1);
  initobj.vertexC = vec3(0,0,1);
  initobj.facenorm = vec3(0, 1, 0);
  blockTriFrame.push_back(initobj);
  initobj.vertexA = vec3(1,0,0);
  initobj.vertexB = vec3(0,0,1);
  initobj.vertexC = vec3(0,0,0);
  initobj.facenorm = vec3(0, 1, 0);
  blockTriFrame.push_back(initobj);
  initobj.vertexA = vec3(0,0,0);
  initobj.vertexB = vec3(0,1,0);
  initobj.vertexC = vec3(1,1,0);
  initobj.facenorm = vec3(0, 0, 1);
  blockTriFrame.push_back(initobj);
  initobj.vertexA = vec3(0,0,0);
  initobj.vertexB = vec3(1,1,0);
  initobj.vertexC = vec3(1,0,0);
  initobj.facenorm = vec3(0, 0, 1);
  blockTriFrame.push_back(initobj);
  initobj.vertexA = vec3(0,1,0);
  initobj.vertexB = vec3(0,1,1);
  initobj.vertexC = vec3(1,1,1);
  initobj.facenorm = vec3(0, -1, 0);
  blockTriFrame.push_back(initobj);
  initobj.vertexA = vec3(0,1,0);
  initobj.vertexB = vec3(1,1,1);
  initobj.vertexC = vec3(1,1,0);
  initobj.facenorm = vec3(0, -1, 0);
}

//Texture
GLuint texNames[1] ; // texture buffer
const char * tfNames [100] ; //filenames of textures
GLint texsampler;
GLuint istex ;  // blend parameter for texturing

//Toremove
vec3 teapotloc = vec3(0, 0, 0) ; // where the teapot is located
GLfloat rotamount = 0.0; // amount to rotate teapot by
GLint animate = 1 ; // whether to animate or not

float accelx, accely, velx, vely;
float coneRot = 0;
int fogMode = 1;
int numPlants = 300;
float plantCoords[600];
char waterCycle = '1';
int waterRate = 30;
int waterCount = 0;

GLubyte grasstexture[256][256][3] ; // texture
GLuint lighteffect; // lighting effect in the shader
GLuint vertexeffect; // vertex effect mapping flag
GLint texturing = 1 ; // turn on/off texturing
GLint lighting = 1 ; // turn on/off lighting


//Uniform Locations
GLuint lightcol ; 
GLuint lightpos ; 
GLuint shadow;
GLuint shadowed;
GLuint numusedcol ;
GLuint ambientcol ; 
GLuint diffusecol ; 
GLuint specularcol ; 
GLuint emissioncol ; 
GLuint shininesscol ; 
GLuint displace ;
GLuint heightMap;
GLuint eyepos;
GLuint fog;
GLuint fogd;

GLuint centerOfDisplacementCol;
GLuint normal;

//common values for lighting terms
const GLfloat one[] = {1, 1, 1, 1};
const GLfloat medium[] = {0.5, 0.5, 0.5, 1};
const GLfloat small[] = {0.2, 0.2, 0.2, 1};
const GLfloat vsmall[] = {0.05, 0.1, 0.02, 1};
const GLfloat high[] = {200} ;
const GLfloat low[] = {1} ;
const GLfloat zero[] = {0.0, 0.0, 0.0, 1.0} ; 

GLfloat shadowAtten = 0.2;
GLfloat shadowUnatten = 1.0;
