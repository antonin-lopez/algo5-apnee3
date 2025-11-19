
#include "arbrebin.h"
#include "bfile.h"
#include "fap.h"
#include "huffman_code.h"
#include <assert.h>
#include <stdio.h>

typedef struct
{
	int tab[256];
} TableOcc_t;

struct code_char HuffmanCode[256];

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

	for (i = 0; i < 256; i++)
	{
		if (TableOcc->tab[i] != 0)
			printf("Occurences du caractere %c (code %d) : %d\n", i, i,
				   TableOcc->tab[i]);
	}
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
		return ArbreVide();
	}

	while (1)
	{
		Arbre ag = ArbreVide();
		int pg;
		file = extraire(file, &ag, &pg);

		if (est_fap_vide(file))
		{
			return ag;
		}

		Arbre ad = ArbreVide();
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

	if (EstVide(FilsGauche(a)) && EstVide(FilsDroit(a)))
	{
		Element c = Etiq(a);
		HuffmanCode[c].lg = lg;
		for (int i = 0; i < lg; i++)
			HuffmanCode[c].code[i] = code[i];
		return;
	}

	code[lg] = 0;
	ConstruireCodeRec(FilsGauche(a), code, lg + 1);

	code[lg] = 1;
	ConstruireCodeRec(FilsDroit(a), code, lg + 1);
}

void ConstruireCode(Arbre huff)
{
	for (int i = 0; i < 256; i++)
		HuffmanCode[i].lg = 0;

	int code[256];
	ConstruireCodeRec(huff, code, 0);
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
	/* A COMPLETER */
	printf("Programme non realise (Encoder)\n");
}

int main(int argc, char *argv[])
{

	TableOcc_t TableOcc;
	FILE *fichier;
	FILE *fichier_encode;

	fichier = fopen(argv[1], "r");
	/* Construire la table d'occurences */
	ConstruireTableOcc(fichier, &TableOcc);
	fclose(fichier);

	/* Initialiser la FAP */
	fap file = InitHuffman(&TableOcc);

	/* Construire l'arbre d'Huffman */
	Arbre ArbreHuffman = ConstruireArbre(file);
	printf("Arbre d'Huffman construit :\n");

	AfficherArbre(ArbreHuffman);

	/* Construire la table de codage */
	ConstruireCode(ArbreHuffman);

	for (size_t i = 0; i < 256; i++)
	{
		printHuffmanCode((unsigned char)i);
	}

	/* Encodage */
	fichier = fopen(argv[1], "r");
	fichier_encode = fopen(argv[2], "w");

	Encoder(fichier, fichier_encode, ArbreHuffman);

	fclose(fichier_encode);
	fclose(fichier);

	return 0;
}
