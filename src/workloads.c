#include <stdlib.h>
#include <string.h>
#include "../include/workloads.h"

/* * Simula el comportamiento de un programa generando direcciones virtuales.
 */
uint64_t generar_direccion(thread_params_t *data) {
    // Escenario 80-20: El 80% de los accesos van al 20% de las páginas (Localidad)
    if (strcmp(data->config->workload, "80-20") == 0) {
        int probabilidad = rand() % 100;
        
        if (probabilidad < 80) {
            // Caso Hot: Accedemos a las primeras páginas (el "hot set")
            uint64_t vpn_hot = rand() % (uint64_t)(data->config->pages * 0.2 + 1);
            uint64_t offset = rand() % data->config->page_size;
            return (vpn_hot * data->config->page_size) + offset;
        }
    }
    
    // Escenario Uniforme: Cualquier parte de la memoria virtual es igual de probable
    uint64_t total_paginas = data->config->pages;
    uint64_t vpn_aleatoria = rand() % total_paginas;
    uint64_t offset_aleatorio = rand() % data->config->page_size;

    return (vpn_aleatoria * data->config->page_size) + offset_aleatorio;
}
