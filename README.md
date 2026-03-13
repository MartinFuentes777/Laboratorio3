# Simulador de Memoria Virtual - Laboratorio 3

**Autor:** 
**Curso:** Sistemas Operativos  
**Fecha:** 12 de marzo de 2026

## 1. Descripción del Proyecto
Este proyecto consiste en un simulador de memoria virtual que implementa dos esquemas de administración de memoria: segmentación y paginación con TLB.
Para simular la ejecución de procesos se utilizan hilos, los cuales acceden a una memoria física limitada. Cuando la memoria se llena, el sistema aplica una política de reemplazo FIFO para decidir qué marco de memoria debe ser desalojado.

## 2. Requisitos del Sistema
* Sistema Operativo: Ubuntu / Linux.
* Compilador: GCC con soporte para el estándar C11.
* Librerías: pthread para la gestión de hilos concurrentes.

## 3. Instrucciones de Compilación y Ejecución
Para compilar el proyecto, utilice el Makefile incluido en la raíz:
$ make clean && make

### Comandos de Ejecución
* Ejecución estándar (Requerida por PDF):
  $ make run
* Ejecución de experimentos de evaluación:
  $ make reproduce
  (Este comando automatiza los 3 escenarios de prueba y genera reportes JSON en la carpeta out/)
* Ejecución de pruebas unitarias:
  $ make test

## 4. Análisis de Resultados Experimentales

### Experimento 1: Segmentación (Protección de Memoria)
Se validó el esquema de segmentación configurado con 4 segmentos y límites específicos (`1024, 2048, 4096, 8192`).
* **Resultados:** Se registraron **774 SegFaults** sobre 10,000 operaciones.
* **Análisis:** La detección de estos fallos confirma que la lógica de protección de límites (base + offset) opera correctamente, bloqueando accesos a direcciones fuera del espacio de direccionamiento autorizado para cada segmento.

### Experimento 2: Impacto de la TLB (Localidad de Referencia)
Utilizando un patrón de acceso **80-20** y una TLB de tamaño 32 con política FIFO:
* **TLB Hits:** 380
* **Page Faults:** 197
* **Análisis:** La tasa de aciertos refleja la efectividad de la localidad de referencia. Al contar con una TLB, una parte significativa de las traducciones de direcciones virtuales a físicas se resolvió sin necesidad de acceder a la tabla de páginas en memoria principal, optimizando el rendimiento.

### Experimento 3: Thrashing (Saturación de Memoria)
Se simuló un escenario de alta competencia con **8 hilos** y solo **8 marcos físicos** disponibles.
* **Resultados promedio:** ~977 Page Faults por hilo.
* **Análisis:** Este experimento evidencia el fenómeno de **Thrashing**. Con una relación 1:1 entre hilos y marcos físicos, el sistema entra en una disputa constante por los recursos. La alta tasa de fallos de página y desalojos demuestra que la política de reemplazo FIFO está bajo carga extrema, ya que los procesos deben intercambiar páginas de forma continua, degradando el rendimiento.

## 5. Estructura de Carpetas
* src/: Archivos fuente (.c).
* include/: Archivos de cabecera (.h).
* tests/: Pruebas unitarias de módulos aislados.
* out/: Reportes de métricas generados en formato JSON.
