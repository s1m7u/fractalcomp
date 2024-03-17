#ifndef ENCODE_H
#define ENCODE_H

#include "image.h"
#include "tf.h"
#include "config.h"

tf_collection* encode(image* range, image* domain, unsigned int blockSize);

#endif
