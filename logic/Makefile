# Variables to make the script cleaner
CC = gcc
LEX = flex
YACC = bison
CFLAGS = -Wall -g

SRCS = ast.c symtab.c codegen_c.c codegen_python.c codegen_java.c main.c
OBJS = $(SRCS:.c=.o) lex.yy.o parser.tab.o

# The default rule that runs when you just type 'make'
all: my_compiler

# 1. Link the generated C files into the final executable
my_compiler: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o my_compiler

# 2. Generate the Parser from parser.y
parser.tab.c parser.tab.h: parser.y
	$(YACC) -d parser.y

# 3. Generate the Lexer from lexer.l
lex.yy.c: lexer.l parser.tab.h
	$(LEX) lexer.l

# 4. Compile C source files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

lex.yy.o: lex.yy.c
	$(CC) $(CFLAGS) -c $< -o $@

parser.tab.o: parser.tab.c
	$(CC) $(CFLAGS) -Wno-implicit-function-declaration -c $< -o $@

# 5. Clean up generated files to start fresh
clean:
	rm -f lex.yy.c parser.tab.c parser.tab.h my_compiler output.c *.o