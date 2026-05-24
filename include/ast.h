/* * Archivo: include/ast.h
 * Descripción: Define la estructura del Árbol de Sintaxis Abstracta (AST).
 * Cada instrucción en el código .aggy será un nodo de este tipo en memoria.
 */

#ifndef AST_H
#define AST_H

// Definimos los tipos de nodos posibles para identificar qué operación realiza el compilador
typedef enum {
    NODE_IF,
    NODE_FOR,
    NODE_WHILE,
    NODE_ASSIGN,
    NODE_DECL,    // Declaración de variables (int x;)
    NODE_EXPR,    // Expresiones matemáticas o relacionales
    NODE_BLOCK    // Bloque de código {}
} NodeType;

// Estructura principal del Nodo del AST
typedef struct ASTNode {
    NodeType type;            // Tipo de nodo (ej. IF, FOR, etc.)
    char value[32];           // Almacena el nombre de la variable o valor constante
    
    struct ASTNode *left;     // Hijo izquierdo: usualmente condiciones o lado izquierdo de una asignación
    struct ASTNode *right;    // Hijo derecho: cuerpo del bloque o lado derecho de una operación
    struct ASTNode *next;     // Enlace a la siguiente sentencia en el mismo nivel
} ASTNode;

/*
 * Función: create_node
 * Crea un nuevo nodo en memoria dinámica (Heap).
 * Se usa cada vez que el Parser reconoce una nueva sentencia.
 */
ASTNode* create_node(NodeType type);

/*
 * Función: free_ast
 * Libera recursivamente toda la memoria utilizada por el árbol.
 * Crucial para evitar memory leaks.
 */
void free_ast(ASTNode *node);

#endif // AST_H