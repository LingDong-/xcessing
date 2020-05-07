//https://processing.org/examples/shapeprimitives.html

#include "../xcessing/core.h"

void setup(){
    size(640, 360);
    noStroke();

}

void draw(){
	background(0,0,0);
    fill(204,204,204);
    triangle(18, 18, 18, 360, 81, 360);
    
    fill(102,102,102);
    rect(81, 81, 63, 63);
    
    fill(204,204,204);
    quad(189, 18, 216, 18, 216, 360, 144, 360);
    
    fill(255,255,255);
    ellipse(252, 144, 72, 72);
    
    fill(204,204,204);
    triangle(288, 18, 351, 360, 288, 360); 
    
    fill(255,255,255);
    arc(479, 300, 280, 280, PI, TWO_PI, OPEN);
}

void mousePressed(){}
void mouseReleased(){}
void mouseMoved(){}
void mouseDragged(){}
void keyPressed(){}
void stop(){}


