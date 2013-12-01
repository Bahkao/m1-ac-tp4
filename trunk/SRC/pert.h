#ifndef pert_H
#define pert_H

#include "libgraphe.h"



typedef struct TypTache {
	char nom;
	char *intitule;
	int duree;
	char *dependances;  /* ex : "ACD" */
	int dateTot;
	int dateTard;
} TypTache;

typedef struct TypGraphePERT {
	TypGraphe *graphe;
	TypTache **taches;
} TypGraphePERT;


TypTache* creerTache(char, char*, int, char*);

TypGraphePERT* creerGraphePERT(TypTache**, int);

void deleteGraphePERT(TypGraphePERT*);

void calculDates(TypGraphePERT*);

int dureeTotale(TypGraphePERT*);

void afficherDates(TypGraphePERT*);

void afficherCheminCritique(TypGraphePERT*);

TypGraphePERT* lireGraphePERT(FILE*);

#endif