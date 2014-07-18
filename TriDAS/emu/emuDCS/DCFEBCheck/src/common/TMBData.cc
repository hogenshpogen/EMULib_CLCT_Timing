#include "emu/pc/TMBData.h"

namespace emu{
  namespace pc{

class SmallUnpack;

TMBData::TMBData(){ 
  inuse=0;
}

void TMBData::Fill(int offset,short int *data){
  inuse=1;
  printf(" enter TMB:: Fill \n");
  ctbins=(data[offset+19]&0x00F8)>>3;
  cncfebs=data[offset+19]&0x0007;
  printf(" data %d %d %04x \n",ctbins,cncfebs,data[offset+19]);
  fflush(stdout);
  if(data[offset+42]!=0x6e0b)printf(" Bad CLCT Header \n",data[offset+42]);
  for(int i=0;i<5;i++){
    for(int j=0;j<6;j++){
      for(int k=0;k<32;k++){
        for(int l=0;l<8;l++){
          triad[i][j][k][l]=-1;
        }
      }
    }
  }
  //  int layers[6]={2,0,4,5,3,1};
  int layers[6]={0,1,2,3,4,5};
  int ucla=0;
  for(int i=0;i<cncfebs;i++){
    for(int j=0;j<ctbins;j++){
      for(int k=0;k<6;k++){
	rawtriad[i][k][j]=data[offset+43+ucla];ucla++;
	for(int m=0;m<8;m++){
	    triad[i][layers[k]][j][m]=(rawtriad[i][k][j]>>m)&0X0001;
	  /*  if(k<3){
	    triad[i][layers[2*k]][j][m]=(rawtriad[i][k][j]>>m)&0X0001;
	    triad[i][layers[2*k+1]][j][m]=(rawtriad[i][k][j]>>m+4)&0X0001;
	  }
	  else {
	    triad[i][layers[2*(k-3)]][j][m+4]
	      =(rawtriad[i][k][j]>>m)&0X0001;
	    triad[i][layers[2*(k-3)+1]][j][m+4]
	      =(rawtriad[i][k][j]>>m+4)&0X0001;
	      } */
	}
      }
    }
  }
  int bits[3];
  int tbin;
  for(int i=0;i<cncfebs;i++){
    for(int k=0;k<6;k++){
      for(int m=0;m<32;m++)chits[i][k][m]=0;
      for(int l=0;l<8;l++){
	for(int z=0;z<3;z++)bits[z]=0;
	tbin=0; 
        for(int j=0;j<ctbins;j++){
          tbin=j;
	  if(triad[i][k][j][l]==1){
            for(int zz=0;zz<3;zz++){
              if(j+zz<ctbins){
                bits[zz]=triad[i][k][j+zz][l];
              }
            }
            goto GOTIT;
          }
        }
	GOTIT:
	    int ihit=4*l+bits[1]*2+bits[2];
 	    if(bits[0]==1&&tbin!=0)printf("cfeb %d pln %d l %d  hit %d %d %d ihit %d tbin %d \n",i,k,l,bits[0],bits[1],bits[2],ihit,tbin);
            if(bits[0]==1)chits[i][k][ihit]=tbin;
      }
    } 
  }
  return;
}

TMBData::~TMBData(){
}

}
}
