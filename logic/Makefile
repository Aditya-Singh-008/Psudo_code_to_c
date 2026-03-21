# Variables to make the script cleaner
CC = gcc
LEX = flex
YACC = bison
CFLAGS = -Wall

# The default rule that runs when you just type 'make'
all: my_compiler

# 1. Link the generated C files into the final executable
my_compiler: lex.yy.c parser.tab.c
	$(CC) $(CFLAGS) lex.yy.c parser.tab.c -o my_compiler

# 2. Generate the Parser from parser.y
# The -d flag creates the .h file needed by Flex
parser.tab.c parser.tab.h: parser.y
	$(YACC) -d parser.y

# 3. Generate the Lexer from lexer.l
# It depends on parser.tab.h because it needs the token definitions
lex.yy.c: lexer.l parser.tab.h
	$(LEX) lexer.l

# 4. Clean up generated files to start fresh
clean:
	rm -f lex.yy.c parser.tab.c parser.tab.h my_compiler output.c