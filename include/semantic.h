#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "tokens.h"

// DECLARACIÓN ADELANTADA: Rompe la dependencia circular con parser.h
// Le avisa al compilador que la estructura existe sin necesidad de incluir todo el archivo.
struct ASTNode;
typedef struct ASTNode ASTNode;

// Estructura para un símbolo (variable) en la tabla
typedef struct Symbol
{
    char name[50];       // Nombre de la variable (Ej: "i", "total")
    int type;
    int scope_id;        // ID del ámbito (0 para global, 1 para local)
    struct Symbol *next; // Puntero al siguiente símbolo (lista ligada)
} Symbol;

extern int current_scope;
extern int scope_counter;

// Prototipos de funciones para la Tabla de Símbolos
void insert_symbol(const char *name, int type);
Symbol *lookup_symbol(const char *name);
void free_symbol_table();
void liberar_variables_del_ambito(int scope_a_borrar);

// Prototipo del Analizador Semántico principal
void analyze_semantics(ASTNode *node);

#endif // SEMANTIC_H