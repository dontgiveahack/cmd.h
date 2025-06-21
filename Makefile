# See LICENSE file for copyright and license details.

.POSIX:
include config.mk

BIN = example
SRC = main.c
OBJ = $(SRC:.c=.o)

all: options $(BIN)

options:
	@echo ${BIN} build options:
	@echo "CFLAGS = ${CFLAGS}"
	@echo "CC     = ${CC}"

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJ): config.mk

$(BIN): $(OBJ)
	@echo $(CC) -o $@
	@$(CC) -o $@ $(OBJ) $(LDFLAGS)

clean:
	@echo cleaning
	@rm -f $(BIN) $(OBJ) $(TXT)

.PHONY: all options clean
