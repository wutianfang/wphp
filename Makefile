all: scanner parser 
	g++ -x c -o bin_wung wung.c  wung_language_scanner.c  wung_language_parser.tab.c wung_ast.c wung_compile.c wung_execute.c
scanner:
	re2c -o wung_language_scanner.c  wung_language_scanner.l
parser:
	bison --defines wung_language_parser.y
