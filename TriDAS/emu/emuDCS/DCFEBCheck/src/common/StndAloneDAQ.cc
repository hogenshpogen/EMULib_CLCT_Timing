// $Id: StndAloneDAQ,v 1.2 2005/04/07 14:21:51 xdaq Exp $

/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2004, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/
#include "math.h"
#include "emu/pc/StndAloneDAQ.h"

namespace emu{
  namespace pc{

    enum Status_t {
      EndOfEventMissing = 0x0001,       ///< end of event missing                                                                                         
      Timeout           = 0x0002,       ///< timeout occurred while waiting for data                                                                      
      PacketsMissing    = 0x0004,       ///< one or more ethernet packets didn't make it                                                                  
      LoopOverwrite     = 0x0008,       ///< packet info ring buffer has bitten its own tail                                                              
      BufferOverwrite   = 0x0010,       ///< data ring buffer has bitten its own tail                                                                     
      Oversized         = 0x0020        ///< too big an event                                                                                             
    };



  StndAloneDAQ::StndAloneDAQ(int schr){	
  printf(" StndAloneDAQ receiver instatiated \n");

  unsigned char thw_dest_addr[6]={0x02,0x00,0x00,0x00,0x00,0xFF};
  unsigned char thw_source_addr[6]={0x00,0x00,0x00,0x00,0x00,0x00};
  
  for(int i=0;i<6;i++){
    hw_dest_addr[i]=thw_dest_addr[i];
    hw_source_addr[i]=thw_source_addr[i];
  }
  wbuf=ebuf+ETH_HLEN;
  if(schr==2)fpout=fopen("dataout_2.txt","w");
  if(schr==3)fpout=fopen("dataout_3.txt","w");
  if(schr==2)eth_open("schar2");
  if(schr==3)eth_open("schar3");
  eth_register_mac();
  eth_enableBlock();
}

StndAloneDAQ::~StndAloneDAQ(){
  printf(" fclose fpout \n");
   fclose(fpout);
   eth_close();
}

int StndAloneDAQ::getCFEBDataOneEvent(){
  int lengthout=eth_readmm();
  return lengthout;
} 

int StndAloneDAQ::eth_open(char *dev_name)
{
  int i;
  char *dev;
  if((dev=(char *)malloc((6+strlen(dev_name))*sizeof(char)))==0){
    printf("Device name allocation error!\n");
    return -1;
  }
  sprintf(dev,"/dev/%s",dev_name);
  fprintf(fpout,"%s\n",dev);
  fd_schar = open(dev, O_RDWR);
  if (fd_schar == -1) {
    perror("open");
    return -1;
  }
  printf("Opened network path on device %s\n",dev);
  free(dev);

  eth_reset(); 

  buf_start = (char *)mmap(NULL,BIGPHYS_PAGES_2*PAGE_SIZE,PROT_READ,MAP_SHARED,fd_schar,0);
  if (buf_start==MAP_FAILED) {
    printf("emu::daq::reader::Spy::open: FATAL in memorymap - ");
    printf("emu::daq::reader::Spy::open will abort!!!");
      abort();
    }
  printf("emu::daq::reader::Spy::open: Memory map succeeded ");

  buf_end=(BIGPHYS_PAGES_2-RING_PAGES_2)*PAGE_SIZE-MAXPACKET_2;
  buf_eend=(BIGPHYS_PAGES_2-RING_PAGES_2)*PAGE_SIZE-TAILPOS-MAXEVENT_2;
  ring_start=buf_start+(BIGPHYS_PAGES_2-RING_PAGES_2)*PAGE_SIZE;
  ring_size=(RING_PAGES_2*PAGE_SIZE-RING_ENTRY_LENGTH-TAILMEM)/RING_ENTRY_LENGTH;
  tail_start=buf_start+BIGPHYS_PAGES_2*PAGE_SIZE-TAILPOS;
  buf_pnt=0;
  ring_pnt=0;
  ring_loop=0;
  pmissing=0;
  pmissing_prev=0;

  // if(ioctl(fd_schar,SCHAR_GET_MAC,hw_source_addr){
  //  printf("Error reading MAC address\n");
  // }
  printf("NIC MAC addr.  (source): ");
  for(i=0;i<6;i++){
    printf("%02x:",hw_source_addr[i]&0xff);
  }
  printf("\n");
  return i;
}

int StndAloneDAQ::eth_register_mac()
{
  int  i,mac_idx;
  mac_idx = 0; 
  if((ether_header[mac_idx]=(struct ethhdr *)malloc(sizeof(struct ethhdr)))==0){
    printf("Could not register MAC address: Allocation error\n");
    return -1;
  }
  memcpy(ether_header[mac_idx]->h_dest,hw_dest_addr, ETH_ALEN);
  memcpy(ether_header[mac_idx]->h_source,hw_source_addr, ETH_ALEN);
  // printf("Registered Dest. MAC %2d \n ",mac_idx);
  for(i=0;i<6;i++){
    printf("%02x:",ether_header[mac_idx]->h_dest[i]&0xff);
  }
  printf("\n");
  return mac_idx;
}


int StndAloneDAQ:: eth_reset(void)
{ 
  if(ioctl(fd_schar,SCHAR_RESET)){
    printf("Error in SCHAR_RESET \n");
  }
  buf_pnt=0;
  ring_pnt=0;
  ring_loop=0;
  pmissing=0;
  pmissing_prev=0;

  return 0;
}


int StndAloneDAQ::eth_write()
{
  int pkt_size;
  int i,n_written;
  int cur_mac; 
  printf(" Creating the packet: nwbuf %d \n",nwbuf);
 //Creating the packet
  cur_mac=0;
  ether_header[cur_mac]->h_proto = htons(nwbuf);/* Length of data */
  memcpy(ebuf, ether_header[cur_mac], ETH_HLEN);
  pkt_size = ETH_HLEN + nwbuf;
  printf(" nwbuf %d \n",nwbuf);
  //  memcpy(ebuf+ETH_HLEN, wbuf, nwbuf); /*don't need this extra copy now*/
  for(i=0;i<pkt_size;i++)printf("%02X ",ebuf[i]&0xff);
  printf("\n");
  n_written = write(fd_schar, (const void *)ebuf, pkt_size);
  return n_written;
}


void StndAloneDAQ::eth_close(void)
{
  int i;
  int nmacs;
  nmacs=1;
  for(i=0;i<nmacs;i++){
    free(ether_header[i]);
  }  
  // new MemoryMapped DDU readout
  printf( "close and unmmap");
  munmap((void *)buf_start,BIGPHYS_PAGES_2*PAGE_SIZE);
  close(fd_schar);
}

void StndAloneDAQ::eth_enableBlock(void){
  int status;
 if((status=ioctl(fd_schar,SCHAR_BLOCKON))==-1){
   printf("Spy: error in enableBlock ");
 }
}

void StndAloneDAQ::eth_disableBlock(void){
  int status;
  if((status=ioctl(fd_schar,SCHAR_BLOCKOFF))==-1){
    printf("Spy: error in disableBlock ");
  }
}

void StndAloneDAQ::eth_resetAndEnable(){
   eth_reset();
   eth_enableBlock();
}

int StndAloneDAQ::eth_readmm(){
//-------------------------------------------------------------------//
//  MemoryMapped DDU2004 readout
//-------------------------------------------------------------------//
  int jloop,j;
  int length,len;
  long int iloop;
  char *buf;
  len=0;
  iloop=0;
  length=0;
  jloop=0;
  timeout=0;
  overwrite=0;
  pmissing=0;
  pmissing_prev=0;
  packets=0;
  visitCount++;
  printf(" about to loop \n");
  while (jloop!=1){
    jloop=1;
    // printf(" iloop %d \n",iloop);
    // Get the write pointer (relative to buf_start) of the kernel driver.
    buf_pnt_kern=*(unsigned long int *)(buf_start+BIGPHYS_PAGES_2*PAGE_SIZE-TAILPOS);
    // printf(" buf_pnt_kern %ld %ld %d \n",buf_pnt_kern,buf_pnt,iloop);
    if(end_event==0){
      //      for(j=0;j<100;j++)printf(" %08x ",(unsigned long int )buf_start[j]);printf("\n");
    // If no data for a long time, abort.
    if(iloop>50000){printf(" eth_readmm timeout- no data"); break;}

    // If the write pointer buf_pnt_kern hasn't yet moved from the read pointer buf_pnt, 
    // wait a bit and retry in the next loop.
    if(buf_pnt==buf_pnt_kern){for (j=0;j<5000000;j++); iloop++; continue;}

    // The kernel driver has written new data. No more idle looping. Reset the idle loop counter.
    iloop=0;

    // From the current entry of the packet info ring,...
    ring_loop_kern= *(unsigned short int *)(ring_start+ring_pnt*RING_ENTRY_LENGTH);
    // ...get the missing packet flag,...
    pmissing=ring_loop_kern&0x8000;
    // ...the end-of-event flag,...
    end_event=ring_loop_kern&0x4000;
    // ...the reset ("loop-back") counter,...
    ring_loop_kern=ring_loop_kern&0x3fff;
    // ...and the length of data in bytes.
    length=*(unsigned short int *)(ring_start+ring_pnt*RING_ENTRY_LENGTH+4);
    printf(" length %d \n",length);
    printf(" pmissing %04x end_event %04x \n",pmissing,end_event); 
    end_event=0x4000;
    pmissing=0x0000;
    // Get the reset counter from the first entry of the packet info ring...
    ring_loop_kern2=*(unsigned short int *)ring_start;
    ring_loop_kern2=0x3fff&ring_loop_kern2;

    if( ( (ring_loop_kern2==ring_loop+1)&&(buf_pnt<=buf_pnt_kern) ) ||
	(ring_loop_kern2>ring_loop+1)                                  ){
      printf(":eth_readmm buffer overwrite.");
      // Reset the read pointers.
      buf_pnt  = 0;
      ring_pnt = 0;
      // Synchronize our loop-back counter to the driver's.
      ring_loop = ring_loop_kern2;
      // Read no data this time.
      len = 0;

      break;
    }

    // The data may not have been overwritten, but the packet info ring may. 
    // Check whether the driver's loop-back count is different from ours.
    if(ring_loop_kern!=ring_loop){
      printf("eth_readmm: loop overwrite.");

      // Reset the read pointers.
      buf_pnt  = 0;
      ring_pnt = 0;
      // Synchronize our loop-back counter to the driver's.
      ring_loop = ring_loop_kern2;
      // Read no data this time.
      len = 0;
      // Let the next event start with a clean record.


      break;
    }

    // Remember the position of the start of data...
    if(packets==0){
      buf_data=buf_start+buf_pnt;
    }
    // ...and add its length to the total.
    len=len+length;

    // Increment data ring pointer...
    buf_pnt=buf_pnt+length;
    // ...and packet info ring pointer.
    ring_pnt=ring_pnt+1;
    printf(" ring_pnt %d \n",ring_pnt);
    // If this packet ends the event but another event may not fit in the remaining space (beyond buf_eend),
    // OR another packet may not fit in the remaining space (beyond buf_end),
    // OR the end of the packet info ring has been reached, 
    // then reset the read pointers (loop back) and increment the loop-back counter.
    // This condition must be exactly the same as that in the driver (eth_hook_<N>.c) for the
    // write and read pointers to loop back from the same point.
    if (((end_event==0x4000)&&(buf_pnt>buf_eend))||(buf_pnt > buf_end)||(ring_pnt>=ring_size)){
      ring_pnt=0;
      ring_loop=ring_loop+1;
      buf_pnt=0;
    }

    // Increment packet count.
    packets=packets+1;

    // Mark this event as oversized to keep a tally
    if(len>MAXEVENT_2){
      printf(" eth_readmm : event too long \n");
    }

    // If this event already has packets missing, don't read it


    // If packets are missing, don't read out anything, just keep a tally.
    if(pmissing!=0){
      pmissingCount++; 
      // Remember that we are inside a defective event until we reach the end of it.
      // (Or the end of the next event, for that matter, it the end of this one happens to be missing.)
      len = 0;
    }


  } // while (true)

    end_event=0;
  //MAINEND:
  buf=buf_data;
  for(int i=0;i<len;i++)rbuf[i]=buf_data[i];
  for(int i=0;i<len/2;i++){
    fprintf(fpout,"%04d %02x%02x\n",i,rbuf[2*i+1]&0xff,rbuf[2*i]&0xff);
  }
  fprintf(fpout," end of event \n");
  fflush(fpout);
  printf(" end of event \n");
  }
  return len;
}

int StndAloneDAQ::test_more_data()
{
  int flag=1;
  if(buf_pnt==buf_pnt_kern)flag=0;
  return 1;
}

}}
