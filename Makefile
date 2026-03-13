CC = gcc
# CFLAGS incluye -pthread para hilos y -Iinclude para las cabeceras
CFLAGS = -std=c11 -pthread -Wall -Wextra -Iinclude
SRC_DIR = src
INC_DIR = include
OBJ_DIR = src
OUT_DIR = out
TEST_DIR = tests

# Archivos fuente del simulador principal
SRC = $(SRC_DIR)/simulator.c \
      $(SRC_DIR)/paginacion.c \
      $(SRC_DIR)/segmentacion.c \
      $(SRC_DIR)/tlb.c \
      $(SRC_DIR)/frame_allocator.c \
      $(SRC_DIR)/workloads.c \
      $(SRC_DIR)/utils.c

# Generación automática de nombres de archivos objeto (.o)
OBJ = $(SRC:.c=.o)
TARGET = simulator

# --- REGLAS DE COMPILACIÓN ---

# Regla por defecto: crea carpetas y compila el binario
all: $(OUT_DIR) $(TARGET)

# Crea la carpeta de salida si no existe
$(OUT_DIR):
	mkdir -p $(OUT_DIR)

# Enlace del binario final
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# Compilación de cada archivo .c a .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@


# Ejecución simple con flags por defecto
run: all
	./$(TARGET) --mode page --threads 4 --ops_per_thread 100 --stats

# Reproducción de los 3 experimentos del enunciado
reproduce: all
	@rm -f $(OUT_DIR)/*.json
	@echo ">> Ejecutando Experimento 1: Segmentacion (Segfaults)"
	./$(TARGET) --mode seg --threads 1 --workload uniform --ops-per-thread 10000 --segments 4 --seg-limits 1024,2048,4096,8192 --seed 100 --stats
	mv $(OUT_DIR)/summary.json $(OUT_DIR)/experimento1_seg.json
	
	@echo ">> Ejecutando Experimento 2: Impacto de TLB (Pattern 80-20)"
	./$(TARGET) --mode page --threads 1 --workload 80-20 --ops-per-thread 50000 --pages 128 --frames 64 --page-size 4096 --tlb-size 32 --tlb-policy fifo --seed 200 --stats
	mv $(OUT_DIR)/summary.json $(OUT_DIR)/experimento2_tlb.json
	
	@echo ">> Ejecutando Experimento 3: Thrashing (Competencia de hilos)"
	./$(TARGET) --mode page --threads 8 --workload uniform --ops-per-thread 10000 --pages 64 --frames 8 --page-size 4096 --tlb-size 16 --seed 300 --stats
	mv $(OUT_DIR)/summary.json $(OUT_DIR)/experimento3_thrashing.json

# Limpieza de archivos temporales y carpeta out
clean:
	rm -f $(SRC_DIR)/*.o $(TARGET)
	rm -rf $(OUT_DIR)

.PHONY: all clean run reproduce test
