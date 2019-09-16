#ifndef TOF_H
#define TOF_H

#include <stdint.h>

void ToF_Init(uint32_t port);

// A negative 1 is bad
int32_t ToF_Measure(uint32_t port);


#endif
