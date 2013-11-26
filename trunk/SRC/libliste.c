/*
******************************************************************************
*
* Programme : libliste.c
*
* Auteurs : Christophe SAUVAGE et Mustafa NDIAYE
*
* Résumé : Bibliothèque permettant de créer et gérer des listes circulaires
*          doublement chaînées avec sentinelle.
*
* Date : 10/10/2013
*
******************************************************************************
*/


#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "libliste.h"

	
	/*
	* Fonction : creerTypVoisins
	*
	* Paramètres : int voisin, le numéro du voisin à créer
	*              int poids, le poids du voisin à créer
	*              TypVoisins** vS, le voisin qui suivra dans la
	*					liste le voisin créé
	*              TypVoisins** vP, le voisin qui précédera dans
	*					la liste le voisin créé
	*
	* Retour : TypVoisins*, pointeur sur le voisin nouvellement créé
	*
	* Description : Crée et renvoie un nouveau voisin dont les données 
	*               (numéro et poids) sont passées en paramètres,
	*               et lie ce voisin aux deux voisins passés en paramètres
	*/
static TypVoisins* creerTypVoisins(int voisin, int poids, TypVoisins** vS, TypVoisins** vP) {
	TypVoisins *res;	/* Pointeur sur le voisin nouvellement créé */
	
	res = malloc(sizeof(TypVoisins));
	res->voisin = voisin;
	res->poidsVoisin = poids;
	res->voisinSuivant = *vS;
	res->voisinPrecedent = *vP;
	
	return res;
}


	/*
	* Fonction : creerListe
	*
	* Retour : TypVoisins*, pointeur sur le début de la liste nouvellement créée
	*
	* Description : Crée une nouvelle liste et renvoie un pointeur pointant au 
	*               début de celle-ci.
	*               La sentinelle y est insérée et a pour numéro et poids -1.
	*/
TypVoisins* creerListe() {
	TypVoisins *res;	/* Pointeur sur la liste créée */
	
	/* Création de la sentinelle*/
	res = creerTypVoisins(-1,-1,&res,&res);
	res->voisinSuivant = res;
	res->voisinPrecedent = res;
	
	return res;
}


	/*
	* Fonction : supprimerListe
	*
	* Paramètre : TypVoisins** liste, le début de la liste à supprimer
	*
	* Description : Supprime tous les voisins de la liste passée en paramètre
	*				et désalloue la mémoire de celle-ci.
	*/
void supprimerListe(TypVoisins** liste) {
	TypVoisins *vC;		/* Le voisin courant lors du parcours de la liste */
	
	vC = voisinSuivant(liste);
	while (vC != *liste) {
		supprimerVoisin(liste,numeroVoisin(&vC));
		vC = voisinSuivant(liste);
	}
	free(*liste);
	*liste = NULL;
}


	/*
	* Fonction : ajouterVoisin
	*
	* Paramètres : TypVoisins** liste, le début de la liste à laquelle on veut 
	*					ajouter un nouveau voisin
	*              int voisin, le numéro du nouveau voisin à insérer
	*              int poids, le poids du nouveau voisin à insérer
	*
	* Description : Ajoute un nouveau voisin à la fin de la liste passée 
	*				en paramètre
	*/
void ajouterVoisin(TypVoisins** liste, int voisin, int poids) {
	TypVoisins *vS;		/* Le voisin suivant le voisin nouvellement créé */
	TypVoisins *vP;		/* Le voisin précédent le voisin nouvellement créé */
	TypVoisins *vC;		/* Le nouveau voisin ajouté en fin de liste */
	
	vS = *liste;
	vP = vS->voisinPrecedent;
	vC = creerTypVoisins(voisin,poids,&vS,&vP);
	vP->voisinSuivant = vC;
	vS->voisinPrecedent = vC;
}


	/*
	* Fonction : supprimerVoisin
	*
	* Paramètres : TypVoisins** liste, pointeur sur le début d'une liste
	*              int voisinASupprimer, le numéro du voisin à supprimer
	*
	* Description : Supprime le voisin demandé dans la liste passée 
	*				en paramètre. Ne fait rien s'il ne s'y trouve pas.
	*/
void supprimerVoisin(TypVoisins** liste, int voisinASupprimer) {
	if (voisinASupprimer != -1)
	{
		TypVoisins *vC;    /* Le voisin courant lors du parcours de la liste */
		TypVoisins *vP;    /* Le voisin précédent de l'élément à supprimer */
		TypVoisins *vS;    /* Le voisin suivant de l'élément à supprimer */
		bool 	   trouve; /* Vrai si le voisin à supprimer existe */
		
		/* On cherche le voisin dans la liste*/
		vC = voisinSuivant(liste);
		trouve = false;
		while (!trouve && vC != *liste) {
			if (numeroVoisin(&vC) == voisinASupprimer)
				trouve = true;
			else
				vC = voisinSuivant(&vC);
		}
		
		/* Si le voisin est trouvé, on le supprime de la liste*/
		if (trouve == true) {
			vP = voisinPrecedent(&vC);
			vS = voisinSuivant(&vC);
			vP->voisinSuivant = vS;
			vS->voisinPrecedent = vP;
			free(vC);
		}
	}
}


	/*
	* Fonction : numeroVoisin
	*
	* Paramètres : TypVoisins** voisin, un élément d'une liste
	*
	* Retour : int, le numéro du voisin passé en paramètre
	*
	* Description : Renvoie le numéro du voisin passé en paramètre
	*/
int numeroVoisin(TypVoisins** voisin) {
	return (*voisin)->voisin;
}


	/*
	* Fonction : poidsVoisin
	*
	* Paramètres : TypVoisins** voisin, un élément d'une liste
	*
	* Retour : int, le poids du voisin passé en paramètre
	*
	* Description : Renvoie le poids du voisin passé en paramètre
	*/
int poidsVoisin(TypVoisins** voisin) {
	return (*voisin)->poidsVoisin;
}


	/*
	* Fonction : voisinSuivant
	*
	* Paramètres : TypVoisins** voisin, un élément d'une liste
	*
	* Retour : TypVoisins*, pointeur sur le voisin suivant de l'élément 
	*				passé en paramètre
	*
	* Description : Renvoie un pointeur sur le voisin suivant dans la liste de 
	*				l'élément passé en paramètre
	*/
TypVoisins* voisinSuivant(TypVoisins** voisin) {
	return (*voisin)->voisinSuivant;
}


	/*
	* Fonction : voisinPrecedent
	*
	* Paramètres : TypVoisins** voisin, un élément d'une liste
	*
	* Retour : TypVoisins*, pointeur sur le voisin précédent de l'élément 
	*				passé en paramètre
	*
	* Description : Renvoie un pointeur sur le voisin précédent dans la liste de
	*				l'élément passé en paramètre
	*/
TypVoisins* voisinPrecedent(TypVoisins** voisin) {
	return (*voisin)->voisinPrecedent;
}


	/*
	* Fonction : tailleListe
	*
	* Paramètres : TypVoisins** liste, pointeur sur le début d'une liste
	*
	* Retour : int, la taille de la liste passée en paramètre
	*
	* Description : Renvoie la taille de la liste passée en paramètre.
	*/
int tailleListe(TypVoisins** liste) {
	TypVoisins *vC;		/* Le voisin courant lors du parcours de la liste */
	int        res;		/* La taille de la liste */
	
	/* La sentinelle n'est pas prise en compte dans le calcul*/
	vC = voisinSuivant(liste);
	res = 0;
	while (vC != *liste) {
		res++;
		vC = voisinSuivant(&vC);
	}
	
	return res;
}


	/*
	* Fonction : voisinExiste
	*
	* Paramètres : TypVoisins** liste, le début d'une liste
	*			   int voisin, numéro du voisin dont on veut connaître 
	*					l'existence dans la liste
	*
	* Retour : bool, true si le voisin existe dans la liste, sinon false
	*
	* Description : Vérifie si le voisin demandé existe dans la liste passée en
	*				paramètre.
	*/
bool voisinExiste(TypVoisins** liste, int voisin) {
	TypVoisins *vC;    /* Le voisin courant lors du parcours de la liste */
	bool       trouve; /* true si le voisin existe dans la liste */
	
	vC = voisinSuivant(liste);
	trouve = false;
	while (!trouve && vC != *liste) {
		if (numeroVoisin(&vC) == voisin)
			trouve = true;
		vC = voisinSuivant(&vC);
	}
	
	return trouve;
}


	/*
	* Fonction : toString
	*
	* Paramètres : TypVoisins** liste, le début d'une liste
	*
	* Retour : char*, une chaîne représentant le contenu de la liste
	*					(mémoire allouée dans la fonction)
	*
	* Description : Renvoie la représentation de la liste passée en paramètre
	*				sous la forme "(2,3), (4,6)" pour une liste comportant deux 
	*				voisins numérotés 2 et 4 et ayant pour poids 3 et 6.
	*				Renvoie NULL si la liste est vide.
	*/
char* toString(TypVoisins** liste) {
	char       *res; /* La chaîne représentant la liste */
	char       *tmp; /* Chaîne utilisée pour la construction de res */
	TypVoisins *vC;  /* Le voisin courant lors du parcours de la liste */
	
	vC = voisinSuivant(liste);
	
	if (vC == *liste)
		return NULL;
	
	res = malloc(500);
	tmp = malloc(25);
	
	sprintf(res,"(%d,%d)",numeroVoisin(&vC),poidsVoisin(&vC));
	vC = voisinSuivant(&vC);
	
	while (vC != *liste) {
		sprintf(tmp,", (%d,%d)",numeroVoisin(&vC),poidsVoisin(&vC));
		strcat(res,tmp);
		vC = voisinSuivant(&vC);
	}
	
	free(tmp);
	tmp = realloc(res,strlen(res)+1);
	res = tmp;
	
	return res;
}

/*
 *******************************************************************************
 *	Fonction:	afficherListeFichier
 *
 *	Paramettre :	TypVoisins* listeSommets	:est un pointeur sur TypVoisins
 *			FILE *fichier			: est un pointeur sur FILE
 *			
 *
 *	Retour:		rien 
 *
 *	Description:	affiche la liste d'un fichier
 ********************************************************************************
 */
void afficherListeFichier(TypVoisins* listeSommets, FILE *fichier){
	char *chaine;  /* Représente la liste */
	
	chaine = toString(&listeSommets);
	if (chaine != NULL) {
		fprintf(fichier,"%s",chaine);
		free(chaine);
	}
}
