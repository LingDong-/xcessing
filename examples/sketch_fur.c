#include "../xcessing/core.h"
#include "../xcessing/random.h"
#include "../xcessing/util.h"

XGraphics gfx;

void setup(){
    size(600,600);

    gfx = createGraphics(width,height);
    beginDraw(&gfx);
    noStroke();
    fill(50,0,50);
    rect(0,0,width,height);
    noFill();

    for (int i = 0; i < 6000; i++){
        float x = rand()%width;
        float y = rand()%height;
        stroke(rand()%128+128,rand()%255,rand()%50);

        beginShape();
        for (int j = 0; j < 10; j++){
            float a = noise(x*0.01,y*0.01,1)*2*PI-PI;
            float r = noise(x*0.01,y*0.01,2)*10;
            float dx = cos(a)*r;
            float dy = sin(a)*r;
            
            vertex(x,y);
            x += dx*2;
            y += dy*2;
        }
        endShape();
    }
    endDraw();
}

void draw(){
    image(&gfx,0,0,width,height);
}

void stop(){
    destroyGraphics(&gfx);
}

void mousePressed(){}
void mouseReleased(){}
void mouseMoved(){}
void mouseDragged(){}
void keyPressed(){}



