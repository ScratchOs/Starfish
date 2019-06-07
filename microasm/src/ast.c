#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "ast.h"
#include "token.h"
#include "memory.h"

void InitMicrocode(Microcode* mcode, const char* fileName) {
    mcode->hasError = false;
    mcode->fileName = fileName;
}

void PrintMicrocode(Microcode* mcode) {
    printf("Microcode:\n");
    printf("  Has Error: %s\n", mcode->hasError ? "true" : "false");
    printf("  File Name: %s\n", mcode->fileName);
    printf("  Header: %u", mcode->head.bitCount);
    for(unsigned int i = 0; i < mcode->head.bitCount; i++) {
        printf("\n    ");
        TokenPrint(&mcode->head.bits[i]);
    }
    printf("\n");
    printf("  Input: %u", mcode->inp.valueCount);
    for(unsigned int i = 0; i < mcode->inp.valueCount; i++) {
        printf("\n    ");
        TokenPrint(&mcode->inp.values[i].name);
        printf(" = %i", mcode->inp.values[i].value);
    }
    printf("\n");
}