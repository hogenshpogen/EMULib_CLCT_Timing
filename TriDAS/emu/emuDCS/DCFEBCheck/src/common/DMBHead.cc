#include "emu/pc/DMBHead.h"

namespace emu{
namespace pc{

class SmallUnpack;

DMBHead::DMBHead(){ 
}

void DMBHead::Fill(int offset,short int *data){
  //  printf(" entered DMBHead::Fill \n");
  //  fflush(stdout);
  // printf(" offset %d \n",offset);
  // printf(" data: %04x %04x %04x %04x \n",data[offset+0],data[offset+1],data[offset+2],data[offset+3]);
  // printf(" zeroed \n"); 
  nfeb=0; 
  for(int ifeb=0;ifeb<5;ifeb++)if(data[offset+2]&(0x0001<<ifeb)){
    // printf(" loop %d \n",ifeb);
    nfeb++;
    use_cfeb[ifeb]=1;
  }else{
    use_cfeb[ifeb]=0; 
  }
  //printf(" nfeb %d \n",nfeb);
  //for(int i=0;i<5;i++)printf("%d",use_cfeb[i]);printf("\n");
  daqmb_id=data[offset+5]&0x001f;
  crt_id=(data[offset+5]&0xffe0)>>5;
  bxnum=(data[offset+3]&0x0fff);
  l1a=((data[offset+1]&0x0fff)<<12)|((data[offset]&0x0fff));             
  nalct=(0x400&data[offset+2])>>10;
  nclct=(0x800&data[offset+2])>>10;
  printf(" nalct %d \n",nalct);
  printf(" nclct %d \n",nclct);
  return;
 }

DMBHead::~DMBHead(){
}

}
}
