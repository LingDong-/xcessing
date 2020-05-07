// https://processing.org/examples/reach2.html

#include "../xcessing/core.h"

#define numSegments 15

float x[numSegments];
float y[numSegments];
float angle[numSegments];
float segLength = 26;
float targetX, targetY;

void setup(){
    size(640,360);
    strokeWeight(20.0);
    stroke(100,100,100);
    x[numSegments-1] = width/2;
    y[numSegments-1] = height;

}

void positionSegment(int a, int b) {
  x[b] = x[a] + cos(angle[a]) * segLength;
  y[b] = y[a] + sin(angle[a]) * segLength; 
}

void reachSegment(int i, float xin, float yin) {
  float dx = xin - x[i];
  float dy = yin - y[i];
  angle[i] = atan2(dy, dx);  
  targetX = xin - cos(angle[i]) * segLength;
  targetY = yin - sin(angle[i]) * segLength;
}

void segment(float x, float y, float a, float sw) {
  strokeWeight(sw);
  pushMatrix();
  translate(x, y);
  rotate(a);
  line(0, 0, segLength, 0);
  popMatrix();
}

void draw() {
  background(0,0,0);
  
  reachSegment(0, mouseX, mouseY);
  for(int i=1; i<numSegments; i++) {
    reachSegment(i, targetX, targetY);
  }
  for(int i=numSegments-1; i>=1; i--) {
    positionSegment(i, i-1);  
  } 
  for(int i=0; i<numSegments; i++) {
    segment(x[i], y[i], angle[i], (i+1)*2); 
  }
}

void mousePressed(){}
void mouseReleased(){}
void mouseMoved(){}
void mouseDragged(){}
void keyPressed(){}
void stop(){}


