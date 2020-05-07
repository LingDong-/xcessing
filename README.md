# xcessing

*Friendly Processing-like interface to X11/Xlib in C*

![](https://user-images.githubusercontent.com/7929704/81260683-81cc2800-9008-11ea-8def-d193ca104fb6.png)

Sometimes it's nice to add some visualization to the crazy thing you just coded up in C, but getting a GUI/CG dependency and compile all that seems like an overkill. X11 is readily available on most systems, can easily be forwarded through SSH, but the convoluted API's often get in the way when all you want is to get some stuff drawn onto the screen.

Xcessing is a header-only thin wrapper on top of X11 that makes the experience much less painful. It is inspired by Processing, and most of the API's are simply identical. Sometimes you can even copy paste a simple processing program and expect it to compile with minimal modification.

Xcessing might also be a helpful toolkit if you just started learning C.

To get an idea, here's xcessing version of the ["Getting Started"](https://processing.org/tutorials/gettingstarted/) example on the processing website: (use mouse to draw circles)

```c
#include "xcessing/core.h"

void setup() {
  size(480, 120);
}

void draw() {
  if (mouseIsPressed) {
    fill(0,0,0);
  } else {
    fill(255,255,255);
  }
  ellipse(mouseX, mouseY, 80, 80);
}

void mousePressed(){}
void mouseReleased(){}
void mouseMoved(){}
void mouseDragged(){}
void keyPressed(){}
void stop(){}

```

Compare it to what you would've written if you're just using X11.

```c
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdlib.h>
Display *dis; int screen; Window win; GC gc;
int main () {
    XEvent event;   
    KeySym key;
    char text[255]; 
    int mouseDown = 0;
    dis=XOpenDisplay((char *)0);
    screen=DefaultScreen(dis);
    win=XCreateSimpleWindow(dis,DefaultRootWindow(dis),0,0, 480, 120, 5,0,0x7f7f7f);
    XSelectInput(dis, win, ExposureMask|ButtonPressMask|ButtonReleaseMask|KeyPressMask|PointerMotionMask);
    gc=XCreateGC(dis, win, 0,0);
    XMapRaised(dis, win);
    while(1) {      
        XNextEvent(dis, &event);
        if (event.type==Expose && event.xexpose.count==0) {
            XClearWindow(dis, win);
        }else if (event.type==KeyPress&&XLookupString(&event.xkey,text,255,&key,0)==1) {
            if (text[0]==0x1B) { XFreeGC(dis, gc); XDestroyWindow(dis,win); XCloseDisplay(dis); exit(1); }
        }else if (event.type==ButtonPress) {   mouseDown = 1;
        }else if (event.type==ButtonRelease) { mouseDown = 0;
        }else if (event.type==MotionNotify) {
            int x=event.xbutton.x,
                y=event.xbutton.y;
            XSetForeground(dis,gc,mouseDown?0:0xFFFFFF);
            XFillArc(dis,win, gc, x-40, y-40, 80, 80, 0, 360*64);
            XSetForeground(dis,gc,0);
            XDrawArc(dis,win, gc, x-40, y-40, 80, 80, 0, 360*64);
        }
    }
}
```

To see more code samples, please check out the `./examples` folder!

## Features

Besides wrapping what X11 can do, xcessing additionally implemented the following to make your life even easier:

- Transformation matrix stack. `translate()`, `rotate()`, `shearX()`, `shearY()` can be used to transform your drawing, and `pushMatrix()` and `popMatrix()` to save and restore states.
- `beginShape()`, `vertex()/bezierVertex()` and `endShape()` can be used draw complicated polygons, in place of `XFillPolygon` which involves allocating and populating arrays and passing pointers etc.
- See **API Listing** section for a full list of functions you can use.

Core features directly related to making 2D drawings are in `xcessing/core.h`, while other
popular features from Processing that are not directly related to drawing can optionally be `#include`'d:

| file | description |
|---|---|
| `xcessing/image.h` | Reading PNG images from disk (compile with `-lpng`). |
| `xcessing/random.h` | Perlin noise and Gaussian randomness. |
| `xcessing/util.h` | Scalar, vector and color math |
| `xcessing/list.h` | A `std::vector`-ish (or `ArrayList`-ish) dynamically resizable array implementation. |

The idea is that while these are useful, by putting them in separate headers we try not to force them on you. If you just want to draw stuff, use `xcessing/core.h`. If you'd like a full Processing experience, `#include` those files too.


## Usage

To use the library, simply drop it in your project folder. Xcessing is a header-only library so no need to struggle with linking and all that.

Then, in the first line of your own program, e.g. `sketch.c`: 

```
#include "xcessing/core.h" // or path to wherever you placed the library
```

To compile, try:

```
gcc sketch.c -I /opt/X11/include -L/opt/X11/lib -lX11 -lm -std=c99
```

You might need to change the `-I` and `-L` paths to where your system installation of X11 is. On a mac, it is usually in the paths listed above, on Linux, you probably wouldn't need those flags at all!

If you're using `xcessing/image.h`, remember to add `-lpng` (for libpng) too.

For convenience, there is a `run.sh` script in `example` folder that compiles and run your program for you:

```
sh run.sh sketch.c
```

## Examples

There is a comprehensive list of examples in the `example` folder, check them out! Many of them are directly taken from the Processing website (see the link in code commnets). To run an example, `cd` to `example` folder, and do:

```
sh run.sh sketch_flock.c
```

## X11 Limitations

Here are some flaws built into X11 that seem difficult to circumvent:

- X11 doesn't have anti-aliasing.
- Text glyphs cannot be rotated or sheared.
- 1-bit alpha only. From X11 API's it seems possible to set 8-bit alpha channel, but at least on my system it just crashes if I try to do that. Xcessing currently simulates 1-bit alpha internally with a clip-mask.
- Conversion from Pixmap to XImage (`XGetImage`) seems to be unavailable (at least on my system).
- X11 drawing is async, and seems to lack a reliable way of controlling framerate.

These could be problematic if you're creating a massive beautiful interactive software art, but hopefully you're aware that that's not X11 is for, and just use OpenGL/OpenFrameworks instead.

That said, since Xcessing doesn't prevent you from using X11 directly, if you happen to know how to get any of the above to work, you can just do it by calling X11 functions directly. (and kindly submit a PR if possible :).


## API Listing

Below are Processing functionalities that already are or potentially will be implemented for Xcessing. 3D, File IO, or any functionailty already included by C standard libraries are not included in the table.

- `√` means implemented in `xcessing/core.h`.
- `*.h` means implemented there.
- Empty cell means not implemented. While the goal is to minimize these, some functions might never be implemented due to the way X11 works.

| Functionality     | Impl?  |     
|-------------------|--------|     
| draw()            |  √     |      
| exit()            |  √     |      
| loop()            |        |      
| noLoop()          |        |      
| pop()             |        |      
| popStyle()        |        |      
| push()            |        |      
| pushStyle()       |        |      
| redraw()          |        |      
| setLocation()     |  √     |      
| setResizable()    |        |      
| setTitle()        |  √     |      
| setup()           |  √     |      
| thread()          |        |      
| cursor()          |  √     |      
| focused           |        |      
| frameCount        |  √     |      
| frameRate()       |        |      
| frameRate         |        |      
| fullScreen()      |        |      
| height            |  √     |      
| noCursor()        |        |      
| size()            |  √     |       
| ArrayList         | list.h |            
| arc()             |  √     |      
| circle()          |  √     |      
| ellipse()         |  √     |      
| line()            |  √     |      
| point()           |  √     |      
| quad()            |  √     |      
| rect()            |  √     |      
| square()          |  √     |      
| triangle()        |  √     |      
| bezier()          |  √     |      
| bezierDetail()    |  √     |      
| bezierPoint()     |        |      
| bezierTangent()   |        |      
| curve()           |        |      
| curveDetail()     |        |      
| curvePoint()      |        |      
| curveTangent()    |        |      
| curveTightness()  |        |      
| ellipseMode()     |        |      
| rectMode()        |        |      
| strokeCap()       |  √     |      
| strokeJoin()      |  √     |      
| strokeWeight()    |  √     |      
| beginContour()    |        |      
| beginShape()      |  √     |      
| bezierVertex()    |  √     |      
| curveVertex()     |        |      
| endContour()      |        |      
| endShape()        |  √     |      
| quadraticVertex() |        |      
| vertex()          |  √     |          
| mouseButton       |  √     |      
| mouseClicked()    |        |      
| mouseDragged()    |  √     |      
| mouseMoved()      |        |      
| mousePressed()    |  √     |      
| mousePressed      |        |      
| mouseReleased()   |  √     |      
| mouseWheel()      |        |      
| mouseX            |  √     |      
| mouseY            |  √     |      
| pmouseX           |  √     |      
| pmouseY           |  √     |      
| key               |        |      
| keyCode           |  √     |      
| keyPressed()      |  √     |      
| keyPressed        |        |      
| keyReleased()     |        |      
| keyTyped()        |        |      
| save()            |        |      
| saveFrame()       |        |      
| applyMatrix()     |  √     |      
| popMatrix()       |  √     |      
| printMatrix()     |        |      
| pushMatrix()      |  √     |      
| resetMatrix()     |        |      
| rotate()          |  √     |          
| scale()           |  √     |      
| shearX()          |  √     |      
| shearY()          |  √     |      
| translate()       |  √     |      
| background()      |  √     |      
| clear()           |  √     |      
| colorMode()       |        |      
| fill()            |  √     |      
| noFill()          |  √     |      
| noStroke()        |  √     |      
| stroke()          |  √     |      
| alpha()           |        |      
| blue()            | util.h |      
| brightness()      |        |      
| color()           |  √     |      
| green()           | util.h |      
| hue()             |        |      
| lerpColor()       |        |      
| red()             | util.h |      
| saturation()      |        |      
| createImage()     |  √     |      
| PImage            |        |      
| image()           |        |      
| imageMode()       |        |      
| loadImage()       |image.h |      
| noTint()          |        |      
| requestImage()    |        |      
| tint()            |        |      
| loadPixels()      |  √     |      
| pixels[]          |  √     |      
| set()             |        |      
| updatePixels()    |  √     |      
| blendMode()       |        |      
| clip()            |        |      
| createGraphics()  |  √     |           
| noClip()          |        |      
| PGraphics         |  √     |      
| createFont()      |  √     |      
| loadFont()        |        |      
| text()            |  √     |      
| textFont()        |  √     |      
| textAlign()       |        |      
| textLeading()     |        |      
| textMode()        |        |      
| textSize()        |        |      
| textWidth()       |        |      
| textAscent()      |        |      
| textDescent()     |        |  
| PVector           | util.h |    
| constrain()       | util.h |   
| dist()            | util.h |      
| lerp()            | util.h |  
| map()             | util.h |      
| degrees()         | util.h |      
| radians()         | util.h |           
| noise()           |random.h|      
| noiseDetail()     |random.h|      
| noiseSeed()       |random.h|     
| randomGaussian()  |random.h|           
| HALF_PI           |  √     |      
| PI                |  √     |      
| QUARTER_PI        |  √     |      
| TAU               |  √     |      
| TWO_PI            |  √     |     