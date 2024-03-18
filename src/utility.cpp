#include "utility.h"
#include <stdio.h>
#include <stdlib.h>
#include <png.h>


void save_image(const char *s, tile image_to_print[NUM_TILES][NUM_TILES]) {
    png_byte** row_pointers;
    FILE* fp;

    row_pointers = (png_byte**)malloc(sizeof(png_byte*) * IMAGE_SIZE);

    for (int i = 0; i < IMAGE_SIZE; i++) {
        row_pointers[i] = (png_byte*)malloc(4*IMAGE_SIZE);
    }

    for(int y = 0; y < IMAGE_SIZE; y++) {
        for(int x = 0; x < 4*IMAGE_SIZE; x+=4) {
            row_pointers[y][x] = yx_to_val(y,x/4,image_to_print);
            row_pointers[y][x+1] = row_pointers[y][x];
            row_pointers[y][x+2] = row_pointers[y][x];
            row_pointers[y][x+3] = 255;
        }
    }

    fp = fopen(s, "wb");
    if(fp == NULL) {
        printf("YIKES\n");
        return;
    }

    png_struct* png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    png_infop info = png_create_info_struct(png); 
    if (setjmp(png_jmpbuf(png))) 
    {
        printf("setjmp failed\n");
    }
    png_init_io(png, fp); 
    png_set_IHDR( 
            png, 
            info, 
            IMAGE_SIZE, 
            IMAGE_SIZE, 
            8, 
            PNG_COLOR_TYPE_RGBA, 
            PNG_INTERLACE_NONE, 
            PNG_COMPRESSION_TYPE_DEFAULT, 
            PNG_FILTER_TYPE_DEFAULT 
            );
    png_write_info(png, info); 
    png_write_image(png, row_pointers); 
    png_write_end(png, NULL);
    
    fclose(fp);
    for (int i = 0; i < IMAGE_SIZE; i++) {
        if (row_pointers[i]) {
            free(row_pointers[i]);
        }
    }
    if (row_pointers) {
        free(row_pointers);
    }
}
