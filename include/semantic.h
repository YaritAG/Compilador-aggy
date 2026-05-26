#ifndef SEMANTIC_H
#define SEMANTIC_H

// Análisis Semántico para el compilador Aggy
/*  
    * El Análisis Semántico es la fase del compilador que se encarga de validar el significado del código fuente más allá de su estructura sintáctica.
    * En esta etapa, se verifica que las variables estén declaradas antes de usarse, que los tipos de datos sean compatibles en las operaciones, y que se cumplan las reglas semánticas del lenguaje.
    * Para esto, se suele utilizar una Tabla de Símbolos que almacena información sobre las variables (nombre, tipo, etc.) y se recorre el AST para realizar estas validaciones.
*/
#include "tokens.h"
#include "parser.h" // Para poder recibir el ASTNode

// Estructura para un símbolo (variable) en la tabla
typedef struct Symbol
{
    char name[50];       // Nombre de la variable (Ej: "i", "total")
    int type;            // Tipo de token (TOKEN_INT o TOKEN_FLOAT)
    struct Symbol *next; // Puntero al siguiente símbolo (lista ligada)
} Symbol;

// Prototipos de funciones para la Tabla de Símbolos
// Estas funciones permiten insertar nuevos símbolos, buscar símbolos existentes y liberar la memoria de la tabla.
void insert_symbol(const char *name, int type);

// La función lookup_symbol devuelve un puntero al símbolo encontrado o NULL si no existe, lo que es crucial para validar el uso de variables en el código.
Symbol *lookup_symbol(const char *name);
void free_symbol_table();

// Función para realizar el análisis semántico principal, que recorre el AST y valida las reglas semánticas del lenguaje.
// Prototipo del Analizador Semántico principal
void analyze_semantics(ASTNode *node);

#endif // SEMANTIC_H