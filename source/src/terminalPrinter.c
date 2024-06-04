#include "../include/terminalPrinter.h"

void printMovieOptions(){
    printf("|-----------------------| \n");
    printf("|0 - Sair               | \n");
    printf("|1 - O Senhor dos Aneis | \n");
    printf("|2 - O Poderoso Chefao  | \n");
    printf("|3 - Clube da Luta      | \n");
    printf("|-----------------------| \n");
}

void printBorder(){
    printf("|-----------------------| \n");
}

void printPhrase(char* msg){
    printf("| %s | \n",msg);
}