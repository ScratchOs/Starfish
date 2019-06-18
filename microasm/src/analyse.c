#include "token.h"
#include "ast.h"
#include "analyse.h"
#include "parser.h"
#include "error.h"

typedef void(*Analysis)(Parser* parser);

static void AnalyseOutput(Parser* parser) {
    Microcode* mcode = &parser->ast;

    if(mcode->out.width.data.value < 1) {
        errorAt(parser, 100, &mcode->out.width, "Output width has to be one or greater");
    }
}

static void AnalyseInput(Parser* parser) {
    Microcode* mcode = &parser->ast;

    for(unsigned int i = 0; i < mcode->inp.valueCount; i++) {
        InputValue val = mcode->inp.values[i];
        if(val.value.data.value < 1) {
            errorAt(parser, 101, &val.value, "Input width has to be one or greater");
        }
    }
}

static Analysis Analyses[] = {
    AnalyseInput,
    AnalyseOutput
};

void Analyse(Parser* parser) {
    for(unsigned int i = 0; i < sizeof(Analyses)/sizeof(Analysis); i++) {
        Analyses[i](parser);
    }
}