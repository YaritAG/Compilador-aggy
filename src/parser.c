/* * Archivo: src/parser.c
 * Descripción: Implementación del Analizador Sintáctico (Parser) para .aggy.
 */

#include <stdio.h>
#include <stdlib.h>
#include "../include/parser.h"


static Token lookahead;

void advance()
{
    lookahead = get_next_token();
}

void consume(TokenType expected_type)
{
    if (lookahead.type == expected_type)
    {
        advance();
    }
    else
    {
        fprintf(stderr, "[ERROR] Sintáctico en línea %d: Se esperaba token tipo %d, pero se obtuvo %d\n",
                lookahead.line, expected_type, lookahead.type);
        exit(1);
    }
}

// Prototipos de funciones gramaticales
ASTNode *parse_statement();
ASTNode *parse_declaration();
ASTNode *parse_assignment();
ASTNode *parse_expression();    
ASTNode *parse_if();

/* * Regla: <programa> ::= <sentencia>* */
ASTNode *parse_program()
{
    advance(); // Carga el primer token
    ASTNode *root = create_node(NODE_BLOCK);

    while (lookahead.type != TOKEN_EOF)
    {
        ASTNode *stmt = parse_statement();
        if (stmt)
        {
            // TODO: Aquí conectarías el nodo stmt al nodo root como hijo
        }
    }
    return root;
}

/* * Regla: Selector de sentencias */
ASTNode *parse_statement()
{
    switch (lookahead.type)
    {
    case TOKEN_IF:
        return parse_if();
    case TOKEN_INT:
    case TOKEN_FLOAT:
        return parse_declaration();
    case TOKEN_ID:
        return parse_assignment();
    default:
        fprintf(stderr, "[ERROR] Sentencia no reconocida en línea %d: [%s]\n", lookahead.line, lookahead.lexeme);
        exit(1);
    }
}

/* * Regla: <declaracion> ::= ("int" | "float") <identificador> ";" */
ASTNode *parse_declaration()
{
    ASTNode *node = create_node(NODE_DECLARATION);

    // 1. Consumimos el tipo (int/float)
    advance();

    // 2. Intentamos consumir el ID, si no es ID, reportamos error y detenemos
    if (lookahead.type == TOKEN_ID)
    {
        strcpy(node->value, lookahead.lexeme);
        advance();
    }
    else
    {
        fprintf(stderr, "[ERROR] Sintáctico en línea %d: Se esperaba un identificador después del tipo.\n", lookahead.line);
        exit(1); // Detenemos la compilación ante un error de sintaxis
    }

    // 3. Consumimos el punto y coma
    consume(TOKEN_SEMICOLON);

    return node;
}

/* * Regla: <asignacion> ::= <identificador> "=" <valor> ";" */
/* * Regla: <asignacion> ::= <identificador> "=" <expresion> ";" */
ASTNode *parse_assignment()
{
    ASTNode *node = create_node(NODE_ASSIGN);

    // 1. Guardamos el ID de la variable que está siendo asignada
    if (lookahead.type == TOKEN_ID)
    {
        strcpy(node->value, lookahead.lexeme);
        advance();
    }
    else
    {
        fprintf(stderr, "[ERROR] Sintáctico en línea %d: Se esperaba un identificador.\n", lookahead.line);
        exit(1);
    }

    // 2. Consumimos el operador '='
    consume(TOKEN_ASSIGN);

    // 3. Parseamos la expresión (el valor a asignar)
    // Aquí asignamos el resultado a un hijo del nodo,
    // lo que le da una estructura jerárquica al árbol.
    node->right = parse_expression();

    // 4. Consumimos el punto y coma final
    consume(TOKEN_SEMICOLON);

    return node;
}

// parse_expression sirve para parsear tanto números como expresiones más complejas (en futuras expansiones del lenguaje).
/* * Regla: <expresion> ::= <numero> */
ASTNode *parse_expression()
{
    ASTNode *node = create_node(NODE_EXPR);

    // Verificamos si lo que viene es un número (int o float)
    if (lookahead.type == TOKEN_NUM_INT || lookahead.type == TOKEN_NUM_FLOAT)
    {
        // Guardamos el valor numérico en el nodo
        strcpy(node->value, lookahead.lexeme);
        advance(); // Consumimos el número
    }
    else
    {
        fprintf(stderr, "[ERROR] Sintáctico en línea %d: Se esperaba un valor numérico.\n", lookahead.line);
        exit(1);
    }

    return node;
}

/* * Regla: <if> ::= "if" "(" <id> "==" <valor> ")" <bloque> */
ASTNode *parse_if()
{
    ASTNode *node = create_node(NODE_IF);
    consume(TOKEN_IF);
    consume(TOKEN_LPAREN);
    consume(TOKEN_ID);
    consume(TOKEN_EQ);
    consume(TOKEN_NUM_INT); // O NUM_FLOAT
    consume(TOKEN_RPAREN);
    consume(TOKEN_LBRACE);
    // TODO: Parsear cuerpo del if (recursividad)
    consume(TOKEN_RBRACE);
    return node;
}