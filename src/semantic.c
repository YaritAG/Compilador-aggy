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
    new_symbol->scope_id = current_scope; // <--- Se registra con el ámbito en el que se encuentra
    new_symbol->next = symbol_table;      // Se inserta al inicio de la lista
    symbol_table = new_symbol;

    printf("[INFO Semantico]: Variable '%s' registrada exitosamente en ambito %d.\n", name, current_scope);
}

// 2. Buscar una variable activa y visible desde el ámbito actual
Symbol *lookup_symbol(const char *name)
{
    Symbol *current = symbol_table;

    while (current != NULL)
    {
        // Una variable es visible si coincide el nombre Y:
        // - Pertenece al mismo ámbito local exacto en el que estamos parados.
        // - O pertenece al ámbito global (0).
        if (strcmp(current->name, name) == 0)
        {
            if (current->scope_id == current_scope || current->scope_id == 0)
            {
                return current; // Encontrada y accesible de forma legítima
            }
        }
        current = current->next;
    }
    return NULL; // Existe en la tabla pero está en un ámbito privado/muerto, o nunca se declaró
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

    int previous_scope = current_scope;

    // Ajusta según los números reales de tu enum (Nodo Tipo 3 es tu IF)
    if (node->type == 3) // Cambia '3' por tu constante NODE_IF si corresponde
    {
        scope_counter++;
        current_scope = scope_counter;
    }

    // Si el nodo contiene un texto que no sea un operador o número, validamos existencia
    if (node->value != NULL && strlen(node->value) > 0)
    {
        // Validamos si es una cadena alfabética pura (un identificador de variable)
        if ((node->value[0] >= 'a' && node->value[0] <= 'z') || (node->value[0] >= 'A' && node->value[0] <= 'Z'))
        {
            // Ignoramos palabras clave explícitas que puedan venir en nodos genéricos
            if (strcmp(node->value, "true") != 0 && strcmp(node->value, "false") != 0)
            {
                if (lookup_symbol(node->value) == NULL)
                {
                    fprintf(stderr, "[ERROR Semantico]: Variable '%s' usada sin declarar o fuera de su ambito legal.\n", node->value);
                    exit(1);
                }
            }
        }
    }

    // Asegúrate de recorrer TODOS los caminos posibles del nodo
    analyze_semantics(node->left);
    analyze_semantics(node->right);

    current_scope = previous_scope;
}

void liberar_variables_del_ambito(int scope_a_borrar)
{
    Symbol *current = symbol_table;
    Symbol *prev = NULL;

    while (current != NULL)
    {
        if (current->scope_id == scope_a_borrar)
        {
            // Guardamos el nodo que vamos a eliminar
            Symbol *temp = current;

            if (prev == NULL)
            {
                // Si el nodo a borrar es la cabeza de la lista, movemos la cabeza global
                symbol_table = current->next;
                current = symbol_table;
            }
            else
            {
                // Si está en medio o al final, puenteamos el puntero anterior
                prev->next = current->next;
                current = current->next;
            }

            // Liberamos físicamente la memoria del símbolo muerto
            free(temp);
        }
        else
        {
            // Avanzamos normalmente si no pertenece al ámbito expirado
            prev = current;
            current = current->next;
        }
    }
}