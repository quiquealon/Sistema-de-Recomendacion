#include <bits/stdc++.h>

using namespace std;

#define FILE_PRODUCT "movies.csv"
#define FILE_NAME_READ "ratings.csv"
#define NUM_USER 283228
#define NUM_PROD 130878
#define SIZE_FIRST_LINE 32

typedef unordered_map<int,pair<double,int>> mipdi;
typedef unordered_map<int,double> mif;
typedef pair<double,int> pdi;
typedef vector<pdi> vpdi;
typedef vector<bool> vb;

vb visited;
mif userData;
vpdi k_near;
mipdi recom;
vpdi recomP;


/*  Funcion usada para ordenar los k vecinos de manera descendente segun su rating */
bool myComp1(const pdi &p1,const pdi &p2){
	return p1.first > p2.first;
}

/*  Funcion usada para ordenar los k vecinos de manera ascendente segun su ID */
bool myComp2(const pdi &p1,const pdi &p2){
	return p1.second < p2.second;
}

/* Busqueda Binaria para buscar un elemento en un vector        */
/* Usada para buscar el idUser dentro de los k mas cercanos     */
/* Usada para buscar el idProduct dentro de las recomendaciones */
/* Parametros:                                                  */
/*             int x   -> elemento a buscar                     */
/*             vpdi &V -> vector de elementos                   */
/* Return:  La posicion del elemento si lo encuentra y -1 si no */

int bs(int x,vpdi &V){
	int a=0,b=V.size()-1,k;
	while(a<=b){
		k=(a+b)/2;
		if(V[k].second == x)return k;
		if(V[k].second < x) a=k+1;
		else b=k-1;
	}
	return -1;
}

/* Funcion encargada de sacar la informacion del idQuery del archivo CSV */
/* Parametros:                                                           */
/*              int idQuery -> id del usuario que se desea recomendar    */
/*                                                                       */
/* Al final de la funcion la variable general "userData" estara con      */
/* todos los rating que el usuario dio a las peliculas que miro          */
void find_user_data_file(int idQuery){
	
	float rat;
	int idU,idP,t;
	char firstline[SIZE_FIRST_LINE];

	bool flag;

	if (FILE *fp = fopen(FILE_NAME_READ, "r")) {
		
		fscanf(fp, "%s",firstline);
		
		flag=0;

		while (fscanf(fp, "%d,%d,%f,%d", &idU, &idP, &rat,&t) == 4) {
		
			/* si el idU actual es igual al idQuery que deseamos almacenamos 
			 * informacion */	
			if(idU == idQuery){
				flag = 1;
				userData[idP]=rat;
			}
			/* si es diferente pero ya encontramos previamente el id entonces ya no
		 	* leemos mas datos*/
			else{
				if(flag) break;
			}
		}

		fclose(fp);
	}
}

/* Funcion encargada de obtener los datos de los demas usuarios y compararlos con la informacion
 * del usuario recorrido
 * se encarga de calcular la distancia cada vez que obtiene la informacion de un nuevo usuario */
/* Parametros:
 *            int idQuery -> id del usuario(para no volver a leer su informacion)
 *            int kk -> numero de los usuarios mas cercanos que queremos calcular
 *
 * Al terminar la funcion la variable "k_near" tendra los ids y distancias de los 
 * k usuarios mas cercanos                                                                     */

void get_kn(int idQuery,int kk){

	char firstline[SIZE_FIRST_LINE];
	int idU,idP,t;
	float rat;

	int idC=-1;
	double n;
	double sx,sy,sxy,sx2,sy2;
	double corr,num,den;

	if (FILE *fp = fopen(FILE_NAME_READ, "r")) {
		
		fscanf(fp, "%s",firstline);
		
		sx=sy=sxy=sx2=sy2=0.0;
		n=0.0;

		while (fscanf(fp, "%d,%d,%f,%d", &idU, &idP, &rat,&t) == 4) {
        		if(idU == idQuery || visited[idU])continue;
			if(idC==-1){
				idC=idU;
			}
			/* si el idU es igual al que estamos leyendo significa que aun es el mismo
			 * usuario por lo tanto seguimos calculando los valores para hallar la
			 * correlacion                                                       */
			if(idU==idC){
				/* si el usuario tmb puntuo la pelicula */
				if(userData[idP]>0.0){
					sx+=userData[idP];
					sy+=rat;
					sxy+=(userData[idP]*rat);
					sx2+=(userData[idP]*userData[idP]);
					sy2+=(rat*rat);
					n++;
				}
			}
			/* En caso que sea diferente se calcula la correlacion con las variables
			 * previamente calculadas se ve si influye en los k mas cercanos y se
			 * reinician las variables para el siguiente usuario                 */
			else{
				num = (sxy-((sx*sy)/n));
				den = sqrt(sx2-((sx*sx)/n))*(sqrt(sy2-((sy*sy)/n)));
				
				if(den == 0){corr=-1e9;}
				else{corr = double(num/den);}
				
				/* si es mayor al mas lejano de los k usuarios entonces se
				 * reemplaza y se ordena nuevamente el vector "k_near"  */
				if(k_near[kk-1].first < corr)
					k_near[kk-1]={corr,idC};
				sort(k_near.begin(),k_near.end(),myComp1);
				
				sx=sy=sxy=sx2=sy2=0.0;
				n=0.0;
				visited[idC]=1;

				idC=idU;

				if(userData[idP]>0.0){
					sx+=userData[idP];
					sy+=rat;
					sxy+=(userData[idP]*rat);
					sx2+=(userData[idP]*userData[idP]);
					sy2+=(rat*rat);
					n++;
				}
			}	
		}

		num = (sxy-((sx*sy)/n));
		den = sqrt(sx2-((sx*sx)/n))*(sqrt(sy2-((sy*sy)/n)));
		
		if(den == 0){corr=-1e9;}
		else{corr = double(num/den);}

		if(k_near[kk-1].first < corr)
			k_near[kk-1]={corr,idC};
		sort(k_near.begin(),k_near.end(),myComp1);	
		fclose(fp);
	}
}

/* Funcion encargada de sacar el rating de las peliculas     */
/* Parametros:
 * 	int kk     -> k usuarios mas cercanos
 * 	double umb -> umbral para el rating de las peliculas */
/* Al finalizar la variable "recomP" tendra los IDs y
 * puntajes de las peliculas a recomendar                    */
void recomender(int kk,double umb){
	
	char firstline[SIZE_FIRST_LINE];
	int idU,idP,t;
	float rat;
	
	visited.assign(k_near.size(),0);

	if (FILE *fp = fopen(FILE_NAME_READ, "r")) {
		
		fscanf(fp, "%s",firstline);
		
		while (fscanf(fp, "%d,%d,%f,%d", &idU, &idP, &rat,&t) == 4) {
			/* if encuentro el idUser en los k mas cercanos y el usuario no la 
			 * puntuo entonces influye su rating */
			if(bs(idU,k_near)!=-1 && userData[idP]==0){
				recom[idP].first+=rat;
				recom[idP].second++;
			}
		}

		fclose(fp);
	}

	double aux;

	/* vemos que peliculas pasan el umbral para considerarlas en la recomendacion */
	for(auto it:recom){
		aux=(it.second.first)/double(it.second.second);
		
		if(aux >= umb && it.second.second > 3)
			recomP.push_back({aux,it.first});	
	}

}

/* Funcion encargada de sacar los nombres de las peliculas */
/* Imprime en cuanto vaya encontrando la pelicula          */
void print_product_name(){  
	
	char firstline[20];

	int idP,pos;
	string title;
	char c;

	if (FILE *fp = fopen(FILE_PRODUCT, "r")) {
		
		fscanf(fp, "%s",firstline);

		while (fscanf(fp, "%d",&idP) == 1) {
			title="";
			while(fscanf(fp,"%c",&c)){
				if(c=='\n')break;
				title+=c;
			}
			pos = bs(idP,recomP);
			if(pos!=-1){
				cout << recomP[pos].first << "\t";
				cout << title.substr(1)  << endl;
			}
		}

		fclose(fp);
	}
}

int main(){

	/* variables iniciales para el calculo de recomendacion */
	visited.assign(NUM_USER+1,0);
	int K=5;
	double umbral=4;
	k_near.assign(K,{-1e9,1});
	//int idQ = 138493;
	int idQ = 16006;
	//int idQ = 16006;
	//int idQ = 283228;

	find_user_data_file(idQ);
	get_kn(idQ,K);
	
	sort(k_near.begin(),k_near.end(),myComp2);

	for(int i=0;i<k_near.size();i++){
		printf("%d %f\n",k_near[i].second,k_near[i].first);
	}

	recomender(K,umbral);

	sort(recomP.begin(),recomP.end(),myComp2);
	
	print_product_name();	
	

	return 0;
}
