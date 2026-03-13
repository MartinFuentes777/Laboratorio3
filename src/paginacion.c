#define _DEFAULT_SOURCE
#include <unistd.h>
#include <stdlib.h>
#include "../include/paginacion.h"
#include "../include/tlb.h"
#include "../include/frame_allocator.h"

uint64_t traducir_paginacion(uint64_t va, thread_params_t *data) {
    // Descomponemos la dirección virtual en VPN (página) y Offset
    uint64_t vpn = va / data->config->page_size;
    uint64_t offset = va % data->config->page_size;
    struct page_table *pt = (struct page_table*)data->table;

    // PASO 1: Consultar la TLB (Caché local del hilo)
    if (buscar_en_tlb(vpn, data)) {
        data->metrics.tlb_hits++;
        return (obtener_frame_tlb(vpn, data) * data->config->page_size) + offset;
    }

    // PASO 2: TLB Miss. Consultar Tabla de Páginas
    // Si la entrada no es válida (o fue invalidada por otro hilo), ocurre un Page Fault
    if (!pt->entries[vpn].valid) {
        data->metrics.page_faults++;
        
        // Anotamos la VPN que queremos cargar para el frame_allocator
        data->ultima_vpn_solicitada = vpn;

        // Simulamos la latencia de disco (1-5ms) requerida por el PDF
        usleep((rand() % 4000) + 1000); 

        // Pedimos un marco físico (aquí es donde podemos desalojar a otros)
        uint64_t frame = solicitar_frame(data);
        
        // Cargamos la página en la tabla
        pt->entries[vpn].frame_number = frame;
        pt->entries[vpn].valid = true;
    }

    // PASO 3: Actualizar la TLB con la nueva traducción para futuros accesos
    actualizar_tlb(vpn, pt->entries[vpn].frame_number, data);

    // Retornamos dirección física final
    return (pt->entries[vpn].frame_number * data->config->page_size) + offset;
}
