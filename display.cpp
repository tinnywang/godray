/*****************************************************************************/
/* This is the program skeleton for homework 2 in CS 184 by Ravi Ramamoorthi */
/* Extends HW 1 to deal with shading, more transforms and multiple objects   */
/*****************************************************************************/

// This file is display.cpp.  It includes the skeleton for the display routine

// Basic includes to get this file to work.  
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <deque>
#include <stack>
#include <GL/glut.h>
#include "Transform.h"
#include <vector>

using namespace std ; 
#include "variables.h"
#include "readfile.h"

// New helper transformation function to transform vector by modelview 
// May be better done using newer glm functionality.
// Provided for your convenience.  Use is optional.  
// Some of you may want to use the more modern routines in readfile.cpp 
// that can also be used.  

void transformvec (const GLfloat input[4], GLfloat output[4]) {
  GLfloat modelview[16] ; // in column major order
  glGetFloatv(GL_MODELVIEW_MATRIX, modelview);

  for (int i = 0 ; i < 4 ; i++) {
    output[i] = 0 ; 
    for (int j = 0 ; j < 4 ; j++) { 
      output[i] += modelview[4*j+i] * input[j] ;
    }
  }
}

void getScreenLightPos() {
 // Calculate light position on screen
  double projectionMatrix[16], modelViewMatrix[16];
  int view[4];
  glGetDoublev(GL_PROJECTION_MATRIX, projectionMatrix);
  glGetDoublev(GL_MODELVIEW_MATRIX, modelViewMatrix);
  glGetIntegerv(GL_VIEWPORT, view);
  GLdouble lightcoords[3];
  gluProject((double)lightposn[0], (double)lightposn[1], (double)lightposn[2], modelViewMatrix, projectionMatrix, view, &lightcoords[0], &lightcoords[1], &lightcoords[2]);
  screenLightX = lightcoords[0]/view[2];
  screenLightY = lightcoords[1]/view[3];
}


void drawOcclusionMap() {
  glClearColor(.2, .2, .2, 1);
  glGenerateMipmapEXT(GL_TEXTURE_2D);
  //glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, occlusionFramebuffer);
  glUseProgram(0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0, 0, w/2, h/2);

  glMatrixMode(GL_PROJECTION);
  float aspect = w / (float) h, zNear = 0.1, zFar = 99.0 ;
  mat4 mv = Transform::perspective(fovy,aspect,zNear,zFar) ;
  mv = glm::transpose(mv); // accounting for row major 
  glLoadMatrixf(&mv[0][0]);

  glMatrixMode(GL_MODELVIEW) ;
  mv = Transform::lookAt(eye,center,up) ;
  mv = glm::transpose(mv); // accounting for row major
  glLoadMatrixf(&mv[0][0]);
  for (int i = 0; i < numused; i++) {
    const GLfloat _light[] = {lightposn[4*i], lightposn[4*i+1], lightposn[4*i+2], lightposn[4*i+3]};
    GLfloat light[4];
    transformvec(_light, light);
    const GLfloat _color[] = {lightcolor[4*i], lightcolor[4*i+1], lightcolor[4*i+2], lightcolor[4*i+3]};
  /*
    glColor3fv(&_color[0]);
    glBegin(GL_POINTS);
    glVertex3fv(light);
    glEnd();
  */
  }

  // Transformations for objects, involving translation and scaling
  mat4 sc(1.0) , tr(1.0), transf(1.0) ; 
  sc = Transform::scale(sx,sy,1.0) ;
  tr = Transform::translate(tx,ty,0.0) ; 
  glLoadMatrixf(&transf[0][0]) ; 
  transf = glm::transpose(mv) * transf;	
  transf = tr * transf;
  transf = sc * transf;

  glColor3d(0, 0, 0);
  for (int i = 0 ; i < numobjects ; i++) {
    object * obj = &(objects[i]) ;
    mat4 transform = obj -> transform;
    glLoadMatrixf(&glm::transpose(transform * transf)[0][0]);
    // Actually draw the object
    // We provide the actual glut drawing functions for you.  
    if (obj -> type == cube) {
      glutSolidCube(obj->size) ; 
    }
    else if (obj -> type == sphere) {
      const int tessel = 20 ; 
      glutSolidSphere(obj->size, tessel, tessel) ; 
    }
    else if (obj -> type == teapot) {
      glutSolidTeapot(obj->size) ;
    }
  }
  getScreenLightPos();
}

void drawSceneRender() {
  glGenerateMipmapEXT(GL_TEXTURE_2D);
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
  //glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, sceneRenderFramebuffer);
  glUseProgram(shaderprogram);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  mat4 mv = Transform::lookAt(eye,center,up) ;
  mv = glm::transpose(mv) ; // accounting for row major
  glLoadMatrixf(&mv[0][0]) ;
  for (int i = 0; i < numused; i++) {
    const GLfloat _light[] = {lightposn[4*i], lightposn[4*i+1], lightposn[4*i+2], lightposn[4*i+3]};
    GLfloat light[4];
    transformvec(_light, light);
    GLfloat color[] = {lightcolor[4*i], lightcolor[4*i+1], lightcolor[4*i+2], lightcolor[4*i+3]};
    glUniform4fv(lightpos+i, 1, light);
    glUniform4fv(lightcol+i, 1, color);
  }
  glUniform1i(numusedcol, numused);
  glUniform1i(enablelighting, true);

  // Transformations for objects, involving translation and scaling
  mat4 sc(1.0) , tr(1.0), transf(1.0) ; 
  sc = Transform::scale(sx,sy,1.0) ;
  tr = Transform::translate(tx,ty,0.0) ; 
  glLoadMatrixf(&transf[0][0]) ; 
  transf = glm::transpose(mv) * transf;	
  transf = tr * transf;
  transf = sc * transf;

  for (int i = 0 ; i < numobjects ; i++) {
    object * obj = &(objects[i]) ;
    mat4 transform = obj -> transform;
    glLoadMatrixf(&glm::transpose(transform * transf)[0][0]);
    glUniform4fv(ambientcol, 1, obj->ambient);
    glUniform4fv(diffusecol, 1, obj->diffuse);
    glUniform4fv(specularcol, 1, obj->specular);
    glUniform4fv(emissioncol, 1, obj->emission);
    glUniform1f(shininesscol, obj->shininess);
    // Actually draw the object
    // We provide the actual glut drawing functions for you.  
    if (obj -> type == cube) {
      glutSolidCube(obj->size) ; 
    }
    else if (obj -> type == sphere) {
      const int tessel = 20 ; 
      glutSolidSphere(obj->size, tessel, tessel) ; 
    }
    else if (obj -> type == teapot) {
      glutSolidTeapot(obj->size) ;
    }
  }
}

void drawToScreen() {
  glGenerateMipmapEXT(GL_TEXTURE_2D);
  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
  glUseProgram(godrayshaderprogram);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, 1, 0, 1, -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glClear(GL_DEPTH_BUFFER_BIT);  
  
  glUniform2f(screenLightPos, screenLightX, screenLightY);
  
  // Send occlusion map
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, occlusionMap);
  glUniform1i(occlusionMapLoc, 0);
  
  // Send scene render
  /*
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, sceneRender);
  glUniform1i(sceneRenderLoc, 1);
  */
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);

  glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex2f(0, 0);
    glTexCoord2f(1, 0);
    glVertex2f(1, 0);
    glTexCoord2f(1, 1);
    glVertex2f(1, 1);
    glTexCoord2f(0, 1);
    glVertex2f(0, 1);
  glEnd();

  glDisable(GL_BLEND);
  glDisable(GL_TEXTURE_2D);
}

void display() {
	glClearColor(1, 1, 1, 1);
        drawOcclusionMap();
	glViewport(0, 0, w, h);	
	drawSceneRender();
	drawToScreen();
        glutSwapBuffers();
}
