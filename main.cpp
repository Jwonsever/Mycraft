/***************************************************************************/
/* CS184 Fall 2012                                                         */
/* Benjamin Han                                                            */
/* James Wonsever                                                          */
/***************************************************************************/

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <FreeImage.h>

#include "Quaternion.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cstdlib>
#include "shaders.h"
#include "variables.h"

#include "raytrace.h"

/*todo
#include "cloneio.h"
#include "display.h"
#include "human.h"
*/

void loadTexture(const char* filename);
bool shadowTrace(vec3 lightpos, vec3 vertexpos);
void removeBlock(int id);

/* trandsformation function to transform vector by modelview */ 
void addLight(vec4 p, vec4 c) {
    lightposn[4*numused] = p.x;
    lightposn[4*numused+1] = p.y;
    lightposn[4*numused+2] = p.z;
    lightposn[4*numused+3] = p.w;
    lightcolor[4*numused] = c.x;
    lightcolor[4*numused+1] = c.y;
    lightcolor[4*numused+2] = c.z;
    lightcolor[4*numused+3] = c.w;
   
    numused++;
}

int newBlock(vec3 position, Type type) {
  int blockid;

  if (type == TORCH) {
    vec4 flooreye = vec4(eye.x, eye.y, floor(eye.z - 1), 1);
    addLight(flooreye, vec4(.2,.2,.2,1));
    return 0;
  }
  //else add a block
  if ((floor(eye.x) != floor(position.x) or floor(eye.y) != floor(position.y) or floor(eye.z) != floor(position.z + 1) and floor(eye.z) != floor(position.z + 0)) and abs(position.x) <= worldDimX and abs(position.y) <= worldDimY and abs(position.z) <= worldDimZ  + 1) {

    Block blk;
    blk.type = type;
    blk.position = position;
    blk.id = currId;
    allBlocks.push_back(blk);
    numBlocksUsed ++;
    currId ++;
    blockid = currId;
  }
  return blockid;
}

void drawPortal(int id, vec3 normal, int portNum) {
  for (int i = 0; i < numBlocksUsed; i++) {
    if (allBlocks[i].id == id and allBlocks[i].position.z >= -worldDimZ) {
      if (portNum == 0) {
	if (portal1Exists) {
	  removeBlock(portal1id);
	  newBlock(portal1Pos, portal1Type);
	}
	portal1Pos = allBlocks[id].position;
	portal1Type = allBlocks[id].type;
	removeBlock(id);
	portal1id = newBlock(portal1Pos, PORT1);
	if (portal1id != -1)
	  portal1Exists = true;
      } else {
	if (portal2Exists) {
	  removeBlock(portal2id);
	  newBlock(portal2Pos, portal2Type);
	}
	portal2Pos = allBlocks[id].position;
	portal2Type = allBlocks[id].type;
	removeBlock(id);
	portal1id = newBlock(portal2Pos, PORT2);
	if (portal2id != -1)
	  portal2Exists = true;

      }
      break;
    }
  }
}

void removeBlock(int id) {
  for (int i = 0; i < numBlocksUsed; i++) {
   if (allBlocks[i].id == id and allBlocks[i].position.z >= -worldDimZ) {
      allBlocks.erase(allBlocks.begin() + i);
      numBlocksUsed--;
      if (id == portal1id)
	portal1Exists = false;
      else if (id == portal2id)
	portal2Exists = false;
      break;
    }
  }
}

//Function to displacement player
void displaceFunc (void) {
  float oldZ = displacement.z;
  displacement = vec3(0, 0, 0.00001);
  if (keydown[0]) {
    lookVec = glm::normalize(eye - center);
    rightdir = -glm::cross(lookVec, vec3(0, 0, 1));
    rightdir = glm::normalize(vec3(rightdir.x, rightdir.y, 0));
    displacement += rightdir;
  }
  if (keydown[1]) {
    lookVec = glm::normalize(eye - center);
    leftdir = glm::cross(lookVec, vec3(0, 0, 1));
    leftdir = glm::normalize(vec3(leftdir.x, leftdir.y, 0));
    displacement += leftdir;
  }
  if (keydown[2]) {
    lookVec = glm::normalize(eye - center);
    lookVec = glm::normalize(vec3(lookVec.x, lookVec.y, 0));
    displacement -= lookVec;
  }
  if (keydown[3]) {
    lookVec = glm::normalize(eye - center);
    lookVec = glm::normalize(vec3(lookVec.x, lookVec.y, 0));
    displacement += lookVec;
  }
  displacement = moveSpeed * glm::normalize(displacement);
  displacement.z = oldZ + fakeGravity;
}

//Function to limit eye (user/human) position
float boundEye(vec3 * currEye,vec3 * displacement) {
  //std::cout << currEye->x << "," << currEye->y << "," << currEye->z << "\n";
  vec3 eyelimits = vec3(worldDimX - 0.10, worldDimY - 0.10, worldDimZ - 0.10);
  vec3 neglimits = vec3(-worldDimX + 0.10, -worldDimY + 0.10, -worldDimZ + eyeHeight);

  //Collision Detection
  for (int i = 0; i < numBlocksUsed; i++) {
    Block myblock = allBlocks[i];
    vec3 blockpos = myblock.position;
    if (floor(currEye->z) == floor(blockpos.z) or (floor(currEye->z) - 1  == floor(blockpos.z))) {
      if (floor(currEye->y) == floor(blockpos.y)) {
	if (currEye->x < blockpos.x) {
	  eyelimits.x = min(eyelimits.x, (float) (blockpos.x - 0.20));
	} else {
	  neglimits.x = max(neglimits.x, (float) (blockpos.x + 1.20));
	}
      }
      if (floor(currEye->x) == floor(blockpos.x)) {
	if (currEye->y < blockpos.y) {
	  eyelimits.y = min(eyelimits.y, (float) (blockpos.y - 0.20));
	} else {
	  neglimits.y = max(neglimits.y, (float) (blockpos.y + 1.20));
	}
      }
    }
    //if (((floor(currEye->x + 0.25) == floor(blockpos.x)) or (floor(currEye->x - 0.25) == floor(blockpos.x))) and 
    //    ((floor(currEye->y + 0.25) == floor(blockpos.y)) or (floor(currEye->y - 0.25) == floor(blockpos.y)))) {
    if (floor(currEye->x) == floor(blockpos.x) and floor(currEye->y) == floor(blockpos.y)) {
      if (floor(currEye->z) < floor(blockpos.z)) {
	eyelimits.z = min(eyelimits.z, (float) floor(blockpos.z) - 0.10);
      } else  if (floor(currEye->z) >= floor(blockpos.z) + 2) {
	neglimits.z = max(neglimits.z, (float) floor(blockpos.z) + 1 + eyeHeight);
      }
    }
  }

  //x bounds
  if (currEye -> x + displacement -> x > eyelimits.x) {
    displacement -> x = max(eyelimits.x - currEye->x, 0);
    //displacement -> x = eyelimits.x - currEye->x;
  }
  if (currEye -> x + displacement -> x < neglimits.x) {
    displacement -> x = min(neglimits.x - currEye->x, 0);
    //displacement -> x = neglimits.x - currEye->x;
  }
//y bounds
  if (currEye -> y + displacement -> y > eyelimits.y) {
    displacement -> y = max(eyelimits.y - currEye->y, 0);
    //displacement -> y = eyelimits.y - currEye->y;
  }
  if (currEye -> y + displacement -> y < neglimits.y) {
    displacement -> y = min(neglimits.y - currEye->y, 0);
    //displacement -> y = neglimits.y - currEye->y;
  }
//z bounds
  if (currEye -> z + displacement -> z > eyelimits.z) {
    displacement -> z = max(eyelimits.z - currEye->z, 0);
    //displacement -> z = eyelimits.z - currEye->z;
  }
  if (currEye -> z + displacement -> z < neglimits.z) {
    displacement -> z = min(neglimits.z - currEye->z, 0);
    //displacement -> z = neglimits.z - currEye->z;
  }

  // thinking with portals
  if (portal1Exists and portal2Exists) {
    vec3 portal1Center = portal1Pos + vec3(0.5, 0.5, 0.5);
    vec3 portal2Center = portal2Pos + vec3(0.5, 0.5, 0.5);
    vec3 Eye = eye - vec3(0, 0, 0.12);
    vec3 shortEye = eye - vec3(0, 0, 1.12);
    if (magnitude(Eye - portal1Center) < 0.85 or (magnitude(shortEye - portal1Center) < 0.85)) {
      eye = portal2Center + vec3(0, 0, 2);
    } else if (magnitude(Eye - portal2Center) < 0.85 or (magnitude(shortEye - portal2Center) < 0.85)) {
      eye = portal1Center + vec3(0, 0, 2);
    }
  }
  glutPostRedisplay();
  return neglimits.z;
}

//Checks if the eye location triggers something
void eyeTriggers() {
  if (true) {
    //show new eye position
    //std::cout << eye.x << " " << eye.y << std::endl;
  }
}

void transformvec (const GLfloat input[4], GLfloat output[4]) {
	GLfloat modelview[16] ; // in column major order
	glGetFloatv(GL_MODELVIEW_MATRIX, modelview) ; 

	for (int i = 0 ; i < 4 ; i++) {
		output[i] = 0 ; 
		for (int j = 0 ; j < 4 ; j++) 
		  output[i] += modelview[4*j+i] * input[j] ; 
	}

}
void displayHUD() {
  //Draw a 2d HUD (TODO)
  //...After Drawing 3d Stuff...

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0.0, windowWidth, windowHeight, 0.0, -1.0, 10.0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glDisable(GL_CULL_FACE);
	glClear(GL_DEPTH_BUFFER_BIT);
	glUniform1i(fog, 0);
			
	//ActiveTextureBar
	int bottomMidX = windowWidth/2;
	int bottomLeftY = windowHeight - 60;
	int boxSize = 32;
	int spacing = 40;	

	glUniform1i(istex, 1);
	glUniform1i(lighteffect, 5);
	glColor3f(1.0f, 1.0f, 1.0f);
	for (int i = 0; i < 10; i++) {
	  int j = i - 5;
	  loadTexture(tfNames[i]);
	  glBegin(GL_QUADS);
	  glTexCoord2d(1.0,0.0); glVertex2f(bottomMidX + j * spacing, bottomLeftY);
	  glTexCoord2d(1.0,1.0); glVertex2f(bottomMidX + j * spacing, bottomLeftY + boxSize);
	  glTexCoord2d(0.0,1.0); glVertex2f(bottomMidX + j * spacing + boxSize, bottomLeftY + boxSize);
	  glTexCoord2d(0.0,0.0); glVertex2f(bottomMidX + j * spacing + boxSize, bottomLeftY);
	  glEnd();

	  if ((int) activeType == i) {
	    glUniform1i(istex, 0);
	    glUniform1i(lighteffect, 0);
	    glColor3f(1.0f, 1.0f, 0.0f);
	    glBegin(GL_QUADS);
	    glVertex2f(bottomMidX + j * spacing - 2, bottomLeftY - 2);
	    glVertex2f(bottomMidX + j * spacing - 2, bottomLeftY + boxSize + 2);
	    glVertex2f(bottomMidX + j * spacing + boxSize + 2, bottomLeftY + boxSize + 2);
	    glVertex2f(bottomMidX + j * spacing + boxSize + 2, bottomLeftY - 2);
	    glEnd();
	    glUniform1i(lighteffect, 5);
	    glColor3f(1.0f, 1.0f, 1.0f);
	    glUniform1i(istex, 1);
	  }
	}
	
	glUniform1i(lighteffect, 0);
	glUniform1i(istex, 0);
	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_QUADS);
	glVertex2f(bottomMidX - 5 * spacing - 4, bottomLeftY - 4);
	glVertex2f(bottomMidX - 5 * spacing - 4, bottomLeftY + boxSize + 4);
	glVertex2f(bottomMidX + 5 * spacing - 4, bottomLeftY + boxSize + 4);
	glVertex2f(bottomMidX + 5 * spacing - 4, bottomLeftY - 4);
	glEnd();
	

	//Cursor
	int cursLength = 10;
	int cursWidth = 2;
	int cursCenX = windowWidth/2;
	int cursCenY = windowHeight/2;
	glUniform1i(lighteffect, 0);
	glUniform1i(istex, 0);		
	glColor3f(0.1f, 0.1f, 0.0f);
	glBegin(GL_QUADS);
	glVertex2f(cursCenX - cursWidth, cursCenY + cursLength);
	glVertex2f(cursCenX + cursWidth, cursCenY + cursLength);
	glVertex2f(cursCenX + cursWidth, cursCenY - cursLength);
	glVertex2f(cursCenX - cursWidth, cursCenY - cursLength);
	glEnd();
	glBegin(GL_QUADS);
	glVertex2f(cursCenX - cursLength, cursCenY + cursWidth);
	glVertex2f(cursCenX + cursLength, cursCenY + cursWidth);
	glVertex2f(cursCenX + cursLength, cursCenY - cursWidth);
	glVertex2f(cursCenX - cursLength, cursCenY - cursWidth);
	glEnd();

	// Making sure we can render 3d again
	glUniform1i(fog, 1);
	      
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}
void display(void)
{
        if (fpsCount) {
	  timeval curTime;
	  gettimeofday(&curTime, NULL);
	  int now = curTime.tv_sec * 1000 + curTime.tv_usec / 1000;
	  std::cout << 1000 / difftime(now, lastTick) << std::endl;
	  lastTick = now;
	}

        displaceFunc();
        boundEye(&eye, &displacement);
	eye += displacement;
	center += displacement;
	sunx += displacement.x;
	suny += displacement.y;
	eyeTriggers();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW) ; 
	mat4 mv = glm::lookAt(eye, center, up);

	GLfloat eyeposn[] = {eye.x, eye.y, eye.z, 1};
	glUniform4fv(eyepos, 1, eyeposn);
	glLoadMatrixf(&mv[0][0]);
	glUniform1i(displace, 0);
	glUniform1i(fog, 0);
	glUniform1i(shadowed, 0);
	glUniform1f(shadow, shadowUnatten);
	glUniform1i(lighteffect,1) ;
	glUniform1i(numusedcol,numused);

	//----------------LIGHTING--------------//
	if (numused > 0) {

	  
	  GLfloat transformedposn[4*numused];
	  GLfloat tempin[4];
	  GLfloat tempout[4];
	  for (int i = 0; i < numused; i++) {
	    for (int j = 0; j < 4; j++) tempin[j] = lightposn[i*4 + j];
	    vec4 temp = vec4(tempin[0], tempin[1], tempin[2], tempin[3]);
	    transformvec(tempin,tempout); 

	    if (i == 0) {
	      glPushMatrix();
	      glTranslatef(sunx, suny, 0);
	      glRotatef(rotamount, 1.0, 1.0, 0.0);
	      brightness = 0.8 * max(cos(rotamount * 3.14 / 180), 0) + 0.2;
	      glTranslated(0, 0, sunDist);
	      glColor3f(1, 179/255.0, 16/255.0);
	      glUniform1i(lighteffect, 0);
	      glutSolidCube(5);
	      glPopMatrix();
	    }
	    //torch
	    else {
	      glPushMatrix();
	      glTranslatef(tempin[0],tempin[1],tempin[2]);
	      glTranslatef(0,0,.45);
	      glColor3f(1, 149/255.0, 16/255.0);
	      glUniform1i(lighteffect, 0);
	      glutSolidCube(.1);

	      if (dot(vec3(temp) - eye, vec3(temp) - eye) < 64) {
		glUniform1i(lighteffect,0) ;
		//smoke
		glColor3f(0.7,0.7,0.6);
		if (smokiness > 0) {
		  for (int j= 0; j< 300; j++) {
		    smokeTri* tempobj = (smokeTri *)calloc(1, sizeof(smokeTri));
		    smokiness--;
		    int r1 = rand() % 100 - 50, r2 = rand() % 100 - 50;
		    int r3 = rand() % 100 + 800, r4 = rand() % 100 - 50;
		    tempobj -> motion = vec3(r1/10000.0, r2/10000.0, r3/100000.0);
		    tempobj -> verts[0] = vec3(0.01, 0.01, 0.01);
		    tempobj -> verts[1] = vec3(0.02, -0.01, 0.005);
		    tempobj -> verts[2] = vec3(0.0, 0.0, 0.0);
		    tempobj -> normal = normalize(glm::core::function::geometric::cross(vec3(0.01, 0.01, 0.01) - vec3(0.02, -0.01, 0.005), vec3(0.02, -0.01, 0.005) - vec3(0.0, 0.0, 0.0)));
		    tempobj -> location = vec3(r4/10000.0, r1/10000.0, 0);
		    smokeParticles.push_back(tempobj);
		  }
		}
		//
		smokeTri * myiter;
		int lim = smokeParticles.size();
		for (int j= 0; j< lim; j++) {
		  myiter = smokeParticles[j];
		  glPushMatrix();
		  glTranslatef(myiter -> location.x, myiter -> location.y, myiter -> location.z);
		  glRotatef(100, myiter -> location.y * 10, myiter -> location.z * 10, myiter -> location.x * 10);
		  glScalef(rand() % 2 + .4, rand() % 2 + .4 , rand() % 2 + .4);
		  glBegin(GL_TRIANGLES);
		  glVertex3d(myiter -> verts[0].x, myiter -> verts[0].y, myiter -> verts[0].z);
		  glVertex3d(myiter -> verts[1].x, myiter -> verts[1].y, myiter -> verts[1].z);
		  glVertex3d(myiter -> verts[2].x, myiter -> verts[2].y, myiter -> verts[2].z);
		  glEnd();
		  glPopMatrix();

		  if (rand() % 100 * myiter -> location.z > 10) {
		    free(myiter);
		    smokeParticles.erase(smokeParticles.begin() + j);
		    smokiness ++;
		    lim --;
		    } else {
		    myiter -> location = myiter -> location + myiter -> motion;
		    }	  
		}

		glUniform1i(lighteffect,0) ;
	      }
	      glTranslatef(0,0,-.25);
	      glScalef(1,1,4);
	      glColor3f(.5, .35, 16/255.0);
	      glutSolidCube(.1);
	      glPopMatrix();
	    }

	   
	    for (int j = 0; j < 4; j++) transformedposn[4*i + j] = tempout[j];
	  }

	  glUniform4fv(lightpos,numLights,transformedposn); 
	  glUniform4fv(lightcol,numLights,lightcolor); 

	  glUniform4fv(ambientcol,1,small) ; 
	  glUniform4fv(diffusecol,1,medium) ; 
	  glUniform4fv(specularcol,1,one) ; 
	  glUniform1fv(shininesscol,1,low) ; 
	  glUniform4fv(emissioncol,1,zero);     
	}
	else glUniform1i(lighteffect,false) ; 
	
	    
	//--------------------OBJECTS--------------------//

	glUniform1i(texsampler, GL_TEXTURE0);
	glUniform1i(lighteffect, 2);
	glUniform1i(fog, 1);
	glUniform1i(istex, 1);

	//Does this help???
	vector<Block> blockBuf;
	for (int i = 0; i < numBlocksUsed; i++) {
	    Block myblock = allBlocks[i];
	    vec3 tmppos = myblock.position - eye;
	    if (tmppos.x * tmppos.x + tmppos.y * tmppos.y < fogDistance * fogDistance) {
	      blockBuf.push_back(myblock);
	    }
	}

	glColor3f(.8, .8, .8);
	for(int i=0; i< lengthOfTypes; i++)
	{
	  Type current = (Type) i; 
	  loadTexture(tfNames[current]);

	  float maxZ[2 * worldDimX * 2 * worldDimY];
	  for (int i = 0; i < 2 * worldDimX * 2 * worldDimY; i++)
	    maxZ[i] = -worldDimZ;
	  for (int i = 0; i < blockBuf.size(); i++) {
	    Block myblock = blockBuf[i];
	    vec3 tmppos = myblock.position;
	    int b = (floor((float) tmppos.x) + worldDimX) * 2 * worldDimY + floor((float) tmppos.y) + worldDimY;
	    maxZ[b] = max(floor((float) tmppos.z), maxZ[b]);
	  }
										      
	  for (int i = 0; i < blockBuf.size(); i++) {
	    Block myblock = blockBuf[i];
	    vec3 tmppos = myblock.position;
	    if (magnitude(tmppos - eye) < fogDistance) {
	      if (myblock.type == current or (myblock.type == GRASS and current == DIRT)) {
		glPushMatrix();
		int b = (floor((float) tmppos.x) + worldDimX) * 2 * worldDimY + floor((float) tmppos.y) + worldDimY;
		
		if (floor(maxZ[b]) == floor((float) tmppos.z)) {

		  glUniform1f(shadow, shadowUnatten * brightness);
		  glUniform1i(shadowed, 0);
		}
		else { 
		  glUniform1f(shadow, shadowAtten * brightness);
		  glUniform1i(shadowed, 1);
		}

		glTranslatef(tmppos.x, tmppos.y, tmppos.z);
		if (myblock.type == GRASS) {
		  if (current == GRASS) 
		    glCallList(top);
		    //myblock.drawTop();
		  else if (current == DIRT)
		    glCallList(sides);
		    //myblock.drawSides();
		} else {
		  glCallList(index);
		  //myblock.draw(index);
		}
		glPopMatrix();
	      }
	    }
	  }
	  //glDeleteLists(index, 1);
	}
	/*
	if (view == THIRDPERSON) {
	  human h = myhuman;
	  h.draw();
	}
	*/

	glUniform1f(shadow, shadowUnatten); //Dont shadow hud
	displayHUD();

	glutSwapBuffers() ; 
	glFlush ();
}

// Defines a Mouse callback to zoom in and out 
// This is done by modifying gluLookAt         
// The actual motion is in mousedrag           
// mouse simply sets state for mousedrag       
void mouse(int button, int state, int x, int y) 
{
  if ((button == 3) || (button == 4)) // It's a wheel event
    {
      if (state == GLUT_DOWN) {
	if (button == 3) {
	  fovy --;
	  if (fovy < 20) fovy = 20;
	  glMatrixMode(GL_PROJECTION);	glLoadIdentity();
	  gluPerspective(fovy, (GLdouble)windowWidth/(GLdouble)windowHeight, 0.1, zFar) ;
	}
	if (button == 4) {
	  fovy ++;
	  if (fovy > 90) fovy = 90;
	  glMatrixMode(GL_PROJECTION);	glLoadIdentity();
	  gluPerspective(fovy, (GLdouble)windowWidth/(GLdouble)windowHeight, 0.1, zFar) ;
	}
      }
    }
  else
  {  
    //Selections
    if (state == GLUT_DOWN) {
      Block * selected = raytrace();
      if (selected != NULL) {
	if (button == GLUT_LEFT_BUTTON) {
	  if (portal) 
	    drawPortal(selected -> id, selectedNormal, 0);
	  else
	    newBlock(selected -> position - selectedNormal, activeType);
	}
	if (button == GLUT_RIGHT_BUTTON) {
	  if (portal)
	    drawPortal(selected -> id, selectedNormal, 1);
	  else
	    removeBlock(selected -> id);
	}
      }
    }
  }
  glutPostRedisplay();
}

void mousedrag(int x, int y) {

  if (!cursorflag) {
    float centery = (float) windowHeight / 2.0;
    float centerx = (float) windowWidth / 2.0;
    float yNorm = (float) (y - mouseoldy) / (float) windowHeight;
    float xNorm = (float) (x - mouseoldx) / (float) windowWidth;

    Quaternion elevation, azimuth, rotQ;
    azimuth.FromAxis(vec3(0, 0, 1), xNorm);
    vec3 viewing = eye-center;
    vec3 side = glm::normalize(glm::cross(up, viewing));
    //fix camera at near up/down angles
    elevation.FromAxis(side, yNorm);
    rotQ = azimuth*elevation;
    
    mat4 rotate = rotQ.getMatrix();
    mat4 M = translate(eye.x, eye.y, eye.z) * rotate * translate(-eye.x, -eye.y, -eye.z);
    vec4 center4 = M * vec4(center, 1);
    center = vec3(center4.x/center4.w, center4.y/center4.w, center4.z/center4.w);
    mouseoldy = centery;
    mouseoldx = centerx;
    glutWarpPointer(centerx, centery);
    cursorflag = true;
  } else {
    cursorflag = false;
  }
  glutPostRedisplay();
}

void printHelp() {
	std::cout << "Available commands:\n"
		  << "WASD to move, mouse to look.\n"
		  << "press 'h' to print this message again.\n"
		  << "press Esc to quit.\n"
		  << "press 'o' to save a screenshot to \"./screenshot.bmp\"\n"
		  << "press 'i' to toggle wireframe mode \n"
		  << "press 'p' to start/stop teapot animation.\n"
		  << "press 't' to turn texturing on/off.\n"
		  << "press 'j' to turn shading on/off.\n"
		  << "press 'g' to turn night on/off.\n";
}

void saveScreenshot() {
	int pix = windowWidth * windowHeight;
	BYTE pixels[3*pix];
	glReadBuffer(GL_FRONT);
	glReadPixels(0,0,windowWidth,windowHeight,GL_BGR,GL_UNSIGNED_BYTE,pixels);
	
	FIBITMAP *img = FreeImage_ConvertFromRawBits(pixels, windowWidth, windowHeight, windowWidth * 3, 24, 0xFF0000, 0x00FF00, 0x0000FF, false);

	std::cout << "Saving screenshot: screenshot.png";

	FreeImage_Save(FIF_PNG, img, "screenshot.png", 0);
}

void animation(void) {
  rotamount = rotamount + 0.25;
  if (rotamount > 360.0) rotamount = 0;
}

// Defines what to do when various keys are released
void keyboardUp (unsigned char key, int x, int y) {
  if (key == 'd') // move right
    keydown[0] = false;
  if (key == 'a') // move left
    keydown[1] = false;
  if (key == 'w') // move forward
    keydown[2] = false;
  if (key == 's') // move backward
    keydown[3] = false;
}


// Defines what to do when various keys are pressed 
void keyboard (unsigned char key, int x, int y) 
{
 
  if (key == '1') {
    activeType = (Type) 0;  
    portal = false;
  }
  if (key == '2') {
    activeType = (Type) 1;
    portal = false;
  }
  if (key == '3') {
    activeType = (Type) 2;
    portal = false;
  }
  if (key == '4')  {
    activeType = (Type) 3;  
    portal = false;
  }
  if (key == '5') {
    activeType = (Type) 4;
    portal = false;
  }
  if (key == '6') {
    activeType = (Type) 5;
    portal = false;
  }
  if (key == '7') {
    activeType = (Type) 6;  
    portal = false;
  }
  if (key == '8') {
    activeType = (Type) 7;
    portal = false;
  }
  if (key == '9') {
    activeType = (Type) 8;
    portal = false;
  }
  if (key == '0') {
    portal = false;
    activeType = (Type) 9;
  }

  if (key == 'e') portal = !portal;

  if (key == 'h')
    printHelp();
  if (key == 'o')
    saveScreenshot();
  if (key == 'c') // fps toggle
    fpsCount = !fpsCount;
  if (key == 'i') { // wireframe toggle
    wireframe = !wireframe;
    if (wireframe) glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    else glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    glutPostRedisplay();
  }
  if (key == 27)  // Escape to quit
    exit(0) ;
  if (key == 't') { // turn on/off texturing ; 
    texturing = !texturing ;
    glutPostRedisplay() ; 
  }
  if (key == 'j') { // turn on/off shading (always smooth) ; 
    lighting = !lighting ;
    glutPostRedisplay() ; 
  }
  if (key == 'r') { // reset eye and up vectors, scale and translate. 
    glutWarpPointer(windowWidth/2.0, windowHeight/2.0);
    mouseoldx = centerx;
    mouseoldy = centery;
    eye = eyeinit ; 
    up = upinit ;
    fovy = 70;
    center = centerinit;
    teapotloc = vec3(0, 0, 0);
    sunx = eyeinit.x/2;
    suny = eyeinit.y;
    accelx = accely = velx = vely = 0;
  }
  if (key == 'd') // move right
    keydown[0] = true;
  if (key == 'a')  // move left
    keydown[1] = true;
  if (key == 'w')  // move forward
    keydown[2] = true;
  if (key == 's')  // move backward
    keydown[3] = true;
  if (key == 32) {
    float zness = boundEye(&eye, &displacement);
    if (floor(eye.z) < floor(1.75 + zness) and (displacement.z == 0))
      displacement.z += jumpHeight;
  }
  if (key == 'f') { // full screen mode
    fullscreen = !fullscreen;
    if (fullscreen)
      glutFullScreen();
    else
      {
	glutReshapeWindow(800,600);
	glutPositionWindow(100,100);
      }
    glutWarpPointer(centerx, centery);
  }

}

/* Reshapes the window appropriately */
void reshape(int w, int h)
{
	windowWidth = w;
	windowHeight = h;
	glViewport (0, 0, (GLsizei) w, (GLsizei) h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, (GLdouble)w/(GLdouble)h, 0.1, zFar) ;

}

void loadTexture(const char* filename) {
  
  //Load Image
  //This is the array that will contain the image color information.
  //3 represents red, green and blue color info.
  int w = 32;
  int h = 32;
  GLubyte image[w][h][3];
  int i,j,k ;
    
  for (i = 0 ; i < w ; i++)
    for (j = 0 ; j < h ; j++)
      for (k = 0 ; k < 3 ; k++)
	image[i][j][k] = 0;

  FILE * fp;
  GLint err ; 
  assert(fp = fopen(filename,"rb")) ;
  fscanf(fp,"%*s") ;
  char tmpbuffer[100];
  fgets(tmpbuffer, 100, fp);
  fscanf(fp, "%*d %*d %*d%*c") ;
  for (i = 0 ; i < w ; i++)
    for (j = 0 ; j < h ; j++)
      for (k = 0 ; k < 3 ; k++)
	fscanf(fp,"%c",&(image[i][j][k])) ;
  fclose(fp);

  //Generate texture buffer

  glActiveTexture(GL_TEXTURE0);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texNames[0]);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
  gluBuild2DMipmaps(GL_TEXTURE_2D, 3, w, h, GL_RGB, GL_UNSIGNED_BYTE, image);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP) ;
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP) ;
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
  

}
void initTexFNames() {
  //Load texture filenames
  tfNames[0] = "grass.ppm";
  tfNames[1] = "dirt.ppm";
  tfNames[2] = "stone.ppm";
  tfNames[3] = "bedrock.ppm";
  tfNames[4] = "coal.ppm";
  tfNames[5] = "iron.ppm";
  tfNames[6] = "gold.ppm";
  tfNames[7] = "diamond.ppm";
  tfNames[8] = "wood.ppm";
  tfNames[9] = "torch.ppm";
  tfNames[10] = "sand.ppm";
  tfNames[11] = "leaf.ppm";
  tfNames[12] = "portal1.ppm";
  tfNames[13] = "portal2.ppm";
  tfNames[14] = "gordo.ppm";
}

void init (void) 
{
  //Make a block frame from triangles.
  initBlockFrame();

  //Init texture buffer
  glGenTextures(1, texNames) ;
  glEnable(GL_TEXTURE_2D);
  initTexFNames();
 
  /* select clearing color 	*/
  glClearColor (0.3, 0.5, 0.9, 0.0);
  
  /* initialize viewing values  */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  
  glMatrixMode(GL_MODELVIEW) ;
  glLoadIdentity() ;
  gluLookAt(eye.x, eye.y, eye.z, center.x, center.y, center.z, up.x, up.y, up.z) ;
  
  //make shaders
  vertexshader = initshaders(GL_VERTEX_SHADER, "shaders/light.vert.glsl") ;
  fragmentshader = initshaders(GL_FRAGMENT_SHADER, "shaders/light.frag.glsl") ;
  
  GLuint program = glCreateProgram() ;
  shaderprogram = initprogram(vertexshader, fragmentshader) ;
  GLint linked;
  glGetProgramiv(shaderprogram, GL_LINK_STATUS, &linked) ;  
  
  //Set up the shader parameter mappings properly for lighting.
  lighteffect = glGetUniformLocation(shaderprogram,"lighteffect") ;           
  lightpos = glGetUniformLocation(shaderprogram,"lightposn") ;       
  lightcol = glGetUniformLocation(shaderprogram,"lightcolor") ;
  numusedcol = glGetUniformLocation(shaderprogram,"numused");
  ambientcol = glGetUniformLocation(shaderprogram,"ambient") ;       
  diffusecol = glGetUniformLocation(shaderprogram,"diffuse") ;       
  specularcol = glGetUniformLocation(shaderprogram,"specular") ;       
  shininesscol = glGetUniformLocation(shaderprogram,"shininess") ;  
  emissioncol = glGetUniformLocation(shaderprogram,"emission") ;
	
  normal = glGetUniformLocation(shaderprogram,"mynormal");
  eyepos = glGetUniformLocation(shaderprogram,"eye");
  
  //textures
  texsampler = glGetUniformLocation(shaderprogram, "tex") ;

  shadowed = glGetUniformLocation(shaderprogram, "shadow") ;
  shadow = glGetUniformLocation(shaderprogram, "shadowatten");
  istex = glGetUniformLocation(shaderprogram, "istex") ;

  //force rendering of less objects using fog
  fog = glGetUniformLocation(shaderprogram,"fogFlag");

  //Initialize Lights
  //todo torches
  addLight(vec4(0,0,sunDist,0), vec4(.2,.2,.2,1));

  // Enable the depth test
  glEnable(GL_DEPTH_TEST) ;
  glutPostRedisplay();
  glDepthFunc (GL_LESS) ; // The default option

}

void initMap(int maptype) { 	

  //flat map (For testing)
  if (maptype == -1) {
    for (int i = -worldDimX; i < worldDimX; i++)
      for (int j = -worldDimY; j < worldDimY; j++) {
	newBlock(vec3(i, j, 0), GRASS);
      }
    return;
  }

  //Normal map
  float a = rand() % worldDimX / 3 + 1;
  float b = rand() % worldDimY / 3 + 1;
  float c = rand() % 3;
  int random;
  for (int i = -worldDimX; i < worldDimX; i++)
    for (int j = -worldDimY; j < worldDimY; j++) {
      int randHeight = floor(c * cos(2 * i / (float) worldDimX) * sin(2 * j / (float)  worldDimY) + sin(i / a) + cos(j / b));
      newBlock(vec3(i, j, -worldDimZ - 1), BEDROCK);
      for (int k = -worldDimZ; k < randHeight; k++) {
	random = rand() % 8;
	if (k > -0.3 * worldDimZ and random == 0) {
	  newBlock(vec3(i, j, k), SAND);
	} else if (k < -0.4 * worldDimZ and k > -0.7 * worldDimZ and random == 0) {
	  newBlock(vec3(i, j, k), IRON);
	} else if (k < -0.6 * worldDimZ and k > -0.9 * worldDimZ and random == 1) {
	  newBlock(vec3(i, j, k), COAL);
	} else if (k < -0.8 * worldDimZ and k > -worldDimZ and random == 2) {
	  newBlock(vec3(i, j, k), GOLD);
	} else if (k < -0.9 * worldDimZ and random == 3) {
	  newBlock(vec3(i, j, k), DIAMOND);
	} else {
	  newBlock(vec3(i, j, k), DIRT);
	}
      }
      newBlock(vec3(i, j, randHeight), GRASS);
      random = rand() % 64;
      if (random == 0) {
	random = rand() % 3 + 2;
	for (int k = 0; k < random; k++)
	  newBlock(vec3(i, j, randHeight + k + 1), WOOD);
	for (int k = 0; k < random / 2 + 1; k++) {
	  int offsetQ1 = rand() % 2 - 1;
	  int offsetQ2 = rand() % 2;
	  for (int q = -random / 3 + k + offsetQ1; q < random / 3 - k + 1 + offsetQ2; q++) {
	    int offsetR1 = rand() % 2 - 1;
	    int offsetR2 = rand() % 2;
	    for (int r = -random / 3 + k + offsetR1; r < random / 3 - k + 1 + offsetR2; r++) {
	      if (q == 0 and r == 0 and k == 0) 
		continue;
	      newBlock(vec3(i + q, j + r, randHeight + random + k), LEAF);
	    }
	  }
	}
      }
    }
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);

	// Requests the type of buffers (Single, RGB).
	// Request the depth if needed, later swith to double buffer 
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize (windowWidth, windowHeight); 
	glutInitWindowPosition (100, 100);
	glutCreateWindow ("Minecloned");
	init (); // Always initialize first
	initMap ( 0 ); // Put all the blocks in the map
	Block b;
	index = b.initDraw(); //block in displaylist
	top = b.initTop();
	sides = b.initSides();

	// callbacks and functions for various tasks.
	glutDisplayFunc(display); 
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboardUp);
	glutMouseFunc(mouse);
	glutPassiveMotionFunc(mousedrag);
	glutIdleFunc(animation);

	//init mouse
	mouseoldx = windowWidth/2;
	mouseoldy = windowHeight/2;
	glutWarpPointer(windowWidth/2, windowHeight/2);
	glutSetCursor(GLUT_CURSOR_NONE);

	//printHelp();

	glutMainLoop(); // Start the main code
	return 0;   /* ANSI C requires main to return int. */
}
