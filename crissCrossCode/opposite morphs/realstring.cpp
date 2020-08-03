/*
 *   Implementation file for an integer string representation to take care
 *   of variation and maintenence.
 *
 */

#include <iostream>
#include <fstream>
#include <cstdlib>

#include "realstring.h"

//constructors and destructors
rylstr::rylstr(){//default constructor

  len=0;                //zero length is the no initialization clue
  lov=lo_rylstring;     //set low value
  hiv=hi_rylstring;     //set hi value
  val=0;                //nil the pointer

}

rylstr::rylstr(int L,double lo,double hi){//length L, range

  len=0;            //note lack of allocation
  create(L,lo,hi);  //call the creation method

}

rylstr::rylstr(rylstr &other){//copy constructor

   len=0;        //note lack of allocation
   copy(other);  //copy the other

}

rylstr::~rylstr(){//destructor

  destroy();    //call the deallocator

}

//utility routines
void rylstr::create(int L,double lo,double hi){//allocator, legth L modulus M

  if(L!=len)destroy();   //clear if needed
  len=L;lov=lo;hiv=hi;   //assign descriptors
  val=new double[len];   //get storage

} 

void rylstr::destroy(){//clears it out

  if(len>0){//if it was ever allocated
    delete [] val;         //give back storage
    val=0;                 //nil the pointer
    len=0;                 //note lack of allocation
    lov=lo_rylstring;      //set default low value
    hiv=hi_rylstring;      //set default hi value
  }
}

void rylstr::copy(rylstr &other){//make a copy

  if(len!=other.len){//check for compatibility
    destroy();                 //not compatible?
    create(other.len,other.lov,other.hiv);   //reallocate
  } else {
    lov=other.lov;hiv=other.hiv; //transfer range
  }
  for(int i=0;i<len;i++)val[i]=other.val[i];  //copy content

}

void rylstr::randomize(){//fill with random values

  for(int i=0;i<len;i++)val[i]=drand48()*(hiv-lov)+lov; //fill randomly

}

void rylstr::flat(double v){//fill with v

  for(int i=0;i<len;i++)val[i]=v; //fill with specified value

}

//use
double rylstr::loci(int k){//return the value of the kth loci

  //Safety first and then return value
  if((k>0)&&(k<len))return(val[k]); else return(0);

}

void rylstr::report(double *v){//fill v with values

  for(int i=0;i<len;i++)v[i]=val[i];

}

//crossover operators
void rylstr::opc(rylstr &other){//one point crossover

  if(len<2)return; //nothing to do
  if(len!=other.len)return; //no mixed crossover

int i,cp;  //loop variable and crossover point
double sw;    //swap variable

  cp=lrand48()%len;  //choose crossover point
  //perform one point crossover
  for(i=0;i<cp;i++){sw=val[i];val[i]=other.val[i];other.val[i]=sw;}

}

void rylstr::tpc(rylstr &other){//two point crossover

  if(len<2)return; //nothing to do
  if(len!=other.len)return; //no mixed crossover

int i;        //loop index points
int cp1,cp2;  //crossover point
double sw;    //swap variable

  cp1=lrand48()%len;  //choose first crossover point
  cp2=lrand48()%len;  //choose second point
  if(cp1>cp2){sw=cp1;cp1=cp2;cp2=sw;} //put them in order
  //perform two-point crossover
  for(i=cp1;i<cp2;i++){sw=val[i];val[i]=other.val[i];other.val[i]=sw;}

}

void rylstr::unf(rylstr &other){//uniform crossover

  if(len<2)return; //nothing to do
  if(len!=other.len)return; //no mixed crossover

int i;      //loop index
double sw;  //swap variable

  for(i=0;i<len;i++)if(lrand48()%2==1){//if loci swap
    sw=val[i];val[i]=other.val[i];other.val[i]=sw;
  }

}

//mutation
void rylstr::pmt(double msize){//point mutation

int nov,loc;  //new value and location

  if(len>0){//safety first
    loc=lrand48()%len;   //generate location
    val[loc]+=drand48()*2*msize-msize;  //modify the value
    if((val[loc]>hiv)||(val[loc]<lov)){//repair to interior if needed
      val[loc]=drand48()*(hiv-lov)+lov;
    }
  }

}

void rylstr::mut(int k,double msize){//k point mutation

  if((len==0)||(k>=len))return;  //avoid the impossible

int *vis;       //already mutated array
int i;         //loop idex variable
int nov,loc;  //new value and location

  vis=new int[len];  //create the visited array
  for(i=0;i<len;i++)vis[i]=0;  //initialize the visited array
  for(i=0;i<k;i++){//loop over points
    do {loc=lrand48()%len;}while(vis[loc]==1);    //find unvisited location
    val[loc]+=drand48()*2*msize-msize;  //modify the value
    if((val[loc]>hiv)||(val[loc]<lov)){//repair to interior if needed
      val[loc]=drand48()*(hiv-lov)+lov;
    }
    vis[loc]=1;                //mark location as used
  }
  delete [] vis;     //delete te visited array
}

void rylstr::ufm(double alpha,double msize){//probabalistic mutation P=alpha

int i;     //loop index
int nov;   //novel value

  for(i=0;i<len;i++)if(drand48()<alpha){//loop over loci
    val[i]+=drand48()*2*msize-msize;  //modify the value
    if((val[i]>hiv)||(val[i]<lov)){//repair to interior if needed
      val[i]=drand48()*(hiv-lov)+lov;
    }
  }
}

//information
int rylstr::length(){//return the length

  return(len);

}

//input-output
void rylstr::write(ostream &aus){//output to stream

  aus << len << " " << lov << " " << hiv << endl;  //output descriptors
  if(len>0)aus << val[0];            //output first value
  for(int i=1;i<len;i++)aus << " " << val[i]; //output remaining values
  aus << endl;                       //end line
  

}

void rylstr::read(istream &inp){//input from stream

char buf[IBL_rylstring];  //input buffer
int L,M;                  //length and top buffer for input
int i,k;                  //loop variable and buffer pointer

  inp.getline(buf,IBL_rylstring-1); //get descriptors
  L=atoi(buf);                   //translate length
  k=0;while(buf[k]!=' ')k++;while(buf[k]==' ')k++; //find lov
  lov=atof(buf+k);                 //translate lov
  while(buf[k]!=' ')k++;while(buf[k]==' ')k++; //find hiv
  hiv=atof(buf+k);                 //translate hiv
  create(L,lov,hiv);              //make the structure
  inp.getline(buf,IBL_rylstring-1); //get numbers
  val[0]=atof(buf);              //get first number
  k=0; //initialize buffer poiter
  for(i=1;i<len;i++){//loop over remaining numbers
    while(buf[k]!=' ')k++;while(buf[k]==' ')k++; //find next number
    val[i]=atof(buf+k);
  }
}
