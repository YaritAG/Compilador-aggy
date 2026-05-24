/* * Archivo: src/utils.c
 * Descripción: Implementación de las funciones auxiliares de gestión de memoria.
 */

#include <stdio.h>
#include <stdlib.h>
#include "../include/ast.h"

/*
 * Función: create_node
 * Reserva memoria para un nuevo nodo y lo inicializa con valores predeterminados.
 */
ASTNode *create_node(NodeType type)
{
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));

    if (node == NULL)
    {
        fprintf(stderr, "[ERROR]: Fallo al asignar memoria para un nuevo nodo.\n");
        exit(1); // Terminamos si no hay memoria disponible (error crítico)
    }

    node->type = type;
    node->left = NULL;
    node->right = NULL;
    node->next = NULL;

    // Inicializamos el valor como una cadena vacía
    node->value[0] = '\0';

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

    // Liberación recursiva
    free_ast(node->left);
    free_ast(node->right);
    free_ast(node->next);

    // Finalmente, liberamos el nodo actual
    free(node);
}