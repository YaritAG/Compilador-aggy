#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/ast.h"
#include "../include/semantic.h"

// Puntero global a la cabeza de nuestra Tabla de Símbolos
Symbol *symbol_table = NULL;

// Variables globales para el control de ámbitos (Definición)
int current_scope = 0; // Ámbito actual (0 = Global)
int scope_counter = 0; // Contador correlativo para generar IDs de ámbitos únicos

// 1. Insertar una variable en la tabla respetando el ámbito
void insert_symbol(const char *name, int type)
{
    // Buscamos si ya existe la variable EN EL MISMO ÁMBITO ACTUAL
    Symbol *current = symbol_table;
    while (current != NULL)
    {
        if (strcmp(current->name, name) == 0 && current->scope_id == current_scope)
        {
            fprintf(stderr, "[ERROR Semantico]: La variable '%s' ya ha sido declarada en este ambito.\n", name);
            exit(1);
        }
        current = current->next;
    }

    // Crear el nuevo símbolo si pasó la validación
    Symbol *new_symbol = (Symbol *)malloc(sizeof(Symbol));
    strcpy(new_symbol->name, name);
    new_symbol->type = type;
    new_symbol->scope_id = current_scope; // <--- Se registra con el ámbito en el que se encuentra
    new_symbol->next = symbol_table;      // Se inserta al inicio de la lista
    symbol_table = new_symbol;

    printf("[INFO Semantico]: Variable '%s' registrada exitosamente en ambito %d.\n", name, current_scope);
}

// 2. Buscar una variable activa y visible desde el ámbito actual
Symbol *lookup_symbol(const char *name)
{
    Symbol *current = symbol_table;

    // Al recorrer desde el inicio de la lista, inspeccionamos primero las variables
    // locales más recientes debido a la inserción al inicio (LIFO).
    while (current != NULL)
    {
        // Una variable es visible si coincide el nombre Y:
        // - Pertenece al mismo ámbito local actual.
        // - O pertenece al ámbito global (0).
        if (strcmp(current->name, name) == 0 && (current->scope_id == current_scope || current->scope_id == 0))
        {
            return current; // Encontrada y accesible
        }
        current = current->next;
    }
    return NULL; // No visible o no declarada
}

// 3. Liberar la memoria de la tabla al terminar
void free_symbol_table()
{
    Symbol *current = symbol_table;
    while (current != NULL)
    {
        Symbol *temp = current;
        current = current->next;
        free(temp);
    }
}

// 4. El Analizador Semántico con control dinámico de ámbitos (Recorrido del AST)
void analyze_semantics(ASTNode *node)
{
    if (node == NULL)
        return;

    // Guardamos el ámbito previo antes de procesar cualquier nodo
    int previous_scope = current_scope;

    // Detectamos si este nodo representa la apertura de un bloque con sus propias llaves { }
    // Modifica o añade tipos de nodo según correspondan en tu enum ast.h
    if (node->type == NODE_IF || node->type == NODE_WHILE || node->type == NODE_DO_WHILE || node->type == NODE_FOR)
    {
        scope_counter++;
        current_scope = scope_counter; // Entramos a un nuevo sub-ámbito único
    }

    // Validación de usos/asignaciones
    if (node->type == NODE_ASSIGN)
    {
        if (lookup_symbol(node->value) == NULL)
        {
            fprintf(stderr, "[ERROR Semantico]: Variable '%s' usada sin declarar.\n", node->value);
            exit(1);
        }
    }

    // Continuar recorrido recursivo por los subárboles
    analyze_semantics(node->left);
    analyze_semantics(node->right);

    // Al regresar del recorrido de este nodo, restauramos el ámbito padre
    current_scope = previous_scope;
}