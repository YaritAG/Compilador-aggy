/* * Archivo: src/parser.c
 * Descripción: Implementación de la gramática del lenguaje .aggy.
 */

#include <stdio.h>
#include <stdlib.h>
#include "../include/parser.h"

static Token lookahead; // El token que estamos analizando actualmente

// Obtiene el siguiente token del scanner
void advance()
{
    lookahead = get_next_token();
}

// Verifica que el token actual sea el esperado
void consume(TokenType expected_type)
{
    if (lookahead.type == expected_type)
    {
        advance();
    }
    else
    {
        fprintf(stderr, "[ERROR] Sintáctico: Se esperaba un token diferente en línea %d\n", lookahead.line);
        exit(1);
    }
}

// Inicia el proceso: lee tokens hasta encontrar EOF
ASTNode *parse_program()
{
    advance(); // Carga el primer token

    ASTNode *root = create_node(NODE_BLOCK);
    // Aquí iría la lógica para procesar todas las sentencias del programa
    // Mientras no sea EOF, llamamos a parse_statement()

    return root;
}