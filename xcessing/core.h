/* 
gcc sketch.c -I /opt/X11/include -L/opt/X11/lib -lX11
*/

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <libgen.h>

#define MITER JoinMiter
#define ROUND JoinRound
#define BEVEL JoinBevel
#define SQUARE CapButt
#define PROJECT CapProjecting
#define LEFT 1
#define RIGHT 3
#define ARROW 68
#define CROSS 34
#define HAND  60
#define MOVE  52
#define TEXT  152
#define WAIT  150

#define PI 3.14159265359
#define HALF_PI 1.5707963268
#define TWO_PI 6.2831853072
#define QUARTER_PI 0.7853981634
#define TAU TWO_PI
#define BUFFSIZE 256

#define OPEN  1
#define CHORD 2
#define PIE   3

#define XFont Font

// internal variables
Display *dis;
int screen;
Window win;
GC gc;
long eventMasks = 
     ExposureMask
    |ButtonPressMask
    |ButtonReleaseMask
    |KeyPressMask
    |PointerMotionMask
    |ResizeRedirectMask;

char cwd[BUFFSIZE];
Drawable* draw_ptr;
Drawable* clip_ptr;
GC*       clip_gc_ptr;

XPoint*   vtx_buf = NULL;
int       n_vtx_buf = 0;
int       m_vtx_buf = 0;

int no_loop = 0;
int fill_color = 0xFFFFFF;
int stroke_color = 0;
int stroke_weight = 1;
int polygon_mode = Nonconvex;
int ellipse_detail = 20;
int bezier_detail = 20;
int stroke_cap = CapRound;
int stroke_join = JoinMiter;
int stroke_style = LineSolid;
int get_width(Drawable* d);
int get_height(Drawable* d);

// API variables
int* pixels = NULL;
char keyCode = -1;
int mouseX;
int mouseY;
int pmouseX;
int pmouseY;
int mouseButton;
int mouseIsPressed;
int frameCount;
unsigned int width  = 100;
unsigned int height = 100;

typedef struct _XGraphics {
    Pixmap  pix;
    Pixmap  clip;
    GC      clip_gc;
    XImage* img;
    int     width;
    int     height;
} XGraphics;


// Data structures

typedef struct _XMatrix3x3 {
    float _00; float _01; float _02;
    float _10; float _11; float _12;
    float _20; float _21; float _22;
    struct _XMatrix3x3* prev;
    struct _XMatrix3x3* next;
} XMatrix3x3;

XMatrix3x3* matrix_stack_head = NULL;
XMatrix3x3* matrix_stack_tail = NULL;

void print_matrix3x3(XMatrix3x3* A){
    printf("[[%f\t%f\t%f\t]\n [%f\t%f\t%f\t]\n [%f\t%f\t%f\t]]\n",
        A->_00,A->_01,A->_02,
        A->_10,A->_11,A->_12,
        A->_20,A->_21,A->_22);
}

void apply_matrix3x3(XMatrix3x3* A, float* xp, float* yp){
    float x = *xp;
    float y = *yp;
    float xx = A->_00*x + A->_01*y + A->_02;
    float yy = A->_10*x + A->_11*y + A->_12;
    float zz = A->_20*x + A->_21*y + A->_22;
    *xp = xx/zz;
    *yp = yy/zz;
}

void apply_matrix3x3_stack(float* xp, float* yp){
    XMatrix3x3* it = matrix_stack_tail;
    while (it){
        apply_matrix3x3(it,xp,yp);
        it = it->prev;
    }
}


void matmul3x3(XMatrix3x3* A, XMatrix3x3* B, XMatrix3x3* C){
    float C00 = A->_00 * B->_00 + A->_01 * B->_10 + A->_02 * B->_20;
    float C01 = A->_00 * B->_01 + A->_01 * B->_11 + A->_02 * B->_21;
    float C02 = A->_00 * B->_02 + A->_01 * B->_12 + A->_02 * B->_22;
    float C10 = A->_10 * B->_00 + A->_11 * B->_10 + A->_12 * B->_20;
    float C11 = A->_10 * B->_01 + A->_11 * B->_11 + A->_12 * B->_21;
    float C12 = A->_10 * B->_02 + A->_11 * B->_12 + A->_12 * B->_22;
    float C20 = A->_20 * B->_00 + A->_21 * B->_10 + A->_22 * B->_20;
    float C21 = A->_20 * B->_01 + A->_21 * B->_11 + A->_22 * B->_21;
    float C22 = A->_20 * B->_02 + A->_21 * B->_12 + A->_22 * B->_22;
    C->_00 = C00; C->_01 = C01; C->_02 = C02;
    C->_10 = C10; C->_11 = C11; C->_12 = C12;
    C->_20 = C20; C->_21 = C21; C->_22 = C22;
}


// API functions
void setup();
void draw();
void keyPressed();
void mousePressed();
void mouseReleased();
void mouseMoved();
void mouseDragged();
void stop();

void size(int w, int h){
    width = w;
    height = h;

    win=XCreateSimpleWindow(dis,DefaultRootWindow(dis),0,0, 
        width, height, 0, 0x0, 0x7f7f7f);

    XSetStandardProperties(dis,win,basename(cwd),"",None,NULL,0,NULL);
    XSelectInput(dis, win, eventMasks);
    gc=XCreateGC(dis, win, 0,0);  

    XClearWindow(dis, win);
    XMapRaised(dis, win);
}
void setTitle(char* window_name){
     XSetStandardProperties(dis,win,window_name,"",None,NULL,0,NULL);
}
void setLocation(int x, int y, int w, int h){
     XMoveResizeWindow(dis, win, x, y, w, h);
     width = w;
     height = h;
}
void cursor(unsigned int shape){
    XDefineCursor(dis, win, XCreateFontCursor(dis, shape));
}
XGraphics createGraphics(int w, int h){
    XGraphics g;
    g.pix =    XCreatePixmap(dis, win, w, h, XDefaultDepth(dis, screen));
    g.clip =   XCreatePixmap(dis, win, w, h, 1);
    g.clip_gc =XCreateGC(dis, g.clip, 0,0);
    g.width =  w;
    g.height = h;
    g.img = NULL;
    XSetForeground(dis, g.clip_gc, 0);
    XFillRectangle(dis, g.clip, g.clip_gc, 0, 0, w, h);
    return g;
}
XGraphics createImage(int w, int h){
    XGraphics g;
    g.width =  w;
    g.height = h;
    char* data = malloc(w*h*4);
    g.img = XCreateImage(
        dis, 
        DefaultVisual(dis,screen), 
        DefaultDepth(dis,screen), 
        ZPixmap,
        0,
        data,
        w,
        h,
        32, 0);
    return g;
}

void clear(XGraphics* g){
    XSetForeground(dis, g->clip_gc, 0);
    XFillRectangle(dis, g->clip, g->clip_gc, 0, 0, g->width, g->height);
    
}

void destroyGraphics(XGraphics* g){
    if (g->img){
        XDestroyImage(g->img);
    }else{
        XFreePixmap(dis, g->pix);
        XFreePixmap(dis, g->clip);
        XFreeGC(dis,     g->clip_gc);
    }
}
void beginDraw(XGraphics* g){
    draw_ptr    = &(g->pix);
    clip_ptr    = &(g->clip);
    clip_gc_ptr = &(g->clip_gc);
}
void endDraw(){
    draw_ptr = &win;
    clip_ptr = NULL;
    clip_gc_ptr = NULL;
}
void image(XGraphics* g, int x, int y, int width, int height){
    if (g->img){
        XPutImage(dis,*draw_ptr,gc,g->img,0,0,0,0,width,height);
    }else{
        XSetClipMask(dis, gc, g->clip);
        XCopyArea(dis, g->pix, *draw_ptr, gc, 0, 0, width, height, x, y);
        XSetClipMask(dis, gc, None);
    }
}
int color(int r,int g,int b){
    int r_mask = 0xFF0000;
    int g_mask = 0x00FF00;
    return ((r << 16) & r_mask) | ((g << 8) & g_mask) | b;
}
void fill(int r, int g, int b){
    fill_color = color(r,g,b);
}
void stroke(int r, int g, int b){
    stroke_color = color(r,g,b);
}
void noFill(){
    fill_color = -1;
}
void noStroke(){
    stroke_color = -1;
}
void strokeWeight(int n){
    stroke_weight = n;
}
void strokeCap(int cap){
    if (cap == ROUND){
        stroke_cap = CapRound;
    }else{
        stroke_cap = cap;
    }
}
void strokeJoin(int join){
    stroke_join = join;
}
void loop(){
    no_loop = 0;
}
void noLoop(){
    no_loop = 1;
}
void background(int r, int g, int b){
    // XSetBackground(dis,gc,color(r,g,b));
    // XClearWindow(dis,win);
    // XSetForeground(dis, gc, 0xdd000000UL);
    XSetForeground(dis,gc,color(r,g,b));
    XFillRectangle(dis,*draw_ptr, gc, 0, 0, width, height);
}

XFont createFont(char* family, char* weight, char italic, int size){
    char buf[128];
    sprintf(buf,"*%s-%s-%c-*-%d-*",family,weight,italic,size);
    // printf("\n%s\n",buf);
    return XLoadFont(dis,buf);
}

void destroyFont(XFont font){
    XUnloadFont(dis,font);
}

void printFontList(){
    int actual_count_return;
    char** fonts = XListFonts(dis,"*",99999,&actual_count_return);
    for (int i = 0; i < actual_count_return; i++){
        printf("%s\n",fonts[i]);
    }
}

void textFont(XFont f){
    XSetFont(dis,gc,f);
}

void text(char* str,float x,float y){
    float xx = x;
    float yy = y;
    apply_matrix3x3_stack(&xx,&yy);

    XSetForeground(dis,gc,fill_color);
    XDrawString(dis,*draw_ptr, gc, (int)xx, (int)yy, str, strlen(str));    
}

void polygonMode(int mode){
    polygon_mode = mode;
}

void beginShape(){
    m_vtx_buf = 16;
    n_vtx_buf = 0;
    vtx_buf = (XPoint*)malloc(sizeof(XPoint)*m_vtx_buf);
}

void endShape(){
    if (fill_color != -1){
        XSetForeground(dis,gc,fill_color);
        XFillPolygon(dis,*draw_ptr, gc, vtx_buf, n_vtx_buf, polygon_mode, CoordModeOrigin);
        
        if (clip_ptr != NULL){
            XSetForeground(dis, *clip_gc_ptr, 1);
            XFillPolygon(dis,*clip_ptr, *clip_gc_ptr, vtx_buf, n_vtx_buf, polygon_mode, CoordModeOrigin);
        }
    }
    if (stroke_color != -1){
        XSetLineAttributes(dis, gc, stroke_weight, stroke_style, stroke_cap, stroke_join);
        XSetForeground(dis,gc,stroke_color);
        XDrawLines(dis,*draw_ptr, gc, vtx_buf, n_vtx_buf, CoordModeOrigin);

        if (clip_ptr != NULL){
            XSetLineAttributes(dis, *clip_gc_ptr, stroke_weight, stroke_style, stroke_cap, stroke_join);
            XSetForeground(dis, *clip_gc_ptr, 1);
            XDrawLines(dis,*clip_ptr, *clip_gc_ptr, vtx_buf, n_vtx_buf, CoordModeOrigin);
        }
    }

    m_vtx_buf = 0;
    n_vtx_buf = 0;
    free(vtx_buf);
}

void vertex(float x, float y){
    XPoint p;
    float xx = x;
    float yy = y;
    apply_matrix3x3_stack(&xx,&yy);
    
    p.x = (short)xx;
    p.y = (short)yy;
    if (n_vtx_buf >= m_vtx_buf){
        m_vtx_buf *= 2;
        vtx_buf = realloc(vtx_buf, sizeof(XPoint)*m_vtx_buf);
    }
    vtx_buf[n_vtx_buf] = p;
    n_vtx_buf ++;
}


void bezierDetail(int d){
    bezier_detail = d;
}
void ellipseDetail(int d){
    ellipse_detail = d;
}

void bezierVertex(float x1, float y1, float x2, float y2, float x3, float y3) {
    float x0 = vtx_buf[n_vtx_buf-1].x;
    float y0 = vtx_buf[n_vtx_buf-1].y;
    for (int i = 0; i < bezier_detail+1; i++){
        float t = (float)i/(float)bezier_detail;
        float c1 = 1-t;
        float c2 = c1*c1;
        float c3 = c2*c1;
        float t2 = t*t;
        float t3 = t2*t;
        float x = c3*x0 + 3*c2*t*x1 + 3*c1*t2*x2 + t3*x3;
        float y = c3*y0 + 3*c2*t*y1 + 3*c1*t2*y2 + t3*y3;
        vertex(x,y);
    }
}
void bezier(float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3){
    beginShape();
    vertex(x0,y0);
    bezierVertex(x1,y1,x2,y2,x3,y3);
    endShape();
}

void rect(float x,float y,float w,float h){
    int old_mode = polygon_mode;
    polygon_mode = Convex;
    beginShape();
    vertex(x,y);
    vertex(x+w,y);
    vertex(x+w,y+h);
    vertex(x,y+h);
    vertex(x,y);
    endShape();
    polygon_mode = old_mode;
}

void square(float x, float y, float w){
    rect(x,y,w,w);
}

void point(float x, float y){
    float xx = x;
    float yy = y;
    apply_matrix3x3_stack(&xx,&yy);

    XSetForeground(dis,gc,stroke_color);
    XDrawPoint(dis,*draw_ptr, gc, xx,yy);

    if (clip_ptr != NULL){
        XSetForeground(dis, *clip_gc_ptr, 1);
        XDrawPoint(dis,*clip_ptr, *clip_gc_ptr, xx,yy);
    }
}

void line(float x0,float y0,float x1,float y1){
    float xx0 = x0;
    float yy0 = y0;
    float xx1 = x1;
    float yy1 = y1;
    apply_matrix3x3_stack(&xx0,&yy0);
    apply_matrix3x3_stack(&xx1,&yy1);

    XSetLineAttributes(dis, gc, stroke_weight, stroke_style, stroke_cap, stroke_join);
    XSetForeground(dis,gc,stroke_color);
    XDrawLine(dis,*draw_ptr, gc, xx0,yy0,xx1,yy1);

    if (clip_ptr != NULL){
        XSetForeground(dis, *clip_gc_ptr, 1);
        XSetLineAttributes(dis, *clip_gc_ptr, stroke_weight, stroke_style, stroke_cap, stroke_join);
        XDrawLine(dis,*clip_ptr, *clip_gc_ptr, xx0,yy0,xx1,yy1);
    }
}

void triangle(float x0,float y0,float x1,float y1,float x2,float y2){
    int old_mode = polygon_mode;
    polygon_mode = Convex;
    beginShape();
    vertex(x0,y0);
    vertex(x1,y1);
    vertex(x2,y2);
    vertex(x0,y0);
    endShape();
    polygon_mode = old_mode;
}

void quad(float x0,float y0,float x1,float y1,float x2,float y2, float x3, float y3){
    int old_mode = polygon_mode;
    polygon_mode = Convex;
    beginShape();
    vertex(x0,y0);
    vertex(x1,y1);
    vertex(x2,y2);
    vertex(x3,y3);
    vertex(x0,y0);
    endShape();
    polygon_mode = old_mode;
}

void arc(float x,float y,float w,float h,float start,float stop,int mode){
    int old_mode = polygon_mode;
    polygon_mode = Convex;
    beginShape();
    int x0=x;
    int y0=y;
    for (int i = 0; i < ellipse_detail+1; i++){
        float a = start+(float)i/(float)ellipse_detail*(stop-start);
        float xx = x+(w/2.0)*cos(a);
        float yy = y+(h/2.0)*sin(a);
        if (i == 0){
            x0 = xx;
            y0 = yy;
        }
        vertex(xx,yy);
    }
    if (mode == CHORD){
        vertex(x0,y0);
    }else if (mode == PIE){
        vertex(x,y);
        vertex(x0,y0);
    }
    endShape();
    polygon_mode = old_mode;
}


void ellipse(float x,float y,float w,float h){
    int old_mode = polygon_mode;
    polygon_mode = Convex;
    beginShape();
    for (int i = 0; i < ellipse_detail+1; i++){
        float a = (float)i/(float)ellipse_detail*PI*2;
        float xx = x+(w/2.0)*cos(a);
        float yy = y+(h/2.0)*sin(a);
        vertex(xx,yy);
    }
    endShape();
    polygon_mode = old_mode;
}

void circle(float x, float y, float w){
    ellipse(x,y,w,w);
}

void pushMatrix(){
    XMatrix3x3* A = (XMatrix3x3*)malloc(sizeof(XMatrix3x3));
    A->_00=1; A->_01=0; A->_02=0;
    A->_10=0; A->_11=1; A->_12=0;
    A->_20=0; A->_21=0; A->_22=1;

    A->prev = matrix_stack_tail;
    A->next = NULL;

    if (matrix_stack_head == NULL){
        matrix_stack_head = A;
        matrix_stack_tail = A;
    }else{
        matrix_stack_tail->next = A;
    }
    matrix_stack_tail = A;
}

void popMatrix(){
    XMatrix3x3* A = matrix_stack_tail;
    if (matrix_stack_head == matrix_stack_tail){
        matrix_stack_head = NULL;
        matrix_stack_tail = NULL;
    }else{
        matrix_stack_tail = matrix_stack_tail->prev;
        matrix_stack_tail->next = NULL;
    }
    free(A);
}


void translate(float x, float y){
    XMatrix3x3 B;
    B._00=1; B._01=0; B._02=x;
    B._10=0; B._11=1; B._12=y;
    B._20=0; B._21=0; B._22=1;
    matmul3x3(matrix_stack_tail,&B,matrix_stack_tail);
}

void scale(float x, float y){
    XMatrix3x3 B;
    B._00=x; B._01=0; B._02=0;
    B._10=0; B._11=y; B._12=0;
    B._20=0; B._21=0; B._22=1;
    matmul3x3(matrix_stack_tail,&B,matrix_stack_tail);
}

void rotate(float th){
    XMatrix3x3 B;
    float sinth = sin(th);
    float costh = cos(th);
    B._00=costh; B._01=-sinth; B._02=0;
    B._10=sinth; B._11= costh; B._12=0;
    B._20=0;     B._21=0;      B._22=1;
    matmul3x3(matrix_stack_tail,&B,matrix_stack_tail);

}

void shearX(float th){
    XMatrix3x3 B;
    B._00=1;     B._01=th;     B._02=0;
    B._10=0;     B._11=1;      B._12=0;
    B._20=0;     B._21=0;      B._22=1;
    matmul3x3(matrix_stack_tail,&B,matrix_stack_tail);
}
void shearY(float th){
    XMatrix3x3 B;
    B._00=1;     B._01=0;      B._02=0;
    B._10=th;    B._11=1;      B._12=0;
    B._20=0;     B._21=0;      B._22=1;
    matmul3x3(matrix_stack_tail,&B,matrix_stack_tail);
}

void applyMatrix(float n00, float n01, float n02, float n10, float n11, float n12){
    XMatrix3x3 B;
    B._00=n00;   B._01=n01;    B._02=n02;
    B._10=n10;   B._11=n11;    B._12=n12;
    B._20=0;     B._21=0;      B._22=1;
    matmul3x3(matrix_stack_tail,&B,matrix_stack_tail);
}

void loadPixels(XGraphics* g){
    int w = g->width;
    int h = g->height;
    if (pixels){
        free(pixels);
    }
    if (!g->img){
        pixels = NULL;
        return;
    }
    pixels = malloc(w*h*(sizeof(int)));
    memcpy(pixels, g->img->data, w*h*sizeof(int));
}

void updatePixels(XGraphics* g){
    int w = g->width;
    int h = g->height;
    if (!g->img){
        g->img = XCreateImage(dis, DefaultVisual(dis,screen), DefaultDepth(dis,screen), ZPixmap, 0, (char*) pixels, w, h, 32, 0);
        return;
    }
    for (int i = 0; i < w*h; i++){
        XPutPixel(g->img,i%w,i/w,pixels[i]);
    }
}


// implementation

void XInit() {
    draw_ptr = &win;
    clip_ptr = NULL;
    clip_gc_ptr = NULL;
    dis=XOpenDisplay((char *)0);
    screen=DefaultScreen(dis);
    //XSynchronize(dis,True);
    setup();
    
};

void XExit() {
    stop();
    XFreeGC(dis, gc);
    XDestroyWindow(dis,win);
    XCloseDisplay(dis); 
    exit(0);                
};


int get_width(Drawable* d){
    Window _root_return;
    int _x_return, _y_return;
    unsigned int _width_return, _height_return;
    unsigned int _border_width_return;
    unsigned int _depth_return;
    XGetGeometry(dis, *d, &_root_return, &_x_return, &_y_return, 
                         &_width_return, &_height_return, &_border_width_return, &_depth_return);
    return (int)_width_return;
}
int get_height(Drawable* d){
    Window _root_return;
    int _x_return, _y_return;
    unsigned int _width_return, _height_return;
    unsigned int _border_width_return;
    unsigned int _depth_return;
    XGetGeometry(dis, *d, &_root_return, &_x_return, &_y_return, 
                         &_width_return, &_height_return, &_border_width_return, &_depth_return);
    return (int)_height_return;
}



int main () {
    XEvent event;
    KeySym key;    
    char keybuf[BUFFSIZE];

    getcwd(cwd,BUFFSIZE);

    XInit();

    while (1) {
        if (no_loop){
            XNextEvent(dis, &event);
        }
        if (no_loop || XCheckMaskEvent(dis, eventMasks, &event)){
            if (event.type==KeyPress&&
                XLookupString(&event.xkey,keybuf,BUFFSIZE,&key,0)==1) {

                if (keybuf[0]==0x1B) {
                    XExit();
                }
                keyCode = keybuf[0];
                keyPressed();  
                if(!no_loop)XSync(dis, False);
            }else if (event.type==ButtonPress) {
                pmouseX=mouseX;
                pmouseY=mouseY;
                mouseX=event.xbutton.x,
                mouseY=event.xbutton.y; 
                mouseButton = event.xbutton.button;
                mouseIsPressed = 1;
                mousePressed();
                if(!no_loop)XSync(dis, False);
                
            }else if (event.type==ButtonRelease) {
                pmouseX=mouseX;
                pmouseY=mouseY;
                mouseX=event.xbutton.x,
                mouseY=event.xbutton.y;
                mouseIsPressed = 0;
                mouseReleased();
                if(!no_loop)XSync(dis, False);
                
            }else if (event.type==MotionNotify){
                pmouseX=mouseX;
                pmouseY=mouseY;
                mouseX=event.xbutton.x,
                mouseY=event.xbutton.y;
                if (event.xbutton.state == 0){
                    mouseIsPressed = 0;
                    mouseMoved();
                }else{
                    mouseIsPressed = 1;
                    if (event.xbutton.state == 256){
                        mouseButton = LEFT;
                    }else if (event.xbutton.state == 1024){
                        mouseButton = RIGHT;
                    }
                    mouseDragged();
                }
                if(!no_loop)XSync(dis, True);
            } 
        }
        if (frameCount % 100 == 1){
            if(!no_loop)XSync(dis, True);
        }else{
            if(!no_loop)XSync(dis, False);
        }
        pushMatrix();
        draw();

        popMatrix();
        
        frameCount ++;
        // usleep(1000);

    }
}

