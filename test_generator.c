#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
    // Nettoyage des dossiers de test
    printf("Nettoyage des dossiers de test...\n");
    system("rm -rf ./tests/source ./tests/encoded ./tests/decoded");
    system("mkdir -p ./tests/source ./tests/encoded ./tests/decoded");
    printf("✓ Dossiers nettoyés et recréés\n\n");
    
    srand(time(NULL));
    
    // Paramètres de test : tailles de fichiers à tester
    int tailles[] = {100, 500, 1000, 5000, 10000, 50000};
    int nb_tailles = 6;
    
    // Paramètres de test : nombres de caractères distincts
    int distincts[] = {2, 5, 10, 20, 50, 100};
    int nb_distincts = 6;
    
    // Nombre de répétitions pour chaque combinaison
    int nb_repetitions = 100;
    
    printf("Génération de %d fichiers de test...\n", 
           nb_tailles * nb_distincts * nb_repetitions);
    
    for (int t = 0; t < nb_tailles; t++)
    {
        for (int d = 0; d < nb_distincts; d++)
        {
            int taille = tailles[t];
            int distinct = distincts[d];
            
            // On ne peut pas avoir plus de caractères distincts que de caractères totaux
            if (distinct > taille) continue;
            
            for (int rep = 0; rep < nb_repetitions; rep++)
            {
                char nom_fichier[128];
                snprintf(nom_fichier, sizeof(nom_fichier), 
                         "test_n%d_d%d_r%d.txt", taille, distinct, rep);
                
                char chemin[256];
                snprintf(chemin, sizeof(chemin), "./tests/source/%s", nom_fichier);
                
                FILE *fichier = fopen(chemin, "w");
                if (!fichier) {
                    fprintf(stderr, "Erreur: impossible de créer %s\n", chemin);
                    continue;
                }
                
                // Génération avec distribution variée pour plus de réalisme
                for (int i = 0; i < taille; i++)
                {
                    int choix = rand() % distinct;
                    char c = 'a' + choix;
                    fputc(c, fichier);
                }
                
                fclose(fichier);
            }
            printf("Créé: 100 fichiers pour taille=%d, distincts=%d\n", taille, distinct);
        }
    }
    
    printf("\nGénération terminée!\n");
    return 0;
}