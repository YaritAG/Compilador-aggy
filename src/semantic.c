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
    if (node == NULL)
        return;

    // Tu detector de tráfico súper útil para seguir depurando
    printf("[SEMANTICO VISITANDO]: Tipo: %d | Valor: '%s'\n", node->type, node->value ? node->value : "NULL");

    int previous_scope = current_scope;

    // 1. Manejo de ámbitos dinámicos (Tipo 3: Bloques IF/WHILE)
    if (node->type == 3)
    {
        scope_counter++;
        current_scope = scope_counter;
    }

    // 2. REGLA DE ORO SEMÁNTICA CON FILTROS DE PROTECCIÓN:
    // Evaluamos nodos Tipo 2 (asignación) y Tipo 7 (expresiones/condiciones)
    // ====================================================================
    // REGLA SEMÁNTICA TOTAL BLINDADA
    // ====================================================================
    if (node->value != NULL && strlen(node->value) > 0)
    {
        // 1. Si es un nodo de DECLARACIÓN (Tipo 1), NO lo validamos como error de uso
        // porque la variable apenas está naciendo aquí.
        if (node->type != 1)
        {
            // Si el valor empieza con una letra (identificador potencial)
            if ((node->value[0] >= 'a' && node->value[0] <= 'z') || (node->value[0] >= 'A' && node->value[0] <= 'Z'))
            {
                // Filtro de palabras reservadas
                if (strcmp(node->value, "true") != 0 &&
                    strcmp(node->value, "false") != 0 &&
                    strcmp(node->value, "int") != 0 &&
                    strcmp(node->value, "float") != 0 &&
                    strcmp(node->value, "print") != 0 &&
                    strcmp(node->value, "if") != 0 &&
                    strcmp(node->value, "while") != 0)
                {
                    // Validamos si la variable que se intenta USAR es legal y está activa
                    if (lookup_symbol(node->value) == NULL)
                    {
                        fprintf(stderr, "[ERROR Semantico]: Variable '%s' usada sin declarar o fuera de su ambito legal.\n", node->value);
                        exit(1);
                    }
                }
            }
        }
    }

    // 3. Recorremos primero el cuerpo interno del bloque (hijo izquierdo)
    analyze_semantics(node->left);

    // 4. Al terminar de procesar el cuerpo interno, apagamos las variables de ese ámbito
    if (node->type == 3)
    {
        liberar_variables_del_ambito(current_scope);
    }

    // 5. Restauramos el ámbito padre antes de movernos de forma secuencial al resto del programa
    current_scope = previous_scope;

    // 6. Recorremos el resto del programa o instrucciones siguientes (hijo derecho)
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