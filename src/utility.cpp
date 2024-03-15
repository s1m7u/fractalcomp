#include "utility.h"
#include <stdio.h>
#include <stdlib.h>
#include <png.h>

void import_image(const char* s) {
    /* printf("Image: circle gradient.\n"); */
    /* for (int i = 0; i < IMAGE_SIZE; i++) { */
    /*     for (int j = 0; j < IMAGE_SIZE; j++) { */
    /*         image[i][j] = sqrt((IMAGE_SIZE / 2 - i) * (IMAGE_SIZE / 2 - i) + (IMAGE_SIZE / 2 - j) * (IMAGE_SIZE / 2 - j)); */
    /*     } */
    /* } */

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

    for (i = 0 ; i < height ; i++)
    {
        for (j = 0 ; j < width ; j++)
        {
            image[i][j] = row_pointers[i][j*4];
        }
    }
    fclose(fp);
    for (i = 0; i < height; i++)
        free(row_pointers[i]) ;
    free(row_pointers);
}

void init_image() {
    for(int y = 0; y < IMAGE_SIZE; y++) {
        for (int x = 0; x < IMAGE_SIZE; x++) {
            yx_to_val(y,x,range_blocks) = image[y][x];
        }
    }

    for(int y = 0; y < NUM_R_BLOCKS; y++) {
        for(int x = 0; x < NUM_R_BLOCKS; x++) {
            tile_stats_calc(&range_blocks[y][x]);
        }
    }


    for(int y = 0; y < NUM_D_BLOCKS; y++) {
        for(int x = 0; x < NUM_D_BLOCKS; x++) {
            for(int i = 0; i < R_BLOCK_SIZE; i++) {
                for(int j = 0; j < R_BLOCK_SIZE; j++) {
                    domain_blocks[y][x]
                        .vals[i][j] = (
                                yx_to_val(y*D_BLOCK_SIZE+2*i,x*D_BLOCK_SIZE+2*j,range_blocks) +
                                yx_to_val(y*D_BLOCK_SIZE+2*i+1,x*D_BLOCK_SIZE+2*j,range_blocks) +
                                yx_to_val(y*D_BLOCK_SIZE+2*i+1,x*D_BLOCK_SIZE+2*j+1,range_blocks) +
                                yx_to_val(y*D_BLOCK_SIZE+2*i,x*D_BLOCK_SIZE+2*j+1,range_blocks)
                                )/4;
                }
            }
            tile_stats_calc(&domain_blocks[y][x]);
        }
    }
}

void print_encoding() {
    FILE *encoding_file = fopen("encoding.txt", "w");
    for (int i = 0; i < NUM_R_BLOCKS; i++) {
        for (int j = 0; j < NUM_R_BLOCKS; j++) {          
            fprintf(
                    encoding_file,
                    "%4d: (%2d, %2d) -> (%2d %2d) "
                    "s:%4d o:%2d c:%+2.2f b:%+2.2f r:%+2.2f\n", 
                    i * NUM_R_BLOCKS + j, 
                    compressed_data[i][j].d_block[0],
                    compressed_data[i][j].d_block[1],
                    i, j,
                    compressed_data[i][j].shrink_ratio,
                    compressed_data[i][j].orient,
                    compressed_data[i][j].contrast,
                    compressed_data[i][j].brightness,
                    compressed_data[i][j].residual
                   );
        }        
    }
    fclose(encoding_file);
    printf("Success: encoding.txt created.\n");
}

void print_image(const char *s, tile image_to_print[NUM_TILES][NUM_TILES]) {
    FILE *image_file = fopen(s, "w");
    for (int i = 0; i < IMAGE_SIZE; i++) {
        for (int j = 0; j < IMAGE_SIZE; j++) {
            fprintf(image_file, "%3d ", image_to_print[i / 4][j / 4].vals[i % 4][j % 4]);
        }
        fprintf(image_file, "\n");
    }
    fclose(image_file);
    printf("Success: %s created.\n", s);
}

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

tile& yx_to_tile(int y, int x, tile i[64][64]) {
    return i[y/TILE_SIZE][x/TILE_SIZE];
}

void tile_stats_calc(tile* t) {
    int sum = 0;
    for(int i = 0; i < TILE_SIZE; i++) {
        for(int j = 0; j < TILE_SIZE; j++) {
            sum += t->vals[i][j];
        }
    }
    t->mean = (float)(sum)/(float)(TILE_SIZE*TILE_SIZE);

    t->sum_sq = 0;
    for(int i = 0; i < TILE_SIZE; i++) {
        for(int j = 0; j < TILE_SIZE; j++) {
            t->sum_sq += (t->vals[i][j] - t->mean) * (t->vals[i][j] - t->mean);
        }
    }
}

