#ifndef ENCODE_H
#define ENCODE_H

#include "config.h"

#include "image.h"
#include "tf.h"

tf_collection* encode(image* range, image* domain, unsigned int blockSize);

#endif
