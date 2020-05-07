#include "../xcessing/core.h"
#include "../xcessing/image.h"
#include "../xcessing/util.h"

XGraphics img;

int* pixels0; // original copy

void setup(){
    size(512,512);
    img = loadImage("data/peppers.png");

    pixels0 = malloc(sizeof(int)*img.width*img.height);
    
    loadPixels(&img);
    memcpy(pixels0,pixels,sizeof(int)*img.width*img.height);
}

void draw(){
    // wavy effect controlled by mouse
    for (int i = 1; i < img.height-1; i++){
        int d = (int)((sin((i+frameCount)*0.1)+1)*(mouseX/8));
        memcpy(pixels+i*img.width,pixels0+(i*img.width+d),img.width*sizeof(int));
    }
    updatePixels(&img);

    image(&img,0,0,img.width,img.height);
}

void stop(){
    free(pixels0);
    free(pixels);
    destroyGraphics(&img);   
}

void mousePressed(){}
void mouseReleased(){}
void mouseMoved(){}
void mouseDragged(){}
void keyPressed(){}



