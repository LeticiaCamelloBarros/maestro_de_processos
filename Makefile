CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g

BIN = processflow
SRCS = src/main.c src/task.c src/job.c src/acoesprocess.c

# compila tudo em um único comando e gera o binário ./processflow
all: $(BIN)

$(BIN): $(SRCS)
	$(CC) $(CFLAGS) -o $(BIN) $(SRCS)

# remove o binário gerado
clean:
	rm -f $(BIN)

# roda um workflow de exemplo (tests/basic.pf) para validar rapidamente
test: all
	./$(BIN) tests/basic.pf

.PHONY: all clean test