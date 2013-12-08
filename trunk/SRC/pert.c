#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>
#include "libgraphe.h"
#include "pert.h"


/*
* Structure associant une tâche à sa durée restante.
* Utilisée pour le calcul de la durée totale du chantier.
*/
typedef struct TypTacheEnCours {
	TypTache *tache;
	int dureeRestante;
} TypTacheEnCours;


static void calculDatesAuPlusTot(TypGraphePERT*,int);
static void calculDatesAuPlusTard(TypGraphePERT*);
static TypTacheEnCours* creerTacheEnCours(TypTache*);
static void decompteDuree(TypTacheEnCours**,int,int);
static void chercheTachesLibres(TypTache**,TypTache**,TypTache**,int*,int*,int);
static void affecteTaches(TypTache**,TypTacheEnCours**,int*,int*,int);
static void triTachesEnCours(TypTacheEnCours**,int);
static int tacheEnSommet(char);
static char sommetEnTache(int);
static char* replace(const char*, const char*, const char*);


	/*
	* Fonction : creerTache
	*
	* Paramètres : char nom, le nom de la tâche
	*              char *intitule, l'intitulé de la tâche
	*              char *dependances, les prédecesseurs de la tâche
	*                  (ou chaîne vide s'il n'y en a pas)
	*
	* Retour : TypTache*, pointeur sur la tâche créée
	*
	* Description : Crée une tâche dont les informations sont passées
	*               en paramètre. La mémoire est allouée dans la fonction.
	*/
TypTache* creerTache(char nom, char *intitule, int duree, char *dependances) {
	TypTache *tache;  /* La nouvelle tâche créée */
	
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
	* Paramètres : TypTache **taches, un tableau de tâches
	*              int nbTaches, la taille du tableau taches
	*
	* Retour : TypGraphePERT*, pointeur sur le graphe PERT créé
	*
	* Description : Crée un graphe PERT à partir d'un tableau de tâches.
	*               Les tâches alpha et oméga sont créées dans la fonction.
	*/
TypGraphePERT* creerGraphePERT(TypTache **taches, int nbTaches) {
	TypGraphePERT *graphePERT;               /* Le graphe PERT créé */
	TypGraphe     *graphe;                   /* Le graphe associé */
	TypTache      *tacheDepart;              /* La tâche alpha de départ */
	TypTache      *tacheArrivee;             /* La tâche oméga d'arrivée */
	char          *dependances;               /* Les dépendances d'une tâche */
	int           sommet;                    /* Le numéro d'un sommet */
	bool          sansSuccesseur[nbTaches];  /* si [i] = true, alors la tâche i+1 
												est à relier au sommet oméga */
	int           i;                         /* Permet le parcours des boucles */
	int           j;                         /* Permet le parcours des boucles */
	
	/* Initialisation du tableau sansSuccesseur */
	for (i = 0; i <= nbTaches - 1; i++) {
		sansSuccesseur[i] = true;
	}
	
	/* Création du graphe associé au graphe PERT */
	graphe = creerGraphe(nbTaches + 2);
	
	/* Création des sommets */
	for (i = 1; i <= nbTaches + 2; i++) {
		insertionSommet(graphe,i);
	}
	
	/* Création des tâches alpha et oméga */
	tacheDepart = creerTache(sommetEnTache(nbTaches+1),'\0',0,'\0');
	tacheArrivee = creerTache(sommetEnTache(nbTaches+2),'\0',0,'\0');
	
	/* Création des arêtes */
	for (i = 1; i <= nbTaches; i++) {
		j = 0;
		dependances = taches[i-1]->dependances;
		
		while (dependances[j] != '\0') {
			sommet = tacheEnSommet(dependances[j]);
			insertionAreteOriente(graphe,sommet,i,taches[sommet-1]->duree);
			sansSuccesseur[sommet-1] = false;
			j++;
		}
		
		/* Si la tâche n'a pas de prédecesseur, on la relie à alpha */
		if (j == 0) {
			insertionAreteOriente(graphe,nbTaches+1,i,0);
		}
	}
	
	/* On relie les tâches sans successeurs au sommet oméga */
	for (i = 0; i <= nbTaches - 1; i++) {
		if (sansSuccesseur[i] == true) {
			insertionAreteOriente(graphe,i+1,nbTaches+2,taches[i]->duree);
		}
	}
	
	/* On ajoute les tâches alpha et oméga au tableau des tâches */
	taches = realloc(taches,(nbTaches+2) * sizeof(TypTache));
	taches[nbTaches] = tacheDepart;
	taches[nbTaches+1] = tacheArrivee;
	
	/* Création du graphe PERT */
	graphePERT = malloc(sizeof(TypGraphePERT));
	graphePERT->graphe = graphe;
	graphePERT->taches = taches;
	
	return graphePERT;
}


	/*
	* Fonction : deleteGraphePERT
	*
	* Paramètres : TypGraphePERT *graphePERT, pointeur sur un graphe PERT
	*
	* Description : Supprime le graphe PERT passé en paramètre. Toute la
	*				mémoire qui lui avait été allouée est libérée.
	*/
void deleteGraphePERT(TypGraphePERT *graphePERT) {
	int i;
  
	/* Libération de la mémoire occupée par les tâches */
	for (i = 0; i < graphePERT->graphe->nbrMaxSommets; i++) {
	  free(graphePERT->taches[i]);
	}
	free(graphePERT->taches);
	
	/* Suppression du graphe associé */
	deleteGraphe(graphePERT->graphe);
	
	/* Libération de la mémoire occupée par le graphe PERT */
	free(graphePERT);
}


	/*
	* Fonction : calculDates
	*
	* Paramètres : TypGraphePERT *graphePERT, pointeur sur un graphe PERT
	*
	* Description : Calcule les dates au plus tôt et au plus tard 
	*				de toutes les tâches du graphe PERT passé en paramètre
	*/
void calculDates(TypGraphePERT *graphePERT) {
	calculDatesAuPlusTot(graphePERT,graphePERT->graphe->nbrMaxSommets-1);
	calculDatesAuPlusTard(graphePERT);
}


	/*
	* Fonction : calculDatesAuPlusTot
	*
	* Paramètres : TypGraphePERT *graphePERT, pointeur sur un graphe PERT
	*              int sommetDepart, le numéro du sommet de départ
	*
	* Description : Calcule les dates au plus tôt d'un graphe de toutes les
	*				tâches du graphe PERT passé en paramètre.
	*				Pour faire cela, on implémente l'algo de Bellman-Ford dans
	*				sa version de calcul de plus long chemin à origine unique.
	*/
static void calculDatesAuPlusTot(TypGraphePERT *graphePERT, int sommetDepart) {
	TypGraphe *graphe;   /* Le graphe associé au graphe PERT */
	TypTache  **taches;  /* Les tâches associées au graphe PERT */
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
		if (i != sommetDepart) {
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
	TypTache      **taches;            /* Les tâches associées au graphe PERT */
	TypTache      **tachesInverse;     /* Les tâches associées au graphe inversé */
	TypGraphe     *graphe;             /* Le graphe du graphe PERT inversé avant l'inversion */
	int           nbSommets;           /* Le nombre de sommets du graphe */
	int           i;                   /* Permet le parcours des sommets */
	
	taches = graphePERT->taches;
	nbSommets = graphePERT->graphe->nbrMaxSommets;
	
	/* Création du graphe inversé */
	tachesInverse = malloc((nbSommets-2) * sizeof(TypTache));
	for (i = 0; i <= nbSommets - 3; i++) {
		tachesInverse[i] = creerTache(taches[i]->nom,taches[i]->intitule,taches[i]->duree,taches[i]->dependances);
	}
	graphePERTInverse = creerGraphePERT(tachesInverse,nbSommets-2);
	
	graphe = graphePERTInverse->graphe;
	graphePERTInverse->graphe = NULL;
	
	graphePERTInverse->graphe = grapheInverse(graphe);
	
	/* On calcule les dates au plus tôt du graphe inversé */
	calculDatesAuPlusTot(graphePERTInverse,nbSommets);
	
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
	
	deleteGraphe(graphe);
	deleteGraphePERT(graphePERTInverse);
}


	/*
	* Fonction : dureeTotale
	*
	* Paramètres : TypGraphePERT *graphePERT, pointeur sur un graphe PERT
	*              int nbOuvriers, le nombre d'ouvriers (>= 1)
	*
	* Retour : int, la durée totale du chantier
	*
	* Description : Renvoie la durée totale du chantier représenté par le
	*				graphe PERT passé en paramètre selon le nombre d'ouvriers.
	*/
int dureeTotale(TypGraphePERT *graphePERT, int nbOuvriers) {
	int nbTaches = graphePERT->graphe->nbrMaxSommets; /* Nombre de tâches */
	
	/* Si le nb d'ouvriers est égal au nb de tâches, le problème est trivial */
	if (nbOuvriers >= nbTaches) {
		return graphePERT->taches[graphePERT->graphe->nbrMaxSommets - 1]->dateTot;
	}
	else {
		TypTache **taches; /* Toutes les tâches du graphe PERT */
		TypTache **tachesTerminees; /* Tableau des tâches terminées */
		TypTache **tachesLibres; /* Tableau des tâches libres, c'est-à-dire des
									tâches affectables à un ouvrier */
		TypTache **tachesNonLibres; /* Tableau des tâches ni commencées 
										ni libres */
		TypTacheEnCours **tachesEnCours; /* Tableau des tâches en cours 
											de réalisation */
		int nbTachesTerminees; /* Nb de tâches terminées */
		int nbTachesEnCours; /* Nb de tâches en cours */
		int nbTachesLibres; /* Nb de tâches libres */
		int nbTachesNonLibres; /* Nb de tâches non libres */
		int dureeTotale; /* Durée totale du chantier (résultat à renvoyer) */
		int i;  /* Indice de parcours des boucles */
		
		taches = graphePERT->taches;
		tachesTerminees = malloc(nbTaches * sizeof(TypTache));
		nbTachesTerminees = 0;
		tachesLibres = malloc(nbTaches * sizeof(TypTache));
		nbTachesLibres = 0;
		tachesNonLibres = malloc(nbTaches * sizeof(TypTache));
		nbTachesNonLibres = 0;
		tachesEnCours = malloc(nbTaches * sizeof(TypTacheEnCours));
		nbTachesEnCours = 0;
		
		dureeTotale = 0;
		
		/* 
		* Initialisation : on affecte la tache de départ à un ouvrier,
		* et on range les autres tâches dans tachesNonLibres
		*/ 
		tachesEnCours[0] = creerTacheEnCours(taches[nbTaches-2]);
		nbTachesEnCours = 1;
		
		for (i = 0; i <= nbTaches - 1; i++) {
			if (i != nbTaches - 2) {
				tachesNonLibres[nbTachesNonLibres] = taches[i];
				nbTachesNonLibres++;
			}
		}
		
		/* Tant que toutes les tâches ne sont pas terminées */
		while (nbTachesTerminees != nbTaches) {
			int dureeRestante; /* La durée restante de la tâche qui se termine */
			
			/* 
			* On considère que la prochaine tâche qui va se terminer est en
			* tête (position 0) du tableau des tâches en cours. Cette tâche
			* est retirée de tachesEnCours est est ajoutée à tachesTerminees.
			*
			* On ajoute la durée restante de cette tâche à la durée totale.
			*/
			tachesTerminees[nbTachesTerminees] = tachesEnCours[0]->tache;
			nbTachesTerminees++;
			dureeRestante = tachesEnCours[0]->dureeRestante;
			nbTachesEnCours--;
			dureeTotale += dureeRestante;
			
			/* On calcule la durée restante des tâches en cours restantes */
			decompteDuree(tachesEnCours,nbTachesEnCours,dureeRestante);
			
			/* 
			* On ajoute à tachesLibres d'éventuelles nouvelles tâches libres,
			* c'est-à-dire les tâches non libres dont toutes les tâches dont
			* elles dépendent sont terminées
			*/
			chercheTachesLibres(tachesNonLibres,tachesLibres,tachesTerminees,
				&nbTachesNonLibres,&nbTachesLibres,nbTachesTerminees);
				
			/* On affecte des tâches libres à des ouvriers */
			affecteTaches(tachesLibres,tachesEnCours,
				&nbTachesLibres,&nbTachesEnCours,nbOuvriers);
			
			/* 
			* Tri du tableau des tâches en cours dans l'ordre croissant
			* des durées de réalisation restantes. Cela permet d'avoir
			* en tête la prochaine tâche qui se termine.
			*/
			triTachesEnCours(tachesEnCours,nbTachesEnCours);
		}
		
		/* Libération de la mémoire */
		/* A COMPLETER */
		
		return dureeTotale;
	}
}


	/*
	* Fonction : creerTacheEnCours
	*
	* Paramètres : TypTache *tache, la tâche qui va être réalisée
    *
    * Retour : TypTacheEnCours, la nouvelle structure
	*
	* Description : Crée une structure de type TypTacheEnCours, lui alloue
    *               la mémoire et la renvoie.
	*/
static TypTacheEnCours* creerTacheEnCours(TypTache* tache) {
	TypTacheEnCours* tacheEnCours; /* La nouvelle tacheEnCours */
    
    tacheEnCours = malloc(sizeof(TypTacheEnCours));
    tacheEnCours->tache = tache;
    tacheEnCours->dureeRestante = tache->duree;
    
    return tacheEnCours;
}


	/*
	* Fonction : decompteDuree
	*
	* Paramètres : TypTacheEnCours **tachesEC, tableau des 
	*					tâches en cours de réalisation.
	*              int nbTachesEC, nb de taches en cours de réalisation.
	*              int duree, durée à décompter.
	*
	* Description : Soustrait à la durée restante de chacune des tâches en
	*               cours la durée passée en paramètre
	*/
static void decompteDuree(TypTacheEnCours **tachesEC,int nbTachesEC,int duree) {
    int      i;                /* indice de parcours */

	for(i = 1; i <= nbTachesEC; i++){
		
		tachesEC[i]->dureeRestante = tachesEC[i]->dureeRestante - duree;
		
	}
}


	/*
	* Fonction : chercheTachesLibres
	*
	* Paramètres : TypTache **tachesNL, tableau de tâches non libres.
	*              TypTache **tachesL, tableau de tâches libres.
	*              TypTache **tachesT, tableau des tâches terminées.
	*              int *nbTachesNL, nb de tâches non libres.
	*              int *nbTachesL, nb de tâches libres.
	*              int nbTachesT, nb de tâches terminées.
	*
	* Description : Ajoute à tachesL et supprime de taches NL l'ensemble des
	*               tâches non libres dont tous les prédécesseurs ont terminé
	*               leur tâche. Les entiers nbTachesNL et nbTachesL sont
	*               susceptibles d'être modifiés.
	*/
static void chercheTachesLibres(TypTache **tachesNL,TypTache **tachesL,
		TypTache **tachesT,int *nbTachesNL,int *nbTachesL, int nbTachesT) {
	
	int i, j; /* Permettent le parcours des boucles */
	
	i = 0;
	
	while (i <= *nbTachesNL - 1) {
		TypTache *tacheNL;  /* Une tâche non libre */
		char dependances[50];  /* Les dépendances de la tâche */
		int nbDependancesT; /* Nb de dépendances terminées */
        
		tacheNL = tachesNL[i];
        strcpy(dependances,tacheNL->dependances);
		nbDependancesT = 0;
		
		for (j = 0; j <= nbTachesT - 1; j++) {
			TypTache *tacheT;
			
			tacheT = tachesT[j];
			/* Si la tâche terminée est une dépendance de tacheNL */
			if (strchr(dependances,tacheT->nom) != NULL) {
				nbDependancesT++;
			}
		}
		
		/* 
		* Si toutes les dépendances de tacheNL sont terminées, alors on peut
		* l'ajouter au tableau des tâches libres, et la supprimer des tâches
		* non libres
		*/
		if (nbDependancesT == strlen(dependances)) {
			TypTache *tmp; /* Permet l'échange de tâches dans tachesNL */
			
			tachesL[*nbTachesL] = tacheNL;
			(*nbTachesL)++;
			
			if (i != *nbTachesNL - 1) {
				tachesNL[i] = tachesNL[*nbTachesNL-1];
				(*nbTachesNL)--;
			}
			else {
				i++;
			}
		}
		else {
			i++;
		}
	}
}


	/*
	* Fonction : affecteTaches
	*
	* Paramètres : TypTache **tachesL, tableau de tâches libres trié dans
	*                  l'ordre croissant de leur durée restante
	*              TypTacheEnCours **tachesEC, tableau de tâches en cours.
	*              int *nbTachesL, nb de tâches libres.
	*              int *nbTachesEC, nb de tâches en cours.
	*              int nbOuvriers, le nombre d'ouvriers.
	*
	* Description : Affecte des ouvriers à des tâches : déplace de tachesL à
	*               tachesEC certaines tâches. Le choix des tâches à affecter
	*               se fait selon l'ordre suivant :
	*               1. Une tâche se situe dans le chemin critique.
	*               2. Tâche dont la date au plus tard est la plus éloignée
	*                  de la fin du chantier
	*
	*               Les 2 entiers nbTachesL et nbTachesEC sont susceptibles
	*               d'être modifiés.
	*/
static void affecteTaches(TypTache **tachesL,TypTacheEnCours **tachesEC,
								int *nbTachesL,int *nbTachesEC,int nbOuvriers) {
	
	int i;  /* Permet le parcours des tâches libres */
	TypTache *tacheL;  /* Une tâche libre */
	bool choixCCTermine; /* true si il n'y a plus de tâche libre 
							se trouvant sur le chemin critique */
	int indDateMin;  /* indice de la date au + tard la plus faible */
							
	i = 0;
	choixCCTermine = false;
	
	/* 
	* La boucle s'arrête soit lorsque tous les ouvriers ont été affectés à une
	* tâche, soit lorsqu'il n'y a plus de tâche libre
	*/
	while ((nbOuvriers != *nbTachesEC) && (*nbTachesL > 0)) {
		tacheL = tachesL[i];
	
		if (choixCCTermine == false) {
			/* 
			* Si la tâche libre se trouve sur le chemin critique alors on
			* l'affecte à un ouvrier
			*/
			if (tacheL->dateTot == tacheL->dateTard) {
				tachesL[i] = tachesL[*nbTachesL - 1];
				(*nbTachesL)--;
				tachesEC[*nbTachesEC] = creerTacheEnCours(tacheL);
				(*nbTachesEC)++;
			}
			else {
				i++;
			}
			
			/*
			* Si on a fini de parcourir toutes les tâches libres une première
			* fois, alors on est sûr qu'il n'y a plus de tâche libre se
			* trouvant sur le chemin critique
			*/
			if (i == *nbTachesL - 1) {
				choixCCTermine = true;
			}
		}
		else {
			/* 
			* On cherche la tâche libre dont la date au plus tard est la plus
			* éloignée de la fin du chantier
			*/
			indDateMin = 0;
			for (i = 1; i <= *nbTachesL - 1; i++) {
				if (tachesL[i]->dateTard < tachesL[indDateMin]->dateTard) {
					indDateMin = i;
				}
			}
			
			/*
			* On affecte la tâche trouvée à un ouvrier
			*/
			tacheL = tachesL[indDateMin];
			tachesL[indDateMin] = tachesL[*nbTachesL - 1];
			(*nbTachesL)--;
			tachesEC[*nbTachesEC] = creerTacheEnCours(tacheL);
			(*nbTachesEC)++;
		}
	}
}


	/*
	* Fonction : triTachesEnCours
	*
	* Paramètres : TypTacheEnCours **tachesEC, tableau de tâches en cours.
	*              int nbTachesEC, nb de tâches en cours.
	*
	* Description : Trie le tableau des tâches en cours par ordre croissant
	*               de leur durée de réalisation restante.
	*/
static void triTachesEnCours(TypTacheEnCours **tachesEC,int nbTachesEC) {
	int      i, j;        /* indice de parcours */
	TypTacheEnCours *tmp;
	
	do {
        j = 0;
        for(i = 0; i <= nbTachesEC-1; i++){
            
            if(tachesEC[i]->dureeRestante > tachesEC[i+1]->dureeRestante) {
                tmp = tachesEC[i];
                tachesEC[i] = tachesEC[i+1];
                tachesEC[i+1] = tmp;
                j = 1;

            }
            
        }
    } while(j == 1);
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
	TypTache **taches; /* Les tâches associées au graphe PERT */
	int      i;        /* Permet le parcours des tâches */
	
	taches = graphePERT->taches;
	
	printf("# nom, date au plus tot, date au plus tard\n");
	
	for (i = 1; i <= graphePERT->graphe->nbrMaxSommets - 2; i++) {
		printf("%c, ",taches[i-1]->nom);
		printf("%d, ",taches[i-1]->dateTot);
		printf("%d\n",taches[i-1]->dateTard);
	}
}


	/*
	* Fonction : afficherCheminCritique
	*
	* Paramètres : TypGraphePERT *graphePERT, pointeur sur un graphe PERT
	*
	* Description : Affiche le Chemin critique sous la forme :
	*
	*				# nomTache1 -> nomTache2 -> .....
	*				A -> B -> C -> D
	*				
	*/
void afficherCheminCritique(TypGraphePERT* graphePERT){
	TypTache      **taches;            /* Les tâches associées au graphe PERT */
	int 		i;		/* variable de parcours des tâches du graphe*/
	int 		tmp1;		/* date au plus tôt de la tâche courante */
	int 		tmp2;		/* ate au plus tard de la tâche courante */
    int         cmp;        /* Le nombre de sommets trouvés sur le chemin critique */

    cmp = 0;
	taches = graphePERT->taches;
	printf("Chemin critique correspondant au graphe en cours :\n");

	for(i = 1; i <= graphePERT->graphe->nbrMaxSommets - 2; i++){

		tmp1 = taches[i-1]->dateTot;
		tmp2 = taches[i-1]->dateTard;

        if(tmp1 == tmp2){
            if (cmp != 0) {
                printf(" --> ");
            }
            
            printf("%c",taches[i-1]->nom);
            cmp++;
        }
	}
}


	/*
	* Fonction : lireGraphePERT
	*
	* Paramètres : FILE *fichier, un fichier de type chantier.txt 
    *                 ouvert en lecture
    *
    * Retour : TypGraphePERT*, le graphe créé
	*
	* Description : Lit un fichier de type chantier.txt, puis crée le graphe
    *               correspondant et le renvoie
	*/
TypGraphePERT* lireGraphePERT(FILE *fichier) {
    TypGraphePERT *graphePERT; /* Le graphe PERT créé */
    TypTache      **taches;    /* Les tâches du graphe */
    char          ligne[512];  /* Ligne lue dans le fichier */
    int           nbTaches;    /* Nombre de tâches total */
    
    nbTaches = 0;
    taches = malloc(10 * sizeof(TypTache));
    
    /* Lecture des 2 premières lignes */
    fgets(ligne,sizeof(ligne),fichier);
    fgets(ligne,sizeof(ligne),fichier);
    
    while (! feof(fichier)) {
        char nom;
        char *intitule;
        int  duree;
        char *dependances;
        
        intitule = malloc(200 * sizeof(char));
        dependances = malloc(50 * sizeof(char));
        
        /* Lecture d'une ligne */
        fgets(ligne,sizeof(ligne),fichier);
        
        sscanf(ligne,"%c, '%[^']', %d, %[^\r\n]",&nom,intitule,&duree,dependances);
        
        if (strcmp(dependances,"-") == 0) {
            dependances = replace(dependances,"-","");
        }
        else {
            dependances = replace(dependances,", ","");
        }
        
        intitule = realloc(intitule,strlen(intitule)+1);
        dependances = realloc(dependances,strlen(dependances)+1);
        
        /* Création de la tâche décrite dans cette ligne */
        if ( ((nbTaches % 10) == 0) && (nbTaches != 0) ) {
            taches = realloc(taches,(nbTaches + 10) * sizeof(TypTache));
        }
        taches[nbTaches] = creerTache(nom,intitule,duree,dependances);
        nbTaches++;
	
	//free(intitule);
    //free(dependances);
    }
    
    taches = realloc(taches,nbTaches * sizeof(TypTache));
    
    /* Création du graphe PERT */
    graphePERT = creerGraphePERT(taches,nbTaches);
    
    return graphePERT;
}


	/*
	* Fonction : tacheEnSommet
	*
	* Paramètres : char nom, le nom d'une tâche
	*
	* Retour : int, le numéro du sommet correspondant
	*
	* Description : Renvoie le numéro du sommet correspondant au nom
	*               de la tâche. On considère que :
	*               'A' <-> 1
	*               'B' <-> 2  etc...
	*/
static int tacheEnSommet(char nom) {
	return (int) nom - 64;
}


	/*
	* Fonction : sommetEnTache
	*
	* Paramètres : int sommet, le numéro d'un sommet d'un graphe
	*
	* Retour : char, le nom de la tâche correspondante
	*
	* Description : Renvoie le nom de la tâche correspondant au numéro
	*               du sommet. On considère que :
	*               1 <-> 'A'
	*               2 <-> 'B'  etc...
	*/
static char sommetEnTache(int sommet) {
	return (char) (sommet + 64);
}


	/*
	* Fonction : replace
	*
	* Paramètres : char *str, la chaîne d'origine
    *              char *old, la sous-chaîne à modifier par new
    *              char *new, la sous-chaîne qui remplace old
    *
    * Retour : char*, pointeur sur la nouvelle chaîne
	*
	* Description : Remplace new par old dans str et renvoie un pointeur
    *               sur la nouvelle chaine
	*/
static char* replace(const char *str, const char *old, const char *new) {
	char *ret, *r;
	const char *p, *q;
	size_t oldlen = strlen(old);
	size_t count, retlen, newlen = strlen(new);

	if (oldlen != newlen) {
		for (count = 0, p = str; (q = strstr(p, old)) != NULL; p = q + oldlen)
			count++;
		retlen = p - str + strlen(p) + count * (newlen - oldlen);
	} else
		retlen = strlen(str);

	if ((ret = malloc(retlen + 1)) == NULL)
		return NULL;

	for (r = ret, p = str; (q = strstr(p, old)) != NULL; p = q + oldlen) {
		ptrdiff_t l = q - p;
		memcpy(r, p, l);
		r += l;
		memcpy(r, new, newlen);
		r += newlen;
	}
	strcpy(r, p);

	return ret;
}