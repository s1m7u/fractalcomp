#include <stdio.h>
extern "C" {
    #include <jpeglib.h>
}
#include <stdlib.h>
#include <cstddef>

#include "jpeg.h"

image *jpeg_to_image(const char *filename) {
    struct jpeg_decompress_struct info;
    struct jpeg_error_mgr err;

    int height, width;
    FILE* fp = fopen(filename, "rb");

	info.err = jpeg_std_error(&err);
	jpeg_create_decompress(&info);
	jpeg_stdio_src(&info, fp);
	jpeg_read_header(&info, TRUE);
	jpeg_start_decompress(&info);

    width = (int) info.output_width;
	height = (int) info.output_height;

    image *ret = image_create(height, width);


    if (info.out_color_components != 1 && info.output_components != 1) 
        exit(1);

    unsigned char *buf = (unsigned char *) malloc(width);
    
    while (info.output_scanline < info.image_height) {
        jpeg_read_scanlines(&info, &buf, 1);
        for (int i = 0; i < width; i++) {
            yx_to_val(info.output_scanline - 1, i, ret) = (unsigned int) buf[i];
        }
    }

    jpeg_finish_decompress(&info);
    jpeg_destroy_decompress(&info);
    fclose(fp);

    return ret;
}

void image_to_jpeg(const char *s, image *img, int quality) {
    struct jpeg_compress_struct info;
    struct jpeg_error_mgr err;

    FILE* fp = fopen(s, "wb");

	info.err = jpeg_std_error(&err);
	jpeg_create_compress(&info);

    jpeg_stdio_dest(&info, fp);

    info.image_width = img->width;
	info.image_height = img->height;
	info.input_components = 1;
	info.in_color_space = JCS_GRAYSCALE;

    jpeg_set_defaults(&info);
    jpeg_set_quality(&info, quality, TRUE);
    jpeg_start_compress(&info, TRUE);

    unsigned char *buf = (unsigned char *) malloc(sizeof(unsigned char) * img->width);
    	
    while(info.next_scanline < info.image_height) {
        for (unsigned int i = 0; i < img->width; i++) {
            buf[i] = (unsigned int) yx_to_val(info.next_scanline, (int) i, img);
        }
		jpeg_write_scanlines(&info, &buf, 1);
	}

    jpeg_finish_compress(&info);
    jpeg_destroy_compress(&info);
    fclose(fp);

    return;
}