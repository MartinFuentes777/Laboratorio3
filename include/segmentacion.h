#ifndef SEGMENTACION_H
#define SEGMENTACION_H

#include "simulator.h"

struct segment_entry {
    uint64_t base;
    uint64_t limit;
};

struct segment_table {
    struct segment_entry *segments;
    int num_segments;
};

// Traduce y verifica si el offset es menor al límite
uint64_t traducir_segmentacion(int seg_id, uint64_t offset, struct segment_table *table, bool *error);

#endif
