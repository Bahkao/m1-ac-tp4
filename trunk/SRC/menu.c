/*
******************************************************************************
*
* Programme : menu.c
*
* Auteurs : Christophe SAUVAGE et Mustafa NDIAYE
*
* Résumé : Affichage et gestion du menu du programme
*
* Date : 02/12/2013
*
******************************************************************************
*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "menu.h"
#include "libgraphe.h"
#include "libliste.h"
#include "pert.h"
#include "erreurs.h"


TypGraphePERT* graphePERT;
int nbOuvriers = -1;


static void sous_menu();
static void sousMenuAfficherGraphe();
static void sousMenuSauvegarderGraphe();
static void sousMenuDureeTotale();
static void sousMenuDatesTaches();
static void sousMenuCheminsCritiques();
static void sousMenuSaisieNbOuvriers();
static void sousMenuDureeSelonOuvriers();
static void sousMenuChargerFichier();
static void sousMenuQuitter();
static void cleanBuffer(const char *chaine);
static void pause();


	/*
	* Fonction : menu_principal
	*
	* Description : Affiche le premier menu du programme
	*/
void menu_principal() {
	int  choix;        /* Choix de l'utilisateur dans le menu */
	char ligne[201];   /* Contient l'entrée au clavier de l'utilisateur */
	int  compte;       /* Permet de vérifier le retour de sscanf */
	char chemin[201];  /* Chemin vers un fichier à charger */
	FILE *fichier;     /* Fichier à charger */
	
	printf(  "##########################################\n" );
	printf(  "#             MENU PRINCIPAL             #\n" );
	printf(  "##########################################\n" );
	printf(  "#	1 : Charger un fichier           #\n");
	printf(  "#	2 : Quitter                      #\n");
	printf(  "##########################################\n" );
	
	do {
		printf("Faites un choix: ");
		fgets(ligne,2,stdin);
		cleanBuffer(ligne);
		compte = sscanf(ligne,"%d",&choix);
	} while (compte != 1 || choix < 1 || choix > 2);
	
	switch (choix){
		case 1 : 
			printf("Saisissez le nom du fichier : ");
			fgets(ligne,200,stdin);
			sscanf(ligne,"%s",chemin);
			if ((fichier = fopen(chemin,"r")) != NULL) {
			    graphePERT = lireGraphePERT(fichier);
			    calculDates(graphePERT);
			    fclose(fichier);
			    sous_menu();
			}
			else {
			    printf("Le fichier demandé n'existe pas\n");
			    pause();
			    menu_principal();
			}
			break;
		case 2 :
			exit(0);
			break;
	}
}


/****************************************************
 * Fonction   		:sous_menu
 *
 * Paramettre 		:rien
 *
 * Retour     		:rien
 * 
 * Description		: affichage du sous menu
 ****************************************************
 */
static void sous_menu() {
	int choix;      /* Choix de l'utilisateur dans le menu */
	char ligne[4];  /* Contient l'entrée au clavier de l'utilisateur */
	int compte;     /* Permet de vérifier le retour de sscanf */
	
	printf("\n\n\n");
	printf("##############################################################\n");
    printf("#                 SOUS MENU                                  #\n");
    printf("##############################################################\n");
	printf("#	1 : Afficher le graphe                               #\n");
	printf("#	2 : Sauvegarder le graphe                            #\n");
	printf("#	3 : Durée totale du chantier                         #\n");
	printf("#	4 : Dates au plus tôt et au plus tard des tâches     #\n");
	printf("#	5 : Chemin(s) critique(s)                            #\n");
	printf("#	6 : Saisir le nombre d'ouvriers                      #\n");
	printf("#	7 : Durée du chantier compte tenu du nb d'ouvriers   #\n");
	printf("#	8 : Charger un autre fichier                         #\n");
	printf("#	9 : Quitter                                          #\n");
	printf("##############################################################\n");
	
	do {
		printf("Faites un choix: ");
		fgets(ligne,3,stdin);
		cleanBuffer(ligne);
		compte = sscanf(ligne,"%d",&choix);
	} while (compte != 1 || choix < 1 || choix > 10);
	
	switch (choix){
		case 1 :
			sousMenuAfficherGraphe();
			break;
		case 2 :
			sousMenuSauvegarderGraphe();
			break;
		case 3 :
			sousMenuDureeTotale();
			break;
		case 4 :
			sousMenuDatesTaches();
			break;
		case 5 :
			sousMenuCheminsCritiques();
			break;
		case 6 :
			sousMenuSaisieNbOuvriers();
			break;
		case 7:
			sousMenuDureeSelonOuvriers();
			break;
		case 8:
			sousMenuChargerFichier();
			break;
		case 9 :
			sousMenuQuitter();
			break;
	}
	
	sous_menu();
}


	/*
	* Fonction : sousMenuAfficherGraphe
	*
	* Description : Permet de demander l'affichage du graphe
	*/
static void sousMenuAfficherGraphe() {
	printf("\n===Afficher le graphe ===\n\n");
	affichage(graphePERT->graphe);
	pause();
}


	/*
	* Fonction : sousMenuSauvegarderGraphe
	*
	* Description : Permet de demander la sauvegarde du graphe dans un fichier
	*/
static void sousMenuSauvegarderGraphe() {
	char chemin[201];  /* Nom du fichier à créer */
	char ligne[201];   /* Contient l'entrée au clavier de l'utilisateur */
	char cheminSauvegarde[250] = "lecture/";  /* Où est sauvegardé le graphe */
	FILE* fichier;

	printf("\n=== Sauvegarder le graphe ===\n\n");

	printf("Saisissez le nom du fichier : ");
	fgets(ligne,200,stdin);
	sscanf(ligne,"%s",chemin);
	strcat(cheminSauvegarde,chemin);
	
	fichier = fopen(cheminSauvegarde,"w");
	if (fichier != NULL) {
		sauvegarde(graphePERT->graphe,fichier);
		if (fclose(fichier) != EOF) {
			printf("Le graphe a été sauvegardé avec succès.\n");
			pause();
		}
		else {
			printf("Erreur lors de la fermeture du fichier.\n");
			pause();
		}
	}
	else {
		printf("Erreur lors de l'ouverture du fichier.\n");
		pause();
	}
}


	/*
	* Fonction : sousMenuDureeTotale
	*
	* Description : Permet de demander l'affichage de la durée totale 
	*               du chantier
	*/
static void sousMenuDureeTotale() {
	printf("\n=== Durée Totale du chantier ===\n\n");
	printf("\nDuree totale : %d\n",dureeTotale(graphePERT,graphePERT->graphe->nbrMaxSommets));
	pause();
}


	/*
	* Fonction : sousMenuDatesTaches
	*
	* Description : Permet de demander l'affichage des dates au plus tôt
	*               et au plus tard de toutes les tâches du chantier.
	*/
static void sousMenuDatesTaches() {
	printf("\n=== Dates au plus tôt et au plus tard des tâches ===\n\n");
	afficherDates(graphePERT);
	pause();
}


	/*
	* Fonction : sousMenuCheminsCritiques
	*
	* Description : Permet de demander l'affichage des chemins critiques
	*/
static void sousMenuCheminsCritiques() {
	printf("\n=== Chemin critique ===\n\n");
	afficherCheminCritique(graphePERT);
	pause();
}


	/*
	* Fonction : sousMenuSaisieNbOuvriers
	*
	* Description : Permet de demander la saisie du nombre d'ouvriers
	*/
static void sousMenuSaisieNbOuvriers() {
	int nb;
	char ligne[11];
	int compte;
	
	printf("\n=== Saisir le nombre d'ouvriers ===\n\n");
	do {
		printf("Nombre d'ouvriers (>= 1) : ");
		fgets(ligne,10,stdin);
		cleanBuffer(ligne);
		compte = sscanf(ligne,"%d",&nb);
	} while (compte != 1 || nb < 1);
	
	nbOuvriers = nb;
	printf("%d ouvriers travaillent désormais sur le chantier\n",nbOuvriers);
	pause();
}


	/*
	* Fonction : sousMenuDureeSelonOuvriers
	*
	* Description : Permet de demander l'affichage de la durée du chantier
	*               compte tenu de la stratégie choisie
	*/
static void sousMenuDureeSelonOuvriers() {
	int duree;
	
	if (nbOuvriers != -1) {
	    printf("\n=== Durée du chantier compte tenu du nombre d'ouvriers ===\n\n");
	    duree = dureeTotale(graphePERT,nbOuvriers);
	    printf("Durée avec %d ouvriers : %d\n",nbOuvriers,duree);
	}
	else {
	    printf("Veuillez d'abord renseigner le nombre d'ouvriers\n");
	}
	
	
	pause();
}


	/*
	* Fonction : sousMenuChargerFichier
	*
	* Description : Permet de demander le chargement d'un autre fichier
	*/
static void sousMenuChargerFichier() {
	char chemin[201];  /* Nom du fichier à charger */
	char ligne[201];   /* Contient l'entrée au clavier de l'utilisateur */
	char continuer;    /* 'o' : continuer l'opération en cours, sinon 'n' */
	int compte;        /* Permet de vérifier le retour de sscanf */
	FILE *fichier;     /* Fichier à charger */
	
	printf("\n=== Charger un autre fichier ===\n\n");
	
	do {
		printf("Etes-vous sûr de vouloir charger un autre fichier ? (o/n) ");
		fgets(ligne,2,stdin);
		cleanBuffer(ligne);
		compte = sscanf(ligne,"%c",&continuer);
	} while (compte != 1 || (continuer != 'o' && continuer != 'n'));
	
	if (continuer == 'o') {
		deleteGraphePERT(graphePERT);
		printf("Saisissez le nom du fichier : ");
		fgets(ligne,200,stdin);
		sscanf(ligne,"%s",chemin);
		fichier = fopen(chemin,"r");
		graphePERT = lireGraphePERT(fichier);
		calculDates(graphePERT);
		fclose(fichier);
	}
}


	/*
	* Fonction : sousMenuQuitter
	*
	* Description : Permet de quitter le programme
	*/
static void sousMenuQuitter() {
	char ligne[3];   /* Contient l'entrée au clavier de l'utilisateur */
	char continuer;  /* 'o' : continuer l'opération en cours, sinon 'n' */
	int compte;      /* Permet de vérifier le retour de sscanf */

	do {
		printf("Etes-vous sûr de vouloir quitter ? (o/n) ");
		fgets(ligne,2,stdin);
		cleanBuffer(ligne);
		compte = sscanf(ligne,"%c",&continuer);
	} while (compte != 1 || (continuer != 'o' && continuer != 'n'));
	
	if (continuer == 'o') {
		deleteGraphePERT(graphePERT);
		exit(0);
	}
}


	/*
	* Fonction : cleanBuffer
	*
	* Paramètre : char* chaine, une chaine dans laquelle on vient d'écrire
	*
	* Description : Permet de supprimer le buffer du clavier après une entrée au
	*				clavier de l'utilisateur dans la console.
	*/
static void cleanBuffer (const char *chaine) {
    char *p;  /* Pointeur sur le "\n" de chaine s'il existe */
	int c;

	p = strchr(chaine,'\n');
    if (p) {
        p = NULL;
    }
    else {
        while ((c = getchar()) != '\n' && c != EOF);
    }
}


	/*
	* Fonction : pause
	*
	* Description : Permet de mettre le programme en pause.
	*				Attend que l'utilisateur appuie sur Entrée.
	*/
static void pause() {
	int c;
	
	printf("\nAppuyez sur Entrer pour continuer\n");
	/* pour faire une pause puis vider le buffer */
	while ((c = getchar()) != '\n' && c != EOF);
}
