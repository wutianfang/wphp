#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include "wung.h"
#include "wung_compile.h"
#include "wung_execute.h"
#include "wung_language_parser.tab.h"

int main(int argc, char * argv[]) {
    wung_op_array * op_array = compile_string(argv[1]);
    wung_execute(op_array);

    return 0;
}
