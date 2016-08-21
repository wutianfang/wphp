all: scanner parser
	g++ -o bin_wung  wung_language_scanner.c  wung_language_parser.tab.c wung_ast.c
scanner:
	re2c -o wung_language_scanner.c  wung_language_scanner.l
parser:
	bison --defines wung_language_parser.y
