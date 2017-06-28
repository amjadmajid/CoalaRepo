#include "global.h"

#ifndef INCLUDE_WRITEBUFFER_H_
#define INCLUDE_WRITEBUFFER_H_

void protectVar(uint16_t * varPt, size_t size);
void resetPointers();
void restoreVar();

#endif /* INCLUDE_WRITEBUFFER_H_ */
