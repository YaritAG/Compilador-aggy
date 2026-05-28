#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/ast.h"
#include "../include/semantic.h"
#include <stdbool.h>

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
    new_symbol->scope_id = current_scope;
    new_symbol->active = 1; // <-- ¡Nace completamente viva!
    new_symbol->next = symbol_table;
    symbol_table = new_symbol;

    printf("[INFO Semantico]: Variable '%s' registrada exitosamente en ambito %d.\n", name, current_scope);
}

// 2. Buscar una variable activa y visible desde el ámbito actual
Symbol *lookup_symbol(const char *name)
{
    Symbol *current = symbol_table;
    while (current != NULL)
    {
        if (strcmp(current->name, name) == 0 && current->active == 1)
        {
            // Si está activa, es totalmente válida y accesible
            return current;
        }
        current = current->next;
    }
    return NULL;
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
    // Si el nodo es nulo, detenemos la recursión de esta rama
    if (node == NULL)
        return;

    // Mantenemos tus hermosas trazas para la presentación del proyecto
    printf("[SEMANTICO VISITANDO]: Tipo: %d | Valor: '%s'\n", node->type, node->value ? node->value : "");

    // 1. RECURSIÓN: Recorremos primero el hijo izquierdo
    analyze_semantics(node->left);

    // 2. RECURSIÓN: Recorremos después el hijo derecho
    analyze_semantics(node->right);
}

void liberar_variables_del_ambito(int scope)
{
    Symbol *current = symbol_table;
    while (current != NULL)
    {
        if (current->scope_id == scope)
        {
            current->active = 0; // <-- Apagamos la variable, ya no es accesible
        }
        current = current->next;
    }
}