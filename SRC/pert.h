#ifndef pert_H
#define pert_H

#include "libgraphe.h"



typedef struct TypTache {
	char nom;
	char* intitule;
	char* dependances;
	int dateTot;
	int dateTard;
} TypTache;

typedef struct TypGraphePERT {
	TypGraphe* graphe;
	TypTache* taches;
} TypGraphePERT;

TypGraphePERT* creerGraphePERT();

void calculDates(TypGraphePERT *graphePERT);

int dureeTotale(TypGraphePERT *graphePERT);

void afficherDates(TypGraphePERT *graphePERT);

void afficheCheminCritique(TypGraphePERT* graphe);

TypGraphePERT* lireGraphePERT(FILE* file);

#endif