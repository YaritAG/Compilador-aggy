/* * Archivo: src/parser.c
 * Descripción: Implementación del Analizador Sintáctico (Parser) para .aggy.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/parser.h"


static Token lookahead;

void advance()
{
    lookahead = get_next_token();
}

// Funcion auxiliar para obtener el nombre legible de un token (útil para mensajes de error)
const char *get_token_name(TokenType type)
{
    switch (type)
    {
    case TOKEN_INT:
        return "int";
    case TOKEN_FLOAT:
        return "float";
    case TOKEN_ID:
        return "identificador";
    case TOKEN_ASSIGN:
        return "=";
    case TOKEN_NUM_INT:
        return "número entero";
    case TOKEN_NUM_FLOAT:
        return "número flotante";
        case TOKEN_IF:
        return "if";
    case TOKEN_GT:
        return ">";
    case TOKEN_LT:
        return "<";
    case TOKEN_EQ:
        return "==";
    case TOKEN_LBRACE:
        return "{";
    case TOKEN_RBRACE:
        return "}";
    case TOKEN_LPAREN:
        return "(";
    case TOKEN_RPAREN:
        return ")";
    case TOKEN_SEMICOLON:
        return ";";
    case TOKEN_EOF:
        return "EOF";

    default:
        return "desconocido";
    }
}

void consume(TokenType expected_type)
{
    if (lookahead.type == expected_type)
    {
        advance();
    }
    else
    {
        fprintf(stderr, "[ERROR] Sintáctico en línea %d: Se esperaba '%s', pero se obtuvo '%s'\n",
                lookahead.line, get_token_name(expected_type), lookahead.lexeme);
        exit(1);
    }
}

// Prototipos de funciones gramaticales
ASTNode *parse_statement();
ASTNode *parse_declaration();
ASTNode *parse_assignment();
ASTNode *parse_expression();    
ASTNode *parse_if();
void parse_condition();

/* * Regla: <programa> ::= <sentencia>* */
ASTNode *parse_program()
{
    advance(); // Carga el primer token (el 'int' de la primera línea)

    ASTNode *root = create_node(NODE_BLOCK);

    // Mientras no lleguemos al final, seguimos buscando sentencias
    while (lookahead.type != TOKEN_EOF)
    {
        ASTNode *stmt = parse_statement();
        // TODO: Enlazar stmt a root
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
        // Aquí es donde el parser "se queja" cuando no sabe qué hacer
        fprintf(stderr, "[ERROR] Sintáctico en línea %d: Se esperaba una sentencia, pero se obtuvo '%s'\n",
                lookahead.line, lookahead.lexeme);
        exit(1);
    }
}

/* * Regla: <declaracion> ::= ("int" | "float") <identificador> ";" */
ASTNode *parse_declaration()
{
    ASTNode *node = create_node(NODE_DECLARATION);
    advance(); // Consume int o float

    // Aquí guardamos el ID
    if (lookahead.type == TOKEN_ID)
    {
        strcpy(node->value, lookahead.lexeme);
        advance();
    }

    // Consume el '=' y la expresión si existen
    if (lookahead.type == TOKEN_ASSIGN)
    {
        advance();          // Consume '='
        parse_expression(); // Consume el valor (10 o 3.14)
    }

    consume(TOKEN_SEMICOLON); // Consume el ';'
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

// Función para parsear condiciones dentro de sentencias if o while (en futuras expansiones del lenguaje).
// Ejemplo de cómo deberías capturar la condición en parse_if:
void parse_condition()
{
    consume(TOKEN_ID); // Identificador (ej. 'a')

    // Aquí validamos cualquier operador relacional
    if (lookahead.type == TOKEN_GT || lookahead.type == TOKEN_LT ||
        lookahead.type == TOKEN_GTE || lookahead.type == TOKEN_LTE ||
        lookahead.type == TOKEN_EQ || lookahead.type == TOKEN_NEQ)
    {
        advance(); // Consumimos el operador ('>', '==', etc.)
    }
    else
    {
        // Si no es ninguno de estos, el error será claro
        fprintf(stderr, "[ERROR] Se esperaba un operador de comparación, se obtuvo '%s'\n", lookahead.lexeme);
        exit(1);
    }

    consume(TOKEN_ID); // O TOKEN_NUM_INT, según lo que permitas
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

    // 1. Primer operando: Puede ser una variable (ID) o un número
    if (lookahead.type == TOKEN_ID)
    {
        consume(TOKEN_ID);
    }
    else if (lookahead.type == TOKEN_NUM_INT)
    {
        consume(TOKEN_NUM_INT);
    }
    else if (lookahead.type == TOKEN_NUM_FLOAT)
    {
        consume(TOKEN_NUM_FLOAT);
    }
    else
    {
        fprintf(stderr, "[ERROR] Sintáctico en línea %d: Se esperaba un identificador o número en la condición, pero se obtuvo '%s'\n",
                lookahead.line, lookahead.lexeme);
        exit(1);
    }

    // 2. MODIFICACIÓN CRUCIAL: Aceptar cualquier operador relacional
    if (lookahead.type == TOKEN_EQ || lookahead.type == TOKEN_NEQ ||
        lookahead.type == TOKEN_LT || lookahead.type == TOKEN_GT ||
        lookahead.type == TOKEN_LTE || lookahead.type == TOKEN_GTE)
    {
        advance(); // Consume el operador relacional que venga (ej: '>')
    }
    else
    {
        fprintf(stderr, "[ERROR] Sintáctico en línea %d: Se esperaba un operador relacional (==, !=, <, >, <=, >=), pero se obtuvo '%s'\n",
                lookahead.line, lookahead.lexeme);
        exit(1);
    }

    // 3. Segundo operando: Igual, puede ser otra variable o un número
    if (lookahead.type == TOKEN_ID)
    {
        consume(TOKEN_ID);
    }
    else if (lookahead.type == TOKEN_NUM_INT)
    {
        consume(TOKEN_NUM_INT);
    }
    else if (lookahead.type == TOKEN_NUM_FLOAT)
    {
        consume(TOKEN_NUM_FLOAT);
    }
    else
    {
        fprintf(stderr, "[ERROR] Sintáctico en línea %d: Se esperaba un identificador o número después del operador, pero se obtuvo '%s'\n",
                lookahead.line, lookahead.lexeme);
        exit(1);
    }

    consume(TOKEN_RPAREN);
    consume(TOKEN_LBRACE);

    // Bucle para conectar las sentencias internas (este se queda igual)
    ASTNode *last_stmt = NULL;
    while (lookahead.type != TOKEN_RBRACE && lookahead.type != TOKEN_EOF)
    {
        ASTNode *stmt = parse_statement();

        if (node->left == NULL)
        {
            node->left = stmt;
        }
        else
        {
            last_stmt->right = stmt;
        }
        last_stmt = stmt;
    }

    consume(TOKEN_RBRACE);
    return node;
}