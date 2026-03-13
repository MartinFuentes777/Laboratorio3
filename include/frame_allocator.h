#ifndef FRAME_ALLOCATOR_H
#define FRAME_ALLOCATOR_H

#include "simulator.h"

// Solicita un marco físico. Si no hay, elige una víctima e INVALIDA su tabla.
uint64_t solicitar_frame(thread_params_t *data);

#endif
