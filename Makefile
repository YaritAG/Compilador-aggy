# Variables para el compilador y banderas
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Iinclude

# Archivos fuente y objetos
SRC = main.c src/scanner.c src/utils.c
OBJ = $(SRC:.c=.o)

# Nombre del ejecutable final
TARGET = compilador_aggy

# Regla principal: Compila el ejecutable
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

# Regla para compilar los archivos .c a .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Regla para limpiar los archivos generados
clean:
	rm -f $(SRC)/*.o *.o $(TARGET)

# Evita conflictos si existen archivos llamados 'clean'
.PHONY: clean