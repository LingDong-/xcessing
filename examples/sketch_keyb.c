//https://processing.org/examples/keyboardfunctions.html

#include "../xcessing/core.h"
#include "../xcessing/util.h"

#define maxHeight 40
#define minHeight 20
#define letterWidth 20          // Width of the letter

int letterHeight = maxHeight; // Height of the letters
int x = -letterWidth;          // X position of the letters
int y = 0;                      // Y position of the letters

int newletter;              

#define numChars 26      // There are 26 characters in the alphabet
int colors[numChars];

void setup() {
  size(640, 360);
  noStroke();
  // Set a hue value for each key
  for(int i = 0; i < numChars; i++) {
    colors[i] = hsb(((float)i/(float)numChars)*360, 1.0,1.0);    
  }
}

void draw() {
  if(newletter == 1) {
    // Draw the "letter"
    int y_pos;
    if (letterHeight == maxHeight) {
      y_pos = y;
      rect( x, y_pos, letterWidth, letterHeight );
    } else {
      y_pos = y + minHeight;
      rect( x, y_pos, letterWidth, letterHeight );
      fill(128,128,128);
      rect( x, y_pos-minHeight, letterWidth, letterHeight );
    }
    newletter = 0;
  }
}

void keyPressed()
{
  // If the key is between 'A'(65) to 'Z' and 'a' to 'z'(122)
  if((keyCode >= 'A' && keyCode <= 'Z') || (keyCode >= 'a' && keyCode <= 'z')) {
    int keyIndex;
    if(keyCode <= 'Z') {
      keyIndex = keyCode-'A';
      letterHeight = maxHeight;
      int c = colors[keyIndex];
      fill(red(c),green(c),blue(c));
    } else {
      keyIndex = keyCode-'a';
      letterHeight = minHeight;
      int c = colors[keyIndex];
      fill(red(c),green(c),blue(c));
    }
  } else {
    fill(0,0,0);
    letterHeight = 10;
  }

  newletter = 1;

  // Update the "letter" position
  x = ( x + letterWidth ); 

  // Wrap horizontally
  if (x > width - letterWidth) {
    x = 0;
    y+= maxHeight;
  }

  // Wrap vertically
  if( y > height - letterHeight) {
    y = 0;      // reset y to 0
  }
}


void mousePressed(){}
void mouseReleased(){}
void mouseMoved(){}
void mouseDragged(){}
void stop(){}
