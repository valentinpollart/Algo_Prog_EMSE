#include "Projet_C.h"


int main(void)
{
	Graphe G;
	Graphe Gi;
	int Choix,n;
	char Nom_Fichier[100];
	srand(time(NULL));
	printf("Veuillez sélectionner votre option :\n1 : Générer un graphe grille de taille n\n2 : Utiliser un fichier de graphe quelconque\n");
	scanf("%d", &Choix);
	switch(Choix)
	{
		case 1 :
		 printf("Veuillez saisir la taille du graphe grille (dimension d'un côté)\nn = ");
		 scanf("%d", &n);
		 Creer_Graphe_Grille(n);
		 creation_graphe(&G,"graph_grille.txt");
		 creation_graphe(&Gi,"graph_grille.txt");
		 Condition_Initiale(&G,&Gi);
		 Simulation(&G,1);
		break;
		case 2 :
		 printf("Veuillez saisir le nom du fichier à exploiter\nNom du fichier :");
		 scanf("%s", Nom_Fichier);
		 creation_graphe(&G,Nom_Fichier);
		 creation_graphe(&Gi,Nom_Fichier);
		 Condition_Initiale(&G,&Gi);
		 Simulation(&G,2);
		break;
	}
	MetricsCalc(&Gi,&G);

	return 0;
}



void Condition_Initiale(Graphe* G,Graphe* Gi)
{
	int Choix_Noeud,Choix=1;
	while(Choix==1)
	{	
		printf("Qui va mourir ce soir ?\n");
		scanf("%d", &Choix_Noeud);
		G->population[Choix_Noeud-1].etat=infecte;
		G->metrics->healthyCount--;
		G->metrics->infectedCount++;
		Gi->population[Choix_Noeud-1].etat=infecte;
		Gi->metrics->healthyCount--;
		Gi->metrics->infectedCount++;
		G->population[Choix_Noeud-1].infection_date=0;
		printf("Quelqu'un d'autre ?\nOui : 1\nNon : 2\n");
		scanf("%d", &Choix);
		printf("\n");
	}
}

void Test_Sain(Graphe* G,int source)
{
	float r=((float)rand())/RAND_MAX;
	int sickCounter = 0;
	Arc* Curseur=G->voisins[source];

	while(Curseur!=NULL)
	{
		if(G->population[Curseur->num].etat==malade)
		{
			sickCounter++;
		}
		Curseur=Curseur->Suivant;
	}
	if(r/sickCounter<=LAMBDA)
	{
		G->population[source].etat=infecte;
		G->metrics->healthyCount--;
		G->metrics->infectedCount++;
		G->population[source].infection_date=G->metrics->simulationDuration;
	}
}


void Journee(Graphe* G)
{
	float r;
	for(int i=0;i<G->nb_sommets;i++)
	{
		switch(G->population[i].etat)
		{
			case sain :
			 	Test_Sain(G,i);
			break;
			case infecte :
			 	G->population[i].temps_incubation++;
				r=((float)rand())/RAND_MAX;
			 	if(G->population[i].temps_incubation>=DUREE_INCUBATION)
			 	{
			 		G->population[i].etat=malade;
					G->metrics->infectedCount--;
					G->metrics->sickCount++;
					break;
			 	}
				if(r<=GAMMA)
				{
					G->population[i].etat=immunise;
					G->metrics->infectedCount--;
					G->metrics->immuneCount++;
				}
			break;

			case malade :
			 	r=((float)rand())/RAND_MAX;
			 	if(r<=BETA)
			 	{
			 		G->population[i].etat=mort;
					G->metrics->sickCount--;
					G->metrics->deadCount++;
			 	}
			break;
		}
	}
}

void Simulation(Graphe* G,int Type_Graphe)
{
	int choix=1,nb_jours;
	while(choix!=4)
	{
		switch(Type_Graphe)
		{
			case 1 :
			 Afficher_Graphe_Grille(G);
			break;
			case 2 :
			 Afficher_Graphe_Quelconque(G);
			break;
		}
		printf("Voulez-vous :\n1 : Passer un jour\n2 : Passer dix jours\n3 : Selectionner le nombre de jours à passer\n4 : quitter\n\nChoix : ");
		scanf("%d", &choix);
		switch(choix)
		{
			case 1 :
			 Journee(G);
			 G->metrics->simulationDuration++;
			break;
			case 2 :
			 for(int i=0;i<10;i++)
			 {
				 Journee(G);
				 G->metrics->simulationDuration++;
			 }
			break;
			case 3 :
			 printf("Combien de jours voulez vous passer ?\n");
			 scanf("%d", &nb_jours);
			 for(int i=0;i<nb_jours;i++)
			 {
			 	Journee(G);
			 	G->metrics->simulationDuration++;
			 }
			break;
		}
	}
}

void MetricsCalc(Graphe* Gi, Graphe* Gf){
	FILE *file;
	Gf->metrics->avgInfectionDate=0;
	int unhealthyCount = 0, pathSum = 0, pathLength;

	file = fopen("Métriques.txt","w");
	fprintf(file,"Temps de simulation total : %d jours\n",Gf->metrics->simulationDuration);
	fprintf(file,"Probabilité d'être infecté (lambda) : %f \n",LAMBDA);
	fprintf(file,"Probabilité d'être immunisé (gamma) : %f \n",GAMMA);
	fprintf(file,"Probabilité de mourir (beta) : %f \n",BETA);
	fprintf(file,"Proportion finale d'individus sains  : %f%% \n",(float)Gf->metrics->healthyCount*100/Gf->nb_sommets);
	fprintf(file,"Proportion finale d'individus immunisés  : %f%% \n",(float)Gf->metrics->immuneCount*100/Gf->nb_sommets);
	fprintf(file,"Proportion finale d'individus infectés  : %f%%\n",(float)Gf->metrics->infectedCount*100/Gf->nb_sommets);
	fprintf(file,"Proportion finale d'individus malades  : %f%%\n",(float)Gf->metrics->sickCount*100/Gf->nb_sommets);
	fprintf(file,"Proportion finale d'individus morts  : %f%%\n",(float)Gf->metrics->deadCount*100/Gf->nb_sommets);
	for(int i=0;i<Gi->nb_sommets;i++){
		fprintf(file,"Individu n°%d initialement %s finalement %s\n",i+1,statusToStr(Gi->population[i].etat),statusToStr(Gf->population[i].etat));
		if(Gf->population[i].etat != sain && Gf->population[i].etat !=immunise)
		{
			pathLength = parcours_largeur(Gi,i);
			pathSum+=pathLength;
			Gf->metrics->avgInfectionDate+=Gf->population[i].infection_date*pathLength;
			unhealthyCount++;
		}
	}
	fprintf(file,"Temps moyen de contamination, pondéré par la distance initiale au plus proche malade : %f jours\n",Gf->metrics->avgInfectionDate/pathSum);
	fclose(file);
	printf("Les métriques ont été générées dans le fichier Métriques.txt\n");

}

































