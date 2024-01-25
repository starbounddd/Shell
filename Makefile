CFLAGS = -g3 -Wall -Wextra -Wconversion -Wcast-qual -Wcast-align -g
CFLAGS += -Winline -Wfloat-equal -Wnested-externs
CFLAGS += -pedantic -std=gnu99
CC = gcc


EXECS = 33sh 33noprompt

PROMPT = -DPROMPT

.PHONY: all clean 
all: $(EXECS)



33sh: sh.c 
	@echo "Compiling program with prompt"
	 $(CC) $(CFLAGS) $(PROMPT) -o $@ $^ 
33noprompt: sh.c
	@echo "Compiling program without prompt"
	$(CC) $(CFLAGS) -o $@ $^ 
clean:
	@echo "Cleaning program and removing files"
	rm -f 33sh 33noprompt

