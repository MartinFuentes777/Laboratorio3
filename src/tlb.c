#include <stdbool.h>
#include "../include/tlb.h"
#include <stddef.h>

/* * Revisa si la VPN (Virtual Page Number) ya está en la caché rápida.
 * Esto evita tener que consultar la Tabla de Páginas en RAM.
 */
bool buscar_en_tlb(uint64_t vpn, thread_params_t *data)
{
    struct tlb *mi_tlb = (struct tlb *)data->tlb;

    for (int i = 0; i < mi_tlb->size; i++)
    {
        // La entrada debe ser válida y coincidir con la página buscada
        if (mi_tlb->entries[i].valid && mi_tlb->entries[i].vpn == vpn)
        {
            return true;
        }
    }
    return false;
}

/* * Devuelve el marco físico (frame) guardado en la TLB.
 * Se llama solo si buscar_en_tlb devolvió true.
 */
uint64_t obtener_frame_tlb(uint64_t vpn, thread_params_t *data)
{
    struct tlb *mi_tlb = (struct tlb *)data->tlb;
    for (int i = 0; i < mi_tlb->size; i++)
    {
        if (mi_tlb->entries[i].vpn == vpn)
        {
            return mi_tlb->entries[i].frame_number;
        }
    }
    return 0;
}

/* * Inserta una nueva traducción en la TLB.
 * Como es FIFO, usamos un puntero 'next_replace' que da vueltas por el arreglo.
 */
void actualizar_tlb(uint64_t vpn, uint64_t frame, thread_params_t *data)
{
    struct tlb *mi_tlb = (struct tlb *)data->tlb;
    int idx = mi_tlb->next_replace;

    mi_tlb->entries[idx].vpn = vpn;
    mi_tlb->entries[idx].frame_number = frame;
    mi_tlb->entries[idx].valid = true;

    // Movemos el puntero al siguiente espacio (circular)
    mi_tlb->next_replace = (idx + 1) % mi_tlb->size;
}

void invalidar_entrada_tlb(struct tlb *tlb_a_limpiar, uint64_t vpn)
{
    if (tlb_a_limpiar == NULL)
        return;
    for (int i = 0; i < tlb_a_limpiar->size; i++)
    {
        if (tlb_a_limpiar->entries[i].valid && tlb_a_limpiar->entries[i].vpn == vpn)
        {
            tlb_a_limpiar->entries[i].valid = false; // Invalidación exitosa
        }
    }
}
