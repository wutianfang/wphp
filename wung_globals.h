typedef struct _wung_scanner_globals {
    unsigned char *yy_cursor;
    unsigned char *yy_marker;
    unsigned char *yy_limit;
	unsigned char *yy_text;
}wung_scanner_globals;

extern wung_scanner_globals language_scanner_globals;
#define SCNG(v) language_scanner_globals.v
