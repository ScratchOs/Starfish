#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "scanner.h"
#include "token.h"
#include "parser.h"
#include "ast.h"
#include "error.h"
#include "platform.h"
#include "memory.h"
#include "test.h"

int main(int argc, char** argv){
    startColor();
    ArenaInit();

    if(argc <= 1) {
        cErrPrintf(TextRed, "Not enough arguments provided, "
            "expected microcode file name or \"test\".");
    }


    if(strcmp("test", argv[1]) == 0) {
        if(argc < 3) {
            cErrPrintf(TextRed, "Not enough arguments provided, "
                "expected name of directory containing compiler tests.");
        } else if(argc > 3) {
            cErrPrintf(TextRed, "Too many arguments provided, "
                "expected: microasm test <directory>");
        } else {
            runTests(argv[2]);
        }

        // runTests will exit on its own depending on test success,
        // to get here an argument input error will have occured
        exit(1);
    }

    if(argc != 2) {
        cErrPrintf(TextRed, "Too many arguments provided, "
            "expected: microasm <filename>");
        exit(1);
    }

    Scanner scan;
    Parser parser;
    runFile(argv[1], &parser, &scan);
    PrintMicrocode(&parser.ast);
}