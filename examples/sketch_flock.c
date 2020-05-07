// https://processing.org/examples/flocking.html

#include "../xcessing/core.h"
#include "../xcessing/list.h"
#include "../xcessing/util.h"

// The Boid class
typedef struct _boid_t {
  Vec2f position;
  Vec2f velocity;
  Vec2f acceleration;
  float r;
  float maxforce;  // Maximum steering force
  float maxspeed;  // Maximum speed
} Boid;

Boid newBoid(float x, float y){
  Boid b;
  b.acceleration = vnew(0, 0);

  float angle = radians((float)(rand()%360));
  b.velocity = vfromAngle(angle);

  b.position = vnew(x,y);
  b.r = 2.0;
  b.maxspeed = 2;
  b.maxforce = 0.03;

  return b;
}

// Separation
// Method checks for nearby boids and steers away
Vec2f separate(Boid* b, List boids){
  float desiredseparation = 25.0f;
  Vec2f steer = {0, 0};
  int count = 0;
  // For every boid in the system, check if it's too close
  for (int i = 0; i < boids->length; i++) {
    Boid* other = (Boid*)listGet(boids,i);
    float d = vdist(b->position, other->position);
    // If the distance is greater than 0 and less than an arbitrary amount (0 when you are yourself)
    if ((d > 0) && (d < desiredseparation)) {
      // Calculate vector pointing away from neighbor
      Vec2f diff = vdiv(vnormalize(vsub(b->position, other->position)),d);
      steer = vadd(steer,diff);
      count++;            // Keep track of how many
    }
  }
  // Average -- divide by how many
  if (count > 0) {
    steer = vdiv(steer, (float)count);
  }

  // As long as the vector is greater than 0
  if (vmag(steer) > 0) {
    // Implement Reynolds: Steering = Desired - Velocity
    steer = vlimit(vsub(vsetMag(steer,b->maxspeed),b->velocity),b->maxforce);
  }
  return steer;
}

// Alignment
// For every nearby boid in the system, calculate the average velocity
Vec2f align (Boid* b, List boids) {
  float neighbordist = 50;
  Vec2f sum = vnew(0, 0);
  int count = 0;
  for (int i = 0; i < boids->length; i++) {
    Boid* other = (Boid*)listGet(boids,i);
    float d = vdist(b->position, other->position);
    if ((d > 0) && (d < neighbordist)) {
      sum = vadd(sum,other->velocity);
      count++;
    }
  }
  if (count > 0) {
    sum = vdiv(sum,(float)count);
    // Implement Reynolds: Steering = Desired - Velocity
    Vec2f steer = vlimit(vsub(vsetMag(sum,b->maxspeed),b->velocity),b->maxforce);
    return steer;
  } 
  else {
    return vnew(0, 0);
  }
}

// A method that calculates and applies a steering force towards a target
// STEER = DESIRED MINUS VELOCITY
Vec2f seek(Boid* b, Vec2f target) {
  Vec2f desired = vsub(target, b->position);  // A vector pointing from the position to the target
  // Scale to maximum speed
  desired = vsetMag(desired,b->maxspeed);

  // Steering = Desired minus Velocity
  Vec2f steer = vsub(desired, b->velocity);
  steer = vlimit(steer,b->maxforce);  // Limit to maximum steering force
  return steer;
}


// Cohesion
// For the average position (i.e. center) of all nearby boids, calculate steering vector towards that position
Vec2f cohesion (Boid* b, List boids) {
  float neighbordist = 50;
  Vec2f sum = vnew(0, 0);   // Start with empty vector to accumulate all positions
  int count = 0;
  for (int i = 0; i < boids->length; i++) {
    Boid* other = (Boid*)listGet(boids,i);
    float d = vdist(b->position, other->position);
    if ((d > 0) && (d < neighbordist)) {
      sum = vadd(sum,other->position); // Add position
      count++;
    }
  }
  if (count > 0) {
    sum = vdiv(sum,(float)count);
    return seek(b,sum);  // Steer towards the position
  } 
  else {
    return vnew(0, 0);
  }
}

void renderBoid(Boid* b) {
  // Draw a triangle rotated in the direction of velocity
  float theta = vheading(b->velocity) + radians(90);

  fill(150,150,150);
  stroke(255,255,255);
  pushMatrix();
  translate(b->position.x, b->position.y);
  rotate(theta);
  beginShape();
  vertex(0, -b->r*2);
  vertex(-b->r, b->r*2);
  vertex(b->r, b->r*2);
  vertex(0, -b->r*2);
  endShape();
  popMatrix();
}

// Wraparound
void bordersBoid(Boid* b) {
  if (b->position.x < -b->r) b->position.x = width+b->r;
  if (b->position.y < -b->r) b->position.y = height+b->r;
  if (b->position.x > width+b->r) b->position.x = -b->r;
  if (b->position.y > height+b->r) b->position.y = -b->r;
}


void applyForce(Boid* b, Vec2f force) {
  b->acceleration = vadd(b->acceleration,force);
}

// We accumulate a new acceleration each time based on three rules
void flockBoid(Boid* b, List boids) {   // flock(2) is taken up by osx
  Vec2f sep = separate(b,boids);   // Separation
  Vec2f ali = align(b,boids);      // Alignment
  Vec2f coh = cohesion(b,boids);   // Cohesion

  // Arbitrarily weight these forces
  sep = vscale(sep,1.5);
  ali = vscale(ali,1.0);
  coh = vscale(coh,1.0);
  // Add the force vectors to acceleration
  applyForce(b,sep);
  applyForce(b,ali);
  applyForce(b,coh);
}

// Method to update position
void updateBoid(Boid* b) {
  // Update velocity
  b->velocity = vadd(b->velocity,b->acceleration);
  // Limit speed
  b->velocity = vlimit(b->velocity, b->maxspeed);
  b->position = vadd(b->position,b->velocity);
  // Reset accelertion to 0 each cycle
  b->acceleration = vnew(0,0);
}

void runBoid(Boid* b, List boids) {
  flockBoid(b,boids);
  updateBoid(b);
  bordersBoid(b);
  renderBoid(b);
}


List boids;

void setup(){
  srand(time(0)); // seed the random

  size(640, 360);

  boids = newList(Boid);

  // Add an initial set of boids into the system
  for (int i = 0; i < 150; i++){
    Boid b = newBoid(width/2,height/2);
    listAdd(boids,&b);
  }
}

void draw(){
  background(50,50,50);
  for (int i = 0; i < boids->length; i++){
    Boid* b = listGet(boids,i);
    runBoid(b,boids); // Passing the entire list of boids to each boid individually
  }
}

// Add a new boid into the System
void mousePressed(){
  Boid b = newBoid(mouseX,mouseY);
  listAdd(boids,&b);
}

void stop(){
  destroyList(boids);
}
void mouseReleased(){}
void mouseMoved(){}
void mouseDragged(){}
void keyPressed(){}

