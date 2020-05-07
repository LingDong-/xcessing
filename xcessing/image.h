#include <png.h>

// use libpng to read png into array of hex colors
int* read_png(char* file_name, int* w, int* h){
  // based on http://zarb.org/~gc/html/libpng.html
  int x, y, z, W, H;
  int _width, _height;
  png_byte color_type;
  png_byte bit_depth;
  png_structp png_ptr;
  png_infop info_ptr;
  int number_of_passes;
  png_bytep * row_pointers;
  char header[8];
  FILE *fp = fopen(file_name, "rb");
  if (!fp)return NULL;
  fread(header, 1, 8, fp);
  if (png_sig_cmp((void*)header, 0, 8))exit(1);
  png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png_ptr)return NULL;
  info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr)return NULL;
  if (setjmp(png_jmpbuf(png_ptr)))return NULL;
  png_init_io(png_ptr, fp);
  png_set_sig_bytes(png_ptr, 8);
  png_read_info(png_ptr, info_ptr);
  _width = png_get_image_width(png_ptr, info_ptr);
  _height = png_get_image_height(png_ptr, info_ptr);
  color_type = png_get_color_type(png_ptr, info_ptr);
  bit_depth = png_get_bit_depth(png_ptr, info_ptr);
  number_of_passes = png_set_interlace_handling(png_ptr);
  png_read_update_info(png_ptr, info_ptr);
  if (setjmp(png_jmpbuf(png_ptr)))return NULL;
  row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * _height);
  if (!row_pointers)return NULL;
  for (y=0; y<_height; y++)
    row_pointers[y] = (png_byte*) malloc(png_get_rowbytes(png_ptr,info_ptr));
  png_read_image(png_ptr, row_pointers);

  int channels;
  if (color_type == PNG_COLOR_TYPE_RGB){
    channels = 3;
  }else if (color_type == PNG_COLOR_TYPE_RGBA){
    channels = 4;
  }else if (color_type == PNG_COLOR_TYPE_GRAY){
    channels = 1;
  }else{
    return NULL;
  }
  W = _width; H = _height;
  int* im = (int*) malloc(sizeof(int)*W*H);
  if (!im)return NULL;
  for (y=0; y<H; y++) {
    png_byte* row = row_pointers[y];
    for (x=0; x<W; x++) {
      png_byte* ptr = &(row[x*channels]);
      if (channels == 1){
        im[y*W+x] = ((int)ptr[0])<<16 | ((int)ptr[0])<<8 | ((int)ptr[0]);
      }else if (channels == 3){
        im[y*W+x] = ((int)ptr[0])<<16 | ((int)ptr[1])<<8 | ((int)ptr[2]);
      }else{
        im[y*W+x] = ((int)ptr[3])<<24 | ((int)ptr[0])<<16 | ((int)ptr[1])<<8 | ((int)ptr[2]);
      }
    }
    free(row_pointers[y]);
  }
  free(row_pointers);
  png_destroy_info_struct(png_ptr,&info_ptr);
  png_destroy_read_struct(&png_ptr,(png_infop*)NULL,(png_infop*)NULL);
  fclose(fp);
  
  *w = W;
  *h = H;
  return im;
}

XGraphics loadImage(char* fname){
  int w, h;
  int* im = read_png(fname,&w,&h);
  // for (int i = 0; i < w*h; i++){im[i] = i;}
  XImage* xim = XCreateImage(
    dis, 
    DefaultVisual(dis,screen), 
    DefaultDepth(dis,screen), 
    ZPixmap,
    0,
    (char*) im,
    w,
    h,
    32, 0);

  XGraphics g;
  g.width =  w;
  g.height = h;
  g.img = xim;
  return g;
}
