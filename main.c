/* * Archivo: main.c
 * Descripción: Punto de entrada del compilador AGGY. Valida los argumentos
 * de la línea de comandos e inicia las fases del análisis.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/scanner.h"
#include "include/ast.h"
#include "include/parser.h"
#include "include/semantic.h" // Conectamos el Header Nuevo unificado

/*
 * Función: has_valid_extension
 * Verifica si el nombre del archivo termina estrictamente en ".aggy".
 */
int has_valid_extension(const char *filename)
{
    const char *ext = strrchr(filename, '.');
    if (!ext)
        return 0;
    return (strcmp(ext, ".aggy") == 0);
}

int main(int argc, char *argv[])
{
    // 1. Validar que el usuario haya proporcionado el archivo fuente
    if (argc < 2)
    {
        fprintf(stderr, "[ERROR] Uso correcto: %s <archivo.aggy>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];

    // 2. Validar la extensión del archivo (.aggy)
    if (!has_valid_extension(filename))
    {
        fprintf(stderr, "[ERROR] El archivo '%s' no tiene la extension valida '.aggy'\n", filename);
        return 1;
    }

    // 3. Intentar abrir el archivo en modo lectura
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        fprintf(stderr, "[ERROR] No se pudo abrir o encontrar el archivo '%s'\n", filename);
        return 1;
    }

    printf("[INFO] Leyendo archivo fuente: %s\n", filename);
    printf("-----------------------------------------\n");

    // 4. Inicializar el Scanner con el archivo abierto
    init_scanner(file);

    // 5. Fase 1 y 2: Análisis Léxico y Sintáctico (Construcción del AST)
    ASTNode *root = parse_program();

    if (root != NULL)
    {
        printf("[SUCCESS] El archivo se ha parseado correctamente.\n");
        printf("[INFO] Analisis lexico y sintactico finalizado con exito.\n");
        printf("-----------------------------------------\n");

        // 6. Fase 3: Análisis Semántico (NUEVO)
        printf("[INFO] Iniciando analisis semantico...\n");

        analyze_semantics(root); // Recorremos el árbol buscando errores de lógica

        printf("[SUCCESS] Analisis semantico finalizado sin errores.\n");
        printf("-----------------------------------------\n");
    }
    else
    {
        fprintf(stderr, "[ERROR] No se pudo generar el Arbol de Sintaxis Abstracta (AST).\n");
        fclose(file);
        return 1;
    }

    // 7. Cerrar el flujo del archivo y limpiar la memoria de la Tabla de Símbolos
    fclose(file);
    free_symbol_table(); // Evitamos fugas de memoria limpiando la lista ligada

    return 0;
}