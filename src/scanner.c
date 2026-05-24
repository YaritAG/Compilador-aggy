/* * Archivo: src/scanner.c
 * Descripción: Implementación de la lectura de caracteres y clasificación de tokens.
 */

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "../include/scanner.h"

// Variable global (o estática) para controlar la lectura del archivo
static FILE *source_file;
static int current_line = 1;

// Función para inicializar el scanner
void init_scanner(FILE *file)
{
    source_file = file;
}

Token get_next_token()
{
    Token token;
    int c;

    // 1. Saltar espacios en blanco y saltos de línea
    while ((c = fgetc(source_file)) != EOF && isspace(c))
    {
        if (c == '\n')
            current_line++;
    }

    if (c == EOF)
    {
        token.type = TOKEN_EOF;
        return token;
    }

    // 2. Clasificar el carácter (esto es un AFD simplificado)
    if (isalpha(c))
    { // Si empieza con letra, es ID o Palabra Reservada
        int i = 0;
        token.lexeme[i++] = c;
        while (isalnum(c = fgetc(source_file)))
        {
            token.lexeme[i++] = c;
        }
        ungetc(c, source_file); // Devolvemos el último carácter leído
        token.lexeme[i] = '\0';

        // Aquí compararíamos el lexema con palabras reservadas (if, while, etc.)
        token.type = TOKEN_ID; // Simplificación
        return token;
    }

    // 3. Manejo de símbolos simples (simplificado para operadores)
    if (c == '=')
    {
        token.type = TOKEN_ASSIGN;
        strcpy(token.lexeme, "=");
        return token;
    }

    // Aquí continuarías con más if/else para tokens como +, -, {, }, etc.

    token.type = TOKEN_ERROR;
    return token;
}