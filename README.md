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
```
    
# Explicación de la funcionalidad de los códigos

## Archivo `include/ast.h`

Este archivo es fundamental porque define cómo se verá cada pieza del código en memoria. Sin esto, el analizador sintáctico no tendría un lugar donde guardar los resultados.

### ¿Por qué empezamos aquí?
- **Modularidad:** Ahora que tienes ast.h, cuando escribas parser.c, simplemente escribirás #include "../include/ast.h" y ya tendrás acceso a la estructura que define el árbol.

- **Preparación para el Parser:** El parser.c no hace más que tomar tokens y llenar estos ASTNode.

- **Control de Memoria:** Al incluir las funciones create_node y free_ast en el encabezado, te obligas a ti mismo a implementar la gestión de memoria desde el primer día, evitando que tu proyecto se vuelva un caos de memory leaks al final- 

## Archivo `src/utils.c`

Este archivo es vital porque aquí es donde "cobran vida" las funciones que declaramos en ast.h. Como te comentaba, la clave de un compilador robusto es gestionar correctamente la memoria dinámica (malloc/free).

### ¿Por qué este archivo es necesario?
- **Seguridad:** Incluimos un if (node == NULL) que protege al programa de fallos. Si tu sistema se queda sin memoria, el compilador lo dirá claramente en lugar de cerrarse sin avisar.

- **Limpieza Profunda (free_ast):** El recorrido post-orden (primero los hijos, luego el padre) es la forma estándar de eliminar árboles en C. Si intentaras borrar el padre primero, perderías la dirección de memoria de los hijos y generarías una "fuga de memoria" (memory leak).

- **Preparación:** Ahora, cuando estés en el parser.c, solo tendrás que escribir ASTNode* nodo = create_node(NODE_IF); y ya tendrás un nodo listo, limpio y seguro.

## Archivo `src/scanner.c`

Este archivo contiene la implementación real del analizador léxico. Funciona como un Autómata Finito Determinista (AFD) en código, leyendo el archivo fuente carácter por carácter para agruparlos en los tokens que definimos en el diccionario.

### ¿Por qué esta lógica es crucial?
- **Análisis de dos caracteres (Lookahead):** Al procesar caracteres como =, < o >, el compilador necesita mirar obligatoriamente el siguiente carácter. Si viene otro =, se convierte en comparación (==), de lo contrario se regresa el carácter usando ungetc y se clasifica como una asignación simple.

- **Manejo Dinámico de Flotantes:** El analizador cambia el tipo de token de TOKEN_NUM_INT a TOKEN_NUM_FLOAT de forma automática en cuanto detecta el primer punto decimal ..

### ¿Por qué este archivo es necesario?
- **Procesamiento de Flujo Estrecho (ungetc):** Es el encargado de interactuar directamente con el archivo en el disco. Utiliza la función `ungetc()` para "devolver" caracteres al flujo de lectura cuando lee un símbolo de más. Esto es vital para saber, por ejemplo, dónde termina el nombre de una variable sin perder el carácter que sigue (como un espacio o un punto y coma).

- **Aislamiento de la Sintaxis (identify_keyword):** de forma inteligente los identificadores creados por el usuario de las palabras protegidas del lenguaje (como if o while). Al procesar la cadena de texto de manera interna, evita que el Parser tenga que hacer comparaciones de texto, delegándole un trabajo puramente numérico y lógico.

- **Ignorancia Inteligente:** Filtra y desecha todo lo que no le sirve al compilador para generar el árbol binario, como los espacios en blanco, las tabulaciones y los saltos de línea, pero aprovechando estos últimos para actualizar el contador current_line y mantener un rastreo preciso de la ubicación del análisis.

## Archivo `include/tokens.h` 

Este archivo actúa como el "diccionario" oficial de tu compilador. Define de forma única y centralizada cada uno de los elementos gramaticales (palabras clave, operadores, delimitadores) que el lenguaje .aggy es capaz de reconocer.

### ¿Por qué este archivo es necesario?
- **Asignación Única (enum):** Al utilizar una enumeración (typedef enum), C le asigna un número entero único a cada token automáticamente. Esto permite que el resto de los módulos se comuniquen usando números sencillos e instrucciones eficientes (como switch), en lugar de comparar costosas cadenas de texto.

- **Independencia Absoluta:** Es el cimiento léxico del proyecto. No incluye ni depende de ningún otro archivo de la arquitectura, lo que evita ciclos de dependencia o problemas de compilación.

- **Consistencia:** Al centralizar aquí todos los operadores (+, -, ==, etc.) y palabras clave (if, while), garantizas que tanto el Scanner (que los detecta) como el Parser (que los valida) hablen exactamente el mismo idioma.

## Archivo `include/scanner.h`

Este archivo define la interfaz del analizador léxico y la estructura del objeto Token. Sirve como el "puente de comunicación" que expone las funciones del Scanner para que el Parser pueda solicitar tokens bajo demanda.

### ¿Por qué empezamos aquí?
- **Resolución de Tipos:** Al colocar la directiva #include "tokens.h" al inicio, le enseñas a C la definición de TokenType antes de intentar usarlo dentro de la estructura Token. Esto elimina por completo los errores de identificadores no definidos.

- **Empaquetado de Datos (struct Token):** No basta con saber qué tipo de token encontramos; el Parser necesita el texto real escrito por el usuario (el lexeme) y saber exactamente en qué fila ocurrió (line) para poder arrojar reportes de error precisos.

- **Encapsulamiento de la Inicialización:**  Al declarar init_scanner(FILE *file), preparamos la arquitectura para recibir de forma limpia el flujo del archivo fuente desde el main.c, manteniendo la lógica de lectura aislada del resto del sistema.

## Archivo `main.c`

Este archivo se encargará de recibir el archivo .aggy desde la consola, validar su extensión (como planeamos en el diseño) e inicializar el Scanner para procesar el código. En esta etapa, lo programaremos para que imprima una lista de todos los tokens encontrados; esto nos servirá para comprobar que el Scanner funciona de forma impecable antes de construir el Parser.

### ¿Por qué este código es importante en esta etapa?
- **Aislamiento de Errores:** Al imprimir los tokens uno por uno en una tabla, puedes crear un archivo de prueba con código en .aggy y ver exactamente si el Scanner está segmentando bien las cadenas de texto antes de complicar el programa con el Parser.

- **Validación de Parámetros:** Protege el sistema mediante el uso de argc e impide que el programa truene con un Segmentation Fault si el usuario olvida pasar el archivo en la consola.