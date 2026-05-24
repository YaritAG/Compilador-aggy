# Compilador-aggy

##  Descripción del Proyecto
El objetivo principal de este desarrollo es el diseño y la implementación de la fase de Front-End de un compilador. Este sistema tiene la tarea de procesar el código fuente, analizar su estructura y validar su lógica interna antes de permitir cualquier paso posterior de traducción.  

El lenguaje **.aggy** es un lenguaje de programación de propósito general, fuertemente tipado, con una sintaxis inspirada en Java y C. Este compilador realiza el proceso completo de traducción: desde la lectura del archivo fuente hasta la validación semántica, utilizando un **Árbol de Sintaxis Abstracta (AST)** para representar la lógica del programa.

##  Organización del Proyecto
El compilador está estructurado para ser modular, facilitando su mantenimiento y escalabilidad:

``text
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

# Explicación de la funcionalidad de los códigos

## Archivo `include/ast.h`

Este archivo es fundamental porque define cómo se verá cada pieza del código en memoria. Sin esto, el analizador sintáctico no tendría un lugar donde guardar los resultados.

### s¿Por qué empezamos aquí?
- **Modularidad:** Ahora que tienes ast.h, cuando escribas parser.c, simplemente escribirás #include "../include/ast.h" y ya tendrás acceso a la estructura que define el árbol.

- **Preparación para el Parser:** El parser.c no hace más que tomar tokens y llenar estos ASTNode.

- **Control de Memoria:** Al incluir las funciones create_node y free_ast en el encabezado, te obligas a ti mismo a implementar la gestión de memoria desde el primer día, evitando que tu proyecto se vuelva un caos de memory leaks al final- 

## Archivo `src/utils.c`

Este archivo es vital porque aquí es donde "cobran vida" las funciones que declaramos en ast.h. Como te comentaba, la clave de un compilador robusto es gestionar correctamente la memoria dinámica (malloc/free).

### ¿Por qué este archivo es necesario?
- **Seguridad:** Incluimos un if (node == NULL) que protege al programa de fallos. Si tu sistema se queda sin memoria, el compilador lo dirá claramente en lugar de cerrarse sin avisar.

- **Limpieza Profunda (free_ast):** El recorrido post-orden (primero los hijos, luego el padre) es la forma estándar de eliminar árboles en C. Si intentaras borrar el padre primero, perderías la dirección de memoria de los hijos y generarías una "fuga de memoria" (memory leak).

- **Preparación:** Ahora, cuando estés en el parser.c, solo tendrás que escribir ASTNode* nodo = create_node(NODE_IF); y ya tendrás un nodo listo, limpio y seguro.

## Archivo `src/scanner.h`

Este componente es un Autómata Finito Determinista (AFD) simplificado. Su labor es leer el archivo fuente y agrupar caracteres en tokens que el Parser entenderá.

