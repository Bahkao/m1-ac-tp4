#include <stdbool.h>
#include "libgraphe.h"
#include "pert.h"

/*
* On considère pour un graphe PERT que les sommets du graphe associé sont numérotés comme suit : (N nombre de tâches)
* Tâche A <-> 1
* Tâche B <-> 2
* .............
* alpha (1ère étape) <-> N+1
* oméga (dernière étape) <-> N+2
*
* On calcule également les dates au plus tôt et au plus tard des tâches
*/
TypGraphePERT* creerGraphePERT(){}


	/*
	* Fonction : calculDates
	*
	* Paramètres : TypGraphePERT *graphePERT, pointeur sur un graphe PERT
	*
	* Description : Calcule les dates au plus tôt et au plus tard 
	*				de toutes les tâches du graphe PERT passé en paramètre
	*/
void calculDates(TypGraphePERT *graphePERT) {
	calculDatesAuPlusTot(graphePERT);
	calculDatesAuPlusTard(graphePERT);
}


	/*
	* Fonction : calculDatesAuPlusTot
	*
	* Paramètres : TypGraphePERT *graphePERT, pointeur sur un graphe PERT
	*
	* Description : Calcule les dates au plus tôt d'un graphe de toutes les
	*				tâches du graphe PERT passé en paramètre.
	*				Pour faire cela, on implémente l'algo de Bellman-Ford dans
	*				sa version de calcul de plus long chemin à origine unique.
	*/
static void calculDatesAuPlusTot(TypGraphePERT *graphePERT) {
	TypGraphe *graphe;   /* Le graphe associé au graphe PERT */
	TypTache  taches;    /* Les tâches associées au graphe PERT */
	int       nbSommets; /* Le nombre de sommets du graphe */
	int       i;         /* Permet le parcours des sommets du graphe */
	int       j;         /* Permet le parcours des sommets du graphe */
	int       nbIter;    /* Le nombre d'itérations de la boucle effectué */
	bool      modif;     /* false si aucune nouvelle date n'a été calculée */
	
	graphe = graphePERT->graphe;
	taches = graphePERT->taches;
	nbSommets = graphe->nbrMaxSommets;
	
	/* Initialisation */
	for (i = 1; i <= nbSommets; i++) {
		/* Si ce n'est pas la tâche de départ (alpha) */
		if (i != nbSommets-2) {
			taches[i-1]->dateTot = -1;
		}
		else {
			taches[i-1]->dateTot = 0;
		}
	}
	
	nbIter = 1;
	modif = true;
	
	/* Calcul des dates au plus tôt de chaque tâche */
	while ( (nbIter < nbSommets + 1) && (modif == true) ) {
		modif = false;
		
		/* Pour chaque arête du graphe... */
		for (i = 1; i <= nbSommets; i++) {
			for (j = 1; j <= nbSommets; j++) {
				if (areteExistante(graphe,i,j) == 0) {
					/* 
					* ... on regarde si un plus court chemin existe 
					* pour le sommet d'arrivée
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
	* Paramètres : TypGraphePERT *graphePERT, pointeur sur un graphe PERT
	*
	* Description : Calcule les dates au plus tard d'un graphe de toutes les
	*				tâches du graphe PERT passé en paramètre.
	*				Cette fonction suppose avoir déjà calculé les dates au plus
	*				tôt de toutes les tâches du graphe.
	*/
static void calculDatesAuPlusTard(TypGraphePERT *graphePERT) {
	TypGraphePERT *graphePERTInverse;  /* Le graphe PERT inversé */
	TypTache      *taches;             /* Les tâches associées au graphe PERT */
	int           nbSommets;           /* Le nombre de sommets du graphe */
	int           i;                   /* Permet le parcours des sommets */
	
	/* On calcule les dates au plus tôt du graphe inversé */
	graphePERTInverse = creerGraphePERT(...);
	graphePERTInverse->graphe = grapheInverse(graphePERTInverse->graphe);
	calculDatesAuPlusTot(graphePERTInverse);
	
	taches = graphePERT->taches;
	nbSommets = graphePERT->graphe->nbrMaxSommets;
	
	/*
	* Soit t"(i) la date au plus tard de la tâche i,
	* t'(i) la date au plus tôt de la tâche i dans le graphe inversé,
	* t(omega) la date de la dernière tâche du graphe.
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
	* Paramètres : TypGraphePERT *graphePERT, pointeur sur un graphe PERT
	*
	* Retour : int, la durée totale du chantier
	*
	* Description : Renvoie la durée totale du chantier représenté par le
	*				graphe PERT passé en paramètre
	*/
int dureeTotale(TypGraphePERT *graphePERT) {
	return graphePERT->taches[graphePERT->graphe->nbrMaxSommets - 1]->dateTot;
}


	/*
	* Fonction : afficherDates
	*
	* Paramètres : TypGraphePERT *graphePERT, pointeur sur un graphe PERT
	*
	* Description : Affiche les dates des tâches du graphe sous la forme suivante :
	*
	*				# nom, date au plus tot, date au plus tard
	*				A, 2, 5
	*				B, 7, 9
	*/
void afficherDates(TypGraphePERT *graphePERT) {
	TypTache *taches;  /* Les tâches associées au graphe PERT */
	int      i;        /* Permet le parcours des tâches */
	
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