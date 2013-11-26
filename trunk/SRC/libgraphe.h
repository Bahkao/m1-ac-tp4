/*
 *Auhor  Christophe Sauvage & Ndiaye Moustapha
 */

#ifndef libgraphe_H
#define libgraphe_H
#include "libliste.h"

typedef struct TypGraphe{
	int nbrMaxSommets; /*Le nombre maximun de sommets du graphe*/
	struct TypVoisins** listesAdjacences; /*Listes d'ajacence*/
} TypGraphe;

TypGraphe* creerGraphe(int);

int insertionSommet(TypGraphe *graphe, int idSommet );

int suppressionSommet(TypGraphe*, int);

int insertionAreteOriente(TypGraphe*, int, int, int);

int insertionAreteNonOriente(TypGraphe*, int, int, int);

int suppressionArete(TypGraphe*, int, int, char);

int sommetExistant(TypGraphe*,int);

int areteExistante(TypGraphe*,int,int);

void deleteGraphe(TypGraphe*);

void affichage(TypGraphe*);

void sauvegarde(TypGraphe*,FILE *);

int lecture(TypGraphe**,char[]);

int degreSommet(TypGraphe*, int);

TypGraphe* grapheInverse(TypGraphe* graphe);

int poidsArete(TypGraphe* graphe, int depart, int arrivee);

#endif
