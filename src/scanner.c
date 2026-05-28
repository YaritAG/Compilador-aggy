/* * Archivo: src/scanner.c
 * Descripción: Implementación completa del Analizador Léxico (Scanner) para .aggy.
 * Lee caracteres del archivo fuente y los agrupa en tokens legibles.
 */

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "../include/scanner.h"

// Variables estáticas para controlar el archivo actual y la línea de ejecución
static FILE *source_file = NULL;
static int current_line = 1;

/*
 * Función: init_scanner
 * Recibe el flujo del archivo abierto desde el main para empezar la lectura.
 */
void init_scanner(FILE *file)
{
    source_file = file;
    current_line = 1;
}

/*
 * Función: identify_keyword
 * Compara una cadena de texto para determinar si es una palabra reservada o un ID.
 */
TokenType identify_keyword(char *lexeme)
{
    if (strcmp(lexeme, "if") == 0)
        return TOKEN_IF;
    if (strcmp(lexeme, "else") == 0)
        return TOKEN_ELSE;
    if (strcmp(lexeme, "elseif") == 0) // <-- Ahora sí llegará aquí limpiamente
        return TOKEN_ELSEIF;
    if (strcmp(lexeme, "while") == 0)
        return TOKEN_WHILE;
    if (strcmp(lexeme, "do") == 0)
        return TOKEN_DO;
    if (strcmp(lexeme, "for") == 0)
        return TOKEN_FOR;
    if (strcmp(lexeme, "int") == 0)
        return TOKEN_INT;
    if (strcmp(lexeme, "float") == 0)
        return TOKEN_FLOAT;
    if (strcmp(lexeme, "bool") == 0)
        return TOKEN_BOOL;
    if (strcmp(lexeme, "true") == 0)
        return TOKEN_TRUE;
    if (strcmp(lexeme, "false") == 0)
        return TOKEN_FALSE;
    if (strcmp(lexeme, "print") == 0)
        return TOKEN_PRINT;

    // Si no coincide con ninguna, es un identificador de variable/función creado por el usuario
    return TOKEN_ID;
}

/*
 * Función: get_next_token
 * El motor del Scanner. Analiza el flujo de caracteres y devuelve el siguiente Token.
 */
/*
 * Función: get_next_token
 * El motor del Scanner. Analiza el flujo de caracteres y devuelve el siguiente Token.
 */
Token get_next_token()
{
    Token token;
    token.line = current_line;
    int c;

    // 1. Ignorar espacios en blanco, tabulaciones, registrar saltos de línea Y COMENTARIOS
    while (1)
    {
        c = fgetc(source_file);

        if (c == EOF)
        {
            break;
        }

        if (isspace(c))
        {
            if (c == '\n')
            {
                current_line++;
                token.line = current_line;
            }
            continue;
        }

        if (c == '/')
        {
            int next_c = fgetc(source_file);
            if (next_c == '/')
            {
                while ((c = fgetc(source_file)) != '\n' && c != EOF)
                    ;
                if (c == '\n')
                {
                    current_line++;
                    token.line = current_line;
                }
                continue;
            }
            else
            {
                ungetc(next_c, source_file);
                break;
            }
        }

        break;
    }

    if (c == EOF)
    {
        token.type = TOKEN_EOF;
        strcpy(token.lexeme, "EOF");
        return token;
    }

    // 2. Estado: Identificadores y Palabras Reservadas
    if (isalpha(c) || c == '_')
    {
        int i = 0;
        token.lexeme[i++] = c;

        while (isalnum(c = fgetc(source_file)) || c == '_')
        {
            if (i < 31)
                token.lexeme[i++] = c;
        }
        ungetc(c, source_file);
        token.lexeme[i] = '\0';

        token.type = identify_keyword(token.lexeme);
        return token;
    }

    // 3. Estado: Números enteros y flotantes (Dígitos)
    if (isdigit(c))
    {
        int i = 0;
        token.lexeme[i++] = c;
        int is_float = 0;

        while (isdigit(c = fgetc(source_file)) || c == '.')
        {
            if (c == '.')
            {
                if (is_float)
                {
                    break;
                }
                is_float = 1;
            }
            if (i < 31)
                token.lexeme[i++] = c;
        }
        ungetc(c, source_file);
        token.lexeme[i] = '\0';

        token.type = is_float ? TOKEN_NUM_FLOAT : TOKEN_NUM_INT;
        return token;
    }

    // 4. Estado: Operadores y Delimitadores con Lookahead
    token.lexeme[0] = c;
    token.lexeme[1] = '\0';

    switch (c)
    {
    case '=':
        c = fgetc(source_file);
        if (c == '=')
        {
            token.type = TOKEN_EQ;
            strcpy(token.lexeme, "==");
        }
        else
        {
            ungetc(c, source_file);
            token.type = TOKEN_ASSIGN;
        }
        return token;

    // =========================================================
    // MODIFICADO: Ahora '!' suelto es un operador NOT booleano
    // =========================================================
    case '!':
        c = fgetc(source_file);
        if (c == '=')
        {
            token.type = TOKEN_NEQ;
            strcpy(token.lexeme, "!=");
        }
        else
        {
            ungetc(c, source_file);
            token.type = TOKEN_NOT; // <-- Cambiado de TOKEN_ERROR a TOKEN_NOT
        }
        return token;

    // =========================================================
    // NUEVO: Operador Lógico AND (&&)
    // =========================================================
    case '&':
        c = fgetc(source_file);
        if (c == '&')
        {
            token.type = TOKEN_AND;
            strcpy(token.lexeme, "&&");
        }
        else
        {
            ungetc(c, source_file);
            token.type = TOKEN_ERROR; // Un solo '&' no es válido en aggy
        }
        return token;

    // =========================================================
    // NUEVO: Operador Lógico OR (||)
    // =========================================================
    case '|':
        c = fgetc(source_file);
        if (c == '|')
        {
            token.type = TOKEN_OR;
            strcpy(token.lexeme, "||");
        }
        else
        {
            ungetc(c, source_file);
            token.type = TOKEN_ERROR; // Un solo '|' no es válido en aggy
        }
        return token;

    case '<':
        c = fgetc(source_file);
        if (c == '=')
        {
            token.type = TOKEN_LTE;
            strcpy(token.lexeme, "<=");
        }
        else
        {
            ungetc(c, source_file);
            token.type = TOKEN_LT;
        }
        return token;

    case '>':
        c = fgetc(source_file);
        if (c == '=')
        {
            token.type = TOKEN_GTE;
            strcpy(token.lexeme, ">=");
        }
        else
        {
            ungetc(c, source_file);
            token.type = TOKEN_GT;
        }
        return token;

    case '+':
        token.type = TOKEN_PLUS;
        return token;
    case '-':
        token.type = TOKEN_MINUS;
        return token;
    case '*':
        token.type = TOKEN_MUL;
        return token;
    case '/':
        token.type = TOKEN_DIV;
        return token;

    case ';':
        token.type = TOKEN_SEMICOLON;
        return token;
    case '(':
        token.type = TOKEN_LPAREN;
        return token;
    case ')':
        token.type = TOKEN_RPAREN;
        return token;
    case '{':
        token.type = TOKEN_LBRACE;
        return token;
    case '}':
        token.type = TOKEN_RBRACE;
        return token;
    }

    token.type = TOKEN_ERROR;
    return token;
}