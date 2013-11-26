#ifndef pert_H
#define pert_H

#include "libgraphe.h"



typedef struct TypTache {
	int numero;
	char* nom;
	char* intitule;
	char* dependances;
	int dateTot;
	int dateTard;
} TypTache;

typedef struct TypGraphePERT {
	TypGraphe* graphe;
	TypTache* taches;
	int dureeTotale;
	char* cheminCritique;
} TypGraphePERT;


TypGraphe* listeSucc(TypGraphe* graphe, int sommet);

int dateAuplusTot(TypGraphePERT* graphe, int sommet);

int dateAuplusTart(TypGraphePERT* graphe, int sommet);

int plusCourtChemin(TypGraphePERT* graphe, int depart, int arrivee);

int* algoBellmanFord(TypGraphePERT* graphe, int depart);

TypVoisins* cheminCritique(TypGraphePERT* graphe);

#endif