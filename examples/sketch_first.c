// https://processing.org/tutorials/gettingstarted/

#include "../xcessing/core.h"

void setup() {
  size(480, 120);
  noLoop();
}

void draw() {
  if (mouseIsPressed) {
    fill(0,0,0);
  } else {
    fill(255,255,255);
  }
  ellipse(mouseX, mouseY, 80, 80);
}

void mousePressed(){}
void mouseReleased(){}
void mouseMoved(){}
void mouseDragged(){}
void keyPressed(){}
void stop(){}


