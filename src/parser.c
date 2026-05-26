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

// Función para consumir un token esperado y avanzar, o reportar un error si no coincide.
void consume(TokenType expected_type)
{
    // Si el token actual coincide con el tipo esperado, avanzamos al siguiente token.
    if (lookahead.type == expected_type)
    {
        advance();
    }

    // Si el token no coincide, reportamos un error sintáctico con detalles claros.
    else
    {
        // El mensaje de error incluye la línea, el token esperado (en formato legible) y el token real que se encontró.
        fprintf(stderr, "[ERROR] Sintactico en linea %d: Se esperaba '%s', pero se obtuvo '%s'\n",
                lookahead.line, get_token_name(expected_type), lookahead.lexeme);
        exit(1);
    }
}

// Prototipos de funciones gramaticales
// Estos sirven para organizar el código y evitar warnings de funciones no declaradas.

ASTNode *parse_statement();
ASTNode *parse_declaration();
ASTNode *parse_assignment();
ASTNode *parse_expression();    
ASTNode *parse_if();
ASTNode *parse_for();
ASTNode *parse_while();
ASTNode *parse_do_while();

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
/*
    * Aquí es donde se decide qué tipo de sentencia estamos parseando.
    * Dependiendo del token que veamos, llamaremos a la función de parseo correspondiente
    * El parse_statement se convierte en el "cerebro" del parser, ya que es el punto de entrada para todas las sentencias.
    * En pocas palabras, es el encargado de dirigir el flujo del análisis sintáctico hacia la función correcta según el token que se encuentre.
    * POr ende, es crucial que este selector esté bien implementado para que el parser funcione correctamente y pueda manejar todas las estructuras del lenguaje.
*/
ASTNode *parse_statement()
{
    switch (lookahead.type)
    {
    case TOKEN_IF:
        return parse_if();

    case TOKEN_FOR:
        return parse_for();

    case TOKEN_WHILE: 
        return parse_while();

    case TOKEN_DO:
        return parse_do_while();

    case TOKEN_INT:
    case TOKEN_FLOAT:
        return parse_declaration();

    case TOKEN_ID:
        return parse_assignment();

    default:
        fprintf(stderr, "[ERROR] Sintactico en linea %d: Se esperaba una sentencia, pero se obtuvo '%s'\n",
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
        fprintf(stderr, "[ERROR] Sintactico en linea %d: Se esperaba un identificador.\n", lookahead.line);
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
        fprintf(stderr, "[ERROR] Se esperaba un operador de comparacion, se obtuvo '%s'\n", lookahead.lexeme);
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
        fprintf(stderr, "[ERROR] Sintactico en linea %d: Se esperaba un valor numerico.\n", lookahead.line);
        exit(1);
    }

    return node;
}

// -----------------------------------------------------------------------------
/* CONDICION IF*/
// -----------------------------------------------------------------------------

/* * Regla: <if> ::= "if" "(" <id> "==" <valor> ")" <bloque> */
// el parse_if es el encargado de construir toda la estructura del if-else, incluyendo la condición y los bloques internos.
ASTNode *parse_if()
{
    // Creamos el nodo IF, que será el padre de toda la estructura del if-else
    ASTNode *node = create_node(NODE_IF);
    consume(TOKEN_IF);
    consume(TOKEN_LPAREN);

    // Aquí validamos la condición del if (ej. a == 10)
    if (lookahead.type == TOKEN_ID || lookahead.type == TOKEN_NUM_INT || lookahead.type == TOKEN_NUM_FLOAT)
        advance();
    else
    {
        // Si no es un ID o número válido, el mensaje de error será claro
        fprintf(stderr, "[ERROR] Error en condicion IF\n");
        exit(1);
    }

    // Validamos el operador relacional (==, !=, >, <, >=, <=)
    if (lookahead.type == TOKEN_EQ || lookahead.type == TOKEN_NEQ ||
        lookahead.type == TOKEN_LT || lookahead.type == TOKEN_GT ||
        lookahead.type == TOKEN_LTE || lookahead.type == TOKEN_GTE)
        advance();
    else
    {
        // Si no es un operador relacional válido, el mensaje de error será claro
        fprintf(stderr, "[ERROR] Se esperaba operador relacional\n");
        exit(1);
    }

    // Validamos el segundo operando de la condición (puede ser un ID o un número)
    if (lookahead.type == TOKEN_ID || lookahead.type == TOKEN_NUM_INT || lookahead.type == TOKEN_NUM_FLOAT)
        advance();
    else
    {
        // Si no es un ID o número válido, el mensaje de error será claro
        fprintf(stderr, "[ERROR] Error en condicion IF\n");
        exit(1);
    }

    // Consumimos el cierre del paréntesis de la condición
    // Si el lookahead no es un TOKEN_RPAREN, el mensaje de error será claro
    consume(TOKEN_RPAREN);
    consume(TOKEN_LBRACE);

    // Bloque interno del IF (Hijo izquierdo)
    // Aquí es donde se construye la estructura jerárquica del árbol.
    ASTNode *last_stmt = NULL;

    // Mientras no lleguemos al cierre de llave, seguimos parseando sentencias dentro del bloque del IF
    while (lookahead.type != TOKEN_RBRACE && lookahead.type != TOKEN_EOF)
    {
        // Cada sentencia dentro del bloque del IF se convierte en un nodo hijo del nodo IF.
        ASTNode *stmt = parse_statement();
        
        // Si el nodo IF no tiene un hijo izquierdo, asignamos la primera sentencia ahí.
        if (node->left == NULL)
            node->left = stmt;
        
        // Para las siguientes sentencias, las enlazamos a la derecha del último nodo agregado.
        else
            last_stmt->right = stmt;
        last_stmt = stmt;
    }

    // Consumimos la llave de cierre del bloque del IF
    consume(TOKEN_RBRACE);

    // --- AQUÍ SE SOPORTA ELSE Y ELSE IF ---
    if (lookahead.type == TOKEN_ELSE)
    {
        consume(TOKEN_ELSE);
        if (lookahead.type == TOKEN_IF)
        {
            // Es un 'else if', encadenamos recursivamente llamando a parse_if()
            node->right = parse_if();
        }
        else
        {
            // Es un 'else' normal, abrimos llave y procesamos su bloque
            consume(TOKEN_LBRACE);
            ASTNode *else_last = NULL;

            // Mientras no lleguemos al cierre de llave, seguimos parseando sentencias dentro del bloque del ELSE
            while (lookahead.type != TOKEN_RBRACE && lookahead.type != TOKEN_EOF)
            {
                // Cada sentencia dentro del bloque del ELSE se convierte en un nodo hijo del nodo ELSE (que es el hijo derecho del IF).
                ASTNode *stmt = parse_statement();
                if (node->right == NULL)
                    node->right = stmt;

                // Para las siguientes sentencias, las enlazamos a la derecha del último nodo agregado en el bloque del ELSE.
                else
                    else_last->right = stmt;
                else_last = stmt;
            }

            // Consumimos la llave de cierre del bloque del ELSE
            consume(TOKEN_RBRACE);
        }
    }

    // Al final, el nodo IF tendrá su condición y su bloque de sentencias correctamente estructurados en el árbol.
    return node;
}

// -----------------------------------------------------------------------------
/* BUCLE DO-WHILE*/
// -----------------------------------------------------------------------------
ASTNode *parse_do_while()
{
    // El nodo DO_WHILE será el padre de toda la estructura del do-while
    ASTNode *node = create_node(NODE_DO_WHILE);
    // Consumimos el 'do' y la llave de apertura del bloque del do-while
    consume(TOKEN_DO);
    consume(TOKEN_LBRACE);

    // Cuerpo del DO
    ASTNode *last_stmt = NULL;
    // Mientras no lleguemos al cierre de llave, seguimos parseando sentencias dentro del bloque del DO
    while (lookahead.type != TOKEN_RBRACE && lookahead.type != TOKEN_EOF)
    {
        // Cada sentencia dentro del bloque del DO se convierte en un nodo hijo del nodo DO_WHILE.
        ASTNode *stmt = parse_statement();
        if (node->left == NULL)
            node->left = stmt;
        
        // Para las siguientes sentencias, las enlazamos a la derecha del último nodo agregado.
        else
            last_stmt->right = stmt;
        last_stmt = stmt;
    }

    // Consumimos la llave de cierre del bloque del DO
    consume(TOKEN_RBRACE);

    // Condición final del WHILE
    consume(TOKEN_WHILE);
    consume(TOKEN_LPAREN);
    advance();
    advance();
    advance(); // Condición sintáctica
    consume(TOKEN_RPAREN);
    consume(TOKEN_SEMICOLON); // El do-while obliga un ';' al final

    // Al final, el nodo DO_WHILE tendrá su bloque de sentencias en el hijo izquierdo y la condición (en futuras expansiones) podría almacenarse en un campo específico del nodo.
    return node;
}

// -----------------------------------------------------------------------------
/* BUCLE FOR */
// -----------------------------------------------------------------------------
ASTNode *parse_for()
{
    // El nodo FOR será el padre de toda la estructura del for
    ASTNode *node = create_node(NODE_FOR);
    consume(TOKEN_FOR);
    consume(TOKEN_LPAREN);

    // 1. Inicialización (Ej: i = 0;)
    parse_assignment(); // Llama a tu función que ya procesa "i = 0;" (ya consume el ';')

    // 2. Condición (Ej: i < 10)
    // Aquí validamos la condición del for (ej. i < 10)
    advance();
    advance();
    advance(); // Condición sintáctica relacional
    consume(TOKEN_SEMICOLON);

    // 3. Incremento (Ej: i = i + 1)
    // Para el incremento, simplemente avanzamos hasta el cierre del paréntesis, ya que no es obligatorio procesar su estructura interna para este ejemplo.    
    while (lookahead.type != TOKEN_RPAREN && lookahead.type != TOKEN_EOF)
    {
        // Avanzamos hasta el cierre del paréntesis, sin necesidad de procesar su estructura interna para este ejemplo.
        advance();
    }
    // Consumimos el cierre del paréntesis del for
    consume(TOKEN_RPAREN);
    consume(TOKEN_LBRACE);

    // Cuerpo del FOR
    ASTNode *last_stmt = NULL;

    // Mientras no lleguemos al cierre de llave, seguimos parseando sentencias dentro del bloque del FOR
    while (lookahead.type != TOKEN_RBRACE && lookahead.type != TOKEN_EOF)
    {
        // Cada sentencia dentro del bloque del FOR se convierte en un nodo hijo del nodo FOR.
        ASTNode *stmt = parse_statement();
        if (node->left == NULL)
            node->left = stmt;
        // Para las siguientes sentencias, las enlazamos a la derecha del último nodo agregado.
        else
            last_stmt->right = stmt;
        last_stmt = stmt;
    }
    // Consumimos la llave de cierre del bloque del FOR
    consume(TOKEN_RBRACE);
    return node;
}

// -----------------------------------------------------------------------------
/* BUCLE WHILE */
// -----------------------------------------------------------------------------
ASTNode *parse_while()
{
    // El nodo WHILE será el padre de toda la estructura del while
    ASTNode *node = create_node(NODE_WHILE);
    consume(TOKEN_WHILE);
    consume(TOKEN_LPAREN);

    // Validación sintáctica simple de la condición (Ej: i < 5)
    if (lookahead.type == TOKEN_ID || lookahead.type == TOKEN_NUM_INT || lookahead.type == TOKEN_NUM_FLOAT)
        advance();
    // Si no es un ID o número válido, el mensaje de error será claro
    else
    {
        // Si no es un ID o número válido, el mensaje de error será claro
        fprintf(stderr, "[ERROR] Error en condicion WHILE\n");
        exit(1);
    }

    // Validamos el operador relacional (==, !=, >, <, >=, <=)
    if (lookahead.type == TOKEN_EQ || lookahead.type == TOKEN_NEQ ||
        lookahead.type == TOKEN_LT || lookahead.type == TOKEN_GT ||
        lookahead.type == TOKEN_LTE || lookahead.type == TOKEN_GTE)
        // Si no es un operador relacional válido, el mensaje de error será claro
        advance();
    // Validamos el segundo operando de la condición (puede ser un ID o un número)
    else
    {
        // Si no es un operador relacional válido, el mensaje de error será claro
        fprintf(stderr, "[ERROR] Se esperaba operador relacional en WHILE\n");
        exit(1);
    }

    // Validamos el segundo operando de la condición (puede ser un ID o un número)
    if (lookahead.type == TOKEN_ID || lookahead.type == TOKEN_NUM_INT || lookahead.type == TOKEN_NUM_FLOAT)
        // Si no es un ID o número válido, el mensaje de error será claro
        advance();
    // Consumimos el cierre del paréntesis de la condición    
    else
    {
        // Si no es un ID o número válido, el mensaje de error será claro
        fprintf(stderr, "[ERROR] Error en condicion WHILE\n");
        exit(1);
    }

    // Consumimos el cierre del paréntesis del while
    consume(TOKEN_RPAREN);
    //  Consumimos la llave de apertura del bloque del while
    consume(TOKEN_LBRACE);

    // Cuerpo del WHILE (Estructura recursiva para soportar anidamiento)
    ASTNode *last_stmt = NULL;
    // Mientras no lleguemos al cierre de llave, seguimos parseando sentencias dentro del bloque del WHILE
    while (lookahead.type != TOKEN_RBRACE && lookahead.type != TOKEN_EOF)
    {
        // Cada sentencia dentro del bloque del WHILE se convierte en un nodo hijo del nodo WHILE.
        ASTNode *stmt = parse_statement();
        if (node->left == NULL)
            node->left = stmt;
        // Para las siguientes sentencias, las enlazamos a la derecha del último nodo agregado.
        else
            last_stmt->right = stmt;
        last_stmt = stmt;
    }
    // Consumimos la llave de cierre del bloque del WHILE
    consume(TOKEN_RBRACE);
    return node;
}