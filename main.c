/* * Archivo: main.c
 * Descripción: Punto de entrada del compilador AGGY. Valida los argumentos
 * de la línea de comandos e inicia las fases del análisis.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/scanner.h"

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
        fprintf(stderr, "[ERROR] El archivo '%s' no tiene la extensión válida '.aggy'\n", filename);
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

    // 5. Fase de Prueba: Bucle para imprimir tokens en consola
    Token token;
    do
    {
        token = get_next_token();

        // Imprimimos el tipo numérico del token, su línea y el lexema real
        printf("Línea %d | Tipo Token: %2d | Lexema: [%s]\n",
               token.line, token.type, token.lexeme);

        if (token.type == TOKEN_ERROR)
        {
            fprintf(stderr, "[LEXICAL ERROR] Carácter no reconocido en la línea %d\n", token.line);
        }

    } while (token.type != TOKEN_EOF);

    printf("-----------------------------------------\n");
    printf("[INFO] Análisis léxico finalizado con éxito.\n");

    // 6. Cerrar el flujo del archivo de forma segura
    fclose(file);
    return 0;
}