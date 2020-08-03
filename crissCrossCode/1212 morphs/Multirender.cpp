/*
 *   This code is intended to rerender a file of best automata in a
 *   different shape.
 */

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cmath>

using namespace std;

#include "realstring.h"
#include "stat.h"
#include "pbm.h"

#define RNS 91207819

//evolution control variables; Z is automata size;U is number of updates
#define genelength 32
#define matsize 16
#define mdim 4

//Number of fitness lines to ignore
#define NLI 5

//PICTURE DIMENSIONS
#define Zx 900
#define Zy 300

#define U 20
#define QL 100000

//color values
int cl[mdim][3]={{0,204,102},
                 {204,102,0},
                 {0,255,0},
                 // {0,0,0},
                 // {255,0,0},
                 // {128,255,0},
                 // {255,127,0},
                 // {255,192,203},
                 // {255,255,0},
                 // {245,245,220},
                 // {51,255,255},
                 {0,0,255}};
void initcond();                 //initialize conditions
void render(rylstr &R,int run);  //render a gene

double CMAT[mdim][mdim];      //competition matrix
double CMATleft[mdim][mdim];  //competition matrix left
double CMATright[mdim][mdim]; //competition matrix right
int M[Zx][Zy],N[Zx][Zy];      //automata growth fields
double sc[Zx][Zy];            //score matrix
int IS[Zx][Zy];               //initial state of the cellular automata

int main(){

rylstr R;      //scratch automata rule
fstream inp;   //input buffer
int pn;        //picture number
char buf[200]; //file input buffer
int i;         //loop index
fstream cv;    //create the shell converter
 
  srand48(RNS);    //Seed the random number generator
  cv.open("convert.sh",ios::out);  //open the shell script
  inp.open("best.autr",ios::in); //open the file of rules
  initcond();      //set the initial conditions
  inp.getline(buf,199);  //get the first line of the file
  pn=-1;  //set problem number to pre-zero
  while(strstr(buf,"fitness")!=0){//while we have a rule record
    for(i=0;i<NLI-1;i++)inp.getline(buf,199);  //read in the fitness lines
    pn++;         //increment problem number
    cout << "Converting picture " << pn << "." << endl; //eye candy
    R.read(inp);  //read the automata rule
    render(R,pn); //render the automata
    sprintf(buf,"convert pic%02d.ppm pic%02d.png",pn,pn);
    cv << buf << endl;
    inp.getline(buf,199); //check for next buffer
  }
  inp.close();
  cv << "rm pic*.ppm" << endl;
  cv.close();
  return(0);  //keep the system happy

}


void initcond(){//initialize the algorithm

int i,j;  //loop index variables

  for(i=0;i<Zx;i++)for(j=0;j<Zy;j++){//for initial state block
    IS[i][j]=lrand48()%mdim; //fill with random values
  }
}

void majority(){//run a majority filter

int cnt[mdim];
int i,j,k;

  for(i=1;i<Zx-1;i++)for(j=1;j<Zy-1;j++){
    N[i][j]=M[i][j];
    for(k=0;k<mdim;k++)cnt[k]=0;
    cnt[M[i][j]]++;
    cnt[M[i+1][j+1]]++;
    cnt[M[i][j+1]]++;
    cnt[M[i-1][j+1]]++;
    cnt[M[i-1][j]]++;
    cnt[M[i-1][j-1]]++;
    cnt[M[i][j-1]]++;
    cnt[M[i+1][j-1]]++;
    cnt[M[i+1][j]]++;
    for(k=0;k<mdim;k++)if(cnt[k]>4)N[i][j]=k;
  }
  for(i=0;i<Zx;i++)for(j=0;j<Zy;j++)M[i][j]=N[i][j];

}

int valis(int x,int y){//find the value of the board

   //correct to torroidal boundaries
   while(x<0)x+=Zx;
   while(y<0)y+=Zy;
   while(x>=Zx)x-=Zx;
   while(y>=Zy)y-=Zy;
   return(M[x][y]);
  
}

double valsc(int x,int y){//find the value of the board

   //correct to torroidal boundaries
   while(x<0)x+=Zx;
   while(y<0)y+=Zy;
   while(x>=Zx)x-=Zx;
   while(y>=Zy)y-=Zy;
   return(sc[x][y]);
  
}

int winner(int x,int y){//who gets the updated square

int tw;
double scv,cp; 
 
  tw=M[x][y];
  scv=sc[x][y];
  cp=valsc(x+1,y);
  if(cp>scv){
    tw=valis(x+1,y);
    scv=cp;
  }
  cp=valsc(x-1,y);
  if(cp>scv){
    tw=valis(x-1,y);
    scv=cp;
  }
  cp=valsc(x,y+1);
  if(cp>scv){
    tw=valis(x,y+1);
    scv=cp;
  }
  cp=valsc(x,y-1);
  if(cp>scv){
    tw=valis(x,y-1);
    scv=cp;
  }

  return(tw);

}

void update(){//run the fashion based CA rule

int i,j;        //Loop index varialbes
double wgt;     //weight
int a,b;        //matrix index variables

  for(i=0;i<Zx;i++){
    wgt=((double)i)/(Zx-1);  //create averaging wieght
    for(a=0;a<mdim;a++)for(b=0;b<mdim;b++){//loop over matrix loci
      CMAT[a][b]=wgt*CMATright[a][b]+(1-wgt)*CMATleft[a][b];  //Morph
    } //done creating local payoff matrix

    for(j=0;j<Zy;j++){//score the loci
      sc[i][j]=CMAT[M[i][j]][valis(i+1,j)]+
               CMAT[M[i][j]][valis(i-1,j)]+
               CMAT[M[i][j]][valis(i,j+1)]+
               CMAT[M[i][j]][valis(i,j-1)];
    }
  }

  for(i=0;i<Zx;i++)for(j=0;j<Zy;j++)N[i][j]=winner(i,j);
  for(i=0;i<Zx;i++)for(j=0;j<Zy;j++)M[i][j]=N[i][j];

}

void render(rylstr &R,int run){//render a gene

int i,j,k,u,v;           //loop index variables
double tr[genelength];   //transfer variables
pbm pic(2*Zx,2*Zy);        //picture buffer
char fn[60];             //file name buffer

  sprintf(fn,"pic%02d.ppm",run);

  R.report(tr);   //get the gene for the competition matrix
  k=0;for(i=0;i<mdim;i++)for(j=0;j<mdim;j++){//loop over matrix
    CMATleft[i][j]=tr[k];  //load the left CM
    CMATright[i][j]=tr[k+matsize];  //load the right CM
    k++;  //increment pointer
  }
  for(i=0;i<Zx;i++)for(j=0;j<Zy;j++)M[i][j]=IS[i][j]; //load initial state
  for(i=0;i<U;i++)update();  //run the CA updates
  majority();

  for(i=0;i<Zx;i++)for(j=0;j<Zy;j++){//render the picture
    k=M[i][j];
    for(u=0;u<2;u++)for(v=0;v<2;v++){
      pic.putpixel(2*i+u,2*j+v,cl[k][0],cl[k][1],cl[k][2]);
      }
  }

  //pic.con3();
  pic.writeXVC(fn);

}
