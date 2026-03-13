#include <stdio.h>
#include <stdlib.h>
#include "../include/simulator.h"
#include "../include/tlb.h"

/**
 * Imprime una barra de progreso o mensajes de depuración.
 * Usamos (void)param para evitar el warning de "unused parameter" 
 * mientras la función esté vacía.
 */
void log_debug(const char *msg, int thread_id) {
    (void)msg;
    (void)thread_id;
    // printf("[Thread %d] %s\n", thread_id, msg);
}

/**
 * Limpieza de memoria al terminar.
 * Importante para no dejar memory leaks.
 */
void liberar_recursos_hilo(thread_params_t *data) {
    if (data->table) {
        // Como 'table' es void*, aquí liberamos la memoria base.
        // Las estructuras internas (entries) deberían liberarse según el modo.
        free(data->table);
    }
    
    if (data->tlb) {
        struct tlb *t = (struct tlb*)data->tlb;
        if (t->entries) {
            free(t->entries);
        }
        free(t);
    }
}
