/* * Archivo: src/utils.c
 * Descripción: Implementación de las funciones auxiliares de gestión de memoria.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/ast.h"

/*
 * Función: create_node
 * Reserva memoria para un nuevo nodo y lo inicializa con valores predeterminados.
 */
ASTNode *create_node(NodeType type)
{
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    if (node != NULL)
    {
        node->type = type;
        node->left = NULL;
        node->right = NULL;
        memset(node->value, 0, sizeof(node->value));
    }
    return node;
}

/*
 * Función: free_ast
 * Recorrido post-orden para liberar memoria.
 * Primero libera a los hijos, luego al nodo padre para no perder la referencia.
 */
void free_ast(ASTNode *node) 
{
    if (node == NULL)
        return;

    // 1. Liberar hijos primero (recursividad)
    free_ast(node->left);
    free_ast(node->right);
    
    // 2. Liberar el nodo actual
    free(node);
}