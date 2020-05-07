//https://processing.org/reference/noise_.html
//https://processing.org/reference/randomGaussian_.html

#include "../xcessing/core.h"
#include "../xcessing/random.h"

void setup(){
    size(100,100);
}

float noiseScale = 0.02;

void draw() {
  background(0,0,0);
  for (int x=0; x < width; x++) {
    float noiseVal = noise((mouseX+x)*noiseScale, mouseY*noiseScale, 0);
    stroke(noiseVal*255,noiseVal*255,noiseVal*255);
    line(x, mouseY+noiseVal*80, x, height);
  }
  stroke(255,0,0);
  for (int y = 0; y < 100; y++) {
    float x = randomGaussian() * 15;
    line(50, y, 50 + x, y);
  }
}

void stop(){}
void mousePressed(){}
void mouseReleased(){}
void mouseMoved(){}
void mouseDragged(){}
void keyPressed(){}

