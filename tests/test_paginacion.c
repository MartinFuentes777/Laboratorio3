#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "../include/tlb.h"

void test_tlb_hit() {
    thread_params_t data;
    struct tlb mi_tlb;
    struct tlb_entry entradas[2];
    
    mi_tlb.size = 2;
    mi_tlb.entries = entradas;
    mi_tlb.next_replace = 0;
    data.tlb = &mi_tlb;

    // Insertamos manualmente una traducción (VPN 10 -> Frame 5)
    actualizar_tlb(10, 5, &data);

    // Verificamos si la TLB la encuentra
    assert(buscar_en_tlb(10, &data) == true);
    assert(obtener_frame_tlb(10, &data) == 5);
    
    printf("Test TLB Hit: PASSED\n");
}

void test_tlb_fifo_replacement() {
    thread_params_t data;
    struct tlb mi_tlb;
    struct tlb_entry entradas[2]; // Solo 2 espacios
    
    mi_tlb.size = 2;
    mi_tlb.entries = entradas;
    mi_tlb.next_replace = 0;
    data.tlb = &mi_tlb;

    actualizar_tlb(1, 100, &data); // Entra en pos 0
    actualizar_tlb(2, 200, &data); // Entra en pos 1
    actualizar_tlb(3, 300, &data); // Debe reemplazar a la VPN 1 (FIFO)

    assert(buscar_en_tlb(1, &data) == false); // Ya no debe estar
    assert(buscar_en_tlb(3, &data) == true);  // Nueva entrada
    
    printf("Test TLB FIFO Replacement: PASSED\n");
}

int main() {
    printf("--- INICIANDO TESTS DE PAGINACIÓN ---\n");
    test_tlb_hit();
    test_tlb_fifo_replacement();
    return 0;
}
