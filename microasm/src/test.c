#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "test.h"
#include "platform.h"
#include "error.h"
#include "analyse.h"

bool runFile(const char* fileName, const char* file, Parser* parse, Scanner* scan, bool testing) {
    const char* fullFileName = resolvePath(fileName);

    ScannerInit(scan, file, fullFileName);
    ParserInit(parse, scan);

    const char* ext = strrchr(fullFileName, '.');
    if(!ext) {
        cErrPrintf(TextRed, "\nCould not detect file type for \"%s\"\n", fullFileName);
        return false;
    } else {
        ext = ext + 1;
    }

#ifdef debug
    int isTestFile = !strcmp(ext, "uasmt");
    if(isTestFile && testing) {
        expectTestStatements(parse);
        Parse(parse);
        Analyse(parse);
        return true;
    } else if(isTestFile && !testing) {
        cErrPrintf(TextRed, "\nNot expecting microcode test file while reading \"%s\"\n", ext, fullFileName);
        return false;
    } else
#else
    (void)testing;
#endif
    if(!strcmp(ext, "uasm")) {
        Parse(parse);
        Analyse(parse);
        return true;
    }

    cErrPrintf(TextRed, "\nUnknown file type \"%s\" when reading file \"%s\"\n", ext, fullFileName);
    return false;
}

#ifdef debug
static int testCount;
static int passedCount;
static void runTest(const char* path, const char* file) {
    testCount++;
    Scanner scanner;
    Parser parser;

    printf("Testing: %s  ->\n", resolvePath(path));
    bool runSuccess = runFile(path, file, &parser, &scanner, true);
    if(!runSuccess) {

    }

    unsigned int currentAstError = 0;
    while(currentAstError < parser.ast.errorCount) {
        if(parser.ast.expectedErrorCount >= currentAstError + 1) {
            Error expected = parser.ast.expectedErrors[currentAstError];
            Error actual = parser.ast.errors[currentAstError];
            if(expected.id != actual.id || expected.token.line != actual.token.line || expected.token.column != actual.token.column) {
                runSuccess = false;
                cErrPrintf(TextRed, "  Expected Error[E%04u] at ", parser.ast.expectedErrors[currentAstError].id);
                cErrPrintf(TextRed, "%u:%u\n", parser.ast.expectedErrors[currentAstError].token.line, parser.ast.expectedErrors[currentAstError].token.column);
                cErrPrintf(TextRed, "  Got Error[E%04u] at ", parser.ast.errors[currentAstError].id);
                cErrPrintf(TextRed, "%u:%u\n", parser.ast.errors[currentAstError].token.line, parser.ast.errors[currentAstError].token.column);
            }
        } else {
            runSuccess = false;
            cErrPrintf(TextRed, "  Error[E%04u] at ", parser.ast.errors[currentAstError].id);
            cErrPrintf(TextRed, "%u:%u\n", parser.ast.errors[currentAstError].token.line, parser.ast.errors[currentAstError].token.column);
        }
        currentAstError++;
    }

    if(parser.ast.expectedErrorCount != currentAstError) {
        runSuccess = false;
        for(; currentAstError < parser.ast.expectedErrorCount; currentAstError++) {
            cErrPrintf(TextRed, "  Expected Error[E%04u] at ", parser.ast.expectedErrors[currentAstError].id);
            cErrPrintf(TextRed, "%u:%u\n", parser.ast.expectedErrors[currentAstError].token.line, parser.ast.expectedErrors[currentAstError].token.column);
        }
    }

    if(runSuccess) {
        cOutPrintf(TextGreen, "  Passed\n");
        passedCount++;
    }
}

void runTests(const char* directory) {
    disableErrorPrint();

    iterateDirectory(directory, runTest);

    if(testCount == passedCount) {
        cOutPrintf(TextGreen, "\nAll Tests Passed\n");
        cOutPrintf(TextGreen, "(%i tests executed)\n", passedCount);
    } else {
        cErrPrintf(TextRed, "\nTesting Failed\n");
        cErrPrintf(TextRed, "(%i of %i passed)\n", passedCount, testCount);
    }

    exit(0);
}
#endif