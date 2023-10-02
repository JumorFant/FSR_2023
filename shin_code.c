#include "lodepng.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

char *loadPng(const char *filename, int *width, int *height) {

  unsigned char *image = NULL;
  int error = lodepng_decode32_file(&image, width, height, filename);
  if (error) {
    printf("error %u: %s\n", error, lodepng_error_text(error));
  }

  return (image);
}

void writePng(const char *inputfile, const unsigned char *image, unsigned width,
              unsigned height) {
  unsigned char *png;
  long unsigned int pngsize;

  int error = lodepng_encode32(&png, &pngsize, image, width, height);
  if (!error) {
    lodepng_save_file(png, pngsize, inputfile);
  }
  /*if there's an error, display it*/
  if (error)
    printf("error %u: %s\n", error, lodepng_error_text(error));

  free(png);
}

void convert_to_grey(int w, int h, unsigned char *image, unsigned char *grey_image) {
  int k = 0;
  for (int i = 0; i < w * h * 4; i += 4) {

    grey_image[k] = 0.299 * image[i] + 0.587 * image[i + 1] + 0.114 * image[ i + 2];
    k++;
  }
  return;
}

void preparation(int w, int h, unsigned char *grey_image) {
  for (int i = 0; i < h; i++) {
    for (int j = 0; j < w; j++) {

      if (grey_image[w * i + j] < 100) {
        grey_image[w * i + j] = 0;
      }
      if (grey_image[w * i + j] > 160) {
        grey_image[w * i + j] = 255;
      }
    }
  }
  return;
}
void symmetryOY(int w, int h, unsigned char *filter_image, unsigned char *symmerty_image) {
  for (int i = 0; i < h; i++) {
    for (int j = 0; j < w; j++) {
      symmerty_image[w * i + j] = filter_image[w * i + w - 1 - j];
    }
  }
  return;
}
void color(int w, int h,n, unsigned char *filter_image, unsigned char *color_image) {
  int pic[w * h];
  for (int i = 0; i < w * h; i++)
    pic[i] = filter_image[i];
  for (int i = 1; i < w * h; i++) {
    if (pic[i] == 0) {
      color_image[i * n] = 0;
      color_image[i * n + 1] = 128;
      color_image[i * n + 2] = 0;
      color_image[i * n + 3] = 255;
    } else {
      color_image[i * n] = 80 + pic[i] + 0.5 * pic[i - 1];
      color_image[i * n + 1] = 4.5 * pic[i];
      color_image[i * n + 2] = 1.5 * pic[i];
      color_image[i * n + 3] = 255;
    }
  }
  return;
}

void fGauss(int w, int h, unsigned char *grey_image, unsigned char *filter_image) {//0.09 0.12 0.09
  for (int i = 2; i < h - 1; i++) {                                               // 0.12 0.15 0.12
    for (int j = 2; j < w - 1; j++) {                                             // 0.09 0.12 0.09
      filter_image[w * i + j] = 0.09 * grey_image[w * (i + 1) + (j - 1)] +
                            0.12 * grey_image[w * (i + 1) + j] +
                            0.09 * grey_image[w * (i + 1) + (j + 1)] +
                            0.12 * grey_image[w * i + (j - 1)] +
                            0.15 * grey_image[w * i + j] +
                            0.12 * grey_image[w * i + (j + 1)] +
                            0.09 * grey_image[w * (i - 1) + (j - 1)] +
                            0.12 * grey_image[w * (i - 1) + j] +
                            0.09 * grey_image[w * (i - 1) + (j + 1)];
    }
  }
  return;
}
void fSobel(int w, int h, unsigned char *grey_image, unsigned char *filter_image) {// -1 0 -1  -1 -2 -1
  unsigned char x, y;                                                              // -2 0 -2   0  0  0
  for (int i = 2; i < h - 1; i++) {                                                // -1 0 -1  -1 -2 -1 
    for (int j = 2; j < w - 1; j++) {
      x = -1 * grey_image[w * (i - 1) + j - 1] - 2 * grey_image[w * (i) + j - 1] -
          1 * grey_image[w * (i + 1) + j - 1] + grey_image[w * (i - 1) + j + 1] +
          2 * grey_image[w * (i) + j + 1] + grey_image[w * (i + 1) + j + 1];
      y = -1 * grey_image[w * (i - 1) + j - 1] - 2 * grey_image[w * (i - 1) + j] -
          1 * grey_image[w * (i - 1) + j + 1] + grey_image[w * (i + 1) + j - 1] +
          2 * grey_image[w * (i + 1) + j] + grey_image[w * (i + 1) + j + 1];
      filter_image[i * w + j] = sqrt(x * x + y * y);
    }
  }
  return;
}
int main() {
  char *inputfile = "shin.png";
  int w, h,n = 4;//n - number of channels 
  char *image = loadPng(inputfile, &w, &h);
  if (image == NULL) {
    printf("error:I can't read file %s. Error.\n", inputfile);
    return -1;
  }
  unsigned char *grey_image =
      (unsigned char *)malloc(h * w * sizeof(unsigned char));
  unsigned char *filter_image =
      (unsigned char *)malloc(h * w * sizeof(unsigned char));
  unsigned char *symmerty_image =
      (unsigned char *)malloc(h * w * sizeof(unsigned char));
  unsigned char *color_image =
      (unsigned char *)malloc(h * w * n * sizeof(unsigned char));
  preparation(w, h, grey_image);
  convert_to_grey(w, h, image, grey_image);
//  fGauss(w, h, grey_image, filter_image);
  fSobel(w, h, grey_image, filter_image);
  symmetryOY(w, h, filter_image, symmerty_image);
  color(w, h,n, symmerty_image, color_image);
  char *new_image = "shin-modefited.png";
  writePng(new_image, color_image, w, h);
  return 0;
}