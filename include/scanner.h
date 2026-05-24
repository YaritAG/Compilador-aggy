#ifndef SCANNER_H
#define SCANNER_H

#include "tokens.h" // ¡ESTO ES LO QUE FALTABA!

typedef struct
{
    TokenType type; // Ahora C ya sabe qué es TokenType
    char lexeme[32];
    int line;
} Token;

Token get_next_token();
void init_scanner(FILE *file);

#endif