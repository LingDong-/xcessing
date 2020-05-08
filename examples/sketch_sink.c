#include "../xcessing/core.h"
#include "../xcessing/random.h"
#include "../xcessing/image.h"
#include <stdio.h>

XGraphics gfx;
XFont font;

void setup(){
    size(500,500);

    gfx = createGraphics(width,height);
    font = createFont("helvetica",  "medium", 'r', 25 );

    setTitle("Kitchen Sink");
    setLocation(20,20,width,height);
    cursor(HAND);

}

void draw(){

    background(200,200,200);
    
    fill(255,255,255);
    stroke(255,0,0);
    strokeWeight(3);
    strokeJoin(ROUND);
    rect(width/2,height/2,200,200);

    noStroke();
    fill(0,0,255);

    beginDraw(&gfx);
    ellipse(mouseX,mouseY,10,10);
    endDraw();
    image(&gfx,0,0,gfx.width,gfx.height);

    fill(255,0,0);
    rect(width/2-50,height/2-50,100,100);

    stroke(0,0,0);
    fill(255,255,255);
    strokeWeight(1);

    for (int i = 10; i < 30; i++){
        ellipse(i*10,100,200,100);
    }

    fill(0,0,0);
    textFont(font);
    text("Hello world",mouseX,mouseY);

    fill(0,255,0);

    pushMatrix();
    translate(0,100);
    rotate(fmod(0.01*frameCount,PI*2));
    translate(200,200);
    beginShape();
    vertex(0,0);
    vertex(0,100);
    vertex(100,100);
    vertex(100,80);
    vertex(0,0);
    endShape();
    popMatrix();

    pushMatrix();
    translate(300,300);
    rotate(fmod(0.02*frameCount,PI*2));
    quad(-10,-10,10,-10,10,10,-10,10);
    popMatrix();

    pushMatrix();
    shearX(QUARTER_PI);
    rect(0,0,100,100);
    popMatrix();

    beginShape();
    vertex(0,0);
    bezierVertex(100,50,200,20,300,100);
    endShape();

    bezier(0,0,100,50,200,20,300,100);
    point(300,300);


    for (int y = 0; y < 100; y++) {
      float x = randomGaussian() * 15;
      line(50, y, 50 + x, y);
    }

    for (int y = 0; y < 30; y++) {
      float x = 10*noise(frameCount*0.01,y*0.01,0) * 15;
      triangle(50, y*8+100, 50 + x, y*8+100, 50, y*8+108);
    }

}
void mousePressed(){
    clear(&gfx);
    printf("mouse pressed!\n");
}

void keyPressed(){
    char buf[8];
    sprintf(buf,"%c(0x%x)",keyCode,keyCode);
    fill(0,0,255);
    text(buf,0,400);
    printf("%c\n",keyCode);
}

void stop() {
    destroyGraphics(&gfx);
    destroyFont(font);
} 

void mouseReleased(){}
void mouseMoved(){}
void mouseDragged(){}


