#include <png.h>
#include <stdlib.h>

#include "image.h"

image* image_create(unsigned int height, unsigned int width) {
    image* ret = new image;
    ret->height = height;
    ret->width = width;
    ret->tileNum = (height / TILE_SIZE) * (width / TILE_SIZE);
    ret->pixels = new tile[ret->tileNum];
    ret->pfxSum = new int[(height+1)*(width+1)]();
    ret->pfxSumOfSq = new long int[(height+1)*(width+1)]();
    return ret;
}

void image_destroy(image* img) {
    delete[] img->pixels;
    delete[] img->pfxSum;
    delete[] img->pfxSumOfSq;
    delete img;
}

static void image_stats_calc(image* img) {
    for(unsigned int y = 1; y <= img->height; y++) {
        for(unsigned int x = 1; x <= img->width; x++) {
            img->pfxSum[y*(img->width+1)+x] = img->pfxSum[(y-1)*(img->width+1)+x]
                + img->pfxSum[y*(img->width+1) + (x-1)]
                - img->pfxSum[(y-1)*(img->width+1) + (x-1)]
                + yx_to_val(y-1,x-1,img);
            img->pfxSumOfSq[y*(img->width+1)+x] = img->pfxSumOfSq[(y-1)*(img->width+1)+x]
                + img->pfxSumOfSq[y*(img->width+1) + (x-1)]
                - img->pfxSumOfSq[(y-1)*(img->width+1) + (x-1)]
                + yx_to_val(y-1,x-1,img)*yx_to_val(y-1,x-1,img);
        }
    }
    
}

image* downsample_by_two(image* img) {
    if(img->height < 1 || img->width < 1) {
        return NULL;
    }

    image* ret = image_create(img->height/2, img->width/2);

    for(unsigned int y = 0; y < ret->height; y++) {
        for(unsigned int x = 0; x < ret->width; x++) {
            yx_to_val(y,x,ret) = (
                    yx_to_val(2*y,2*x,img) +
                    yx_to_val(2*y,2*x+1,img) +
                    yx_to_val(2*y+1,2*x+1,img) +
                    yx_to_val(2*y+1,2*x,img))/4;
        }
    }

    //TODO: there's a nice way to probably rip this out of the original
    image_stats_calc(ret);

    return ret;
}

image* image_from_png(const char *s) {
    FILE* fp = fopen(s, "rb");

    png_byte bit_depth;
    png_byte color_type;
    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop info = png_create_info_struct(png);
    png_init_io(png, fp);
    png_read_info(png, info);

    int width = png_get_image_width(png, info);
    int height = png_get_image_height(png, info);
    color_type = png_get_color_type(png, info);
    bit_depth = png_get_bit_depth(png, info);

    if(color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb(png);
    if(color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_expand_gray_1_2_4_to_8(png);
    if(png_get_valid(png, info, PNG_INFO_tRNS))
        png_set_tRNS_to_alpha(png);
    if(color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_filler(png, 0xFF, PNG_FILLER_AFTER);
    if(color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png);

    png_read_update_info(png, info);

    int numchan = 4;

    // Set up row pointer
    png_bytep *row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
    unsigned int i, j;
    for (i = 0; i < height; i++)
        row_pointers[i] = (png_bytep)malloc(png_get_rowbytes(png,info));
    png_read_image(png, row_pointers);

    fclose(fp);

    image* ret = image_create(height, width);

    debug("IMAGE DIMENSIONS ARE: %d %d\n", height, width);

    for (i = 0 ; i < height ; i++)
    {
        for (j = 0 ; j < width ; j++)
        {
            yx_to_val(i,j,ret) = row_pointers[i][j*4];
        }
    }

    image_stats_calc(ret);

    for (i = 0; i < height; i++) {
        free(row_pointers[i]);
    }
    free(row_pointers);
    return ret;
}


void save_image(const char *s, image* img) {
    png_byte** row_pointers;
    FILE* fp;

    row_pointers = (png_byte**)malloc(sizeof(png_byte*) * img->height);

    for (int i = 0; i < img->height; i++) {
        row_pointers[i] = (png_byte*)malloc(4*img->width);
    }

    for(unsigned int y = 0; y < img->height; y++) {
        for(unsigned int x = 0; x < 4*img->width; x+=4) {
            row_pointers[y][x] = yx_to_val(y,x/4,img);
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
            img->height, 
            img->width, 
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
    for (int i = 0; i < img->height;i++) {
        if (row_pointers[i]) {
            free(row_pointers[i]);
        }
    }
    if (row_pointers) {
        free(row_pointers);
    }
}

void print_image(const char *s, image* img) {
    FILE *image_file = fopen(s, "w");
    for (int y = 0; y < img->height; y++) {
        for (int x = 0; x < img->width; x++) {
            fprintf(image_file, "%3d ", yx_to_val(y,x,img));
        }
        fprintf(image_file, "\n");
    }
    fclose(image_file);
    printf("Success: %s created.\n", s);
}
