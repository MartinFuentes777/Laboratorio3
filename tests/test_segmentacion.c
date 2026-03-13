#include <stdio.h>
#include <assert.h>
#include "../include/segmentacion.h"

void test_traduccion_legal() {
    struct segment_table table;
    struct segment_entry entries[1] = {{.base = 5000, .limit = 100}};
    table.segments = entries;
    bool error = false;

    // Caso: Acceso dentro del límite (5000 + 50 = 5050)
    uint64_t pa = traducir_segmentacion(0, 50, &table, &error);
    assert(error == false);
    assert(pa == 5050);
    printf("Test Segmentación Legal: PASSED\n");
}

void test_violacion_limite() {
    struct segment_table table;
    struct segment_entry entries[1] = {{.base = 5000, .limit = 100}};
    table.segments = entries;
    bool error = false;

    // Caso: Offset 150 en un segmento de límite 100 (Debe fallar)
    traducir_segmentacion(0, 150, &table, &error);
    assert(error == true);
    printf("Test Violación de Límite: PASSED\n");
}

int main() {
    printf("--- INICIANDO TESTS DE SEGMENTACIÓN ---\n");
    test_traduccion_legal();
    test_violacion_limite();
    return 0;
}
