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

    // Si no coincide con ninguna, es un identificador de variable/función creado por el usuario
    return TOKEN_ID;
}

/*
 * Función: get_next_token
 * El motor del Scanner. Analiza el flujo de caracteres y devuelve el siguiente Token.
 */
Token get_next_token()
{
    Token token;
    token.line = current_line;
    int c;

    // 1. Ignorar espacios en blanco, tabulaciones y registrar saltos de línea
    while ((c = fgetc(source_file)) != EOF && isspace(c))
    {
        if (c == '\n')
        {
            current_line++;
            token.line = current_line;
        }
    }

    // Si llegamos al final del archivo, retornar TOKEN_EOF
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
        ungetc(c, source_file); // Devolver el carácter sobrante
        token.lexeme[i] = '\0';

        // Validar si es palabra reservada o ID
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
                    // Si encuentra un segundo punto, es un error de formato flotante
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

    // 4. Estado: Operadores y Delimitadores con Lookahead (Ver un carácter adelante)
    token.lexeme[0] = c;
    token.lexeme[1] = '\0';

    switch (c)
    {
    // Operadores de asignación e igualdad
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

    // Operadores de desigualdad
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
            token.type = TOKEN_ERROR; // Un '!' solo no significa nada en .aggy por ahora
        }
        return token;

    // Menor que y Menor o igual
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

    // Mayor que y Mayor o igual
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

    // Operadores aritméticos simples
    case '+':
        token.type = TOKEN_PLUS;
        return token;
    case '-':
        token.type = TOKEN_MINUS;
        return token;
    case '*':
        token.type = TOKEN_MULT;
        return token;
    case '/':
        token.type = TOKEN_DIV;
        return token;

    // Delimitadores
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

    // Si llega aquí, es un carácter que no pertenece al alfabeto de .aggy
    token.type = TOKEN_ERROR;
    return token;
}