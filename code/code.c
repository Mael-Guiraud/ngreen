#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

#include <limits.h>
#include <math.h>
#include "struct.h"
#include "bruteforce.h"
#include "simons.h"


void affiche_matrice(Graphe g)
{
	for(int i=0;i<g.N;i++)
	{
		for(int j=0;j<g.N;j++)
		{
			printf("%3d ",g.matrice[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}

void affiche_rc(Routage_Graph rc)
{

	affiche_matrice(rc.g);
	for(int i=0;i<rc.nombre_routes;i++)
	{
		printf("Couple %d : (%d,%d)\n",i,rc.couples[i].source,rc.couples[i].target);
	}
}


//Déclare une matrice remplie de 0 de taille N*N
int ** declare_matrice(int N)
{
	int ** matrice;
	assert(matrice = malloc(sizeof(int*)*N));
	for(int i = 0;i<N;i++)
	{
		assert(matrice[i]= calloc(N,sizeof(int)));
	}
	return matrice;
}


Graphe init_graphe(int N)
{
	Graphe g;
	g.N = N;
	g.matrice = declare_matrice(N);
	return g;
}
void libere_matrice(Graphe g)
{
	for(int i=0;i<g.N;i++)
	{
		free(g.matrice[i]);
	}
	free(g.matrice);
}
void libere_routage_graph(Routage_Graph rc)
{
	libere_matrice(rc.g);
	
	free(rc.couples);
}


//prends un graphe vide en argument et le transforme en etoile
void graphe_etoile(Graphe g,int taille_liens)
{
	if(!(g.N%2)){printf("Impossible de générer une étoile avec un nombre pair de sommets\n");exit(5);}
	int pivot = g.N/2;
	int alea;
	for(int i=0;i<g.N;i++)
	{
		alea = rand()%taille_liens;
		g.matrice[pivot][i] = alea;
		g.matrice[i][pivot] = alea;
	}
	g.matrice[pivot][pivot] = 0;
}

//renvoie un anneau avec les routes
Routage_Graph graphe_anneau(int nb_sommets,int nb_routes,int taille_liens)
{

	if(nb_routes>nb_sommets/2){printf("Trop de routes/Pas assez de sommets\n");exit(3);}
	Graphe g;
	g = init_graphe(nb_sommets);
	for(int i=0;i<g.N;i++)
	{
		g.matrice[i][(i+1)%g.N] = taille_liens;
		//g.matrice[i][(i-1+g.N)%g.N] = taille_liens;
	}
	Routage_Graph rc;
	rc.g=g;
	rc.nombre_routes = nb_routes;

	Couples * couples;
	assert(couples = malloc(nb_routes*sizeof(Couples)));
	int alea,source,target;
	int liste_restant[nb_sommets];
	for(int i=0;i<nb_sommets;i++)liste_restant[i]=1;
	for(int i=0;i<nb_routes;i++)
	{
		alea = rand()%nb_sommets;
		for(source =alea;liste_restant[source]==-1;source=(source+1)%nb_sommets);
			couples[i].source = source;
			liste_restant[source]=-1;
		alea = rand()%nb_sommets;
		for(target =alea;liste_restant[target]==-1;target=(target+1)%nb_sommets);
			couples[i].target = target;
			liste_restant[target]=-1;

	}
	rc.couples = couples;
	return rc;


}


//met toutes les cases du message a 1 dans la periode
void ecrire_message_periode(int * p,int periode, int debut, int fin)
{
	if(debut > fin)
	{
		for(int i=0;i<=fin;i++)p[i]=1;
		for(int i=debut;i<periode;i++)p[i]=1;
	}
	else
	{
		for(int i=debut;i<=fin;i++)p[i]=1;
	}
}
void affiche_periode(int *p, int periode)
{
	int actuel = 0;
	for(int i=0;i<periode;i++)
	{
		if(p[i]!=actuel)
		{
			if(actuel)printf("-%d] ",i-1);
			else	  printf("[%d",i);
			actuel = 1-actuel;
		}
	}
	printf("\n");
}
void affiche_tab(int * tab, int taille)
{
	for(int i=0;i<taille;i++)
	{
		printf("%d ",tab[i]);
	}
	printf("\n");
}


//retourne -1 si la periode donnée ne donne pas de solutions, la taille de la periode sinon
int algo_shortest_longest(Graphe g,int periode,int taille_message)
{
	if(!(g.N%2)){printf("G n'est peut être pas une étoile\n");exit(5);}
	int nb_routes = g.N/2;
	int min = g.matrice[nb_routes][nb_routes+1];
	int max = min;
	for(int i=1;i<nb_routes;i++)
	{
		if(g.matrice[nb_routes][i+nb_routes+1] < min)
			min = g.matrice[nb_routes][i+nb_routes+1];
		if(g.matrice[nb_routes][i+nb_routes+1] > max)
			max = g.matrice[nb_routes][i+nb_routes+1];
	}
	if(periode >= (taille_message * nb_routes + 2*(max-min) ) ) 
		return taille_message * nb_routes + 2*(max-min);
	 
	return -1;
}

//retourne -1 si la periode donnée ne donne pas de solutions, la taille de la periode sinon
int algo_3NT(Graphe g,int periode,int taille_message)
{
	if(!(g.N%2)){printf("G n'est peut être pas une étoile\n");exit(5);}
	int nb_routes = g.N/2;
	int routes[nb_routes];
	int nb_slots = periode / taille_message;
	int assigned = 0;
	for(int i=0;i<nb_routes;i++)
	{
		routes[i]=2*g.matrice[nb_routes][i+nb_routes+1];
	}

	int aller[nb_slots], retour[periode];
	for(int i=0;i<nb_slots;i++){aller[i]=0;}
	for(int i=0;i<periode;i++){retour[i]=0;}


	//Pour chaque route
	for(int i=0;i<nb_routes;i++)
	{
		//On recherche le depart le plus petit
		for(int j=0;j<nb_slots;j++)
		{
			if(!aller[j] && !retour[(j*taille_message+routes[i])%periode] && !retour[(j*taille_message+routes[i]+taille_message-1)%periode])
			{

				ecrire_message_periode(aller,nb_routes,j,j);
				ecrire_message_periode(retour,periode,(j*taille_message+routes[i])%periode,(j*taille_message+routes[i]+taille_message-1)%periode);
				assigned = 1;
				break;
			}
		}
		if(!assigned)//Si une route n'a pas trouvé de départ possible, c'est que la fenètre est trop petite.
			return -1;
		assigned = 0;
	}
	return periode;
}


int linear_3NT(Graphe g,int taille_message)
{
	int result;
	int nb_routes = g.N/2;
	for(int i =nb_routes*taille_message;i<taille_message*(nb_routes)*3;i+=2500)
	{
		result = algo_3NT(g,i,taille_message);
		if(result != -1)
		{
			return result;
		}
	}
	return -1;
}


//retourne -1 si la periode donnée ne donne pas de solutions, la taille de la periode sinon
int algo_prime(Graphe g,int periode,int taille_message)
{
	if(!(g.N%2)){printf("G n'est peut être pas une étoile\n");exit(5);}
	int nb_routes = g.N/2;
	int routes[nb_routes];
	int assigned = 0;
	for(int i=0;i<nb_routes;i++)
	{
		routes[i]=2*g.matrice[nb_routes][i+nb_routes+1];
	}

	int aller[periode], retour[periode];
	for(int i=0;i<periode;i++){aller[i]=0;retour[i]=0;}


	//Pour chaque route
	for(int i=0;i<nb_routes;i++)
	{
		//On recherche le depart le plus petit
		for(int j=0;j<periode;j++)
		{
			if(!aller[j] && !aller[(j+taille_message)%periode] && !retour[(j+routes[i])%periode] && !retour[(j+routes[i]+taille_message-1)%periode])
			{

				ecrire_message_periode(aller,periode,j,(j+taille_message-1)%periode);
				ecrire_message_periode(retour,periode,(j+routes[i])%periode,(j+routes[i]+taille_message-1)%periode);
				assigned = 1;
				break;
			}
		}
		if(!assigned)//Si une route n'a pas trouvé de départ possible, c'est que la fenètre est trop petite.
			return -1;
		assigned = 0;
	}
	//affiche_periode(aller,periode);
	//affiche_periode(retour,periode);
//	printf("%d \n",periode);
	return periode;
}

int linear_prime(Graphe g,int taille_message)
{
	int result;
	int nb_routes = g.N/2;
	for(int i =nb_routes*taille_message;i<taille_message*(nb_routes)*3;i++)
	{
		result = algo_prime(g,i,taille_message);
		if(result != -1)
		{
			return result;
		}
	}
	return -1;
}

int linear_brute(Graphe g,int taille_message)
{
	int result;
	int nb_routes = g.N/2;
	for(int i =nb_routes*taille_message;i<taille_message*(nb_routes)*3;i++)
	{
		result = bruteforceiter(g,i,taille_message);
		if((result != -1)&&(result != -2))
		{
			return result;
		}
	}
	return -1;
}


void simuls_periode(int nb_routes, int taille_message, int taille_routes,int nb_simuls)
{
	FILE * F = fopen("results_periode.data","w");
	Graphe g;
	long long int total_3NT, total_brute,total_sl;
	int res_sl,res_brute,res_3NT;

	for(int j = 1 ; j<=nb_routes;j++)
	{
		printf("Calculs pour %d routes: \n",j);
		
		total_3NT = 0;
		total_brute = 0;
		total_sl =0;

		for(int i = 0;i<nb_simuls;i++)
		{
			g = init_graphe(j*2 + 1);
			graphe_etoile(g,taille_routes);
			res_3NT = linear_3NT(g,taille_message);
			if(res_3NT != -1)total_3NT+=res_3NT;
			else printf("error (3nt = -1)\n");

			res_brute = linear_brute(g,taille_message);
			if(res_brute != -1)total_brute+=res_brute;
			else printf("error (brute = -1)\n");

			res_sl = algo_shortest_longest(g,3*taille_message*nb_routes,taille_message);
			if(res_sl != -1)total_sl+=res_sl;
			else printf("error (Sl = -1)\n");

			//if(res_sl < res_brute)affiche_matrice(g);
			fprintf(stdout,"\rStep%5d /%d",i+1,nb_simuls);fflush(stdout);
			libere_matrice(g);
		}
		printf("\n");
		
		fprintf(F, "%d %lld %lld %lld %d %d\n",j,total_3NT/nb_simuls,total_brute/nb_simuls,total_sl/nb_simuls,j*taille_message,2*j*taille_message);
		printf("\n");
	}
	fclose(F);
}

void echec(int nb_routes, int taille_message,int taille_routes, int nb_simuls)
{
	FILE * F = fopen("results_echec_longues.data","w");
	Graphe g;
	long long int total_3NT, total_brute, total_sl,total_theorique;
	int res_brute;

	for(int j = taille_message*nb_routes ; j<=3*taille_message*nb_routes;j+=2500)
	{
		
		total_3NT = 0;
		total_brute = 0;
		total_sl = 0;
		total_theorique = 0;

		for(int i = 0;i<nb_simuls;i++)
		{
			g = init_graphe(nb_routes *2 +1);
			graphe_etoile(g,taille_routes);
			if(algo_3NT(g,j,taille_message) != -1) total_3NT++;
			res_brute = bruteforceiter(g,j,taille_message);
			if(res_brute == -2) total_theorique++;
			else
				if(res_brute != -1 ){ total_brute++;total_theorique++;}
			if(algo_shortest_longest(g,j,taille_message)!= -1) total_sl++;
			libere_matrice(g);
			fprintf(stdout,"\rStep = %5d/%d [Period : %d]",i+1,nb_simuls,j);fflush(stdout);
		}

	
		fprintf(F, "%d %lld %lld %lld %lld\n",j,total_sl,total_3NT,total_brute,total_theorique);

	}
	fclose(F);
	printf("\n");
}

void echec_taille_route(int nb_routes, int taille_message,int taille_routes, int nb_simuls)
{
	FILE * F = fopen("results_taille_route_echec.data","w");
	Graphe g;
	long long int total_sl,total_3nt;

	for(int k = 1000;k<taille_routes;k+=1000)
	{
		for(int j = taille_message*nb_routes ; j<=3*taille_message*nb_routes;j+=1000)
		{
			total_sl = 0;
			total_3nt = 0;

			for(int i = 0;i<nb_simuls;i++)
			{
				g = init_graphe(nb_routes *2 +1);
				graphe_etoile(g,k);
				if(algo_shortest_longest(g,j,taille_message)!= -1) total_sl++;
				if(algo_3NT(g,j,taille_message)!= -1) total_3nt++;
				libere_matrice(g);
				fprintf(stdout,"\rStep = %5d/%d [Period : %d| Routes : %d]",i+1,nb_simuls,j,k);fflush(stdout);
			}

		
			fprintf(F, "%d %d %d \n",j,k,(total_3nt>total_sl));
			//printf("%d %d %d \n",j,k,(total_3nt>total_sl));

		}
		fprintf(F, "\n");
	}
	fclose(F);
	printf("\n");
}

//Appelé par ecrire fichier graphe
void file_dot(Graphe g){

	FILE *f;
	f=fopen("graphe.dot","w+");
	fprintf(f,"graph noeud_%d {\n", g.N);
	fprintf(f,"node [fixedsize=true width=0.4 height=0.25];\n");

	for(int i=0; i <g.N;i++){
		for(int j=0; j < g.N;j++){
			if(g.matrice[i][j] != 0)
				fprintf(f,"%d -- %d [dir=\"forward\",label=\"%d\"]; \n",i,j,g.matrice[i][j]);
		}
	}
	fprintf(f,"}");
	fclose(f);
	
}
void tri_bulles(int* tab,int* ordre,int taille)
{
	int sorted;
	int tmp;
	int tmp_ordre;

	int tabcpy[taille];
	for(int i=0;i<taille;i++)tabcpy[i]=tab[i];

	for(int i=taille-1;i>=1;i--)
	{
		sorted = 1;
		for(int j = 0;j<=i-1;j++)
		{

			if(tabcpy[j+1]>tabcpy[j])
			{
				tmp_ordre = ordre[j+1];
				ordre[j+1]=ordre[j];
				ordre[j]=tmp_ordre;
				tmp = tabcpy[j+1];
				tabcpy[j+1]= tabcpy[j];
				tabcpy[j]= tmp;
				sorted = 0;
			}
		}
		if(sorted){return;}
	}

}

int first_back(int * tab, int taille)
{
	int min = INT_MAX;
	int indice_min = -1;
	for(int i =0;i<taille;i++)
	{
		if((tab[i] != -1)&&(tab[i]<min))
		{
			min = tab[i];
			indice_min = i;
		}
	}
	return indice_min;
}

int longest_etoile(Graphe g,int taille_paquets, int Tmax)
{
	if(!(g.N%2)){printf("Nombre de sommets impair, G n'est peut être pas une étoile\n");exit(5);}
	int nb_routes = g.N/2;
	int offset;
	int nb_assigned;
	int m_i[nb_routes];
	int w_i[nb_routes];

	int ordre[nb_routes];
	int routes[nb_routes];

	int noeud_retour[nb_routes];

	//Initialisation des tableaux
	for(int i=0;i<nb_routes;i++){m_i[i]=0;w_i[i]=0;ordre[i]=i;noeud_retour[i]=0;}

	//Calcul de la taille des routes
	for(int i=0;i<nb_routes;i++)
	{
		routes[i]=g.matrice[nb_routes][i]+g.matrice[nb_routes][i+nb_routes+1];
	}

	//Tri du tableau ordre
	tri_bulles(routes,ordre,nb_routes);

	//On envoie les messages de longest a shortest
	m_i[ordre[0]]=0;
	offset = taille_paquets+g.matrice[nb_routes][ordre[0]];
	for(int i=1;i<nb_routes;i++)
	{
		m_i[ordre[i]]=offset-g.matrice[nb_routes][ordre[i]];
		offset+=taille_paquets;
	}

	//Calcul de la position potentielle dans ct
	for(int i=0;i<nb_routes;i++){
		noeud_retour[i]=m_i[i]+routes[i]+g.matrice[nb_routes][i+nb_routes+1];
		ordre[i]=i;
	}



	//Le premier a arriver passe direct
	int eligible = first_back(noeud_retour,nb_routes);
	w_i[eligible]=0;
	offset = taille_paquets+noeud_retour[eligible];
	noeud_retour[eligible] = -1;
	nb_assigned = 1;
	
	while(nb_assigned<nb_routes)
	{
		eligible = -1;
		for(int i=0;i<nb_routes;i++)
		{
			if((noeud_retour[i]!=-1)&&(noeud_retour[i]<=offset ))
			{
				
				if(eligible == -1)
					eligible = i;
				else
				{
					if(routes[i]>routes[eligible])
						eligible = i;
				}
			}
			
		}

		if(eligible == -1)
		{
			eligible = first_back(noeud_retour,nb_routes);
		
			w_i[eligible]=0;
			offset=taille_paquets+noeud_retour[eligible];
		}
		else
		{
		
			w_i[eligible]=(offset-g.matrice[nb_routes][eligible+nb_routes+1])-(m_i[eligible]+routes[eligible] );			
			
			offset+=taille_paquets;
		}
	
		nb_assigned++;
		noeud_retour[eligible]=-1;

	}


	//affiche_tab(routes,nb_routes);
	//affiche_tab(m_i,nb_routes);
	//affiche_tab(w_i,nb_routes);
	if(!is_ok(g,taille_paquets,m_i,w_i))printf("ERROR !!\n");
		int max = w_i[0]+2*routes[0];
		for(int i=0;i<nb_routes;i++)
		{
			if(w_i[i]+2*routes[i] > Tmax)
				return -1;
			if(w_i[i]+2*routes[i] > max)
				max= w_i[i]+2*routes[i];
		}
	
	return max;
}


int longest_etoile_2(Graphe g,int taille_paquets, int Tmax)
{
	if(!(g.N%2)){printf("Nombre de sommets impair, G n'est peut être pas une étoile\n");exit(5);}
	int nb_routes = g.N/2;
	int offset;
	int nb_assigned;
	int m_i[nb_routes];
	int w_i[nb_routes];

	int ordre[nb_routes];
	int routes[nb_routes];

	int noeud_retour[nb_routes];
	int date_limite[nb_routes];

	//Initialisation des tableaux
	for(int i=0;i<nb_routes;i++){m_i[i]=0;w_i[i]=0;ordre[i]=i;noeud_retour[i]=0;}

	//Calcul de la taille des routes
	for(int i=0;i<nb_routes;i++)
	{
		routes[i]=g.matrice[nb_routes][i]+g.matrice[nb_routes][i+nb_routes+1];
	}
	//Tri du tableau ordre
	tri_bulles(routes,ordre,nb_routes);

	//On envoie les messages de longest a shortest
	m_i[ordre[0]]=0;
	offset = taille_paquets+g.matrice[nb_routes][ordre[0]];
	for(int i=1;i<nb_routes;i++)
	{
		m_i[ordre[i]]=offset-g.matrice[nb_routes][ordre[i]];
		offset+=taille_paquets;
	}

	//Calcul de la position potentielle dans ct
	for(int i=0;i<nb_routes;i++){
		noeud_retour[i]=m_i[i]+routes[i]+g.matrice[nb_routes][i+nb_routes+1];
		ordre[i]=i;
	}

	affiche_tab(noeud_retour,nb_routes);

	//Le premier a arriver passe direct
	int eligible = first_back(noeud_retour,nb_routes);
	w_i[eligible]=0;
	offset = taille_paquets+noeud_retour[eligible];
	noeud_retour[eligible] = -1;
	nb_assigned = 1;
	
	for(int i=0;i<nb_routes;i++){date_limite[i] = m_i[i]+Tmax-g.matrice[nb_routes][i];}

	affiche_tab(date_limite,nb_routes);
	while(nb_assigned<nb_routes)
	{
		
		eligible = -1;
		for(int i=0;i<nb_routes;i++)
		{
			if((noeud_retour[i]!=-1)&&(noeud_retour[i]<=offset ))
			{
				
				if(eligible == -1)
					eligible = i;
				else
				{
					if(date_limite[i]<date_limite[eligible])
						eligible = i;
				}
			}
			
		}

		if(eligible == -1)
		{
			eligible = first_back(noeud_retour,nb_routes);
		
			w_i[eligible]=0;
			offset=taille_paquets+noeud_retour[eligible];
		}
		else
		{
		
			w_i[eligible]=(offset-g.matrice[nb_routes][eligible+nb_routes+1])-(m_i[eligible]+routes[eligible] );			
			
			offset+=taille_paquets;

		}
	
		nb_assigned++;
		noeud_retour[eligible]=-1;

	}

	affiche_tab(m_i,nb_routes);
	affiche_tab(w_i,nb_routes);
	if(!is_ok(g,taille_paquets,m_i,w_i))printf("ERROR !!\n");
	int max = w_i[0]+2*routes[0];
	for(int i=0;i<nb_routes;i++)
	{
		if(w_i[i]+2*routes[i] > Tmax)
			return -1;
		if(w_i[i]+2*routes[i] > max)
			max= w_i[i]+2*routes[i];
	}
	
	return max;
}

void departs(int nb_routes, int taille_paquets,int taille_route,int tmax, int nb_simuls)
{
	FILE * F = fopen("results_departs.data","w");
	Graphe g ;
	long long int a,b,c;
	for(int j=1;j<nb_routes;j++)
	{
		a=0;
		b=0;
		c=0;
		for(int i = 0;i<nb_simuls;i++)
		{
			g = init_graphe(2*j+1);
			graphe_etoile(g,taille_route);
			//affiche_matrice(g);
			
			//b=longest_etoile_2(g,2500,40000);
			a+=simons(g, taille_paquets, tmax,400000,0);
			b+=simons(g, taille_paquets, tmax,400000,1);
			c+=simons(g, taille_paquets, tmax,400000,2);
			libere_matrice(g);
		}
		fprintf(F,"%d %lld %lld %lld\n",j,a/nb_simuls,b/nb_simuls,c/nb_simuls);
	}
	fclose(F);
}
int main()
{
	srand(time(NULL));
	//simuls_periode(8,2500,7000,1000);
	//echec(8,2500,30000,1000);
	//echec_taille_route(8,2500,25000,1000);
	Graphe g;

	int a=0,b=0,c=0;
	for(int i=0;i<10000;i++)
	{

	 	g = init_graphe(15);
		graphe_etoile(g,7000);
		affiche_matrice(g);
		a = simons(g,2500,40000,0,1);
		b= longest_etoile_2(g,2500,40000);
		if(a!= b)
		{
			c++;
			printf("-----------------------------------------------------%d %d \n",a,b);
			break;
		}
		libere_matrice(g);
		

	}
	printf("%d \n",c);
	/*Graphe g = init_graphe(15);
		graphe_etoile(g,7000);
		//affiche_matrice(g);
		 simons(g,6,30000,0,1);
		libere_matrice(g);*/
	
//	departs(8,2500,7000,40000,100000);

	return 0;
}