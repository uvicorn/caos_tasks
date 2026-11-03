#ifndef UTILS_H
#define UTILS_H

typedef unsigned Element;

static inline unsigned lg2(unsigned x){return 31 - __builtin_clz(x);}

#define swap(a,b) {a^=b;b^=a;a^=b;}

#endif