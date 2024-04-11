#include <stdio.h>
#include <cmath>
#include <regex>

#include "tf.h"
#include "block.h"

tf_collection* tf_collection_create(unsigned int height, unsigned int width) {
    tf_collection* ret = new tf_collection;
    ret->height = height;
    ret->width = width;
    return ret;
}

void tf_collection_create(tf_collection* tfc) {
    tfc->tfs.clear();
    delete tfc;
}

float regression(block* domain, block* range, int orient, tf_data& tf) {
    float ds = (float)blockSum(domain);
    float dsq = (float)blockSumOfSq(domain);

    float denom = domain->size * domain->size * dsq - ds * ds;

    if(denom == 0) {
        return 100000;
    }


    float xy = 0;
    for(unsigned int i = 0; i < domain->size; i++) {
        for(unsigned int j = 0; j < domain->size; j++) {
            unsigned int y = i, x = j;
            calc_pos(&y, &x, orient, domain->size);

            xy += yx_to_val(y,x,domain) * yx_to_val(i,j,range);
        }
    }

    float rs = (float)blockSum(range);

    tf.contrast = ((float)(domain->size * domain->size * xy) - ds * rs)/denom;
    /* tf.brightness = (float)(rs * dsq - ds * xy)/denom; */
    tf.brightness = (float)(rs - tf.contrast * ds)/(domain->size*domain->size);
    /* printf("%f %f\n", tf.contrast, tf.brightness); */
    tf.d_block[0] = domain->y;
    tf.d_block[1] = domain->x;
    tf.r_block[0] = range->y;
    tf.r_block[1] = range->x;
    tf.size = range->size;
    tf.orient = orient;

    float residual = 0;

    for(unsigned int i = 0; i < domain->size; i++) {
        for(unsigned int j = 0; j < domain->size; j++) {
            unsigned int y = i, x = j;
            calc_pos(&y, &x, orient,domain->size);

            float temp = (yx_to_val(y,x,domain) * tf.contrast + tf.brightness 
                    - yx_to_val(i,j,range));
            temp *= temp;
            residual += temp;
        }
    }

    return residual;
}

void print_encoding(tf_collection* tfc) {
    FILE *encoding_file = fopen("encoding.txt", "w");
    for(tf_data t : tfc->tfs) {
        fprintf(
                encoding_file,
                "(%2d, %2d) -> (%2d %2d) "
                "s:%4d o:%2d c:%+2.2f b:%+2.2f\n", 
                t.d_block[0],
                t.d_block[1],
                t.r_block[0],
                t.r_block[1],
                t.size,
                t.orient,
                t.contrast,
                t.brightness
               );
    }        
    fclose(encoding_file);
    debug("Success: encoding.txt created.\n");
}

void encode_stats_pp(const char *original, const char *decompressed) {
    image* original_image = image_from_png(original);
    image *decomp_image = image_from_png(decompressed);

    if (original_image->height != decomp_image->height || original_image->width != decomp_image->width) {
        debug("encode_stats: Images are not same size\n");
        return;
    }

    int height = original_image->height;
    int width = original_image->width;

    double rmse = 0;
    int temp = 0;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            temp = yx_to_val(i, j, original_image) - yx_to_val(i, j, decomp_image);
            rmse += temp * temp;
        }
    }

    rmse /= height * width;
    rmse = std::sqrt(rmse);

    FILE *stats = fopen("stats.txt", "a");
    fprintf(stats, "RMSE of %s and %s: %lf\n", original, decompressed, rmse);
    fclose(stats);
    debug("Success: stats.txt created/appended.\n");
}

void encode_stats_pj(const char *png, const char *jpeg) {
    image* png_image = image_from_png(png);
    image *jpeg_image = jpeg_to_image(jpeg);

    if (png_image->height != jpeg_image->height || png_image->width != jpeg_image->width) {
        debug("encode_stats: Images are not same size\n");
        return;
    }

    int height = png_image->height;
    int width = png_image->width;

    double rmse = 0;
    int temp = 0;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            temp = yx_to_val(i, j, png_image) - yx_to_val(i, j, jpeg_image);
            rmse += temp * temp;
        }
    }

    rmse /= height * width;
    rmse = std::sqrt(rmse);

    FILE *stats = fopen("stats.txt", "a");
    fprintf(stats, "RMSE of %s and %s: %lf\n", png, jpeg, rmse);
    fclose(stats);
    debug("Success: stats.txt created/appended.\n");
}

void encoding_stats_pb(const char *s, image* buffer, int i) {
    image *original = image_from_png(s);

    if (buffer->height != original->height || buffer->width != original->width) {
        debug("encode_stats: Images are not same size\n");
        return;
    }

    int height = buffer->height;
    int width = buffer->width;

    double rmse = 0;
    int temp = 0;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            temp = yx_to_val(i, j, buffer) - yx_to_val(i, j, original);
            rmse += temp * temp;
        }
    }

    rmse /= height * width;
    rmse = std::sqrt(rmse);

    FILE *stats = fopen("stats.txt", "a");
    fprintf(stats, "Iteration %d: RMSE %lf\n", i, rmse);
    fclose(stats);
}