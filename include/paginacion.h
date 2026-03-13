#ifndef PAGINACION_H
#define PAGINACION_H

#include "simulator.h"

// Entrada de la Tabla de Páginas (PTE)
struct page_table_entry {
    uint64_t frame_number;
    bool valid; // 1 si está en RAM, 0 si está en disco o fue invalidada
};

struct page_table {
    struct page_table_entry *entries;
    int num_pages;
};

// Función principal para traducir VA -> PA en modo paginación
uint64_t traducir_paginacion(uint64_t va, thread_params_t *data);

#endif
