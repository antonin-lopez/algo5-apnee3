#include <stdio.h>

#include "arbrebin.h"
#include "assert.h"
#include "bfile.h"

void Decoder(FILE *fichier_encode, FILE *fichier_decode, Arbre ArbreHuffman) {
        AfficherArbre(ArbreHuffman);
    /* A COMPLETER */
    printf("Programme non realise (Decoder)\n");
}

int main(int argc, char *argv[]) {
    FILE *fichier_encode = fopen(argv[1], "r");
    FILE *fichier_decode = fopen(argv[2], "w");

    Arbre ArbreHuffman = LireArbre(fichier_encode);

    Decoder(fichier_encode, fichier_decode, ArbreHuffman);

    fclose(fichier_encode);
    fclose(fichier_decode);
    
    return 0;
}
