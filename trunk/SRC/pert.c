#include <stdbool.h>
#include "libgraphe.h"
#include "pert.h"

/*
* On consid�re pour un graphe PERT que les sommets du graphe associ� sont num�rot�s comme suit : (N nombre de t�ches)
* T�che A <-> 1
* T�che B <-> 2
* .............
* alpha (1�re �tape) <-> N+1
* om�ga (derni�re �tape) <-> N+2
*
* On calcule �galement les dates au plus t�t et au plus tard des t�ches
*/
TypGraphePERT* creerGraphePERT(){}


	/*
	* Fonction : calculDates
	*
	* Param�tres : TypGraphePERT *graphePERT, pointeur sur un graphe PERT
	*
	* Description : Calcule les dates au plus t�t et au plus tard 
	*				de toutes les t�ches du graphe PERT pass� en param�tre
	*/
void calculDates(TypGraphePERT *graphePERT) {
	calculDatesAuPlusTot(graphePERT);
	calculDatesAuPlusTard(graphePERT);
}


	/*
	* Fonction : calculDatesAuPlusTot
	*
	* Param�tres : TypGraphePERT *graphePERT, pointeur sur un graphe PERT
	*
	* Description : Calcule les dates au plus t�t d'un graphe de toutes les
	*				t�ches du graphe PERT pass� en param�tre.
	*				Pour faire cela, on impl�mente l'algo de Bellman-Ford dans
	*				sa version de calcul de plus long chemin � origine unique.
	*/
static void calculDatesAuPlusTot(TypGraphePERT *graphePERT) {
	TypGraphe *graphe;   /* Le graphe associ� au graphe PERT */
	TypTache  taches;    /* Les t�ches associ�es au graphe PERT */
	int       nbSommets; /* Le nombre de sommets du graphe */
	int       i;         /* Permet le parcours des sommets du graphe */
	int       j;         /* Permet le parcours des sommets du graphe */
	int       nbIter;    /* Le nombre d'it�rations de la boucle effectu� */
	bool      modif;     /* false si aucune nouvelle date n'a �t� calcul�e */
	
	graphe = graphePERT->graphe;
	taches = graphePERT->taches;
	nbSommets = graphe->nbrMaxSommets;
	
	/* Initialisation */
	for (i = 1; i <= nbSommets; i++) {
		/* Si ce n'est pas la t�che de d�part (alpha) */
		if (i != nbSommets-2) {
			taches[i-1]->dateTot = -1;
		}
		else {
			taches[i-1]->dateTot = 0;
		}
	}
	
	nbIter = 1;
	modif = true;
	
	/* Calcul des dates au plus t�t de chaque t�che */
	while ( (nbIter < nbSommets + 1) && (modif == true) ) {
		modif = false;
		
		/* Pour chaque ar�te du graphe... */
		for (i = 1; i <= nbSommets; i++) {
			for (j = 1; j <= nbSommets; j++) {
				if (areteExistante(graphe,i,j) == 0) {
					/* 
					* ... on regarde si un plus court chemin existe 
					* pour le sommet d'arriv�e
					*/
					if (taches[j-1]->dateTot < taches[i-1]->dateTot + poidsArete(graphe,i,j)) {
						taches[j-1]->dateTot = taches[i-1]->dateTot + poidsArete(graphe,i,j);
						modif = true;
					}
				}
			}
		}
		
		nbIter++;
	}
}

	/*
	* Fonction : calculDatesAuPlusTard
	*
	* Param�tres : TypGraphePERT *graphePERT, pointeur sur un graphe PERT
	*
	* Description : Calcule les dates au plus tard d'un graphe de toutes les
	*				t�ches du graphe PERT pass� en param�tre.
	*				Cette fonction suppose avoir d�j� calcul� les dates au plus
	*				t�t de toutes les t�ches du graphe.
	*/
static void calculDatesAuPlusTard(TypGraphePERT *graphePERT) {
	TypGraphePERT *graphePERTInverse;  /* Le graphe PERT invers� */
	TypTache      *taches;             /* Les t�ches associ�es au graphe PERT */
	int           nbSommets;           /* Le nombre de sommets du graphe */
	int           i;                   /* Permet le parcours des sommets */
	
	/* On calcule les dates au plus t�t du graphe invers� */
	graphePERTInverse = creerGraphePERT(...);
	graphePERTInverse->graphe = grapheInverse(graphePERTInverse->graphe);
	calculDatesAuPlusTot(graphePERTInverse);
	
	taches = graphePERT->taches;
	nbSommets = graphePERT->graphe->nbrMaxSommets;
	
	/*
	* Soit t"(i) la date au plus tard de la t�che i,
	* t'(i) la date au plus t�t de la t�che i dans le graphe invers�,
	* t(omega) la date de la derni�re t�che du graphe.
	*
	* Alors : t"(i) = t(omega) - t'(i)
	*/
	for (i = 1; i <= nbSommets; i++) {
		taches[i-1]->dateTard = 
			taches[nbSommets-1]->dateTot - graphePERTInverse->taches[i-1]->dateTot;
	}
}


	/*
	* Fonction : dureeTotale
	*
	* Param�tres : TypGraphePERT *graphePERT, pointeur sur un graphe PERT
	*
	* Retour : int, la dur�e totale du chantier
	*
	* Description : Renvoie la dur�e totale du chantier repr�sent� par le
	*				graphe PERT pass� en param�tre
	*/
int dureeTotale(TypGraphePERT *graphePERT) {
	return graphePERT->taches[graphePERT->graphe->nbrMaxSommets - 1]->dateTot;
}


	/*
	* Fonction : afficherDates
	*
	* Param�tres : TypGraphePERT *graphePERT, pointeur sur un graphe PERT
	*
	* Description : Affiche les dates des t�ches du graphe sous la forme suivante :
	*
	*				# nom, date au plus tot, date au plus tard
	*				A, 2, 5
	*				B, 7, 9
	*/
void afficherDates(TypGraphePERT *graphePERT) {
	TypTache *taches;  /* Les t�ches associ�es au graphe PERT */
	int      i;        /* Permet le parcours des t�ches */
	
	taches = graphePERT->taches;
	
	printf("# nom, date au plus tot, date au plus tard\n");
	
	for (i = 1; i <= graphePERT->graphe->nbrMaxSommets - 2; i++) {
		printf("%c, ",taches[i-1]->nom);
		printf("%d, ",taches[i-1]->dateTot);
		printf("%d\n",taches[i-1]->dateTard);
	}
}


void afficheCheminCritique(TypGraphePERT* graphe){}

TypGraphePERT* lireGraphePERT(FILE* file){}