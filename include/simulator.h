#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <stdint.h>
#include <stdbool.h>

// Métricas obligatorias por el PDF
typedef struct {
    int tlb_hits;
    int page_faults;
    int evictions;
    int segfaults;
} metrics_t;

// Parámetros de configuración global
typedef struct {
    char mode[10];      // "page" o "seg"
    int threads;
    int ops_per_thread;
    char workload[20];
    int seed;
    bool unsafe;
    int pages;
    int frames;
    int page_size;
    int tlb_size;
} config_t;

// Contexto de cada hilo (Simulación de Proceso)
typedef struct {
    int id;
    config_t *config;
    void *table;       // Apunta a struct page_table o struct segment_table
    void *tlb;         // Apunta a struct tlb
    metrics_t metrics;
    
    // Almacena la página que el hilo está intentando cargar
    // para que el frame_allocator sepa qué anotar en el registro de dueños.
    uint64_t ultima_vpn_solicitada; 
} thread_params_t;

#endif
