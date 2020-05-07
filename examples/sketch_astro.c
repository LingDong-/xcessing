#include "../xcessing/core.h"
#include "../xcessing/random.h"
#include "../xcessing/util.h"

#define N_DOTS 100

float xs[N_DOTS];
float ys[N_DOTS];

void setup(){
    size(600,600);
    for (int i = 0; i < N_DOTS; i++){
        xs[i] = rand()%width;
        ys[i] = rand()%height;
    }
}

void draw(){
    background(10,10,10);

    for (int i = 0; i < N_DOTS; i++){
        for (int j = i+1; j < N_DOTS; j++){
            float d = dist(xs[i],ys[i],xs[j],ys[j]);
            if (d < 80){
                stroke(255-d*3,255-d*3,255-d*3);
                line(xs[i],ys[i],xs[j],ys[j]);
            }
        }

        noStroke();
        fill(255,255,255);
        ellipse(xs[i],ys[i],4,4);
    }
    xs[0] = mouseX;
    ys[0] = mouseY;
    for (int i = 1; i < N_DOTS; i++){
        xs[i] += noise((float)frameCount*0.01,0,i)-0.5;
        ys[i] += noise((float)frameCount*0.01,1,i)-0.5;
        if (xs[i] < 0)    {xs[i] = width; }
        if (xs[i] > width){xs[i] = 0;     }
        if (ys[i] < 0)    {ys[i] = height;}
        if (ys[i] >height){ys[i] = 0;     }
    }
}

void mousePressed(){}
void mouseReleased(){}
void mouseMoved(){}
void mouseDragged(){}
void keyPressed(){}
void stop(){}


