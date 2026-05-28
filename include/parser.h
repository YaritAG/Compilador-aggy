/* * Archivo: include/parser.h
 * Descripción: Interfaz del analizador sintáctico.
 */

#ifndef PARSER_H
#define PARSER_H

#include "scanner.h"
#include <stdbool.h>
#include "ast.h"

// Función principal que inicia el proceso de parseo
ASTNode *parse_program();

// Funciones para forzar el consumo de tokens y manejo de errores
void consume(TokenType expected_type);
void error(const char *message);

#endif