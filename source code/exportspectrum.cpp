#include "exportspectrum.h"

void setImagePalette(image<index_pixel> &img){
    palette pal(256);
    for(int i=0; i<32; i++){
        pal[i]=color(0, 0, i*4);
        pal[i+32]=color(0, 0, 128+i*4);
        pal[i+64]=color(0, i*4, 255);
        pal[i+96]=color(0, 128+i*4, 255);
        pal[i+128]=color(0, 255, 255-i*8);
        pal[i+160]=color(i*8, 255, 0);
        pal[i+192]=color(255, 255-i*8, 0);
        pal[i+224]=color(255, 0, 0);
    }
    img.set_palette(pal);
}
void exportspectrum(double **data, size_t total_x, size_t total_y, float percent, const char *file){
    size_t offset = size_t (percent*total_y);
    image<index_pixel> img(total_x, offset);
    setImagePalette(img);
    double highest=0;
    unsigned int x,y;
    for(x=0;x<total_x;x++){
        for(y=0;y<offset;y++){
            if(data[x][offset-y-1]>highest)highest=data[x][offset-y-1];
        }
    }
    for(x=0;x<total_x;x++){
        for(y=0;y<offset;y++){
            index_pixel p((unsigned char)(255*data[x][offset-y-1]/highest));
            img.set_pixel(x, y, p);
        }
    }
    img.write(file);
}
