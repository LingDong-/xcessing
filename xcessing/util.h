#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

float map(float value,float istart,float istop,float ostart,float ostop){
    return ostart + (ostop - ostart) * ((value - istart)*1.0 / (istop - istart));
}
float lerp(float a, float b, float t){
    return a*(1-t)+b*t;
}
float degrees(float x){
    return x*57.29577951308232;
}
float radians(float x){
    return x*0.017453292519943295;
}
float constrain(int amt, int low, int high) {
    return (amt < low) ? low : ((amt > high) ? high : amt);
}
float dist(float x1, float y1, float x2, float y2){
    float dx = x1-x2;
    float dy = y1-y2;
    return sqrt(dx*dx+dy*dy);
}
int red(int c){
    return (c >> 16) & 0xFF;
}
int green(int c){
    return (c >> 8) & 0xFF;
}
int blue(int c){
    return c & 0xFF;
}
int hsb(float h, float s, float v){
    //https://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both
    double      hh, p, q, t, ff;
    long        i;
    double r,g,b;
    int rr,gg,bb;
    hh = (double)h;
    if(hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    p = v * (1.0 -  s);
    q = v * (1.0 - (s * ff));
    t = v * (1.0 - (s * (1.0 - ff)));
    switch(i) {
        case 0: r = v;g = t;b = p;break;
        case 1: r = q;g = v;b = p;break;
        case 2: r = p;g = v;b = t;break;
        case 3: r = p;g = q;b = v;break;
        case 4: r = t;g = p;b = v;break;
        case 5:
        default:r = v;g = p;b = q;break;
    }
    rr = (int)(r*255);
    gg = (int)(g*255);
    bb = (int)(b*255);
    return (rr<<16)|(gg<<8)|bb;     
}

int hour(){
    time_t rawtime;
    struct tm *info;
    time( &rawtime );
    info = localtime( &rawtime );
    return info->tm_hour;
}
int minute(){
    time_t rawtime;
    struct tm *info;
    time( &rawtime );
    info = localtime( &rawtime );
    return info->tm_min;
}
int second(){
    time_t rawtime;
    struct tm *info;
    time( &rawtime );
    info = localtime( &rawtime );
    return info->tm_sec;
}


// vector math
typedef struct _vec2f_t {float x;float y;} Vec2f;
Vec2f vnew(float x, float y){Vec2f v = {x,y};return v;}
Vec2f vcopy(Vec2f u){return vnew(u.x,u.y);}
Vec2f vadd(Vec2f u,Vec2f v){return vnew(u.x+v.x,u.y+v.y);}
Vec2f vsub(Vec2f u,Vec2f v){return vnew(u.x-v.x,u.y-v.y);}
Vec2f vfromAngle(float th){return vnew(cos(th),sin(th));}
float vdot(Vec2f u, Vec2f v){return u.x*v.x+u.y*v.y;}
float vmagSq(Vec2f u){return vdot(u,u);}
float vmag(Vec2f u){return sqrt(vmagSq(u));}
Vec2f vmult(Vec2f u, Vec2f v){return vnew(u.x*v.x,u.y*v.y);}
Vec2f vscale(Vec2f u, float s){return vnew(u.x*s,u.y*s);}
Vec2f vdiv(Vec2f u, float s){return vnew(u.x/s,u.y/s);}
float vdist(Vec2f u, Vec2f v){return vmag(vsub(u,v));}
Vec2f vnormalize(Vec2f u){float l = vmag(u); return l==0?u:vdiv(u,vmag(u));}
Vec2f vlimit(Vec2f u, float m){float l=vmag(u); return (l>m)?vscale(u,m/l):vcopy(u);}
Vec2f vsetMag(Vec2f u, float m){return vscale(u,m/vmag(u));}
float vheading(Vec2f u){return atan2(u.y,u.x);}
Vec2f vrotate(Vec2f u, float th){float sinth = sin(th); float costh = cos(th); return vnew(u.x*costh-u.y*sinth,u.x*sinth+u.y*costh);}
Vec2f vlerp(Vec2f u, Vec2f v, float t){return vnew(u.x*(1-t)+v.x*t, u.y*(1-t)+v.y*t);}
float vangleBetween(Vec2f u, Vec2f v){return acos(vdot(u,v)/(vmag(u)*vmag(v)));}
float vangle(Vec2f u, Vec2f v){return acos(vdot(u,v)/(vmag(u)*vmag(v)));}
void  vprint(Vec2f u){printf("(%f, %f)\t",u.x,u.y);}
