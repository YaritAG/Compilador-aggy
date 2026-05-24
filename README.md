# Compilador-aggy

##  Descripción del Proyecto
El objetivo principal de este desarrollo es el diseño y la implementación de la fase de Front-End de un compilador. Este sistema tiene la tarea de procesar el código fuente, analizar su estructura y validar su lógica interna antes de permitir cualquier paso posterior de traducción.  

El lenguaje **.aggy** es un lenguaje de programación de propósito general, fuertemente tipado, con una sintaxis inspirada en Java y C. Este compilador realiza el proceso completo de traducción: desde la lectura del archivo fuente hasta la validación semántica, utilizando un **Árbol de Sintaxis Abstracta (AST)** para representar la lógica del programa.

##  Organización del Proyecto
El compilador está estructurado para ser modular, facilitando su mantenimiento y escalabilidad:

```text
/proyecto-compilador-aggy
│
├── Makefile              # Automatización de la compilación (gcc)
├── main.c                # Punto de entrada y validación CLI
│
├── include/              # Encabezados (.h)
│   ├── ast.h             # Estructura de nodos del AST
│   ├── tokens.h          # Definición de tokens y categorías léxicas
│   └── symbols.h         # Estructura de la tabla de símbolos
│
└── src/                  # Código fuente (.c)
    ├── scanner.c         # Analizador léxico (Tokenización)
    ├── parser.c          # Analizador sintáctico (Gramática BNF)
    ├── semantic.c        # Análisis semántico y verificación de tipos
    └── utils.c           # Funciones auxiliares y manejo de memoria
