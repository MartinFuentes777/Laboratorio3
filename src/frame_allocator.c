#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../include/frame_allocator.h"
#include "../include/paginacion.h"
#include "../include/tlb.h"

typedef struct
{
    thread_params_t *owner;
    uint64_t vpn;
    bool in_use;
} frame_map_t;

static frame_map_t *core_map = NULL;
static int global_next_frame = 0;
static int victim_ptr = 0;
static pthread_mutex_t mem_lock = PTHREAD_MUTEX_INITIALIZER;

uint64_t solicitar_frame(thread_params_t *data)
{
    // Inicialización del core_map si es la primera vez
    if (core_map == NULL)
    {
        core_map = calloc(data->config->frames, sizeof(frame_map_t));
    }

    if (!data->config->unsafe)
        pthread_mutex_lock(&mem_lock);

    uint64_t frame;
    if (global_next_frame < data->config->frames)
    {
        frame = global_next_frame++;
    }
    else
    {
        frame = victim_ptr;

        // --- INVALIDACIÓN CRUZADA ---
        frame_map_t *victima = &core_map[frame];
        if (victima->in_use && victima->owner != NULL)
        {
            // 1. Invalidar en su tabla de páginas
            struct page_table *pt = (struct page_table *)victima->owner->table;
            pt->entries[victima->vpn].valid = false;

            // 2. Invalidar en su TLB (llamando a la función que creamos arriba)
            invalidar_entrada_tlb((struct tlb *)victima->owner->tlb, victima->vpn);
        }

        data->metrics.evictions++;
        victim_ptr = (victim_ptr + 1) % data->config->frames;
    }

    // Registrar nuevo dueño
    core_map[frame].owner = data;
    core_map[frame].vpn = data->ultima_vpn_solicitada;
    core_map[frame].in_use = true;

    if (!data->config->unsafe)
        pthread_mutex_unlock(&mem_lock);

    return frame;
}