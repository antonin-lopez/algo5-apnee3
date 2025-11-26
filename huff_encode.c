#include "arbrebin.h"
#include "bfile.h"
#include "fap.h"
#include "huffman_code.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>

typedef struct
{
	int tab[256];
} TableOcc_t;

struct code_char HuffmanCode[256];

void AfficherTableOcc(TableOcc_t *TableOcc)
{
	for (int i = 0; i < 256; i++)
	{
		if (TableOcc->tab[i] != 0)
			printf("Occurences du caractere %c (code %d) : %d\n", i, i, TableOcc->tab[i]);
	}
}

void ConstruireTableOcc(FILE *fichier, TableOcc_t *TableOcc)
{
	int i;
	for (i = 0; i < 256; i++)
		TableOcc->tab[i] = 0;

	int c;
	c = fgetc(fichier);
	while (c != EOF)
	{
		TableOcc->tab[c]++;
		c = fgetc(fichier);
	};

	// AfficherTableOcc(TableOcc);
}

void CalculerEntropie(TableOcc_t *TableOcc)
{
	double entropie = 0.0;
	int nb_occ_total = 0;

	for (int i = 0; i < 256; i++)
		nb_occ_total += TableOcc->tab[i];
	
	double frequence;
	for (int i = 0; i < 256; i++)
	{
		if (TableOcc->tab[i] != 0)
		{
			frequence = (double)TableOcc->tab[i] / nb_occ_total;
			entropie += frequence * log2(frequence);
		}
	}

	entropie = -entropie;
	printf("%f", entropie);
}

fap InitHuffman(TableOcc_t *TableOcc)
{
	fap f = creer_fap_vide();
	for (int i = 0; i < 256; i++)
	{
		if (TableOcc->tab[i] != 0)
		{
			Arbre element = NouveauNoeud(ArbreVide(), (Element)i, ArbreVide());
			int priorite = TableOcc->tab[i];
			f = inserer(f, element, priorite);
		}
	}
	return f;
}

Arbre ConstruireArbre(fap file)
{
	if (est_fap_vide(file))
	{
		fprintf(stderr, "La FAP est vide, on retourne un arbre vide\n");
		return ArbreVide();
	}

	while (1)
	{
		Arbre ag;
		int pg;
		file = extraire(file, &ag, &pg);

		if (est_fap_vide(file))
		{
			return ag;
		}

		Arbre ad;
		int pd;
		file = extraire(file, &ad, &pd);

		Arbre a = NouveauNoeud(ag, -1, ad);
		file = inserer(file, a, pg + pd);
	}
}

void ConstruireCodeRec(Arbre a, int code[], int lg)
{
	if (EstVide(a))
		return;

	// Si les deux enfants sont vides, on a atteind une feuille.
	// On met à jour le code avec la longueur du code et la valeur de chacun de ses bits.
	if (EstVide(FilsGauche(a)) && EstVide(FilsDroit(a)))
	{
		Element c = Etiq(a);
		HuffmanCode[c].lg = lg;
		for (int i = 0; i < lg; i++)
			HuffmanCode[c].code[i] = code[i];
		return;
	}

	// Sinon on met à jour le buffer pour la profondeur actuelle avec la valeur pour le fils gauche et 1 pour le fils droite et
	// on appel recursivement ConstruireCodeRec() pour la profondeur suivante.
	code[lg] = 0;
	ConstruireCodeRec(FilsGauche(a), code, lg + 1);

	code[lg] = 1;
	ConstruireCodeRec(FilsDroit(a), code, lg + 1);
}

void ConstruireCode(Arbre huff)
{
	int buffer[256]; // largement suffisant
	for (int i = 0; i < 256; i++)
	{
		HuffmanCode[i].lg = 0;
	}

	ConstruireCodeRec(huff, buffer, 0);
}

void printHuffmanCode(unsigned char c)
{
	if (HuffmanCode[c].lg == 0)
		return;
	printf("Code pour '%c' : ", c);
	for (int i = 0; i < HuffmanCode[c].lg; i++)
	{
		printf("%d", HuffmanCode[c].code[i]);
	}
	printf("\n");
}

void Encoder(FILE *fic_in, FILE *fic_out, Arbre ArbreHuffman)
{
	int c;
	EcrireArbre(fic_out, ArbreHuffman);

	BFILE *f = bstart(fic_out, "w");

	// Lecture du premier caractère du fichier
	c = fgetc(fic_in);

	while (c != EOF)
	{
		// Cast du caractère de Int à Unsigned Char
		unsigned char uc = (unsigned char)c;
		// printf("Ecriture du caractere %c\n", uc);

		for (int i = 0; i < HuffmanCode[uc].lg; i++)
		{
			if (bitwrite(f, HuffmanCode[uc].code[i]) == -1)
			{
				fprintf(stderr, "Erreur lors de l'écriture du bit\n");
				return;
			}
		}
		c = fgetc(fic_in);
	}
	bstop(f);
}

int main(int argc, char *argv[])
{
	TableOcc_t TableOcc;

	FILE *fichier;
	FILE *fichier_encode;

	clock_t debut, fin;
	double temps_construction_table, temps_construction_arbre, temps_encodage;

	fichier = fopen(argv[1], "r");
	/* Construire la table d'occurences */

	debut = clock();
	ConstruireTableOcc(fichier, &TableOcc);
	fin = clock();
	temps_construction_table = (double)(fin - debut) / CLOCKS_PER_SEC;
	fclose(fichier);

	/* Initialiser la FAP */
	fap file = InitHuffman(&TableOcc);

	/* Construire l'arbre d'Huffman */
	debut = clock();
	Arbre ArbreHuffman = ConstruireArbre(file);
	fin = clock();
	temps_construction_arbre = (double)(fin - debut) / CLOCKS_PER_SEC;

	// AfficherArbre(ArbreHuffman);

	/* Construire la table de codage */
	ConstruireCode(ArbreHuffman);

	// for (size_t i = 0; i < 256; i++)
	// {
	// 	printHuffmanCode((unsigned char)i);
	// }

	/* Encodage */
	fichier = fopen(argv[1], "r");
	fichier_encode = fopen(argv[2], "w");

	debut = clock();
	Encoder(fichier, fichier_encode, ArbreHuffman);
	fin = clock();
	temps_encodage = (double)(fin - debut) / CLOCKS_PER_SEC;

	printf("%f:%f:%f", temps_construction_table, temps_construction_arbre, temps_encodage);
	fclose(fichier_encode);
	fclose(fichier);

	return 0;
}
