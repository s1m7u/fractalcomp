#ifndef CONFIG_H
#define CONFIG_H

#define DEBUG

#ifdef DEBUG
#define debug(...) fprintf(stderr, __VA_ARGS__); fflush(stderr)
#define debugif(...) __VA_ARGS__
#define dassert(...) assert(__VA_ARGS__)
#else
#define debug(...)
#define debugif(...)
#define dassert(...)
#endif

#endif
