#ifndef TLB_H
#define TLB_H

#include "simulator.h"

struct tlb_entry
{
    uint64_t vpn;
    uint64_t frame_number;
    bool valid;
};

struct tlb
{
    struct tlb_entry *entries;
    int size;
    int next_replace; // Puntero para política FIFO
};

// Operaciones de la caché
bool buscar_en_tlb(uint64_t vpn, thread_params_t *data);
void actualizar_tlb(uint64_t vpn, uint64_t frame, thread_params_t *data);
uint64_t obtener_frame_tlb(uint64_t vpn, thread_params_t *data);
void invalidar_entrada_tlb(struct tlb *tlb_a_limpiar, uint64_t vpn);

#endif
