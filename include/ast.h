#ifndef AST_H
#define AST_H

typedef enum
{
    NODE_BLOCK,
    NODE_DECLARATION,
    NODE_ASSIGN,
    NODE_IF,
    NODE_WHILE,
    NODE_EXPR
} NodeType;

typedef struct ASTNode
{
    NodeType type;
    struct ASTNode *left;
    struct ASTNode *right;
    char value[32]; // Para guardar identificadores o números
} ASTNode;

ASTNode *create_node(NodeType type);
void free_ast(ASTNode *node);

#endif