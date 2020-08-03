#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>

#include "pbm.h"

#define s2 1.4142136
#define pi 3.14159265

//last (x,y)
long int px;
long int py;

//Clipping status  0=no 1=yes
int clipPBM=1;

//initial brush color
unsigned short int pr=255;
unsigned short int pg=255;
unsigned short int pb=255;

//initial background color
unsigned short int bgr=0;
unsigned short int bgg=0;
unsigned short int bgb=0;

//mincolor for thicklines
unsigned short int minr=127;
unsigned short int ming=127;
unsigned short int minb=127;

//maxcolor for thicklines
unsigned short int maxr=255;
unsigned short int maxg=255;
unsigned short int maxb=255;

//color for fills
unsigned short int filr=255;
unsigned short int filg=255;
unsigned short int filb=255;

//Big Line Width
int BigLW=3;

unsigned short int min(unsigned short int a,unsigned short int b);
unsigned short int max(unsigned short int a,unsigned short int b);
long int min(long int a,long int b);
//long int abs(long int a);
int below(int x1,int y1,int x2,int y2,int a,int b);

unsigned short int min(unsigned short int a,unsigned short int b){

   if(a<b)return(a);else return(b);

}

unsigned short int max(unsigned short int a,unsigned short int b)
{

   if(a>b)return(a);else return(b);

}

long int min(long int a,long int b){

   if(a<b)return(a);else return(b);

}

//long int abs(long int a){

//   if(a<0)return(-a);else return(a);

//}

int below(int x1,int y1,int x2,int y2,int a,int b){

int u;

  if(x1>x2){//WOLOG x1<=x2
    u=x1;x1=x2;x2=u;
    u=y1;y1=y2;y2=u;
  }
  if((x1>=a)||(x2<a))return(0);
  if(x1==x2){
    if(y1<y2)return((y1<a)&&(a<y2)); else return((y2<a)&&(a<y1)); 
  } else {
    return(y1*x2-y2*x1+a*(y2-y1)>b*(x2-x1));
  }
}

pbm::pbm(){

   x=0;
   y=0;
   pixmode=0;
   r=(unsigned short int *)0;
   g=(unsigned short int *)0;
   b=(unsigned short int *)0;
   stack=0;
   tx=ty=hd=0.0;
   tr=tg=tb=0;
   tpt=0;
}

pbm::pbm(int sizex,int sizey){

  x=0;
  create(sizex,sizey);

}

pbm::~pbm(){

  deallocate();

}

void pbm::create(int sizex,int sizey){//create a picture of a given size

long int size;

   if(x!=0)deallocate();
   size=sizex*sizey;
   x=sizex;
   y=sizey;
   pixmode=0;
   r=new unsigned short int[size];
   g=new unsigned short int[size];
   b=new unsigned short int[size];
   stack=0;
   tx=ty=hd=0.0;
   tr=tg=tb=0;
   tpt=0;

}

void pbm::deallocate(){

TST *kill;

  delete [] r;
  delete [] g;
  delete [] b;

  while(stack){//clean out the turtle stack
    kill=stack;
    stack=stack->next;
    delete [] kill;
  }
}

//utility routines
void pbm::clear(){

long int i;

   for(i=0;i<x*y;i++){
      r[i]=bgr;
      g[i]=bgg;
      b[i]=bgb;
   }

}

void pbm::clear(
                unsigned short int rv, 
                unsigned short int gv, 
                unsigned short int bv
               ){

long int i;

   for(i=0;i<x*y;i++){
      r[i]=rv;
      g[i]=gv;
      b[i]=bv;
   }
}

int pbm::width(){

   return(x);

}

int pbm::height(){

   return(y);

}

void pbm::putpixel(int xp,int yp, 
                   unsigned short int rv, 
                   unsigned short int gv, 
                   unsigned short int bv){
long int pos;

//cout << xp << " " << yp << " " << rv << " " << gv << " " << bv << endl;

  if(clipPBM==1){//clipping requested?
     if((yp>=y)||(yp<0))return;  //failsafe pixels
     if((xp>=x)||(xp<0))return;  //failsafe pixels
   }

   if((yp>=y)||(yp<0))yp=((yp%y)+y)%y;  //failsafe pixels
   if((xp>=x)||(xp<0))xp=((xp%x)+x)%x;  //failsafe pixels
   pos=yp*x+xp;

   switch(pixmode){
   case 0: //write
      r[pos]=rv;  //red
      g[pos]=gv;  //green
      b[pos]=bv;  //blue
      break;
   case 1: //xor
      r[pos]=rv^r[pos];  //red
      g[pos]=gv^g[pos];  //green
      b[pos]=bv^b[pos];  //blue
      break;
   case 2: //and
      r[pos]=rv&r[pos];  //red
      g[pos]=gv&g[pos];  //green
      b[pos]=bv&b[pos];  //blue
      break;
   case 3: //or
      r[pos]=rv|r[pos];  //red
      g[pos]=gv|g[pos];  //green
      b[pos]=bv|b[pos];  //blue
      break;
   case 4: //max
      r[pos]=max(rv,r[pos]);  //red
      g[pos]=max(gv,g[pos]);  //green
      b[pos]=max(bv,b[pos]);  //blue
      break;
   case 5: //min
      r[pos]=min(rv,r[pos]);  //red
      g[pos]=min(gv,g[pos]);  //green
      b[pos]=min(bv,b[pos]);  //blue
      break;
   }
}

void pbm::putpixel(int xp,int yp){

long int pos;

  if(clipPBM==1){//clipping requested?
     if((yp>=y)||(yp<0))return;  //failsafe pixels
     if((xp>=x)||(xp<0))return;  //failsafe pixels
   }

   if((yp>=y)||(yp<0))yp=((yp%y)+y)%y;  //failsafe pixels
   if((xp>=x)||(xp<0))xp=((xp%x)+x)%x;  //failsafe pixels

   pos=yp*x+xp;

   switch(pixmode){
   case 0: //write
      r[pos]=pr;  //red
      g[pos]=pg;  //green
      b[pos]=pb;  //blue
      break;
   case 1: //xor
      r[pos]=pr^r[pos];  //red
      g[pos]=pg^g[pos];  //green
      b[pos]=pb^b[pos];  //blue
      break;
   case 2: //and
      r[pos]=pr&r[pos];  //red
      g[pos]=pg&g[pos];  //green
      b[pos]=pb&b[pos];  //blue
      break;
   case 3: //or
      r[pos]=pr|r[pos];  //red
      g[pos]=pg|g[pos];  //green
      b[pos]=pb|b[pos];  //blue
      break;
   case 4: //max
      r[pos]=max(pr,r[pos]);  //red
      g[pos]=max(pg,g[pos]);  //green
      b[pos]=max(pb,b[pos]);  //blue
      break;
   case 5: //min
      r[pos]=min(pr,r[pos]);  //red
      g[pos]=min(pg,g[pos]);  //green
      b[pos]=min(pb,b[pos]);  //blue
      break;
   }
}

void pbm::setpixmode(int mode){
   /*
       0 - overwrite
       1 - xor
       2 - and
       3 - or
       4 - max
       5 - min
    */
   pixmode=mode;
}
   

void pbm::flippixel(int xp,int yp){//flip the RGB values

long int pos;

   if((yp>=y)||(yp<0))yp=((yp%y)+y)%y;  //failsafe pixels
   if((xp>=x)||(xp<0))xp=((xp%x)+x)%x;  //failsafe pixels

   pos=yp*x+xp;

   r[pos]=255-r[pos];  //red
   g[pos]=255-g[pos];  //green
   b[pos]=255-b[pos];  //blue

}

int pbm::lumi(int xp,int yp){//return the luminance for a given pixel

long int pos;

   if((yp>=y)||(yp<0))yp=((yp%y)+y)%y;  //failsafe pixels
   if((xp>=x)||(xp<0))xp=((xp%x)+x)%x;  //failsafe pixels

   pos=yp*x+xp;

   return((int)(
     (0.299*((double)r[pos]))+
     (0.587*((double)g[pos]))+
     (0.114*((double)b[pos]))
     ));

}


void pbm::getpixel(int xp,int yp,
                 unsigned short int &rv,
                 unsigned short int &gv, 
		 unsigned short int &bv){
long int pos;

   //cout << "X=" << x << " Y=" << y << endl;

   if((yp>=y)||(yp<0))yp=((yp%y)+y)%y;  //failsafe pixels
   if((xp>=x)||(xp<0))xp=((xp%x)+x)%x;  //failsafe pixels

   //cout << "XP=" << xp << " " << " YP=" << yp << endl;

   pos=yp*x+xp;

   rv=r[pos];  //red
   gv=g[pos];  //green
   bv=b[pos];  //blue

}

void pbm::MoveTo(long int xv,long int yv){

   px=xv;
   py=yv;

}

void pbm::LineTo(long int xv,long int yv){

long int a,b,c,d,xl,yl,xc,yc;
int dy;

  if(px>xv){  //make sure a<=c
     a=xv;
     b=yv;
     c=px;
     d=py;
  } else {
     a=px;
     b=py;
     c=xv;
     d=yv;
  }
  putpixel(a,b);
  xl=c-a+1;
  if(b>d)dy=-1;else dy=1;
  yl=(d-b)*dy+1;
  xc=a;
  yc=b;
  if(a==c){
    while(yc!=d){
      putpixel(xc,yc);
      yc+=dy;
    }
  } else if(b==d) {
    while(xc!=c){  
      putpixel(xc,yc);
      xc+=1;
    }
  } else {
    while((xc!=c)||(yc!=d)){
      if((xc-a)*(d-b)*dy>(yc-b)*(c-a)*dy)yc+=dy; else xc++;
      putpixel(xc,yc);
    }
  }
  putpixel(c,d);
  px=xv;
  py=yv;
}
   
void pbm::DrawRect(long int a,long int b,  //fill in a rectangle
                   long int c,long int d){

long int i,j;

  for(i=a;i<=c;i++)for(j=b;j<=d;j++)putpixel(i,j);


}

void pbm::DrawPath(long int *path,int n){//path: (x0y0x1y1...x(n-1)y(n-1))

int i,j;

  if(n<2)return;
  MoveTo(path[0],path[1]);
  j=2;
  for(i=1;i<n;i++){
     LineTo(path[j],path[j+1]);
     j+=2;
  }
}

void pbm::Circle(long int h,long int k,long int r){  //CIRCLES AND ARCS

long int s,t,u,rt,ru,rr,ox,oy,nx,ny;

   oy=r;
   ox=0;
   t=oy;
   rr=r*r;
   for(s=0;s<=(long int)(r/s2)+1;s++){
      u=t-1;
      rt=(s*s+t*t);
      ru=(s*s+u*u);
      while(abs(rr-rt)>abs(rr-ru)){
         rt=ru;
         t=u--;
         ru=(s*s+u*u);
      }
      putpixel(h+s,k+t);
      putpixel(h-s,k+t);
      putpixel(h+s,k-t);
      putpixel(h-s,k-t);
      putpixel(h+t,k-s);
      putpixel(h-t,k-s);
      putpixel(h+t,k+s);
      putpixel(h-t,k+s);
      ox=s;
      oy=t;
      
   }
}

void pbm::Disk(long int h,long int k,long int r){  //CIRCLES AND ARCS

long int i,j,rr;

   rr=r*r;
   for(i=-r;i<=r;i++)for(j=-r;j<=r;j++){
      if(i*i+j*j<rr)putpixel(i+h,k+j);
   }
}

//Fill a sector of a disk
void pbm::Sector(long int h,long int k,long int r,double t1,double t2){

long int i,j,rr;
double th;

  rr=r*r;
  for(i=-r;i<=r;i++)for(j=-r;j<=r;j++){
      if(i*i+j*j<rr){//in bounds?
        th=atan2(j,i)+M_PI;
        if((t1<th)&&(t2>th))putpixel(i+h,k+j);
      }
  }
}

//fill in an annulus
void pbm::Annulus(long int h,long int k,long int r,long int s){
   
long int i,j,rr,ss;
long int q;

  rr=r*r;
  ss=s*s;
  for(i=-r;i<=r;i++)for(j=-r;j<=r;j++){
    q=i*i+j*j;
    if((q<rr)&&(q>ss))putpixel(i+h,k+j);
  }
}

void pbm::Elipse(long int h,long int k,
                 long int R,long int r,
                 double th){
double xp,yp,x,y,sth,cth,d;
long int mr,i,j;

   if(R>r)mr=R; else mr=r;
   sth=sin(th);
   cth=cos(th);
   for(i=-mr;i<=mr;i++)for(j=-mr;j<=mr;j++){
      x=((double) i);
      y=((double) j);
      xp=x*cth-y*sth;
      yp=x*sth+y*cth;
      xp/=R;
      yp/=r;
      d=xp*xp+yp*yp;
      if(d<1)putpixel(i+h,j+k);
   }
}

void pbm::ShadeDisk(long int h,long int k,long int r){  //CIRCLES AND ARCS

double ra;
long int i,j,rr;
unsigned short int sr,sg,sb;

   rr=r*r;
   for(i=-r;i<=r;i++)for(j=-r;j<=r;j++)if(i*i+j*j<rr){
      ra=1.0-((double)(i*i+j*j))/rr;
      sr=((int)(0.499+ra*(maxr-minr)+minr));
      sg=((int)(0.499+ra*(maxg-ming)+ming));
      sb=((int)(0.499+ra*(maxb-minb)+minb));      
      putpixel(i+h,k+j,sr,sg,sb);
   }
}

void pbm::ShadeJelly(long int h,long int k,long int r){  //CIRCLES AND ARCS

double ra;
long int i,j,rr,r2;
unsigned short int sr,sg,sb;

   rr=r*r;
   r2=rr*rr;
   for(i=-r;i<=r;i++)for(j=-r;j<=r;j++)if(i*i*i*i+j*j*j*j<r2){
      ra=1.0-((double)(i*i*i*i+j*j*j*j))/r2;
      if(ra>1.0)ra=1.0;if(ra<0.0)ra=0.0;
      sr=((int)(0.499+ra*(maxr-minr)+minr));
      sg=((int)(0.499+ra*(maxg-ming)+ming));
      sb=((int)(0.499+ra*(maxb-minb)+minb));      
      putpixel(i+h,k+j,sr,sg,sb);
   }
}

void pbm::ShadeElipse(long int h,long int k,
                 long int R,long int r,
                 double th){

double xp,yp,x,y,sth,cth,d,dd;
long int mr,i,j;
unsigned short int sr,sg,sb;

   if(R>r)mr=R; else mr=r;
   sth=sin(th);
   cth=cos(th);
   for(i=-mr;i<=mr;i++)for(j=-mr;j<=mr;j++){
      x=((double) i);
      y=((double) j);
      xp=x*cth+y*sth;
      yp=-x*sth+y*cth;
      xp/=R;
      yp/=r;
      d=xp*xp+yp*yp;
      dd=1-d;
      sr=((int)(0.499+dd*(maxr-minr)+minr));
      sg=((int)(0.499+dd*(maxg-ming)+ming));
      sb=((int)(0.499+dd*(maxb-minb)+minb));      
      if(d<1)putpixel(i+h,j+k,sr,sg,sb);
   }
}

void pbm::ReflectElipse(long int h,long int k,
                 long int R,long int r,
                 double th,double xx,double yy,double zz){

double xp,yp,x,y,sth,cth,d,dd;
long int mr,i,j;
unsigned short int sr,sg,sb;

   if(R>r)mr=R; else mr=r;
   sth=sin(th);
   cth=cos(th);
   dd=xx*xx+yy*yy+zz*zz;
   dd=sqrt(dd);
   xx/=dd;yy/=dd;zz/=dd;
   for(i=-mr;i<=mr;i++)for(j=-mr;j<=mr;j++){
      x=((double) i);
      y=((double) j);
      xp=x*cth+y*sth;
      yp=-x*sth+y*cth;
      xp/=R;
      yp/=r;
      d=xp*xp+yp*yp;
      if(d<1){
        dd=xp*xx+yp*yy+sqrt(1-d)*zz;
        dd=(dd+1)/2.0;
        if(dd>=1.0)cout << dd << " " << xp << " " << yp << "   ";
        sr=((int)(0.499+dd*(maxr-minr)+minr));
        sg=((int)(0.499+dd*(maxg-ming)+ming));
        sb=((int)(0.499+dd*(maxb-minb)+minb));      
        if(d<1)putpixel(i+h,j+k,sr,sg,sb);
      }
   }
}

void pbm::Polygon(long int h,long int k,long int r,int n,double th){

double rd;        //Radius variable
int i;            //loop index
long int xc,yc;   //point coordinates

  for(rd=0;rd<r;rd+=0.5){//loop over radii
    for(i=0;i<=n;i++){//loop over vertices of the polygon
      xc=((int)(h+rd*cos(2*M_PI*i/n+th)));  //X-coordinate
      yc=((int)(k+rd*sin(2*M_PI*i/n+th)));  //Y-coordinate
      if(i==0)MoveTo(xc,yc); else LineTo(xc,yc);  //Draw
    }
  }
}

void pbm::SetCol(unsigned short int red,
            unsigned short int green,
            unsigned short int blue){

   pr=red;
   pg=green;
   pb=blue;

}

void pbm::SetColHSV(int hue,      //HSV COLOR SET
                    int sat,
                    int val){

  if((sat<0)||(sat>100))sat=((sat%101)+101)%101;
  if((val<0)||(val>100))val=((val%101)+101)%101;
  if((hue<0)||(hue>=360))hue=((hue%360)+360)%360;

double s,v;

	s=sat/100.0;
	v=val/100.0;

  if(sat==0){//if we have no color at all its some shade of gray
    pr=int(v*255);
    pg=int(v*255);
    pb=int(v*255);
  } else {

int c;
double p,q,t,f;

  c=hue/60;//c=floor of h/60
  f=hue/60.0-c;
  p=v*(1-s);
  q=v*(1-(s*f));
  t=v*(1-(s*(1-f)));
  switch(c){
    case 0: pr=int(v*255);
            pg=int(t*255);
            pb=int(p*255);
      break;
    case 1: pr=int(q*255);
            pg=int(v*255);
            pb=int(p*255);
      break;
    case 2: pr=int(p*255); 
            pg=int(v*255); 
            pb=int(t*255);
      break;
    case 3:pr=int(p*255);
           pg=int(q*255);
           pb=int(v*255);
      break;
    case 4: pr=int(t*255);
            pg=int(p*255);
            pb=int(v*255);
      break;
    case 5: pr=int(v*255);
            pg=int(p*255);
            pb=int(q*255);
      break;
    }
  }
}

void pbm::SetBack(unsigned short int red,      //BACKGROUND COLOR SET
                  unsigned short int green,
                  unsigned short int blue){

  bgr=red;
  bgg=green;
  bgb=blue;

}

void pbm::SetFill(unsigned short int red,      //FILL COLOR
                  unsigned short int green,
                  unsigned short int blue){
   filr=red;
   filg=green;
   filb=blue;

}

void pbm::BigLineTo(long int xv,long int yv){

double dx,dy,ds,r,ri;
long int svpx,svpy,ux,uy,vx,vy,i,j;
unsigned short int sr,sg,sb;

   sr=pr;sg=pg;sb=pb;   
   svpx=px;
   svpy=py;
   dx=(double)(xv-px);
   dy=(double)(yv-py);
   ds=dy/(r=sqrt(dx*dx+dy*dy));
   dy=-dx/r;
   dx=ds;
   j=-BigLW;
   for(i=-BigLW;i<=BigLW;i++){
      ri=1-abs(i)/((double)BigLW);
      pr=((int)(0.499+ri*(maxr-minr)+minr));
      pg=((int)(0.499+ri*(maxg-ming)+ming));
      pb=((int)(0.499+ri*(maxb-minb)+minb));
      ux=((long int)(0.5+((double)i)*dx+svpx));
      uy=((long int)(0.5+((double)j)*dy+svpy));
      vx=((long int)(0.5+((double)i)*dx+xv));
      vy=((long int)(0.5+((double)j)*dy+yv));
      MoveTo(ux,uy);
      LineTo(vx,vy);      
      j++;
      ux=((long int)(0.5+((double)i)*dx+svpx));
      uy=((long int)(0.5+((double)j)*dy+svpy));
      vx=((long int)(0.5+((double)i)*dx+xv));
      vy=((long int)(0.5+((double)j)*dy+yv));
      MoveTo(ux,uy);
      LineTo(vx,vy);      
   }   
   pr=sr;pg=sg;pb=sb;   
   px=xv;py=yv;
}

void pbm::BigDrawPath(long int *path,int n){//path: (x0y0x1y1...x(n-1)y(n-1))

int i,j;

  if(n<2)return;
  MoveTo(path[0],path[1]);
  j=2;
  for(i=1;i<n;i++){
     BigLineTo(path[j],path[j+1]);
     j+=2;
  }
}

void pbm::SetLargeW(int NewWidth){

   BigLW=NewWidth;

}

void pbm::SetMin(unsigned short int red,
            unsigned short int green,
            unsigned short int blue){

   minr=red;
   ming=green;
   minb=blue;

}

void pbm::SetMax(unsigned short int red,
            unsigned short int green,
            unsigned short int blue){

   maxr=red;
   maxg=green;
   maxb=blue;

}

void pbm::BigCircle(long int h,long int k,long int r){

double ri;
long int i;
unsigned short int sr,sg,sb;

   sr=pr;sg=pg;sb=pb;   
   for(i=BigLW;i>=0;i--){
      ri=1-((double) i)/((double) BigLW);
      pr=((int)(0.499+ri*(maxr-minr)+minr));
      pg=((int)(0.499+ri*(maxg-ming)+ming));
      pb=((int)(0.499+ri*(maxb-minb)+minb));
      if(i<BigLW){
        Circle(h+1,k,r+i);   
        Circle(h-1,k,r-i);   
        Circle(h+1,k,r+i);   
        Circle(h-1,k,r-i);   
        Circle(h,k+1,r+i);   
        Circle(h,k-1,r-i);   
        Circle(h,k+1,r+i);   
        Circle(h,k-1,r-i);   
      }
      Circle(h,k,r+i);   
      Circle(h,k,r-i);   
   }
   pr=sr;pg=sg;pb=sb;   
}

void pbm::FillPath(long int *path,int n){//path: (x0y0x1y1...x(n-1)y(n-1))

int A,B,C,D;
int i,j,k,l,p;

  if(n<2)return;
  A=C=path[0];
  B=D=path[1];
  j=2;
  for(i=1;i<n;i++){
    if(path[j]<A)A=path[j];
    if(path[j]>C)C=path[j];
    j++;
    if(path[j]<B)B=path[j];
    if(path[j]>D)D=path[j];
    j++;
  }
  for(i=A;i<=C;i++)for(j=B;j<=D;j++){
    p=0;
    l=0;
    for(k=0;k<n-1;k++){
      if(below(path[l],path[l+1],path[l+2],path[l+3],i,j))p=1-p;
      l+=2;
    }
    if(p)putpixel(i,j,filr,filg,filb);
  }
}

void pbm::Hypotrochoid(int h,int k,int R, int r, int rho, int sam){

double th,RR,rr,rrho,dt,x,y,A,B,C;
int i,u,v;

  dt=1.0/360.0;
  RR=((double)R);
  rr=((double)r);
  rrho=((double)rho);
  th=0;
  A=RR+rr;
  B=rr+rrho;
  C=A/rr;
  for(i=0;i<sam;i++){
    x=A*cos(th)+B*cos(C*th);
    y=A*sin(th)+B*sin(C*th);
    u=h+((int)x);
    v=k+((int)y);
    if(i==0)MoveTo(u,v); else LineTo(u,v);
    th+=dt;
  }
}

//turtle routines
void pbm::Tstart(int xp,int yp,double th){//creates or restarts a turtle

  tx=((double)xp);
  ty=((double)yp);
  hd=th/180*pi;
  tpt=1;

}

void pbm::TPencolor(unsigned short int rpc, //assign a new pen color
                   unsigned short int gpc,
                   unsigned short int bpc){
  tr=rpc;
  tg=gpc;
  tb=bpc;

}

void pbm::TPenType(int npt){//change the pen type

  tpt=npt;
  if(tpt<0)tpt=0;

}

void pbm::Tpush(){//push the state of the turtle

TST *bld;

  bld=new TST;
  bld->next=stack;
  bld->svx=tx;
  bld->svy=ty;
  bld->svh=hd;
  bld->svr=tr;
  bld->svg=tg;
  bld->svb=tb;
  bld->svpt=tpt;
  stack=bld;

}

void pbm::Tpop(){//pop the state of the turtle

TST *kill;

  if(stack){
    kill=stack;
    stack=stack->next;
    tx=kill->svx;
    ty=kill->svy;
    hd=kill->svh;
    tr=kill->svr;
    tg=kill->svg;
    tb=kill->svb;
    tpt=kill->svpt;
    delete [] kill;
  }

}

void pbm::Tforward(double d){//move the turtle forward

double dx,dy,ttlx,ttly;
int i,pix,pij;

  if(tpt){
    dx=cos(hd)/2;
    dy=sin(hd)/2;
    ttlx=0;
    ttly=0;
    while(ttlx*ttlx+ttly*ttly<d*d){
      pix=((int)(tx+ttlx));
      pij=((int)(ty+ttly));
      switch(tpt){
      case 1 : putpixel(pix,pij,tr,tg,tb);
	       break;
      case 2 : putpixel(pix,pij,tr,tg,tb);
               putpixel(pix+1,pij,tr,tg,tb);
               putpixel(pix,pij+1,tr,tg,tb);
               putpixel(pix-1,pij,tr,tg,tb);
               putpixel(pix,pij-1,tr,tg,tb);
	       break;
      }
      ttlx+=dx;
      ttly+=dy;
    }
    tx+=ttlx;
    ty+=ttly;
  } else {
    tx+=d*cos(hd);
    ty+=d*sin(hd);
  }
}

void pbm::Tturn(double t){//turn the turtle d degrees

  hd+=(t/180.0*pi);

}

void pbm::CSPath(long int *path,int n){//path: (x0y0x1y1...x(n-1)y(n-1))

double x[n],y[n],xp[n],yp[n];
double ax,bx,cx,dx,ay,by,cy,dy;
double xh,yh,t,dt;
int i,px,py;

  if(n<3)return;

  //create points
  for(i=0;i<n;i++){
    x[i]=((double)path[i*2]);
    y[i]=((double)path[i*2+1]);
  }

  //create derivative estimates
  xp[0]=x[1]-x[0];
  yp[0]=y[1]-y[0];
  xp[n-1]=x[n-1]-x[n-2];
  yp[n-1]=y[n-1]-y[n-2];
  for(i=1;i<n-1;i++){
    xp[i]=(x[i+1]-x[i-1])/2;
    yp[i]=(y[i+1]-y[i-1])/2;
  }

  for(i=0;i<n-1;i++){
    ax=x[i];
    bx=xp[i];
    cx=-xp[i+1]+3*x[i+1]-2*xp[i]-3*x[i];
    dx=xp[i+1]-2*x[i+1]+xp[i]+2*x[i];
    ay=y[i];
    by=yp[i];
    cy=-yp[i+1]+3*y[i+1]-2*yp[i]-3*y[i];
    dy=yp[i+1]-2*y[i+1]+yp[i]+2*y[i];
    t=1/(x[i+1]-x[i]);
    if(t<0)t=-t;
    dt=1/(y[i+1]-y[i]);
    if(dt<0)dt=-dt;
    if(t<dt)dt=t;
    dt/=2;
    for(t=0;t<1;t+=dt){
      xh=ax+t*(bx+t*(cx+t*dx));
      yh=ay+t*(by+t*(cy+t*dy));
      px=((int)xh);
      py=((int)yh);
      putpixel(px,py);
      putpixel(px+1,py);
      putpixel(px,py+1);
      putpixel(px+1,py+1);
    }
  }
}
void pbm::CSPoly(long int *path,int n){//path: (x0y0x1y1...x(n-1)y(n-1))

double x[n],y[n],xp[n],yp[n];
double ax,bx,cx,dx,ay,by,cy,dy;
double xh,yh,t,dt;
int i,px,py,sx,sy;

  if(n<3)return;

  //create points
  for(i=0;i<n;i++){
    x[i]=((double)path[i*2]);
    y[i]=((double)path[i*2+1]);
  }

  //create derivative estimates
  xp[0]=(x[1]-x[n-1])/2;
  yp[0]=(y[1]-y[n-1])/2;
  xp[n-1]=(x[0]-x[n-2])/2;
  yp[n-1]=(y[0]-y[n-2])/2;
  for(i=1;i<n-1;i++){
    xp[i]=(x[i+1]-x[i-1])/2;
    yp[i]=(y[i+1]-y[i-1])/2;
  }

  for(i=0;i<n-1;i++){
    ax=x[i];
    bx=xp[i];
    cx=-xp[i+1]+3*x[i+1]-2*xp[i]-3*x[i];
    dx=xp[i+1]-2*x[i+1]+xp[i]+2*x[i];
    ay=y[i];
    by=yp[i];
    cy=-yp[i+1]+3*y[i+1]-2*yp[i]-3*y[i];
    dy=yp[i+1]-2*y[i+1]+yp[i]+2*y[i];
    t=1/(x[i+1]-x[i]);
    if(t<0)t=-t;
    dt=1/(y[i+1]-y[i]);
    if(dt<0)dt=-dt;
    if(t<dt)dt=t;
    dt/=2;
    for(t=0;t<1;t+=dt){
      xh=ax+t*(bx+t*(cx+t*dx));
      yh=ay+t*(by+t*(cy+t*dy));
      px=((int)xh);
      py=((int)yh);
      for(sx=px-1;sx<=px+1;sx++)
        for(sy=py-1;sy<=py+1;sy++)
          putpixel(sx,sy);
    }
  }
  ax=x[n-1];
  bx=xp[n-1];
  cx=-xp[0]+3*x[0]-2*xp[n-1]-3*x[n-1];
  dx=xp[0]-2*x[0]+xp[n-1]+2*x[n-1];
  ay=y[n-1];
  by=yp[n-1];
  cy=-yp[0]+3*y[0]-2*yp[n-1]-3*y[n-1];
  dy=yp[0]-2*y[0]+yp[n-1]+2*y[n-1];
  t=1/(x[0]-x[n-1]);
  if(t<0)t=-t;
  dt=1/(y[0]-y[n-1]);
  if(dt<0)dt=-dt;
  if(t<dt)dt=t;
  dt/=2;
  for(t=0;t<1;t+=dt){
    xh=ax+t*(bx+t*(cx+t*dx));
    yh=ay+t*(by+t*(cy+t*dy));
    px=((int)xh);
    py=((int)yh);
    for(sx=px-1;sx<=px+1;sx++)
      for(sy=py-1;sy<=py+1;sy++)
        putpixel(sx,sy);
  }
}

void pbm::invert(){

long int i;

   for(i=0;i<x*y;i++){
      r[i]=255-r[i];
      g[i]=255-g[i];
      b[i]=255-b[i];
   }

}

void pbm::con1(){//        -1 -1 -1  
                         //-1  8 -1
long int i,j,pos,accu;   //-1 -1 -1
unsigned short int *rr;    //red bit plane  
unsigned short int *gg;    //green bit plane
unsigned short int *bb;    //blue bit plane

   rr=new unsigned short int[x*y];
   gg=new unsigned short int[x*y];
   bb=new unsigned short int[x*y];
   for(i=0;i<x*y;i++){rr[i]=128;gg[i]=128;bb[i]=128;}
   for(i=1;i<x-1;i++)for(j=1;j<y-1;j++){
      pos=j*x+i;
      accu=8*r[pos]-r[pos+1]-r[pos-1]-r[pos+x]-r[pos-x]-
             r[pos+x+1]-r[pos-x+1]-r[pos-x+1]-r[pos-x-1];
      rr[pos]=(accu/16+128);
      accu=8*g[pos]-g[pos+1]-g[pos-1]-g[pos+x]-g[pos-x]-
             g[pos+x+1]-g[pos-x+1]-g[pos-x+1]-g[pos-x-1];
      gg[pos]=(accu/16+128);
      accu=8*b[pos]-b[pos+1]-b[pos-1]-b[pos+x]-b[pos-x]-
             b[pos+x+1]-b[pos-x+1]-b[pos-x+1]-b[pos-x-1];
      bb[pos]=(accu/16+128);
   }
   delete [] r;
   r=rr;
   delete [] b;
   b=bb;
   delete [] g;
   g=gg;
}

void pbm::con2(){//emboss

int ri[x*y],bi[x*y],gi[x*y];  //integer convolution buffer
long int i,j,pos;
long int rmin,gmin,bmin,rmax,gmax,bmax;
unsigned short int rm,gm,bm;
  
  rmax=gmax=bmax=-1024;
  rmin=gmin=bmin=1024;
  for(i=0;i<x;i++)for(j=0;j<y;j++){
    pos=j*x+i;
    ri[pos]=gi[pos]=bi[pos]=0;
    getpixel(i-1,j-1,rm,gm,bm);
    ri[pos]=ri[pos]-2*rm;
    gi[pos]=gi[pos]-2*gm;
    bi[pos]=bi[pos]-2*bm;
    getpixel(i,j-1,rm,gm,bm);
    ri[pos]=ri[pos]-rm;
    gi[pos]=gi[pos]-gm;
    bi[pos]=bi[pos]-bm;
    getpixel(i-1,j,rm,gm,bm);
    ri[pos]=ri[pos]-rm;
    gi[pos]=gi[pos]-gm;
    bi[pos]=bi[pos]-bm;
    getpixel(i+1,j+1,rm,gm,bm);
    ri[pos]=ri[pos]+2*rm;
    gi[pos]=gi[pos]+2*gm;
    bi[pos]=bi[pos]+2*bm;
    getpixel(i,j+1,rm,gm,bm);
    ri[pos]=ri[pos]+rm;
    gi[pos]=gi[pos]+gm;
    bi[pos]=bi[pos]+bm;
    getpixel(i+1,j,rm,gm,bm);
    ri[pos]=ri[pos]+rm;
    gi[pos]=gi[pos]+gm;
    bi[pos]=bi[pos]+bm;
    if(ri[pos]>rmax)rmax=ri[pos]; else if(ri[pos]<rmin)rmin=ri[pos];
    if(gi[pos]>gmax)gmax=gi[pos]; else if(gi[pos]<gmin)gmin=gi[pos];
    if(bi[pos]>bmax)bmax=bi[pos]; else if(bi[pos]<bmin)bmin=bi[pos];
  }
  rmax-=rmin;gmax-=gmin;bmax-=bmin;
  for(pos=0;pos<x*y;pos++){
    r[pos]=((unsigned short int)((ri[pos]-rmin)*255/rmax));
    g[pos]=((unsigned short int)((gi[pos]-gmin)*255/gmax));
    b[pos]=((unsigned short int)((bi[pos]-bmin)*255/bmax));
  }
}

void pbm::con3(){//blur

int ri[x*y],bi[x*y],gi[x*y];  //integer convolution buffer
long int i,j,k,l,pos;
unsigned short int rm,gm,bm;
  
  for(i=0;i<x;i++)for(j=0;j<y;j++){
    pos=j*x+i;
    ri[pos]=gi[pos]=bi[pos]=0;
    for(k=-1;k<2;k++)for(l=-1;l<2;l++){
      getpixel(i+k,j+l,rm,gm,bm);
      ri[pos]=ri[pos]+rm;
      gi[pos]=gi[pos]+gm;
      bi[pos]=bi[pos]+bm;
    }
    ri[pos]/=9;
    gi[pos]/=9;
    bi[pos]/=9;
  }
  for(pos=0;pos<x*y;pos++){
    r[pos]=((unsigned short int)(ri[pos]));
    g[pos]=((unsigned short int)(gi[pos]));
    b[pos]=((unsigned short int)(bi[pos]));
  }
}

void pbm::con4(){//grout

unsigned short int mask[x*y];  //integer convolution buffer
long int i,j,k,l,pos;
unsigned short int rm,gm,bm,rq,gq,bq;

  for(i=0;i<x;i++)for(j=0;j<y;j++){
    pos=j*x+i;mask[pos]=0;
    getpixel(i,j,rm,gm,bm);
    getpixel(i+1,j,rq,gq,bq);
    if((rm!=rq)||(gm!=gq)||(bm!=bq))mask[pos]=1;
    getpixel(i-1,j,rq,gq,bq);
    if((rm!=rq)||(gm!=gq)||(bm!=bq))mask[pos]=1;
    getpixel(i,j+1,rq,gq,bq);
    if((rm!=rq)||(gm!=gq)||(bm!=bq))mask[pos]=1;
    getpixel(i,j-1,rq,gq,bq);
    if((rm!=rq)||(gm!=gq)||(bm!=bq))mask[pos]=1;
  }

  for(pos=0;pos<x*y;pos++)if(mask[pos]==1){
    r[pos]=0;  
    g[pos]=0;  
    b[pos]=0;  
  }

}

void pbm::con5(){//grout based only on green

unsigned short int mask[x*y];  //integer convolution buffer
long int i,j,k,l,pos;
unsigned short int rm,gm,bm,rq,gq,bq;

  for(i=0;i<x;i++)for(j=0;j<y;j++){
    pos=j*x+i;mask[pos]=0;
    getpixel(i,j,rm,gm,bm);
    getpixel(i+1,j,rq,gq,bq);
    if(gm!=gq)mask[pos]=1;
    getpixel(i-1,j,rq,gq,bq);
    if(gm!=gq)mask[pos]=1;
    getpixel(i,j+1,rq,gq,bq);
    if(gm!=gq)mask[pos]=1;
    getpixel(i,j-1,rq,gq,bq);
    if(gm!=gq)mask[pos]=1;
  }

  for(pos=0;pos<x*y;pos++)if(mask[pos]==1){
    r[pos]=0;  
    g[pos]=0;  
    b[pos]=0;  
  }

}

void pbm::avg2(unsigned short int red,         //average toward
               unsigned short int green,
	       unsigned short int blue){

int i,j,pos;

  for(i=0;i<x;i++)for(j=0;j<y;j++){
    pos=j*x+i;
    r[pos]=(r[pos]+red)/2;
    g[pos]=(g[pos]+green)/2;
    b[pos]=(b[pos]+blue)/2;
  }
}

void pbm::gray(){//turn the picture gray

long int size=x*y;

  for(long int i=0;i<size;i++){
    r[i]=g[i]=b[i]=((int)(
                          (0.299*((double)r[i]))+
                          (0.587*((double)g[i]))+
                          (0.114*((double)b[i]))
			  ));
  }

}

void pbm::putat(long int xv,long int yv,pbm &putee){

int i,j;

   for(i=xv;i<min(x,xv+putee.x);i++)
     for(j=yv;j<min(y,yv+putee.y);j++){
       if(
          ( putee.r[(j-yv)*putee.x+(i-xv)]>0)||
          ( putee.g[(j-yv)*putee.x+(i-xv)]>0)||
          ( putee.b[(j-yv)*putee.x+(i-xv)]>0))
       putpixel(i,j,
                putee.r[(j-yv)*putee.x+(i-xv)],
                putee.g[(j-yv)*putee.x+(i-xv)],
                putee.b[(j-yv)*putee.x+(i-xv)]
               );
     }
}

void pbm::pasteat(long int xv,long int yv,pbm &putee){

int i,j;

   for(i=xv;i<min(x,xv+putee.x);i++)
     for(j=yv;j<min(y,yv+putee.y);j++){
       putpixel(i,j,
                putee.r[(j-yv)*putee.x+(i-xv)],
                putee.g[(j-yv)*putee.x+(i-xv)],
                putee.b[(j-yv)*putee.x+(i-xv)]
               );
     }
}

void pbm::sput(long int r,pbm &im){

long int i,j,u,v;
double h,k,th,phe,f;
unsigned short int rr,gg,bb;

   for(i=0;i<im.width();i++){
      h=((double) i)/im.width();
      f=4*(h-h*h);
      th=pi*h;
      for(j=0;j<im.height();j++){
         k=((double) j)/im.height();
         phe=pi*k;
         u=x/2+((long int)(-r*cos(th)));
         v=y/2+((long int)(-r*cos(phe)*f));
         im.getpixel(i,j,rr,gg,bb);
         putpixel(u,v,rr,gg,bb);
      }  
   }
}

//when pixels of the current picture fail to match rm,gm,bm
//import the color from M
void pbm::mask(unsigned short int rm,  //when pic color does not
               unsigned short int gm,  //match specified color
               unsigned short int bm,  //import color from M
               pbm &M){

unsigned short int rr,gg,bb;
long int i,j,pos;

  for(i=0;i<x;i++)for(j=0;j<y;j++){
    pos=j*x+i;
    if((r[pos]!=rm)||(g[pos]!=gm)||(b[pos]!=bm)){
      M.getpixel(i,j,rr,gg,bb);
      r[pos]=rr;
      g[pos]=gg;
      b[pos]=bb;
    }
  }
}

void pbm::mask(pbm &M){//when pic color does not
                       //match background
                       //import color from M

unsigned short int rr,gg,bb;
long int i,j,pos;

  for(i=0;i<x;i++)for(j=0;j<y;j++){
    pos=j*x+i;
    if((r[pos]!=bgr)||(g[pos]!=bgg)||(b[pos]!=bgb)){
      M.getpixel(i,j,rr,gg,bb);
      r[pos]=rr;
      g[pos]=gg;
      b[pos]=bb;
    }
  }
}

void pbm::writeXV(const char *filename){

fstream tx;
long int i;

   tx.open(filename,ios::out);
   tx << "P3" << endl << x << " " << y << endl << "255" << endl;
   for(i=0;i<x*y;i++){
     if((i%5==0)&&(i>0))tx << endl;
     tx << r[i] << " " << g[i] << " " << b[i] << " ";
   }   
   tx.close();
}

void pbm::writeXVC(const char *filename){

fstream tx;
long int i;

   tx.open(filename,ios::out);
   tx << "P6" << endl << x << " " << y << endl << "255" << endl;
   for(i=0;i<x*y;i++){
     tx << ((char)r[i]) << ((char)g[i]) << ((char)b[i]);
   }
   tx.close();
}

void pbm::writeACT(const char *filename,  //write active are in raw char. form
                   unsigned short int rv,
	           unsigned short int gv,
	           unsigned short int bv){

int i,j;
unsigned short int rl,gl,bl;
int xlo,xhi,ylo,yhi;
int flag;
fstream tx;

  xlo=x;xhi=0;ylo=y;yhi=0;flag=0;
  for(i=0;i<x;i++)for(j=0;j<y;j++){
    getpixel(i,j,rl,gl,bl);
    if((rv!=rl)||(gv!=rl)||(bv!=rl)){
      flag=1;
      if(i<xlo)xlo=i;
      if(i>xhi)xhi=i;
      if(j<ylo)ylo=j;
      if(j>yhi)yhi=j;
    }
  }

  tx.open(filename,ios::out);
  tx << "P6" << endl << xhi-xlo+1 << " " << yhi-ylo+1 << endl << "255" << endl;
  for(i=ylo;i<=yhi;i++)for(j=xlo;j<=xhi;j++){
    getpixel(j,i,rl,gl,bl);
    tx << ((char)rl) << ((char)gl) << ((char)bl);
  }
  tx.close();
  
}

void pbm::readXV(const char *filename){

fstream tx;

char inp[300];
int dummy;
long int i;

   tx.open(filename,ios::in);
   tx.getline(inp,99);
   if((inp[0]!='P')||(inp[1]!='3')){
      cout << "wrong magic file type in read " <<  inp << endl;
      return;
   }
   tx.getline(inp,299);
   while(inp[0]=='#')tx.getline(inp,299);
   x=atoi(inp);
   i=0;
   while(inp[i]!=' ')i++;
   i++;
   y=atoi(inp+i);
   tx >> dummy;
   //cout << "READXV " << x << " " << y << " " << dummy << endl;
   delete [] r;
   delete [] g;
   delete [] b;
   r=new unsigned short int[x*y];
   g=new unsigned short int[x*y];
   b=new unsigned short int[x*y];
   for(i=0;i<x*y;i++)tx >> r[i] >> g[i] >> b[i];   
   tx.close();
}

//infinite precision pictures
void pbm::face(double x,double y,      //smiley face
               unsigned short int &rm,
               unsigned short int &gm,
               unsigned short int &bm
	       ){

   rm=gm=bm=255; //initialize to white
   if(((x-0.28)*(x-0.28)+(y-0.28)*(y-0.28)<0.004)||
     ((x-0.72)*(x-0.72)+(y-0.28)*(y-0.28)<0.004)){//eyes
     rm=gm=0;
     return;
   }
   if(((x-0.5)*(x-0.5)+(y-0.51)*(y-0.51)<0.11)&& //mouth
      ((x-0.5)*(x-0.5)+(y-0.46)*(y-0.46)>0.1)&&
      (y>0.52)){
     gm=bm=0;
     return; 
   }
   if((x-0.5)*(x-0.5)+(y-0.5)*(y-0.5)<0.23){//face
     bm=0;
     return;
   }

}
