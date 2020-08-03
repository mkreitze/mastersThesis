/*
 *   Interface file for an integer string representation to take care
 *   of variation and maintenence.
 *
 */

#ifndef	_REALSTRING_H
#define	_REALSTRING_H

using namespace std;

//default default range
#define lo_rylstring 0.0
#define hi_rylstring 1.0
//input buffer length
#define IBL_rylstring 1000

class rylstr {

 public:

  //constructors and destructors
  rylstr();              //default constructor
  rylstr(int L,double lo,double hi);   //length L, modulus M
  rylstr(rylstr &other); //copy constructor
  ~rylstr();             //destructor

  //utility routines
  void create(int L,double lo,double hi);  //allocator, legth L modulus M
  void destroy();            //clears it out
  void copy(rylstr &other);  //make a copy
  void randomize();          //fill with random values
  void flat(double v);          //fill with v

  //use
  double loci(int k);         //return the value of the kth loci
  void report(double *v);     //report the values

  //crossover operators
  void opc(rylstr &other); //one point crossover
  void tpc(rylstr &other); //two point crossover
  void unf(rylstr &other); //uniform crossover

  //mutation
  void pmt(double msize);               //point mutation
  void mut(int k,double msize);         //k point mutation
  void ufm(double alpha,double msize);  //probabalistic mutation P=alpha
 
  //information
  int length();          //return the length

  //input-output
  void write(ostream &aus);
  void read(istream &inp);

 private:

  int len;         //length of string
  double lov,hiv;  //range controls
  double *val;     //real values

};

#endif /* _REALSTRING_H */

