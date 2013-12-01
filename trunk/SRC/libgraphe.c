/*
******************************************************************************
*
* Programme : libgraphe.c
*
* Auteurs : Christophe SAUVAGE et Mustafa NDIAYE
*
* Résumé : Bibliothèque permettant de créer et gérer des graphes.
*
*          On considère que pour un graphe, listesAdjacences[0] désigne le
*          sommet 1, listesAdjacences[1] le sommet 2 etc...
*
* Date : 18/10/2013
*
******************************************************************************
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "libgraphe.h"
#include "libliste.h"
#include "erreurs.h"


	/*
	* Fonction : creerGraphe
	*
	* Paramètres : int nbrMaxSommets, le nombre maximal de sommets que pourra 
	*                 contenir le graphe
	*
	* Retour : TypGraphe*, pointeur sur le graphe créé
	*
	* Description : Crée un graphe vide et le renvoie. La mémoire pour celui-ci
	*               est allouée dans la fonction.
	*/
TypGraphe* creerGraphe(int nbrMaxSommets) {
	int        i;      /* Permet le parcours du tableau de listes */
	TypGraphe* graphe; /* Le graphe créé */
	
	graphe = malloc(sizeof(TypGraphe));
	graphe->nbrMaxSommets = nbrMaxSommets;
	graphe->listesAdjacences = ( TypVoisins** ) malloc( nbrMaxSommets * sizeof( TypVoisins* ) );
		
	for ( i = 0; i < nbrMaxSommets; i++) {
		graphe->listesAdjacences[i] = NULL;
	} 

	return graphe;
}


	/*
	* Fonction : insertionSommet
	*
	* Paramètres : TypGraphe *graphe, pointeur sur un graphe déjà créé
	*			   int idSommet, le numéro du sommet à insérer
	*
	* Retour : int, renvoie 0 si le sommet est inséré sinon renvoie un 
	*             code d'erreur (voir erreurs.h)
	*
	* Description : Ajoute un sommet dans le graphe
	*/
int insertionSommet ( TypGraphe *graphe, int idSommet ) {
	
	if ( graphe->listesAdjacences == NULL ) {
		return GRAPHE_INEXISTANT;
	}
	
	if( (idSommet > 0 ) && ( idSommet <= graphe->nbrMaxSommets ) ) {
		if ( graphe->listesAdjacences[idSommet-1] == NULL ) {
            graphe->listesAdjacences[idSommet-1] = creerListe();
			return 0;
        }
		else
			return SOMMET_EXISTANT;
	}
	else {
		return SOMMET_INVALIDE;
	}
}


	/*
	* Fonction : sommetExistant
	*
	* Paramètres : TypGraphe *graphe, pointeur sur un graphe déjà créé
	*			   int numSommet, le numéro du sommet à rechercher
	*
	* Retour : int, 0 si le sommet est trouvé sinon renvoie un 
	*            code d'erreur (voir erreurs.h)
	*
	* Description : Vérifie si le sommet est présent dans le graphe
	*/
int sommetExistant ( TypGraphe * graphe, int numSommet ) {

	if ( graphe->listesAdjacences == NULL ) {
		return GRAPHE_INEXISTANT;
	} 
	else {
		if ( numSommet > graphe->nbrMaxSommets ) {
			return SOMMET_INVALIDE;
		}
	}

	if ( graphe->listesAdjacences[ numSommet-1 ] != NULL ) {
		return 0;
	} else {
		return SOMMET_INEXISTANT;
	}

}


	/*
	* Fonction : areteExistante
	*
	* Paramètres : TypGraphe *graphe, pointeur sur un graphe déjà créé
	*			   int depart, le sommet de départ de l'arête
	*              int arrivee, le sommet sur lequel pointe l'arête
	*
	* Retour : int, renvoie 0 si l'arête est trouvée sinon renvoie un 
	*             code d'erreur (voir erreurs.h)
	*
	* Description : Vérifie si une arête existe dans le graphe
	*/
int areteExistante (TypGraphe *graphe, int depart, int arrivee) {
	if (graphe->listesAdjacences == NULL)
		return GRAPHE_INEXISTANT;
	else {
		if (sommetExistant(graphe,depart) == 0 && sommetExistant(graphe,arrivee) == 0) {
			if (voisinExiste(&(graphe->listesAdjacences[depart-1]),arrivee) == true)
				return 0;
			else
				return ARETE_INEXISTANTE;
		}
		else
			return SOMMET_INEXISTANT;
	}
}


	/*
	* Fonction : suppressionSommet
	*
	* Paramètres : TypGraphe *graphe, pointeur sur un graphe déjà créé
	*			   int sommet, le numéro du sommet à supprimer
	*
	* Retour : int, renvoie 0 si le sommet est supprimé sinon renvoie un 
	*             code d'erreur (voir erreurs.h)
	*
	* Description : Supprime un sommet du graphe
	*/
int suppressionSommet ( TypGraphe* graphe, int sommet ) {
	int i;  /* Permet le parcours du tableau du graphe */
  
	if (graphe->listesAdjacences == NULL)
		return GRAPHE_INEXISTANT;
	else {
		if (sommetExistant(graphe,sommet) == 0) {
			supprimerListe(&(graphe->listesAdjacences[sommet-1]));
			for (i = 0; i < graphe->nbrMaxSommets; i++) {
				if (graphe->listesAdjacences[i] != NULL)
					supprimerVoisin(&(graphe->listesAdjacences[i]),sommet);
			}
			return 0;
		}
		else
			return SOMMET_INEXISTANT;
	}
}


	/*
	* Fonction : insertionAreteOriente
	*
	* Paramètres : TypGraphe *graphe, pointeur sur un graphe déjà créé
	*			   int depart, le sommet de depart de l'arête
	*              int arrivee, le sommet sur lequel pointe l'arête
	*              int poids, le poids de l'arête
	*
	* Retour : int, renvoie 0 si l'arête est insérée sinon renvoie un 
	*             code d'erreur (voir erreurs.h)
	*
	* Description : Ajoute une arête orientée dans le graphe. Cela suppose
	*               que les sommets soient déjà insérés dans le graphe.
	*/
int insertionAreteOriente ( TypGraphe* graphe, int depart, int arrivee, int poids ) {
	if (graphe->listesAdjacences == NULL)
		return GRAPHE_INEXISTANT;
	else if (areteExistante(graphe,depart,arrivee) == 0)
		return ARETE_EXISTANTE;
	else {
		if (sommetExistant(graphe,depart) == 0 && sommetExistant(graphe,arrivee) == 0) {
			ajouterVoisin(&(graphe->listesAdjacences[depart-1]),arrivee,poids);
			return 0;
		}
		else
			return SOMMET_INEXISTANT;
	}
}


	/*
	* Fonction : insertionAreteNonOriente
	*
	* Paramètres : TypGraphe *graphe, pointeur sur un graphe déjà créé
	*			   int depart, un sommet du graphe
	*              int arrivee, un sommet du graphe
	*              int poids, le poids de l'arête
	*
	* Retour : int, renvoie 0 si l'arête est insérée sinon renvoie un 
	*             code d'erreur (voir erreurs.h)
	*
	* Description : Ajoute une arête non orientée dans le graphe. Cela suppose
	*               que les sommets soient déjà insérés dans le graphe.
	*/
int insertionAreteNonOriente ( TypGraphe* graphe, int depart, int arrivee, int poids ) {
	if (graphe->listesAdjacences == NULL)
		return GRAPHE_INEXISTANT;
	else if (areteExistante(graphe,depart,arrivee) == 0 || areteExistante(graphe,arrivee,depart) == 0)
		return ARETE_EXISTANTE;
	else {
		if (sommetExistant(graphe,depart) == 0 && sommetExistant(graphe,arrivee) == 0) {
			ajouterVoisin(&(graphe->listesAdjacences[depart-1]),arrivee,poids);
			ajouterVoisin(&(graphe->listesAdjacences[arrivee-1]),depart,poids);
			return 0;
		}
		else
			return SOMMET_INEXISTANT;
	}
}

	/*
	* Fonction : suppressionArete
	*
	* Paramètres : TypGraphe *graphe, pointeur sur un graphe déjà créé
	*			   int depart, le sommet de départ de l'arête
	*              int arrivee, le sommet sur lequel pointe l'arête
	*              char orientation, 'o' si l'arête est orientée, sinon 'n'
	*
	* Retour : int, renvoie 0 si l'arête est supprimée sinon renvoie un 
	*             code d'erreur (voir erreurs.h)
	*
	* Description : Supprime une arête  dans le graphe
	*/
int suppressionArete ( TypGraphe* graphe, int depart, int arrivee, char orientation ) {
	if (graphe->listesAdjacences == NULL)
		return GRAPHE_INEXISTANT;
	else {
		if (sommetExistant(graphe,depart) == 0 && sommetExistant(graphe,arrivee) == 0) {
			if (orientation == 'o') {
				if (areteExistante(graphe,depart,arrivee) == 0) {
					supprimerVoisin(&(graphe->listesAdjacences[depart-1]),arrivee);
					return 0;
				}
				else
				return ARETE_INEXISTANTE;
			}
			else {
				if (areteExistante(graphe,depart,arrivee) == 0 && areteExistante(graphe,arrivee,depart) == 0) {
					supprimerVoisin(&(graphe->listesAdjacences[depart-1]),arrivee);
					supprimerVoisin(&(graphe->listesAdjacences[arrivee-1]),depart);
					return 0;
				}
				else
					return ARETE_INEXISTANTE;
			}
		}
		else
			return SOMMET_INEXISTANT;
	}
}


/*
 *******************************************************************************
 *	Fonction:	affichage
 *
 *	Paramettre :	TypGraphe * graphe      : est un pointeur sur TypGraphe
 *			
 *
 *	Retour:		rien
 *
 *	Description:	affichage d'un graphe
 ********************************************************************************
 */
void affichage ( TypGraphe* graphe ) {
	int i;         /* Permet le parcours des listes du graphe */
	char* chaine;  /* Chaîne où seront stockées les représentations des listes*/
	
	printf ( "# nombre maximum de sommets\n" );
	printf ( "%d",graphe->nbrMaxSommets );
	printf ( "\n" );
	printf ( "# sommets : voisins\n" );
	
	for ( i=0; i< graphe->nbrMaxSommets; i++ ) {
		if (sommetExistant(graphe,i+1) == 0) {
			printf ( "%d : ", i+1 );
		    if ( graphe->listesAdjacences[i] != NULL ) {
				chaine = toString(&(graphe->listesAdjacences[i]));
				if (chaine != NULL) {
					printf("%s",chaine);
					free(chaine);
				}
		    }
		    printf( "\n" );
		}
	}
}


/*
 *******************************************************************************
 *	Fonction:	sauvegarde
 *
 *	Paramettre :	TypGraphe * graphe : est un pointeur sur TypGraphe
 *			FILE *fichier	   : est un pointeur sur FILE
 *
 *	Retour:		rien
 *
 *	Description:	sauvegarder le graphe dans un fichier
 ********************************************************************************
 */
void sauvegarde (TypGraphe* graphe,FILE *fichier){
	int i ;  /* Permet le parcours des listes du graphe */
	
	if ( graphe == NULL ) {  	
		fprintf( stderr, "graphe inexistant\n" );
	}
	
	fprintf( fichier, "# nombre maximum de sommets\n%d\n# sommets : voisins\n", graphe->nbrMaxSommets );
	
	for ( i = 0; i < graphe->nbrMaxSommets ; i++ ) {
		if (sommetExistant(graphe,i+1) == 0) {
			fprintf( fichier, "%d : ", i+1 );
		    
		    if ( graphe->listesAdjacences[ i ] != NULL ) {
				afficherListeFichier( graphe->listesAdjacences[ i ], fichier );
		    }
		    
		    fprintf(fichier,"\n");	
		}
	}
}
 

	/*
	* Fonction : deleteGraphe
	*
	* Paramètres : TypGraphe *graphe, pointeur sur un graphe déjà créé
	*
	* Retour : void
	*
	* Description : Supprime le graphe passé en paramètre. Toute la mémoire qui 
	*               lui avait été allouée est libérée.
	*/
void deleteGraphe ( TypGraphe* graphe ) {
	int i;  /* Permet le parcours des sommets du graphe */
	
	/* Suppression de tous les sommets (donc de toutes les listes)*/
	for (i = 0; i < graphe->nbrMaxSommets; i++)
		suppressionSommet(graphe,i+1);
		
	/* Libération de la mémoire occupée par le tableau*/
	free(graphe->listesAdjacences);
	
	/*Libération de la mémoire occupée par le graphe*/
	free(graphe);
}


/*
 *******************************************************************************
 *	Fonction:	lecture
 *
 *	Paramètres : TypGraphe** graphe, le graphe à créer depuis le chargement du fichier
*				char nomFichier[80] : est un tableau de caractére représentant le nom du fichier
 *
 *	Retour:		0 si tout se passe bien, sinon un code d'erreur
 *
 *	Description:	lecture d'un graphe à partir d'un fichier
 ********************************************************************************
 */
int lecture (TypGraphe** graphe, char nomFichier[80]) {
	char buffer[ 512 ],chemin[ 80 ] = "lecture/";
	int maxSommets = 0, sommetCourant = 0, vers = 0, poids = 0, m, i =0, k=0, t=0;
	int tabSommet[ 100 ]; /* Tableau pour stocker tous les sommets du graphe */
	int tabArete[ 100 ][ 100 ];/* Tableau pour stocker tous les aretes du graphe */
	FILE *fichier = NULL;
	tabSommet [ maxSommets ] = 0;/* Initialisation du tableau tabSommet */
	memset ( *tabArete, 0, sizeof ( tabArete ) );/* Initialisation du tableau tabArete */
	strcat ( chemin, nomFichier );
	fichier = fopen ( chemin, "r+" );/*Ouverture du fichier en mode lecture*/
	if ( fichier == NULL ) {
		return ERREUR_FICHIER;
	}
	else {
		if ( fgets ( buffer, 512, fichier ) != NULL ) {
			fscanf ( fichier, "%d", &maxSommets );/*récupération du nombre max de sommet*/
			if ( maxSommets > 0 ) {
				*graphe = creerGraphe(maxSommets );/* Création du graphe si le nbrSommet est positif*/
				fgets ( buffer, 512, fichier );
				fgets ( buffer, 512, fichier );
				while ( !feof ( fichier ) ) {/* parcours du fichier pour récupérer les sommet et les arêtes */
					fscanf ( fichier, "%d : ", &sommetCourant );
					if ( sommetCourant <= maxSommets ) {
						if ( sommetCourant != tabSommet[ i - 1 ] ) {
							tabSommet[ i ] = sommetCourant;
							i++;
						}
						if ( fscanf ( fichier, "(%d,%d),", &vers, &poids ) == 2 ) {
							if ( vers <= maxSommets ) {
								tabArete[ k ][ 0 ] = sommetCourant;
								tabArete[ k ][ 1 ] = vers;
								tabArete[ k ][ 2 ] = poids;
								k++;	
							}
						}
					}
				}
				for ( t = 0; t < i; t++ ) {
					m = insertionSommet ( *graphe, tabSommet[ t ] );/* Insertion des sommets dans le graphe*/	
				}
				for ( t = 0; t < k; t++ ) {
					m = insertionAreteOriente ( *graphe, tabArete[ t ][ 0 ],tabArete[ t ][ 1 ], tabArete[ t ][ 2 ] );
				}	
			}
			else{
				printf ( "Le nombre maximum de sommets est incorrect !\n" );
			}	
		}
		else{
			printf ( "Erreur structuration du fichier!\n" );
		}
		
		fclose ( fichier );
		return 0;
	}
}


	/*
	* Fonction : degreSommet
	*
	* Paramètres : TypGraphe *graphe, pointeur sur un graphe déjà créé
	*              int sommet, le numéro d'un sommet du graphe
	*
	* Retour : int, le degré du sommet
	*
	* Description : Renvoie le degré du sommet passé en paramètre. Renvoie 0 
	*               si le sommet n'existe pas ou n'a pas d'arêtes adjacentes
	*/
int degreSommet(TypGraphe* graphe, int sommet) {
	if (sommetExistant(graphe,sommet) == 0)
		return tailleListe(&(graphe->listesAdjacences[sommet-1]));
	else
		return 0;
}


	/*
	* Fonction : grapheInverse
	*
	* Paramètres : TypGraphe *graphe, pointeur sur un graphe déjà créé
	*
	* Retour : TypGraphe*, le graphe inversé
	*
	* Description : Renvoie un graphe identique à celui passé en paramètre mais
	*               avec les arêtes inversées et qui conservent le même poids
	*/
TypGraphe* grapheInverse(TypGraphe* graphe) {
	TypGraphe* inverse;   /* Le graphe résultat */
    int        i;         /* Permet le parcours des sommets*/
    int        j;         /* Permet le parcours des sommets*/
    
    inverse = creerGraphe(graphe->nbrMaxSommets);
    
    /* Création des sommets (identiques à ceux de graphe) */
    for (i = 1; i <= graphe->nbrMaxSommets; i++) {
		if (sommetExistant(graphe,i) == 0)
			insertionSommet(inverse,i);
    }
    
    /* Création des arêtes */
    for (i = 1; i <= graphe->nbrMaxSommets; i++) {
		for (j = 1; j <= graphe->nbrMaxSommets; j++) {
			if ((i != j) && (areteExistante(graphe,i,j) == ARETE_INEXISTANTE) 
					&& (areteExistante(graphe,j,i) == 0)) {
				insertionAreteOriente(inverse,i,j,poidsArete(graphe,j,i));
			}
		}
    }
    
    return inverse;
}


	/*
	* Fonction : distance
	*
	* Paramètres : TypGraphe *graphe, pointeur sur un graphe déjà créé.
	*	       int depart, le sommet de départ de l'arête
	*	       int arrivee, le sommet d'arrivée de l'arête
	*
	* Retour : int, le poids de l'arête (-1 si celle-ci n'existe pas)
	*
	* Description : Renvoie le poids d'une arête d'un graphe
	*/
int poidsArete(TypGraphe* graphe, int depart, int arrivee) {
	TypVoisins* voisinCourant; /* Le sommet courant dans le parcours des sommets */
	
	voisinCourant = graphe->listesAdjacences[depart-1];
	voisinCourant = voisinSuivant(&voisinCourant);
	
	while (voisinCourant != graphe->listesAdjacences[depart-1]) {
		if (numeroVoisin(&voisinCourant) == arrivee) {
			return poidsVoisin(&voisinCourant);
		}
		else {
			voisinCourant = voisinSuivant(&voisinCourant);
		}
	}
  
	return -1;
}