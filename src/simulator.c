#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include "../include/simulator.h"
#include "../include/workloads.h"
#include "../include/paginacion.h"
#include "../include/segmentacion.h"
#include "../include/tlb.h"
#include "../include/frame_allocator.h"

// --- VARIABLE GLOBAL CRÍTICA ---
// Este puntero permite que el frame_allocator acceda a las tablas 
// de todos los hilos para invalidar páginas cuando hay desalojo.
thread_params_t *global_threads_data;

// Función para guardar los resultados finales en el formato JSON pedido
void generar_resumen_json(config_t *config, metrics_t *global, double runtime, double throughput) {
    FILE *file = fopen("out/summary.json", "w");
    if (!file) return;
    fprintf(file, "{\n  \"mode\": \"%s\",\n  \"metrics\": {\n", config->mode);
    fprintf(file, "    \"tlb_hits\": %d,\n    \"page_faults\": %d,\n", global->tlb_hits, global->page_faults);
    fprintf(file, "    \"evictions\": %d,\n    \"segfaults\": %d\n  },\n", global->evictions, global->segfaults);
    fprintf(file, "  \"runtime_sec\": %.3f,\n  \"throughput_ops_sec\": %.2f\n}\n", runtime, throughput);
    fclose(file);
}

// Función que ejecuta cada hilo (simula un proceso)
void* ejecutar_hilo(void* arg) {
    thread_params_t *data = (thread_params_t*)arg;
    
    // Inicializamos la semilla propia para que los resultados sean reproducibles
    srand(data->config->seed + data->id);

    for (int i = 0; i < data->config->ops_per_thread; i++) {
        // 1. Generar una dirección virtual según el workload (Uniforme o 80-20)
        uint64_t va = generar_direccion(data);
        
        if (strcmp(data->config->mode, "page") == 0) {
            // Modo Paginación
            traducir_paginacion(va, data);
        } else {
            // Modo Segmentación
            // Extraemos un ID de segmento simple (bits altos) y el offset
            int seg_id = (va >> 10) % 4; 
            uint64_t offset = va % 1024;
            bool error = false;
            traducir_segmentacion(seg_id, offset, (struct segment_table*)data->table, &error);
            if (error) data->metrics.segfaults++;
        }
    }
    return NULL;
}

int main(int argc, char** argv) {
    // Valores por defecto según enunciado
    config_t config = {"page", 4, 1000, "uniform", 42, false, 64, 32, 4096, 16};
    
    // Parseo de argumentos de consola
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--mode") == 0) strcpy(config.mode, argv[++i]);
        if (strcmp(argv[i], "--threads") == 0) config.threads = atoi(argv[++i]);
        if (strcmp(argv[i], "--ops_per_thread") == 0) config.ops_per_thread = atoi(argv[++i]);
        if (strcmp(argv[i], "--workload") == 0) strcpy(config.workload, argv[++i]);
        if (strcmp(argv[i], "--seed") == 0) config.seed = atoi(argv[++i]);
        if (strcmp(argv[i], "--unsafe") == 0) config.unsafe = true;
        if (strcmp(argv[i], "--frames") == 0) config.frames = atoi(argv[++i]);
        if (strcmp(argv[i], "--pages") == 0) config.pages = atoi(argv[++i]);
    }

    // Preparar el espacio global para los hilos
    global_threads_data = malloc(sizeof(thread_params_t) * config.threads);
    pthread_t threads[config.threads];

    // Inicialización de estructuras para cada hilo
    for (int i = 0; i < config.threads; i++) {
        global_threads_data[i].id = i;
        global_threads_data[i].config = &config;
        memset(&global_threads_data[i].metrics, 0, sizeof(metrics_t));

        if (strcmp(config.mode, "page") == 0) {
            // Crear Tabla de Páginas
            struct page_table *pt = malloc(sizeof(struct page_table));
            pt->entries = calloc(config.pages, sizeof(struct page_table_entry));
            global_threads_data[i].table = pt;

            // Crear TLB propia
            struct tlb *t = malloc(sizeof(struct tlb));
            t->size = config.tlb_size;
            t->entries = calloc(config.tlb_size, sizeof(struct tlb_entry));
            t->next_replace = 0;
            global_threads_data[i].tlb = t;
        } else {
            // Crear Tabla de Segmentos
            struct segment_table *st = malloc(sizeof(struct segment_table));
            st->segments = malloc(sizeof(struct segment_entry) * 4);
            for(int j=0; j<4; j++) {
                st->segments[j].base = j * 1024;
                st->segments[j].limit = 256; // Límite pequeño para probar SegFaults
            }
            global_threads_data[i].table = st;
        }
    }

    // --- INICIO DE LA SIMULACIÓN ---
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < config.threads; i++) {
        pthread_create(&threads[i], NULL, ejecutar_hilo, &global_threads_data[i]);
    }

    // Consolidación de métricas
    metrics_t resumen_global = {0};
    printf("\n--- Reporte por Hilo ---\n");
    for (int i = 0; i < config.threads; i++) {
        pthread_join(threads[i], NULL);
        printf("Thread %d | Hits: %d | Faults: %d | SegF: %d\n", 
               i, global_threads_data[i].metrics.tlb_hits, 
               global_threads_data[i].metrics.page_faults,
               global_threads_data[i].metrics.segfaults);
        
        resumen_global.tlb_hits += global_threads_data[i].metrics.tlb_hits;
        resumen_global.page_faults += global_threads_data[i].metrics.page_faults;
        resumen_global.evictions += global_threads_data[i].metrics.evictions;
        resumen_global.segfaults += global_threads_data[i].metrics.segfaults;
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    double total_time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    double throughput = (config.threads * config.ops_per_thread) / total_time;

    printf("\n==== SIMULACIÓN FINALIZADA ====\n");
    printf("Throughput: %.2f ops/sec | Tiempo Total: %.3fs\n", throughput, total_time);

    generar_resumen_json(&config, &resumen_global, total_time, throughput);

    // Limpieza
    free(global_threads_data);
    
    return 0;
}
