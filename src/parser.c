/* * Archivo: src/parser.c
 * Descripción: Implementación del Analizador Sintáctico (Parser) para .aggy.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/parser.h"
#include "../include/semantic.h"

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
ASTNode *parse_print();

void parse_condition();

/* * Regla: <programa> ::= <sentencia>* */
ASTNode *parse_program()
{
    advance(); // Carga el primer token (el 'int' de la primera línea) de forma correcta

    ASTNode *root = create_node(NODE_BLOCK);
    ASTNode *last_stmt = NULL;

    // Mientras no lleguemos al final, seguimos buscando sentencias
    while (lookahead.type != TOKEN_EOF)
    {
        ASTNode *stmt = parse_statement();

        if (stmt != NULL)
        {
            // Si es la primerísima sentencia del programa, la colgamos a la izquierda de la raíz
            if (root->left == NULL)
            {
                root->left = stmt;
            }
            // Si ya había sentencias previas, la enlazamos a la derecha de la última que agregamos
            else
            {
                last_stmt->right = stmt;
            }

            // Actualizamos cuál fue la última sentencia procesada para el siguiente ciclo
            last_stmt = stmt;
        }
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
// -----------------------------------------------------------------------------
/* Print */
// -----------------------------------------------------------------------------
ASTNode *parse_print()
{
    ASTNode *node = create_node(NODE_PRINT); // Asegúrate de tener NODE_PRINT en tu enum de nodos
    consume(TOKEN_PRINT);
    consume(TOKEN_LPAREN);

    // Validar qué se va a imprimir (un ID o un número)
    if (lookahead.type == TOKEN_ID || lookahead.type == TOKEN_NUM_INT || lookahead.type == TOKEN_NUM_FLOAT)
    {
        advance();
    }
    else
    {
        fprintf(stderr, "[ERROR Sintactico en linea %d]: Se esperaba un argumento valido dentro de print().\n", lookahead.line);
        exit(1);
    }

    consume(TOKEN_RPAREN);
    consume(TOKEN_SEMICOLON); // Si tu lenguaje obliga a usar ';' tras el print
    return node;
}

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
    case TOKEN_BOOL: // <--- ¡SOLUCIÓN AL ERROR ACTUAL! Ahora acepta declaraciones bool
        return parse_declaration();

    case TOKEN_ID:
        return parse_assignment();

    case TOKEN_PRINT:         // <--- ¡PREVENCIÓN! Para que no truene al llegar a print(i); o print(total);
        return parse_print(); // O como se llame tu función para parsear impresiones

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

    // Agregamos TOKEN_BOOL a la verificación estricta del tipo
    if (lookahead.type == TOKEN_INT || lookahead.type == TOKEN_FLOAT || lookahead.type == TOKEN_BOOL)
    {
        TokenType current_type = lookahead.type;
        advance(); // Consume el tipo (int, float o bool)

        if (lookahead.type == TOKEN_ID)
        {
            strcpy(node->value, lookahead.lexeme);
            insert_symbol(lookahead.lexeme, (int)current_type);
            advance(); // Consume el ID
        }
        else
        {
            fprintf(stderr, "[ERROR Sintactico en linea %d]: Se esperaba un identificador valido.\n", lookahead.line);
            exit(1);
        }
    }
    else
    {
        fprintf(stderr, "[ERROR Sintactico en linea %d]: Se esperaba 'int', 'float' o 'bool'.\n", lookahead.line);
        exit(1);
    }

    if (lookahead.type == TOKEN_ASSIGN)
    {
        advance();
        parse_expression(); // Procesará el '0', '10.5' o 'true' (si true/false se manejan como expresiones básicas o IDs)
    }

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
    ASTNode *node = NULL;

    // Soporte para el operador unario NOT ('!') antes de cualquier operando
    bool tiene_not = false;
    if (lookahead.type == TOKEN_NOT)
    {
        consume(TOKEN_NOT);
        tiene_not = true;
    }

    // 1. PROCESAR PRIMER OPERANDO (O EXPRESIÓN ENTRE PARÉNTESIS)
    if (lookahead.type == TOKEN_LPAREN)
    {
        consume(TOKEN_LPAREN);
        node = parse_expression(); // Recursividad pura para resolver el interior
        consume(TOKEN_RPAREN);
    }
    else if (lookahead.type == TOKEN_NUM_INT || lookahead.type == TOKEN_NUM_FLOAT ||
             lookahead.type == TOKEN_ID || lookahead.type == TOKEN_TRUE || lookahead.type == TOKEN_FALSE)
    {
        // Creamos un nodo hoja para el operando
        node = create_node(NODE_EXPR);
        strcpy(node->value, lookahead.lexeme);
        advance();
    }
    else
    {
        fprintf(stderr, "[ERROR] Sintactico en linea %d: Se esperaba un valor, variable o '('.\n", lookahead.line);
        exit(1);
    }

    // Si la expresión inicial tenía un '!', envolvemos este operando en un nodo unario
    if (tiene_not)
    {
        ASTNode *not_node = create_node(NODE_EXPR);
        strcpy(not_node->value, "!");
        not_node->left = node; // Colgamos la variable o paréntesis a la izquierda
        node = not_node;       // El nodo principal ahora es la negación
    }

    // 2. EL BUCLE DE OPERADORES: Agregamos TOKEN_AND y TOKEN_OR a la fiesta
    while (lookahead.type == TOKEN_PLUS || lookahead.type == TOKEN_MINUS ||
           lookahead.type == TOKEN_MUL || lookahead.type == TOKEN_DIV ||
           lookahead.type == TOKEN_LT || lookahead.type == TOKEN_GT ||
           lookahead.type == TOKEN_EQ || lookahead.type == TOKEN_NEQ ||
           lookahead.type == TOKEN_LTE || lookahead.type == TOKEN_GTE ||
           lookahead.type == TOKEN_AND || lookahead.type == TOKEN_OR) // <-- ¡NUEVOS OPERADORES!
    {
        // Creamos un nuevo nodo para el operador
        ASTNode *op_node = create_node(NODE_EXPR);
        strcpy(op_node->value, lookahead.lexeme); // Guardamos el operador (+, &&, ||, etc.)
        advance();

        op_node->left = node; // Lo que ya teníamos se convierte en el hijo izquierdo

        // Validamos si el operando de la derecha viene con una negación (ej: a && !b)
        bool right_tiene_not = false;
        if (lookahead.type == TOKEN_NOT)
        {
            consume(TOKEN_NOT);
            right_tiene_not = true;
        }

        // Evaluamos el siguiente término a la derecha
        ASTNode *right_node = NULL;
        if (lookahead.type == TOKEN_LPAREN)
        {
            consume(TOKEN_LPAREN);
            right_node = parse_expression();
            consume(TOKEN_RPAREN);
        }
        else if (lookahead.type == TOKEN_NUM_INT || lookahead.type == TOKEN_NUM_FLOAT ||
                 lookahead.type == TOKEN_ID || lookahead.type == TOKEN_TRUE || lookahead.type == TOKEN_FALSE)
        {
            right_node = create_node(NODE_EXPR);
            strcpy(right_node->value, lookahead.lexeme);
            advance();
        }
        else
        {
            fprintf(stderr, "[ERROR] Sintactico en linea %d: Se esperaba un valor valido despues del operador.\n", lookahead.line);
            exit(1);
        }

        // Si el operando derecho venía negado, lo envolvemos antes de colgarlo en el operador principal
        if (right_tiene_not)
        {
            ASTNode *not_node = create_node(NODE_EXPR);
            strcpy(not_node->value, "!");
            not_node->left = right_node;
            op_node->right = not_node;
        }
        else
        {
            op_node->right = right_node;
        }

        node = op_node; // El nodo raíz actual pasa a ser el árbol combinado del operador
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
    // Creamos el nodo raíz del IF
    ASTNode *if_node = create_node(NODE_IF);
    consume(TOKEN_IF);

    consume(TOKEN_LPAREN);
    if_node->left = parse_expression(); // Rama izquierda = Condición (ej: nota >= 90)
    consume(TOKEN_RPAREN);

    consume(TOKEN_LBRACE);
    // ------------------------------------------
    // CONTROL DE ÁMBITO: CUERPO DEL IF PRINCIPAL
    // ------------------------------------------
    int prev_scope = current_scope;
    scope_counter++;
    current_scope = scope_counter;

    ASTNode *last_stmt = NULL;
    ASTNode *if_body = NULL;
    while (lookahead.type != TOKEN_RBRACE && lookahead.type != TOKEN_EOF)
    {
        ASTNode *stmt = parse_statement();
        if (if_body == NULL)
            if_body = stmt;
        else
            last_stmt->right = stmt;
        last_stmt = stmt;
    }
    consume(TOKEN_RBRACE);
    current_scope = prev_scope; // Salida del ámbito del IF

    // Creamos un nodo conector intermedio para albergar el cuerpo verdadero y el falso
    ASTNode *body_connector = create_node(NODE_BLOCK);
    body_connector->left = if_body;  // Izquierda del bloque = Código si es TRUE
    if_node->right = body_connector; // Colgamos el conector a la derecha del IF principal

    // Guardamos una referencia para saber dónde ir colgando los siguientes ELSEIF o ELSE
    ASTNode *current_parent = body_connector;

    // =========================================================
    // PROCESAR ENCADENAMIENTO DE 'ELSEIF' (ANIDAMIENTO DERECHO)
    // =========================================================
    while (lookahead.type == TOKEN_ELSEIF)
    {
        consume(TOKEN_ELSEIF);

        // Cada elseif es un nuevo nodo NODE_IF completo
        ASTNode *elseif_node = create_node(NODE_IF);

        consume(TOKEN_LPAREN);
        elseif_node->left = parse_expression(); // Condición del elseif (ej: nota >= 80)
        consume(TOKEN_RPAREN);

        consume(TOKEN_LBRACE);
        // ------------------------------------------
        // CONTROL DE ÁMBITO: CUERPO DEL ELSEIF
        // ------------------------------------------
        scope_counter++;
        current_scope = scope_counter;

        ASTNode *elseif_last_stmt = NULL;
        ASTNode *elseif_body = NULL;
        while (lookahead.type != TOKEN_RBRACE && lookahead.type != TOKEN_EOF)
        {
            ASTNode *stmt = parse_statement();
            if (elseif_body == NULL)
                elseif_body = stmt;
            else
                elseif_last_stmt->right = stmt;
            elseif_last_stmt = stmt;
        }
        consume(TOKEN_RBRACE);
        current_scope = prev_scope; // Salida del ámbito

        // Conector para el cuerpo de este elseif
        ASTNode *elseif_connector = create_node(NODE_BLOCK);
        elseif_connector->left = elseif_body; // Si es verdadero, ejecuta su cuerpo
        elseif_node->right = elseif_connector;

        // El elseif completo se vuelve la alternativa del bloque anterior (rama derecha)
        current_parent->right = elseif_node;

        // El nuevo conector se vuelve el padre actual para la siguiente iteración
        current_parent = elseif_connector;
    }

    // =========================================================
    // PROCESAR EL 'ELSE' FINAL (OPCIONAL)
    // =========================================================
    if (lookahead.type == TOKEN_ELSE)
    {
        consume(TOKEN_ELSE);
        consume(TOKEN_LBRACE);

        // ------------------------------------------
        // CONTROL DE ÁMBITO: CUERPO DEL ELSE FINAL
        // ------------------------------------------
        scope_counter++;
        current_scope = scope_counter;

        ASTNode *else_last_stmt = NULL;
        ASTNode *else_body = NULL;
        while (lookahead.type != TOKEN_RBRACE && lookahead.type != TOKEN_EOF)
        {
            ASTNode *stmt = parse_statement();
            if (else_body == NULL)
                else_body = stmt;
            else
                else_last_stmt->right = stmt;
            else_last_stmt = stmt;
        }
        consume(TOKEN_RBRACE);
        current_scope = prev_scope; // Salida del ámbito

        // El cuerpo del else se cuelga directamente en la última rama derecha disponible
        current_parent->right = else_body;
    }

    return if_node;
}

// -----------------------------------------------------------------------------
/* BUCLE DO-WHILE*/
// -----------------------------------------------------------------------------
ASTNode *parse_do_while()
{
    ASTNode *node = create_node(NODE_DO_WHILE);
    consume(TOKEN_DO);
    consume(TOKEN_LBRACE);

    // Cuerpo del DO
    ASTNode *last_stmt = NULL;
    ASTNode *do_body = NULL;

    while (lookahead.type != TOKEN_RBRACE && lookahead.type != TOKEN_EOF)
    {
        ASTNode *stmt = parse_statement();
        if (do_body == NULL)
            do_body = stmt;
        else
            last_stmt->right = stmt;
        last_stmt = stmt;
    }

    consume(TOKEN_RBRACE);
    consume(TOKEN_WHILE);
    consume(TOKEN_LPAREN);

    // Delegamos la condición a la nueva expresión lógica/relacional
    node->left = parse_expression();

    consume(TOKEN_RPAREN);
    consume(TOKEN_SEMICOLON); // El do-while termina en );

    node->right = do_body;
    return node;
}

// -----------------------------------------------------------------------------
/* BUCLE FOR */
// -----------------------------------------------------------------------------
ASTNode *parse_for()
{
    ASTNode *node = create_node(NODE_FOR);
    consume(TOKEN_FOR);
    consume(TOKEN_LPAREN);

    // ==========================================
    // CONTROL DE ÁMBITO: ENTRADA AL CICLO FOR
    // (Aislamos la variable de control 'i')
    // ==========================================
    int prev_scope = current_scope;
    scope_counter++;
    current_scope = scope_counter;

    // 1. Inicialización (ej: int i = 0;)
    if (lookahead.type == TOKEN_INT || lookahead.type == TOKEN_FLOAT || lookahead.type == TOKEN_BOOL)
        parse_declaration();
    else if (lookahead.type == TOKEN_ID)
        parse_assignment();

    // 2. Condición (ej: i < 5;)
    parse_expression();
    consume(TOKEN_SEMICOLON);

    // 3. Incremento (ej: i = i + 1)
    if (lookahead.type == TOKEN_ID)
    {
        advance();
        consume(TOKEN_ASSIGN);
        parse_expression();
    }

    consume(TOKEN_RPAREN);
    consume(TOKEN_LBRACE);

    // 4. Cuerpo del FOR
    ASTNode *last_stmt = NULL;
    ASTNode *for_body = NULL;
    while (lookahead.type != TOKEN_RBRACE && lookahead.type != TOKEN_EOF)
    {
        ASTNode *stmt = parse_statement();
        if (for_body == NULL)
            for_body = stmt;
        else
            last_stmt->right = stmt;
        last_stmt = stmt;
    }

    consume(TOKEN_RBRACE);

    // ==========================================
    // CONTROL DE ÁMBITO: SALIDA DEL CICLO FOR
    // ==========================================
    current_scope = prev_scope;

    node->right = for_body;
    return node;
}

// -----------------------------------------------------------------------------
/* BUCLE WHILE */
// -----------------------------------------------------------------------------
ASTNode *parse_while()
{
    ASTNode *node = create_node(NODE_WHILE);
    consume(TOKEN_WHILE);
    consume(TOKEN_LPAREN);
    node->left = parse_expression();
    consume(TOKEN_RPAREN);

    consume(TOKEN_LBRACE); // <-- Aquí se abre el bloque

    // ==========================================
    // CONTROL DE ÁMBITO: ENTRADA AL BUCE WHILE
    // ==========================================
    int prev_scope = current_scope;
    scope_counter++;
    current_scope = scope_counter;

    ASTNode *last_stmt = NULL;
    ASTNode *while_body = NULL;
    while (lookahead.type != TOKEN_RBRACE && lookahead.type != TOKEN_EOF)
    {
        ASTNode *stmt = parse_statement();
        if (while_body == NULL)
            while_body = stmt;
        else
            last_stmt->right = stmt;
        last_stmt = stmt;
    }

    consume(TOKEN_RBRACE); // <-- Aquí se cierra el bloque

    // ==========================================
    // CONTROL DE ÁMBITO: SALIDA DEL BUCLE WHILE
    // ==========================================
    current_scope = prev_scope;

    node->right = while_body;
    return node;
}
