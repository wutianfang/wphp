#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include "wung.h"
#include "wung_compile.h"
#include "wung_language_parser.tab.h"

int main(int argc, char * argv[]) {
    compile_string(argv[1]);

    return 0;
}
