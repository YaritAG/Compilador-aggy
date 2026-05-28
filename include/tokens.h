#ifndef TOKENS_H
#define TOKENS_H

typedef enum
{
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_WHILE,
    TOKEN_DO,
    TOKEN_FOR,
    TOKEN_INT,
    TOKEN_FLOAT,
    TOKEN_ID,
    TOKEN_NUM_INT,
    TOKEN_NUM_FLOAT,
    TOKEN_ASSIGN,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MUL,
    TOKEN_DIV,
    TOKEN_LT,
    TOKEN_GT,
    TOKEN_LTE,
    TOKEN_GTE,
    TOKEN_EQ,
    TOKEN_NEQ,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_SEMICOLON,
    TOKEN_ERROR,
    TOKEN_BOOL,  // Para la palabra clave 'bool'
    TOKEN_TRUE,  // Para el valor 'true'
    TOKEN_FALSE, // Para el valor 'false'
    TOKEN_PRINT,
    TOKEN_AND,
    TOKEN_OR,
    TOKEN_NOT,
    TOKEN_ELSEIF,

    TOKEN_EOF

} TokenType;

#endif