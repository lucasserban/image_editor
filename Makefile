CC=gcc
CFLAGS=-Wall -Wextra -std=c99 

TARGETS=image_editor

build: $(TARGETS)

image_editor: image_editor.c
	$(CC) $(CFLAGS) image_editor.c -o image_editor -lm

clean:
	rm -f $(TARGETS)

pack:
	zip -FSr 315CA_SerbanLucasNicolae_Tema3.zip README Makefile *.c *.h

.PHONY: pack clean