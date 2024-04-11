#ifndef JPEG_H
#define JPEG_H

#include "config.h"
#include "image.h"

image *jpeg_to_image(const char *filename);
void image_to_jpeg(const char *s, image *img, int quality);

#endif
