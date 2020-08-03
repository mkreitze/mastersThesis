/********************************************************
*                                                       *
*         Routines for manipulating pbm Files           *
*                                                       *
********************************************************/

using namespace std;

struct TST {//turtle state

  double svx,svy,svh;              //position and heading
  unsigned short int svr,svg,svb;  //color
  int svpt;                        //pen type
  TST *next;                       //turtle save stack enabler

};

class pbm {

 public:

   //constructors and destructors
   pbm();                     //basic constructor, creates nothing much
   pbm(int sizex,int sizey);  //constructs a picture of the given size 
   ~pbm();                    //Destructor, deallocates everything

   void deallocate();         //called by destructor, manages deallocation
   void create(int sizex,int sizey);  //create a picture of a given size
   
   //utility routines
   void clear();  //clear to black
   void clear(unsigned short int rv,  //clear to a shade
              unsigned short int gv, 
              unsigned short int bv);
   int width();  //report picture width
   int height(); //report picture height


   //local graphics routines
   void putpixel(int xp,int yp,             //PIXELS 
                 unsigned short int rv,
                 unsigned short int gv,   //put pixel in current color
                 unsigned short int bv);

   void putpixel(int xp,int yp);   //put the pixel in the current color
   void flippixel(int xp,int yp);  //invert the pixel's RGB values
   void setpixmode(int mode);      //set the basic pixel writemode
   int lumi(int xp,int yp);        //return the luminance for a given pixel

   /*
       0 - overwrite
       1 - xor
       2 - and
       3 - or
       4 - max
       5 - min
    */

   void getpixel(int xp,int yp,
                 unsigned short int &rv,
                 unsigned short int &gv, 
                 unsigned short int &bv);

   void MoveTo(long int xv,long int yv);    //LINE DRAWING
   void LineTo(long int xv,long int yv);
   void DrawPath(long int *path,int n); //path: (x0y0x1y1...x(n-1)y(n-1))

   void DrawRect(long int a,long int b,  //fill in a rectangle
                 long int c,long int d);

   void Circle(long int h,long int k,long int r);  //CIRCLES AND ARCS
   void Disk(long int h,long int k,long int r);
   void Sector(long int h,long int k,long int r,double t1,double t2);
   void Annulus(long int h,long int k,long int r,long int s);
   void Elipse(long int h,long int k,
               long int R,long int r,
               double th);
   void ShadeDisk(long int h,long int k,long int r);
   void ShadeJelly(long int h,long int k,long int r);
   void ShadeElipse(long int h,long int k,
                    long int R,long int r,
                    double th);

   void ReflectElipse(long int h,long int k,
                    long int R,long int r,
                    double th,double xx,double yy,double zz);

   //REGULAR POLYGON METHODS
   void Polygon(long int h,long int k,long int r,int n,double th);

   void SetCol(unsigned short int red,      //COLOR SET
               unsigned short int green,
               unsigned short int blue);

   void SetColHSV(int hue,      //HSV COLOR SET
                  int sat,
                  int val);

   void SetBack(unsigned short int red,      //BACKGROUND COLOR SET
                unsigned short int green,
                unsigned short int blue);

   void SetFill(unsigned short int red,      //FILL COLOR
                unsigned short int green,
                unsigned short int blue);

   void BigLineTo(long int xv,long int yv);   //BIG LINE-CIRCLE ROUTINES
   void BigDrawPath(long int *path,int n); //path: (x0y0x1y1...x(n-1)y(n-1))
   void SetLargeW(int NewWidth);
   void SetMin(unsigned short int red,
               unsigned short int green,
               unsigned short int blue);
   void SetMax(unsigned short int red,
               unsigned short int green,
               unsigned short int blue);
   void BigCircle(long int h,long int k,long int r);

   void FillPath(long int *path,int n); //path: (x0y0x1y1...x(n-1)y(n-1))

   void Hypotrochoid(int h,int k,  //center
                     int R, int r, //outer and inner radius
                     int rho,      //inner radius of pen rho<r typically
                     int sam);     //number of samples

   //turtle routines
   void Tstart(int xp,int yp,double th); //creates or restarts a turtle
   void TPencolor(unsigned short int rpc, //assign a new pen color
                  unsigned short int gpc,
                  unsigned short int bpc);
   void TPenType(int npt); //change the pen type
   void Tpush(); //push the state of the turtle
   void Tpop();  //pop the state of the turtle
   void Tforward(double d); //move the turtle forward
   void Tturn(double t);    //turn the turtle d degrees

   //cubic spline routines
   void CSPath(long int *path,int n);  //path: (x0y0x1y1...x(n-1)y(n-1))
   void CSPoly(long int *path,int n);  //path: (x0y0x1y1...x(n-1)y(n-1))

   //FULL PICTURE MANIPULATORS
   void invert();                            //invert the pixels
   void con1();                              //central convolution
   void con2();                              //embossing convolution
   void con3();                              //blur convolution
   void con4();                              //Grout filter
   void con5();                              //Grout filter, green only
   void avg2(unsigned short int red,         //average toward
             unsigned short int green,
             unsigned short int blue);
   void gray();                              //turn the picture gray

   //PUT Routines
   void putat(long int xv,long int yv,pbm &putee);
   void pasteat(long int xv,long int yv,pbm &putee);
   void sput(long int r,pbm &im);
   void mask(unsigned short int rm,  //when pic color does not
             unsigned short int gm,  //match specified color
             unsigned short int bm,  //import color from M
             pbm &M);
   void mask(pbm &M);  //when pic color does not
                       //match background
                       //import color from M
            
   
   //  I/O routines
   void writeXV(const char *filename);
   void writeXVC(const char *filename);
   void writeACT(const char *filename,
                 unsigned short int rv, 
                 unsigned short int gv,
		 unsigned short int bv);
   void readXV(const char *filename);

   //infinite precision pictures
   void face(double x,double y,      //smiley face
             unsigned short int &rm,
             unsigned short int &gm,
             unsigned short int &bm
	     );

 private:

   //image properties
   long int x,y;             //size of image
   int pixmode;
   unsigned short int *r;    //red bit plane  
   unsigned short int *g;    //green bit plane
   unsigned short int *b;    //blue bit plane

   //turtle properties
   double tx,ty,hd;             //turtle position and heading
   unsigned short int tr,tg,tb; //turtle pen color
   int tpt;                     //turtle pen type
   TST *stack;                  //turtle state stack   
   /*pens:  0=up 1=down *         *
                          2=down ***
			          *
    */
}; 
