#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

int main(int argc, char const *argv[])
{
    char nom_fichier[64];
    int nb_caractere;
    int nb_distinct;
    int nb_tests;
    char base_char = 'a';

    srand(time(NULL));
    
    printf("Entrez le nom du fichier:\n");
    scanf("%64s", nom_fichier);
    
    printf("Entrez le nombre de caracteres a generer:\n");
    scanf("%d", &nb_caractere);
    
    printf("Entrez le nombre de caracteres distincts:\n");
    scanf("%d", &nb_distinct);

    printf("Entrez le nombre de tests distinct à générer avec ces paramètres:\n");
    scanf("%d", &nb_tests);
    
    for (int i = 0; i < nb_tests; i++)
    {
        char chemin[100];
        char nom_fichier_iteration[70];
        snprintf(nom_fichier_iteration, sizeof(nom_fichier_iteration), "%s_%d.txt", nom_fichier, i + 1);
        snprintf(chemin, sizeof(chemin), "./tests/source-files/%s", nom_fichier_iteration);

        FILE *fichier = fopen(chemin, "a");
    
        for (int i = 0; i < nb_caractere; i++)
        {
            char random_char = base_char + (rand() % nb_distinct);
            fputc(random_char, fichier);
        }
        
        fclose(fichier);
        printf("Fichier généré avec succès : %s\n", nom_fichier_iteration);
    }
    
    return 0;
}
