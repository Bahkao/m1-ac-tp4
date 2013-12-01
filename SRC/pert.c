#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "libgraphe.h"
#include "pert.h"


static void calculDatesAuPlusTot(TypGraphePERT*,int);
static void calculDatesAuPlusTard(TypGraphePERT*);
static int tacheEnSommet(char);
static char sommetEnTache(int);

	/*
	* Fonction : creerTache
	*
	* Param�tres : char nom, le nom de la t�che
	*              char *intitule, l'intitul� de la t�che
	*              char *dependances, les pr�decesseurs de la t�che
	*                  (ou cha�ne vide s'il n'y en a pas)
	*
	* Retour : TypTache*, pointeur sur la t�che cr��e
	*
	* Description : Cr�e une t�che dont les informations sont pass�es
	*               en param�tre. La m�moire est allou�e dans la fonction.
	*/
TypTache* creerTache(char nom, char *intitule, int duree, char *dependances) {
	TypTache *tache;  /* La nouvelle t�che cr��e */
	
	tache = malloc(sizeof(TypTache));
	tache->nom = nom;
	tache->intitule = intitule;
	tache->duree = duree;
	tache->dependances = dependances;
	
	return tache;
}


	/*
	* Fonction : creerGraphePERT
	*
	* Param�tres : TypTache **taches, un tableau de t�ches
	*              int nbTaches, la taille du tableau taches
	*
	* Retour : TypGraphePERT*, pointeur sur le graphe PERT cr��
	*
	* Description : Cr�e un graphe PERT � partir d'un tableau de t�ches.
	*               Les t�ches alpha et om�ga sont cr��es dans la fonction.
	*/
TypGraphePERT* creerGraphePERT(TypTache **taches, int nbTaches) {
	TypGraphePERT *graphePERT;               /* Le graphe PERT cr�� */
	TypGraphe     *graphe;                   /* Le graphe associ� */
	TypTache      *tacheDepart;              /* La t�che alpha de d�part */
	TypTache      *tacheArrivee;             /* La t�che om�ga d'arriv�e */
	char          *dependances;               /* Les d�pendances d'une t�che */
	int           sommet;                    /* Le num�ro d'un sommet */
	bool          sansSuccesseur[nbTaches];  /* si [i] = true, alors la t�che i+1 
												est � relier au sommet om�ga */
	int           i;                         /* Permet le parcours des boucles */
	int           j;                         /* Permet le parcours des boucles */
	
	/* Initialisation du tableau sansSuccesseur */
	for (i = 0; i <= nbTaches - 1; i++) {
		sansSuccesseur[i] = true;
	}
	
	/* Cr�ation du graphe associ� au graphe PERT */
	graphe = creerGraphe(nbTaches + 2);
	
	/* Cr�ation des sommets */
	for (i = 1; i <= nbTaches + 2; i++) {
		insertionSommet(graphe,i);
	}
	
	/* Cr�ation des t�ches alpha et om�ga */
	tacheDepart = creerTache(sommetEnTache(nbTaches+1),'\0',0,'\0');
	tacheArrivee = creerTache(sommetEnTache(nbTaches+2),'\0',0,'\0');
	
	/* Cr�ation des ar�tes */
	for (i = 1; i <= nbTaches; i++) {
		j = 0;
		dependances = taches[i-1]->dependances;
		
		while (dependances[j] != '\0') {
			sommet = tacheEnSommet(dependances[j]);
			insertionAreteOriente(graphe,sommet,i,taches[sommet-1]->duree);
			sansSuccesseur[sommet-1] = false;
			j++;
		}
		
		/* Si la t�che n'a pas de pr�decesseur, on la relie � alpha */
		if (j == 0) {
			insertionAreteOriente(graphe,nbTaches+1,i,0);
		}
	}
	
	/* On relie les t�ches sans successeurs au sommet om�ga */
	for (i = 0; i <= nbTaches - 1; i++) {
		if (sansSuccesseur[i] == true) {
			insertionAreteOriente(graphe,i+1,nbTaches+2,taches[i]->duree);
		}
	}
	
	/* On ajoute les t�ches alpha et om�ga au tableau des t�ches */
	taches = realloc(taches,(nbTaches+2) * sizeof(TypTache));
	taches[nbTaches] = tacheDepart;
	taches[nbTaches+1] = tacheArrivee;
	
	/* Cr�ation du graphe PERT */
	graphePERT = malloc(sizeof(TypGraphePERT));
	graphePERT->graphe = graphe;
	graphePERT->taches = taches;
	
	return graphePERT;
}


	/*
	* Fonction : deleteGraphePERT
	*
	* Param�tres : TypGraphePERT *graphePERT, pointeur sur un graphe PERT
	*
	* Description : Supprime le graphe PERT pass� en param�tre. Toute la
	*				m�moire qui lui avait �t� allou�e est lib�r�e.
	*/
void deleteGraphePERT(TypGraphePERT *graphePERT) {
	/* Suppression du graphe associ� */
	deleteGraphe(graphePERT->graphe);
	
	/* Lib�ration de la m�moire occup�e par les t�ches */
	free(graphePERT->taches);
	
	/* Lib�ration de la m�moire occup�e par le graphe PERT */
	free(graphePERT);
}


	/*
	* Fonction : calculDates
	*
	* Param�tres : TypGraphePERT *graphePERT, pointeur sur un graphe PERT
	*
	* Description : Calcule les dates au plus t�t et au plus tard 
	*				de toutes les t�ches du graphe PERT pass� en param�tre
	*/
void calculDates(TypGraphePERT *graphePERT) {
	calculDatesAuPlusTot(graphePERT,graphePERT->graphe->nbrMaxSommets-1);
	calculDatesAuPlusTard(graphePERT);
}


	/*
	* Fonction : calculDatesAuPlusTot
	*
	* Param�tres : TypGraphePERT *graphePERT, pointeur sur un graphe PERT
	*              int sommetDepart, le num�ro du sommet de d�part
	*
	* Description : Calcule les dates au plus t�t d'un graphe de toutes les
	*				t�ches du graphe PERT pass� en param�tre.
	*				Pour faire cela, on impl�mente l'algo de Bellman-Ford dans
	*				sa version de calcul de plus long chemin � origine unique.
	*/
static void calculDatesAuPlusTot(TypGraphePERT *graphePERT, int sommetDepart) {
	TypGraphe *graphe;   /* Le graphe associ� au graphe PERT */
	TypTache  **taches;  /* Les t�ches associ�es au graphe PERT */
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
		if (i != sommetDepart) {
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
	TypTache      **taches;            /* Les t�ches associ�es au graphe PERT */
	TypTache      **tachesInverse;     /* Les t�ches associ�es au graphe invers� */
	int           nbSommets;           /* Le nombre de sommets du graphe */
	int           i;                   /* Permet le parcours des sommets */
	
	taches = graphePERT->taches;
	nbSommets = graphePERT->graphe->nbrMaxSommets;
	
	/* Cr�ation du graphe invers� */
	tachesInverse = malloc((nbSommets-2) * sizeof(TypTache));
	for (i = 0; i <= nbSommets - 3; i++) {
		tachesInverse[i] = creerTache(taches[i]->nom,taches[i]->intitule,taches[i]->duree,taches[i]->dependances);
	}
	graphePERTInverse = creerGraphePERT(tachesInverse,nbSommets-2);
	graphePERTInverse->graphe = grapheInverse(graphePERTInverse->graphe);
	
	/* On calcule les dates au plus t�t du graphe invers� */
	calculDatesAuPlusTot(graphePERTInverse,nbSommets);
	
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
	
	deleteGraphePERT(graphePERTInverse);
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
	TypTache **taches; /* Les t�ches associ�es au graphe PERT */
	int      i;        /* Permet le parcours des t�ches */
	
	taches = graphePERT->taches;
	
	printf("# nom, date au plus tot, date au plus tard\n");
	
	for (i = 1; i <= graphePERT->graphe->nbrMaxSommets - 2; i++) {
		printf("%c, ",taches[i-1]->nom);
		printf("%d, ",taches[i-1]->dateTot);
		printf("%d\n",taches[i-1]->dateTard);
	}
}


void afficherCheminCritique(TypGraphePERT* graphe){}

TypGraphePERT* lireGraphePERT(FILE* file){}


	/*
	* Fonction : tacheEnSommet
	*
	* Param�tres : char nom, le nom d'une t�che
	*
	* Retour : int, le num�ro du sommet correspondant
	*
	* Description : Renvoie le num�ro du sommet correspondant au nom
	*               de la t�che. On consid�re que :
	*               'A' <-> 1
	*               'B' <-> 2  etc...
	*/
static int tacheEnSommet(char nom) {
	return (int) nom - 64;
}


	/*
	* Fonction : sommetEnTache
	*
	* Param�tres : int sommet, le num�ro d'un sommet d'un graphe
	*
	* Retour : char, le nom de la t�che correspondante
	*
	* Description : Renvoie le nom de la t�che correspondant au num�ro
	*               du sommet. On consid�re que :
	*               1 <-> 'A'
	*               2 <-> 'B'  etc...
	*/
static char sommetEnTache(int sommet) {
	return (char) (sommet + 64);
}