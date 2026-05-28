#ifndef AST_H
#define AST_H

// AST (Abstract Syntax Tree) para representar la estructura del programa durante el análisis sintáctico.
/*
    * El AST es una representación jerárquica del código fuente que refleja la estructura gramatical del programa.
    * Cada nodo del AST representa una construcción sintáctica (declaración, asignación, expresión, etc.).
    * El AST se construye durante el análisis sintáctico y se utiliza posteriormente para la generación de código o interpretación.
*/
typedef enum
{
    NODE_BLOCK,
    NODE_DECLARATION,
    NODE_ASSIGN,
    NODE_IF,
    NODE_FOR,
    NODE_DO_WHILE,
    NODE_WHILE,
    NODE_EXPR,
    NODE_PRINT
} NodeType;

// Cada nodo del AST tiene un tipo, punteros a sus hijos (izquierdo y derecho) y un campo para almacenar valores (como identificadores o números).
typedef struct ASTNode
{
    int type;
    char value[100];
    struct ASTNode *left;
    struct ASTNode *right;
    int scope_id; // <-- AGREGA ESTA LÍNEA si no existe
} ASTNode;

// Funciones para crear nodos y liberar memoria del AST.
ASTNode *create_node(NodeType type);
void free_ast(ASTNode *node);

#endif