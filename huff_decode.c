#include <stdio.h>

#include "arbrebin.h"
#include "assert.h"
#include "bfile.h"

void Decoder(FILE *fichier_encode, FILE *fichier_decode, Arbre ArbreHuffman) {
    printf("Decodage du fichier\n");
    AfficherArbre(ArbreHuffman);

    BFILE *bfile = bstart(fichier_encode, "r");

    while (1) {

        Arbre arbre = ArbreHuffman;

        while (!EstVide(FilsGauche(arbre)) || !EstVide(FilsDroit(arbre))) {
            int bit_lu = bitread(bfile);

            if (bit_lu == -1) {
                bstop(bfile);
                printf("Fichier decode\n");
                return;
            }

            if (bit_lu == 0) {
                arbre = FilsGauche(arbre);
            }

            else if (bit_lu == 1) {
                arbre = FilsDroit(arbre);
            }

            else {
                // bit_lu invalide (ne devrait jamais arriver)
                fprintf(stderr, "Erreur : bit invalide lu (%d).\n", bit_lu);
                bstop(bfile);
                return;
            }
        }

        unsigned char c = Etiq(arbre);
        printf("Ecriture du caractere: %c\n", c);
        fputc(c, fichier_decode);

    }
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
