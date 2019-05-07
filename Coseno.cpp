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

double lenX;

bool myComp1(const pdi &p1,const pdi &p2){
	return p1.first > p2.first;
}

bool myComp2(const pdi &p1,const pdi &p2){
	return p1.second < p2.second;
}

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

void find_user_data_file(int idQuery){
	
	float rat;
	int idU,idP,t;
	char firstline[SIZE_FIRST_LINE];

	bool flag;

	if (FILE *fp = fopen(FILE_NAME_READ, "r")) {
		
		fscanf(fp, "%s",firstline);
		
		flag=0;
		
		lenX = 0.0;

		while (fscanf(fp, "%d,%d,%f,%d", &idU, &idP, &rat,&t) == 4) {
			
			if(idU == idQuery){
				flag = 1;
				userData[idP]=rat;
				lenX+=(rat*rat);
			}
			else{
				if(flag) break;
			}
		}

		fclose(fp);
	}
}


void get_kn(int idQuery,int kk){

	char firstline[SIZE_FIRST_LINE];
	int idU,idP,t;
	float rat;

	int idC=-1;
	double lenY,Sxy,cosD;

	if (FILE *fp = fopen(FILE_NAME_READ, "r")) {
		
		fscanf(fp, "%s",firstline);
		
		Sxy=lenY=0.0;

		while (fscanf(fp, "%d,%d,%f,%d", &idU, &idP, &rat,&t) == 4) {
        		if(idU == idQuery || visited[idU])continue;
			if(idC==-1){
				idC=idU;
			}
			if(idU==idC){
				lenY+=(rat*rat);
				Sxy+=(userData[idP]*rat);
			}
			else{
	
				cosD = Sxy/(sqrt(lenX)*sqrt(lenY));

				if(k_near[kk-1].first < cosD){
					k_near[kk-1]={cosD,idC};
					sort(k_near.begin(),k_near.end(),myComp1);
				}

				Sxy=lenY=0.0;

				visited[idC]=1;

				idC=idU;

				lenY+=rat;
				Sxy+=(userData[idP]*rat);
			}
		}

		cosD = Sxy/(sqrt(lenX)*sqrt(lenY));
		
		if(k_near[kk-1].first < cosD){
			k_near[kk-1]={cosD,idC};
			sort(k_near.begin(),k_near.end(),myComp1);
		}

		
	}
}

void recomender(int kk,double umb){
	
	char firstline[SIZE_FIRST_LINE];
	int idU,idP,t;
	float rat;
	
	visited.assign(k_near.size(),0);

	if (FILE *fp = fopen(FILE_NAME_READ, "r")) {
		
		fscanf(fp, "%s",firstline);
		
		while (fscanf(fp, "%d,%d,%f,%d", &idU, &idP, &rat,&t) == 4) {
			if(bs(idU,k_near)!=-1 && userData[idP]==0){
				recom[idP].first+=rat;
				recom[idP].second++;
			}
		}

		fclose(fp);
	}

	double aux;

	for(auto it:recom){
		aux=(it.second.first)/double(it.second.second);
		
		if(aux >= umb && it.second.second > 3)
			recomP.push_back({aux,it.first});	
	}

}

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
	visited.assign(NUM_USER+1,0);
	int K=10;
	double umbral=3;
	k_near.assign(K,{-1e9,1});
	//int idQ = 138493;
	int idQ = 30503;
	
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
