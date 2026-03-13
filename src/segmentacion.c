#include "../include/segmentacion.h"

/**
 * Traduce una dirección virtual a física usando segmentación.
 * El hardware simulado suma la base y verifica que el offset sea legal.
 */
uint64_t traducir_segmentacion(int seg_id, uint64_t offset, struct segment_table *table, bool *error) {
    // 1. Buscamos la entrada en la tabla de segmentos del hilo
    struct segment_entry entry = table->segments[seg_id];

    // 2. Verificación de Protección: Si el offset supera el límite, es violación de memoria
    if (offset >= entry.limit) {
        *error = true; // Activamos la bandera de Segmentation Fault
        return 0;      // Retornamos dirección nula
    }

    // 3. Si es legal, calculamos la dirección física final
    *error = false;
    uint64_t direccion_fisica = entry.base + offset;
    
    return direccion_fisica;
}
