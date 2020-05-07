#include "../xcessing/core.h"
#include "../xcessing/util.h"

#define SQRT3 1.73205


void setup() {
  size(300,(int)(300/2*SQRT3));
}

void sierp(Vec2f p0, Vec2f p1, Vec2f p2) {
  
  if (vdist(p0,p1)<5){
    triangle(p0.x, p0.y, p1.x, p1.y, p2.x, p2.y);
    return;
  }

  Vec2f p01 = vlerp(p0,p1,0.5);
  Vec2f p12 = vlerp(p1,p2,0.5);
  Vec2f p20 = vlerp(p2,p0,0.5);
  
  sierp(p0,p01,p20);
  sierp(p1,p12,p01);
  sierp(p2,p20,p12);

}

void draw() {
  background(0,0,0);
  noFill();
  stroke(255,255,0);

  int d = frameCount%(width/2);
  sierp(
    vnew(width/2,0),
    vnew(width+d, height+d*SQRT3),
    vnew(     -d, height+d*SQRT3));


}

void mousePressed(){}
void mouseReleased(){}
void mouseMoved(){}
void mouseDragged(){}
void keyPressed(){}
void stop(){}
