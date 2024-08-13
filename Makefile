# Compilador
CC = gcc

# Flags de compilação
CFLAGS = -I$(INCDIR) -g

# Diretórios
SRCDIR = src
BINDIR = bin
INCDIR = include

# Nome do executável
EXECUTABLE = $(BINDIR)/meu_programa.exe

# Fonte principal
MAIN_SRC = main.cpp

# Regra padrão para compilar o executável
all: $(EXECUTABLE)

# Regra para criar o executável
$(EXECUTABLE): $(MAIN_SRC)
	$(CC) $(CFLAGS) -o $@ $<

# Limpeza de arquivos compilados
clean:
	rm -f $(EXECUTABLE)

# Garantir que `clean` e `all` sejam tratados como alvos que não são arquivos
.PHONY: all clean
