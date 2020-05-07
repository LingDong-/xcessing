// https://processing.org/examples/arraylistclass.html

#include "../xcessing/core.h"
#include "../xcessing/list.h"

typedef struct _ball_t {
  float x;
  float y;
  float speed;
  float gravity;
  float w;
  float life;
} Ball;

Ball newBall(float tempX, float tempY, float tempW){
    Ball b;
    b.x = tempX;
    b.y = tempY;
    b.w = tempW;
    b.speed = 0;
    b.gravity = 0.1;
    b.life = 255;
    return b;
}

void moveBall(Ball* b) {
    // Add gravity to speed
    b->speed = b->speed + b->gravity;
    // Add speed to y location
    b->y = b->y + b->speed;
    // If square reaches the bottom
    // Reverse speed
    if (b->y > height) {
      // Dampening
      b->speed = b->speed * -0.8;
      b->y = height;
    }
}
int finishedBall(Ball* b) {
    // Balls fade out
    b->life--;
    if (b->life < 0) {
      return 1;
    } else {
      return 0;
    }
}
  
void displayBall(Ball* b) {
    // Display the circle
    fill(255-b->life,255-b->life,255-b->life);
    //stroke(0,life);
    ellipse(b->x,b->y,b->w,b->w);
}


List balls;
int ballWidth = 48;


void setup(){
    size(640, 360);
    noStroke();
  
    // Create an empty ArrayList (will store Ball objects)
    balls = newList(Ball);
    
    // Start by adding one element
    Ball b = newBall(width/2, 0, ballWidth);
    listAdd(balls,&b);
}

void draw(){
	background(255,255,255);

	// The length of an List is dynamic
	// Notice how we are looping through the List backwards
	// This is because we are deleting elements from the list  
	for (int i = balls->length-1; i >= 0; i--) { 

	  Ball* ball = listGet(balls,i);
	  moveBall    (ball);
	  displayBall (ball);
	  if (finishedBall(ball)) {
	    // Items can be deleted with listRemove()
	    listRemove(balls,i);
	  }
	} 
}

void mousePressed() {
  // A new ball object is added to the List (by default to the end)
  Ball b = newBall(mouseX, mouseY, ballWidth);
  listAdd(balls,&b);
}

void stop(){
	destroyList(balls);
}

void mouseReleased(){}
void mouseMoved(){}
void mouseDragged(){}
void keyPressed(){}



