#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "lodepng.h"

char* loadPng(const char*filename, int*width, int*height) {
  unsigned char*image = NULL;
  int error = lodepng_decode32_file(&image, width, height, filename);
  if(error){
    printf("error %u: %s\n", error, lodepng_error_text(error));
  }
  return (image);
}

void print(const char*filename, const unsigned char*image, unsigned width, unsigned height) {
  unsigned char*png;
  long unsigned int pngsize;

  int error = lodepng_encode32(&png, &pngsize, image, width, height);
  if(!error) {
    lodepng_save_file(png, pngsize, filename);
  }

  if(error) printf("error %u: %s\n", error, lodepng_error_text(error));

  free(png);

}

void col(unsigned char*picture_2,unsigned char*picture_4,int width, int height){
int i,square = width*height;
    for(i=0;i<square;i++) {
        picture_4[i*4] = 271;
        picture_4[i*4+1] = 1.9*picture_2[i];
        picture_4[i*4+2] = 1.2*picture_2[i];
        picture_4[i*4+3] = 255;
    }
    return;
}
void brightness(unsigned char*task,unsigned char *picture_1,int width, int height){
 int i,square = width*height;
for(i=0;i<4*square;i+=4){
picture_1[i/4]=0.19*task[i]+0.66*task[i+1]+0.33*task[i+2];
    }
    return;
}

void condition(unsigned char*picture_1,int width,int height){
int i,j;
    for(i=1;i<height-1;i++){
        for( j=1;j<width-1;j++){
          
            if(picture_1[width*i+j]<80){
                picture_1[width*i+j]=0;
            }
            if(picture_1[width*i+j]>160){
                picture_1[width*i+j]=255;
            }
        }
    }
    return;
}


void Gauss(unsigned char*picture_1, unsigned char*picture_2,int width,int height){
int i,j;
     for(i = 1;i < height-1 ;i++){
        for( j = 1;j < width-1 ;j++){
           picture_2[width*i+j]=
             0.1799735*picture_1[width*i+j]
             +0.0359344*picture_1[width*(i+1)+(j-1)]
             +0.1054768*picture_1[width*(i+1)+j]
             +0.0359344*picture_1[width*(i+1)+(j+1)]
             +0.1294206*picture_1[width*i+(j-1)]
             +0.0654768*picture_1[width*i+(j+1)]
             +0.0294206*picture_1[width*(i-1)+(j-1)]
             +0.1294206*picture_1[width*(i-1)+j]
             +0.0359344*picture_1[width*(i-1)+(j+1)];
        }
     }
   return;
}

int main() {
    char*open_file = "skull.png";
    int width,height,square;
    char*task = loadPng(open_file, &width, &height);
    square = width*height;
    unsigned char *picture_1=(unsigned char*)malloc(square*sizeof(unsigned char));
    unsigned char *picture_2=(unsigned char*)malloc(square*sizeof(unsigned char));
    unsigned char *picture_4=(unsigned char*)malloc(square*4*sizeof(unsigned char));
    condition(picture_1,width,height);
    brightness(task,picture_1,width,height);
    Gauss(picture_1,picture_2,width,height);
    col(picture_2,picture_4,width,height);
    char*new_image="renthgen.png";
    print(new_image,picture_4,width,height);
    return 0;
}