#include "../xcessing/core.h"
#include <limits.h>

XFont font1;
XFont font2;
XFont font3;
XFont font4;
XFont font5;
XFont font6;

// advanced: loop over all available fonts and display them
XFont font = 0;
int num_fonts;
char** font_names;
int font_idx = 0;
int handler(Display * d, XErrorEvent * e){return 0;} // silence font not found error

void setup(){
    size(700,400);

                       //family      weight  style size
    font1 = createFont("helvetica",  "medium", 'r', 24 );
    font2 = createFont("helvetica",  "bold",   'r', 18 );
    font3 = createFont("helvetica",  "medium", 'o', 24 );
    font4 = createFont("helvetica",  "medium", 'r', 8  );
    font5 = createFont("times",      "medium", 'r', 24 );
    font6 = createFont("courier",    "medium", 'r', 24 );

    XSetErrorHandler(handler); // silence font not found error
    font_names = XListFonts(dis,"*",INT_MAX,&num_fonts); // use raw X11 to list the fonts
}

void draw(){

    background(50,50,50);

    fill(255,0,0);
    textFont(font1);
    text("Helvetica 24",10,100);

    fill(0,255,0);
    textFont(font2);
    text("Helvetica Bold 18",210,100);

    fill(0,0,255);
    textFont(font3);
    text("Helvetica Oblique 24",410,100);

    fill(255,255,0);
    textFont(font4);
    text("Helvetica 8",10,200);

    fill(255,0,255);
    textFont(font5);
    text("Times 24",210,200);

    fill(0,255,255);
    textFont(font6);
    text("Courier 24",410,200);

    fill(255,255,255);
    // load next available font and draw it
    font = XLoadFont(dis,font_names[font_idx]);
    textFont(font);
    text(font_names[font_idx],10,300);
    font_idx = (font_idx + 1)%num_fonts;
    destroyFont(font);

}
void stop(){
    destroyFont(font1);
    destroyFont(font2);
    destroyFont(font3);
    destroyFont(font4);
    destroyFont(font5);
    destroyFont(font6);
}

void keyPressed(){}
void mousePressed(){}
void mouseReleased(){}
void mouseMoved(){}
void mouseDragged(){}



