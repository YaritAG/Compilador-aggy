/*
    semantic.c - Implementación del Análisis Semántico para el compilador Aggy

    En este archivo se implementan las funciones necesarias para realizar el análisis semántico del código fuente. 
    Esto incluye la gestión de la Tabla de Símbolos, que es una estructura de datos que almacena información sobre las variables declaradas en el programa (nombre, tipo, etc.), y la función principal analyze_semantics que recorre el Árbol de Sintaxis Abstracta (AST) para validar las reglas semánticas del lenguaje.

    El análisis semántico es crucial para garantizar que el código fuente no solo sea sintácticamente correcto, sino también tenga sentido desde el punto de vista del significado del programa. Por ejemplo, se verifica que las variables estén declaradas antes de usarse, que los tipos de datos sean compatibles en las operaciones, y que se cumplan otras reglas específicas del lenguaje.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "semantic.h"

// Puntero global a la cabeza de nuestra Tabla de Símbolos
Symbol *symbol_table = NULL;

// 1. Insertar una variable en la tabla
void insert_symbol(const char *name, int type)
{
    // Primero revisamos si ya existe para evitar duplicados
    if (lookup_symbol(name) != NULL)
    {
        fprintf(stderr, "[ERROR Semantico]: La variable '%s' ya ha sido declarada.\n", name);
        exit(1);
    }

    // Crear el nuevo símbolo
    Symbol *new_symbol = (Symbol *)malloc(sizeof(Symbol));
    strcpy(new_symbol->name, name);
    new_symbol->type = type;
    new_symbol->next = symbol_table; // Lo ponemos al inicio de la lista
    symbol_table = new_symbol;

    printf("[INFO Semantico]: Variable '%s' registrada exitosamente.\n", name);
}

// 2. Buscar si una variable ya existe en la tabla
Symbol *lookup_symbol(const char *name)
{
    Symbol *current = symbol_table;
    while (current != NULL)
    {
        if (strcmp(current->name, name) == 0)
        {
            return current; // Encontrada
        }
        current = current->next;
    }
    return NULL; // No existe
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

// 4. El Analizador Semántico (Recorrido del AST)
void analyze_semantics(ASTNode *node)
{
    if (node == NULL)
        return;

    // Aquí empezaremos a analizar qué tipo de nodo es
    switch (node->type)
    {
        // En los siguientes pasos agregaremos la lógica para verificar declaraciones y asignaciones

    default:
        break;
    }

    // Recorrer recursivamente el resto del árbol
    analyze_semantics(node->left);
    analyze_semantics(node->right);
}