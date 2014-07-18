/*************************************************************************
 * XDAQ Components for Distributed Data Acquisition                      *
 * Copyright (C) 2000-2004, CERN.			                 *
 * All rights reserved.                                                  *
 * Authors: J. Gutleber and L. Orsini					 *
 *                                                                       *
 * For the licensing terms see LICENSE.		                         *
 * For the list of contributors see CREDITS.   			         *
 *************************************************************************/
#include <iostream>
#include <iomanip>
#include <sstream>
#include "emu/pc/DCFEBCheck.h"
#include "emu/pc/StndAloneDAQ.h"
#include "emu/pc/SmallUnpack.h"
#include "math.h"
#include "dirent.h"

#define RAWDAT 9000

namespace emu{
  namespace pc{

    StndAloneDAQ *daq;
    StndAloneDAQ *daq2;
    SmallUnpack *unpk;
    int DataSource;
    int NSAMPLE;
    int split;
    short int rawdat[RAWDAT];
    int next_split;
    short int nextevt[RAWDAT];

    int rawadc[6][16][100];
    char thistime[150];

    char cRed[40];
    char cReset[40];

    float ped[6][16],rms[6][16];
    float ped_kill[6][16],rms_kill[6][16];
    float ped_shrt[6][16],rms_shrt[6][16];

    int dcfeb_index;
    int sonoff;
    // routine
    std::string ttst_[18];
    // comment
    std::string ctst_[18];
    // test result
    int passed_[18];
    // pass cuts
    int passcut_[18];
        
    // control
    int idone;
    
    //files
    // const std::string DCFEB_FIRMWARE_FILENAME = "feb_prom/cfeb_pro.svf";
   const std::string DCFEB_FIRMWARE_FILENAME = "dcfeb2_top_v29.mcs";
    // const std::string DCFEB_FIRMWARE_FILENAME2 = "feb_prom/cfebm3p1bx_v1_r9c.svf";
    // const std::string DCFEB_FIRMWARE_VERIFY_FILENAME2 = "feb_prom/cfeb_v10_r2_chk.svf";
    // const std::string DCFEB_FIRMWARE_TESTLINK_FILENAME = "feb_prom/cfebtest18v01.svf";
   const unsigned int EXPECTED_CFEB_USERID   = 0xcfeda092;     
   const std::string DMB_FIRMWARE_FILENAME    = "mthb_cprom/dmb6cntl_pro.svf";
   const unsigned int EXPECTED_DMB_USERID      = 0x48547231;
   const std::string DMBVME_FIRMWARE_FILENAME = "mthb_vprom/dmb6vme_pro.svf";
   const std::string SAVED_DATA_PREPATH = "boards/";
    // const std::string STORED_DATA_PATH = "config/firmware/feb_test/";

DCFEBCheck::DCFEBCheck(xdaq::ApplicationStub * s): xdaq::Application(s) 
{	
  printf(" DCFEBCheckMain instatiated \n"); 

  xgi::bind(this,&DCFEBCheck::Default, "Default");
  xgi::bind(this,&DCFEBCheck::BrdPrmpt,"BrdPrmpt");
  xgi::bind(this,&DCFEBCheck::Run,"Run");
  xgi::bind(this,&DCFEBCheck::Reset,"Reset");
  xgi::bind(this,&DCFEBCheck::Update,"Update");
  xgi::bind(this,&DCFEBCheck::NewBoard,"NewBoard");
  xgi::bind(this,&DCFEBCheck::CallBack,"CallBack");
  sprintf((char *)&cRed,"%c[1;31m",27);
  sprintf((char *)&cReset,"%c[0m",27);
  ttst_[0]="Test1 ";
  ttst_[1]="Test2 ";
  ttst_[2]="Test3 ";
  ttst_[3]="Test4 ";
  ttst_[4]="Test5 ";
  ttst_[5]="Test6 ";
  ttst_[6]="Test7 ";
  ttst_[7]="Test8 ";
  ttst_[8]="Test9 ";
  ttst_[9]="Test10";
  ttst_[10]="Test11";
  ttst_[11]="Test12";
  ttst_[12]="Test13";
  ttst_[13]="Test14";
  ttst_[14]="Test15";
  ttst_[15]="Test16";
  ttst_[16]="Test17";
  ttst_[17]="Test18";
  ctst_[0]=":Test Voltages";
  ctst_[1]=":Check JTAG and FPGA status";
  ctst_[2]=":EPROM - load and readback";
  ctst_[3]=":Comparator/Pulser DACs/ADCs";
  ctst_[4]=":Buckeye shifts";
  ctst_[5]=":Communications to Motherboard";
  ctst_[6]=":Pedestals";
  ctst_[7]=":RMSs";
  ctst_[8]=":Medium Cap Pulser Test";
  ctst_[9]=":Small Cap Pulser Test";
  ctst_[10]=":Pulse Gain Test";
  ctst_[11]=":Pulse Timing Test";
  ctst_[12]=":Pulse Shape Test";
  ctst_[13]=":Hard Reset  Test"; 
  ctst_[14]=":Comparator Direct";
  ctst_[15]=":Comparator Pulse";
  ctst_[16]=":Inputs Test";
  ctst_[17]=":Kill Test";
  curt_=time(NULL);
  loct_=localtime(&curt_);
  daq = new StndAloneDAQ(3);
  daq2 = new StndAloneDAQ(2);
  unpk=new SmallUnpack();
  DataSource=0;  // 0 -dmb  1- dcfebfpga
  NSAMPLE=8;
  for(int i=0;i<6;i++){
    for(int j=0;j<16;j++){
      ped[i][j]=-99.0;
    }
  }
  /*  for(int i=0;i<18;i++){
    passed_[i]=0;
    passcut_[i]=0;
  }
  ibrdnum_=0;
  cbrdnum_="dcfeb000";
  Finis(18); */
  split=0;
  next_split=0;
  for(int i=0;i<18;i++){
    passed_[i]=9999;
    passcut_[i]=0;
  }
  //  passcut_[1]=2;
  passcut_[3]=3;
  passcut_[7]=1;
  passcut_[12]=2;
  passcut_[13]=2;
  passcut_[14]=2;
  test7_minPedestal_ = 900.;
  test7_maxPedestal_ = 1300.;
  test8_minPedestalRms_ = 0.7;
  test8_maxPedestalRms_ = 2.5;
  test9_expectedPeak_ = 1900.;
  test9_peakVariationThresh_ = 150.; // ~4%
  // test9_xtalkThresh_ = 25.;
  // test9_chi2Thresh_=80000.;
  test10_expectedPeak_ = 980.;
  test10_peakVariationThresh_ = 75.; // ~4%
  // test10_xtalkThresh_ = 25.;
  //test10_chi2Thresh_=80000.;
  test11_expectedPeak_ = 1220.; // This is a bit higher than average but 
                                // outliers tend to be much higher than
                                // the others (not lower)
  test11_peakVariationThresh_ = 70.; // ~4%
  test11_xtalkThresh_ = 25.;
  test11_chi2Thresh_=80000.;
  test13_expectedGain_=150.;
  test13_rmsGain_=3.3;
  test13_iexpectedGain_=0.;
  test13_irmsGain_=20.0;
  test14_maxResidual_ = 12.0;
  test14_maxSumResidual_=500.;
 
  test15_maxCapVariation_ = 4.;
  test15_minCapVariation_ = -4.;
  test17_minRmsDifference_ = 3.00;
  test18_minRmsDifference_ = 0.20;

  ibrdnum_=-99;
  brdnum_="";
  prmtbox_="";
  sprintf(head_," ");
  sprintf(cmnt_," ");
  sprintf(cmntcolor_," ");  

  FirmwareDir_ = getenv("HOME");
  FirmwareDir_ += "/firmware/dcfeb/";

  evt=0;
  // open input 
  //start peripheral crate
  emuController_ = new emu::pc::EmuController();
  // emuController_ = new EmuController();
  std::string xmlFile="/home/dcfebcheck/config/pc/OSU_TestStand.xml";
  std::cout << " xmlFile " << xmlFile <<std::endl;   
  emuController_->SetConfFile(xmlFile.c_str());
  std::cout << " Mycontroller:setConfFIle " << std::endl;
  emuController_->init();
  std::cout << " Mycontroller init called " << std::endl;
  emuController_->NotInDCS();
  emuEndcap_ = emuController_->GetEmuEndcap(); 
  crateVector = emuEndcap_->crates();
  printf(" crateVector size %d \n",crateVector.size());
  
  thisCrate_ = crateVector[0];
  thisCCB_ = thisCrate_->ccb();
  odmbVector_=thisCrate_->odaqmbs();
  printf("  odmbVector_ size %d \n",odmbVector_.size());   
  tmbVector_ = thisCrate_->tmbs();
  if(odmbVector_.size()>0)thisODMB_ =odmbVector_[0];
  thisTMB_ = tmbVector_[0];
  printf(" after tmb \n");
  if(odmbVector_.size()>0){
   thisDCFEBs_=thisODMB_->dcfebs();
   printf("  cfeb vector size %d ",thisDCFEBs_.size());
  }
  printf(" after cfeb \n");
  thisCrate_->vmeController()->SetUseDCS(true);
  // thisDDMB_->start();
  fflush(stdout);  
  hexbrdnum_[0]=0xdc;
  hexbrdnum_[1]=0xfe;
  hexbrdnum_[2]=0xb0;
  hexbrdnum_[3]=0x01;
  dcfeb_index=-99;
  if(odmbVector_.size()>0){
    int i=0;
    typedef std::vector<emu::pc::DCFEB>::iterator DCFEBItr;
    for(DCFEBItr cfebItr = thisDCFEBs_.begin(); cfebItr != thisDCFEBs_.end(); ++cfebItr) {
      dcfeb_index = (*cfebItr).number();
      printf(" dcfeb number xxx %d \n",dcfeb_index);
      if(dcfeb_==dcfeb_index)ndcfeb_=i;
      i++;
    }
    printf(" ndcfeb %d \n",ndcfeb_);
    thisDCFEB_=&thisDCFEBs_[0];
  }
  dcfeb_=dcfeb_index;
  length=0;
  thisCrate_->vmeController()->init();
  printf(" before testroutines \n");
  thisODMB_->calctrl_global();
  usleep(10000);
  daq->eth_reset();
  daq2->eth_reset();
  //  thisCCB_->setCCBMode(emu::pc::CCB::VMEFPGA);
  // thisCCB_->hardReset(); // load fpga 
  // usleep(2000000);
  int xonoff[5]={0x01,0x02,0x04,0x08,0x10};
  sonoff=xonoff[dcfeb_index];
  printf(" sonoff %d \n",sonoff);
  //usleep(2); 
  thisODMB_->lowv_onoff(sonoff);
  usleep(200000);
  idone=0;
  run_=0;
  ibrdnum_=1;
  float temp=thisDCFEB_->volt2temp(thisDCFEB_->ADC(4));
  printf(" buckeye temp %f \n",temp);
  fflush(stdout);  
  thisDCFEB_->set_DAQRate(0);
  // Test4();
  // fflush(stdout);
  /*  for(int i=0;i<10;i++){
    usleep(1000000);
    // printf("usercode %08x \n",thisDCFEB_->febfpgauser());
    usleep(1000000);
    thisDCFEB_->set_DAQRate(1);
    usleep(2000000);
    thisDCFEB_->set_DAQRate(0);
    } */
  // for(int i=0;i<1000000;i++){
  //  thisODMB_->pulse(1,0xf);
  //  usleep(100000);
  //} 
  //Test3();
  /*  char wtst[10],rtst[10];
  for(int i=0;i<10;i++){wtst[i]=i;rtst[i]=0xaa;}
  thisDCFEB_->epromdirect_bpireset();
  usleep(10000);
   thisDCFEB_->epromdirect_reset();
   usleep(10000);
   thisDCFEB_->epromdirect_clearstatus();
   usleep(10000);
   printf(" after epromdirect_reset %04x \n",thisDCFEB_->epromdirect_status());
   thisDCFEB_->epromload_parameters(0,5,(unsigned short int *)wtst);
   usleep(1000000);
   thisDCFEB_->epromread_parameters(0,5,(unsigned short int *)rtst);
   for(int i=0;i<10;i++)printf(" *****rtst %d %02x \n",i,rtst[i]&0xff);
   fflush(stdout);
   return; */

  // unsigned short int valx[164];
  //for(int jj=0;jj<50;jj++)thisODMB_->epromread_block(ndcfeb_,jj);
  // for(int i=0;i<162;i++)printf("%d %04x \n",valx[i]&0xffff); 

  //  usleep(100000);
  // thisODMB_->buck_shift_test();
  

  /*  int nval=2;
  unsigned short int val[2];
  int ibrd=0x0123;
  val[0]=0xcfeb;
  val[1]=ibrd|0xd000;
  thisODMB_->epromload_parameters(thisDCFEBs_[ndcfeb_].dscamDevice(),3,nval,val); 
  int nval2=2;
  unsigned short  int val2[10];
  thisODMB_->epromread_parameters(thisDCFEBs_[ndcfeb_].dscamDevice(),3,nval2,val2);
  for(int iv=0;iv<nval2;iv++)printf(" val%d %04x ",iv,val[iv]);printf("\n");fflush(stdout); */
 
  /*  Test1();
  Test2(); 
  Test3(); 
  Test4();
  Test5(); 
  Test6(); 
  Test7to8(); 
  Test9(); 
  Test10();
  Test11to13();
  Test17();
  Test18(); 
  Test11to13(); */
  playsound(25);  
}

DCFEBCheck::~DCFEBCheck(){
  unpk->~SmallUnpack();
  daq->~StndAloneDAQ();
  daq2->~StndAloneDAQ();
}

void DCFEBCheck::Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
  printf(" entered Default: idone %d \n",idone);

  *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
  *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
  *out << cgicc::head();

             
  *out << cgicc::title("DCFEBTest Main") << std::endl;            
  *out << cgicc::head() << std::endl;
  if(run_==1){
    idone=1;
    *out << cgicc::meta().set("http-equiv","Refresh").set("content","1")<<std::endl; 
  *out << cgicc::body();
  }else{
    *out << cgicc::body();
  }
  xgi::Utils::getPageHeader
    (out, 
     "DCFEBTest Main",
     getApplicationDescriptor()->getContextDescriptor()->getURL(),
     getApplicationDescriptor()->getURN(),
     "/xgi/images/stuffed8.jpg"
     );   
  *out << cgicc::span().set("style",cmntcolor_) << cmnt_ << cgicc::span() << std::endl;
  *out << cgicc::hr() << endl;       
  *out << cgicc::table().set("boarder","1");
  *out << cgicc::tr() << cgicc::td();       
  *out << cgicc::form().set("method","GET").set("action",
						"/" + getApplicationDescriptor()->getURN() + "/BrdPrmpt") << endl;
  *out << cgicc::label().set("for","board number") << "Board Number(e.g. 105) ";
  *out << cgicc::label()  << std::endl;
  *out << cgicc::input().set("type", "textdata")
    .set("name", "BrdPrmpt").set("size","6")
    .set("id","board number").set("value",prmtbox_) << std::endl;
  *out << cgicc::input().set("type","submit").set("value","Enter") << std::endl ;
  *out << cgicc::form() << std::endl;
  *out << cgicc::td() << std::endl;
  *out << cgicc::tr() << std::endl;
  *out << cgicc::table() << std::endl;

  *out << cgicc::table();
  *out << cgicc::tr();
  *out << cgicc::td();
  *out << cgicc::form().set("method","GET").set("action",
						"/" + getApplicationDescriptor()->getURN() + "/Run") << endl;
  *out << cgicc::input().set("type","submit").set("value","Run") << std::endl ;

  *out << cgicc::form() << std::endl;
  *out << cgicc::td() << std::endl;
  *out << cgicc::td();
  *out << cgicc::form().set("method","GET").set("action",
						"/" + getApplicationDescriptor()->getURN() + "/Update") << endl;
  *out << cgicc::input().set("type","submit").set("value","Update Stats") << std::endl ;

  *out << cgicc::form() << std::endl;
  *out << cgicc::td() << std::endl;

  *out << cgicc::td(); 
  *out << cgicc::form().set("method","GET").set("action",
						"/" + getApplicationDescriptor()->getURN() + "/Reset") << endl;
  *out << cgicc::input().set("type","submit").set("value","Reset this Board")
       << std::endl ;
  *out << cgicc::form() << std::endl;
  *out << cgicc::td() << std::endl;
  *out << cgicc::td(); 
  *out << cgicc::form().set("method","GET").set("action",
						"/" + getApplicationDescriptor()->getURN() + "/NewBoard") << endl;
  *out << cgicc::input().set("type","submit").set("value","Fresh Start (New Board)")
       << std::endl ;
  *out << cgicc::form() << std::endl;
  *out << cgicc::td() << std::endl;
  *out << cgicc::tr() << std::endl;
  *out << cgicc::table() << std::endl;

  *out << cgicc::hr() << endl;
 
  printf(" *****  ibrdnum %d \n",ibrdnum_);fflush(stdout);  
  if(ibrdnum_<0){
    sprintf(cmnt_,"No Board Chosen, Enter a Board First ");
    sprintf(cmntcolor_,"color:black;background-color:Red");
    xgi::Utils::getPageFooter(*out);
    *out << cgicc::body() << std::endl;	
    *out << cgicc::html() << std::endl;

  }else{       
    *out << cgicc::table().set("boarder","1");
    idone=1;
    for(int i=0;i<18;i++){
    printf(" i %d run_ %d \n",i,run_);
    if(passed_[i]==9999&&run_==1&&idone==1){
     Call(i);
     printf(" ****** done with call %d \n",i);fflush(stdout);
     idone=0;
     if(passed_[i]>passcut_[i]||i==14){
       idone=0;
       Finis(i);
       if(i==15){
         playsound(17);
       }else{
         playsound(14);
       }
       printf(" Finis called \n");
       run_=0;
     }
    }
    *out << cgicc::tr();
    *out << cgicc::td();
    *out << cgicc::form().set("method","GET").set("action",
						    "/" + getApplicationDescriptor()->getURN() + "/CallBack") << endl;
    char buf[20];
    sprintf(buf," %04d",passed_[i]);
    std::string ctest = buf;
    float wid=(ttst_[i].size()+ctest.size()+ctst_[i].size())*0.52;
    char buf2[20];
    sprintf(buf2,";width:%4.1fem",wid);
    std::string wd=buf2;
    *out << cgicc::input().set("type","submit").set("style",bkcolor(i)+wd).set("value",ttst_[i]+ctest+ctst_[i]) << std::endl ;
    char buf3[20];
    sprintf(buf3,"%d",i);
     *out << cgicc::input().set("type","hidden").set("value",buf3).set("name","flag");
    *out << cgicc::form() << std::endl;
    *out << cgicc::td()<<std::endl;
    *out << cgicc::tr()<<std::endl;
   } 
   *out<< cgicc::table() << std::endl;
      /*   if(idone==1){
    *out << cgicc::form().set("method","GET").set("action",
						    "/" + getApplicationDescriptor()->getURN() + "/Default") << endl;
    *out << cgicc::input().set("type","onload")<< std::endl ;
    *out << cgicc::form() << std::endl; 
  } */
  xgi::Utils::getPageFooter(*out);
  *out << cgicc::body() << std::endl;	
  *out << cgicc::html() << std::endl;
  }
  printf(" exit default \n");fflush(stdout);
 }

void DCFEBCheck::CallBack(xgi::Input * in, xgi::Output * out){
  const cgicc::Cgicc cgi(in);
  int flag;
  cgicc::const_form_iterator name = cgi.getElement("flag");
  if(name !=cgi.getElements().end()){
    flag = cgi["flag"]->getIntegerValue();
  }else{
    printf(" There is a problem in Callback\n");
  }
  printf(" THE FLAG is %d \n",flag);
  if(flag==0)Test1();
  if(flag==1)Test2();
  if(flag==2)Test3();
  if(flag==3)Test4();
  if(flag==4)Test5();
  if(flag==5)Test6();
  if(flag==6)Test7to8();
  if(flag==7)Test7to8();
  if(flag==8)Test9();
  if(flag==9)Test10();
  if(flag==10)Test11to13();
  if(flag==11)Test11to13();
  if(flag==12)Test11to13();
  if(flag==13)Test14();
  if(flag==14)Test15();
  if(flag==15)Test16();
  if(flag==16)Test17();
  if(flag==17)Test18(); 
  this->Default(in,out);
}

void DCFEBCheck::Call(int flag){
  if(flag==0)Test1();
  if(flag==1)Test2();
  if(flag==2)Test3();
  if(flag==3)Test4();
  if(flag==4)Test5();
  if(flag==5)Test6();
  if(flag==6)Test7to8();
  if(flag==7)Test7to8();
  if(flag==8)Test9();
  if(flag==9)Test10();
  if(flag==10)Test11to13();
  if(flag==11)Test11to13();
  if(flag==12)Test11to13();
  if(flag==13)Test14();
  if(flag==14)Test15();
  if(flag==15)Test16(); 
}


std::string DCFEBCheck::bkcolor(int i){
  std::string ret;
  if(passed_[i]==0)ret="background-color:green";
  if(passed_[i]>0&&passed_[i]<=passcut_[i])ret="background-color:yellow";
  if(passed_[i] == 9999) ret="background-color:grey";
  if(passed_[i]>passcut_[i] && passed_[i] != 9999)ret="background-color:red";
  return ret;
} 

void DCFEBCheck::Test1(){
  printf("STARTING TEST 1\n");
  int ierr = 0;

  // first check voltages and currents 
  int I1[3][5]={{1,1,1,2,2},{1,1,1,2,2},{1,1,2,2,2}};
  int I2[3][5]={{0,3,6,1,4},{1,4,7,2,5},{2,5,0,3,6}};
  int V1[3][5]={{3,3,4,4,4},{3,3,4,4,5},{3,4,4,4,5}};
  int V2[3][5]={{3,6,1,4,7},{4,7,2,5,0},{5,0,3,6,1}};
  FILE *ftest1;
  int nvalx=2;
  unsigned short int valx[10];
  thisDCFEB_->epromread_parameters(2,nvalx,valx);
  for(int iv=0;iv<nvalx;iv++)printf(" val %d %04x ",iv,valx[iv]);printf("\n");fflush(stdout);
  if(valx[0]!=0xdcfe||(valx[1]&0xFFF)!=ibrdnum_){
    FILE *fnew;
    fnew=fopen("bad.dat","w");
    fprintf(fnew," TEST1: wrong eprom board information so quit \n");
    fprintf(fnew," board is %d, readback is %04x %04x  board is %d\n",ibrdnum_,valx[0]&0xffff,valx[1]&0xffff,(valx[1]&0xfff));
    fclose(fnew);
    system("emacs bad.dat");
    return;
  } 
  char dirnam[50];
  sprintf(dirnam,"DCFEB%03d/",ibrdnum_);
  std::string sdirnam=dirnam;
  std::string filename=SAVED_DATA_PREPATH+dirnam+"test1.dat";
 
  ftest1 = fopen(filename.c_str(),"w");
  if (ftest1 == 0) {
    printf("Open test1.dat failed. End Test1.\n");
    passed_[0] = 1;
    return;
  }
  fprintf(ftest1,"\n LVMB Readback \n");
  float I33 = thisODMB_->lowv_adc(I1[0][dcfeb_],I2[0][dcfeb_]);
  float I50 = thisODMB_->lowv_adc(I1[1][dcfeb_],I2[1][dcfeb_]);
  float I60 = thisODMB_->lowv_adc(I1[2][dcfeb_],I2[2][dcfeb_]);
  float V33 = thisODMB_->lowv_adc(V1[0][dcfeb_],V2[0][dcfeb_]);
  float V50 = thisODMB_->lowv_adc(V1[1][dcfeb_],V2[1][dcfeb_]);
  float V60 = thisODMB_->lowv_adc(V1[2][dcfeb_],V2[2][dcfeb_]);
  if(I33<2000.||I33>5000.){
    ierr++;
    fprintf(ftest1,"BAD:");
  }
  fprintf(ftest1," I3.3 %f \n",I33);
  if(I50<375.||I50>650.){
    ierr++;
    fprintf(ftest1,"BAD:");
  }
  fprintf(ftest1," I5.0 %f \n",I50);
  if(I60<1750.||I60>1950.){
    ierr++;
    fprintf(ftest1,"BAD:");
  }
  fprintf(ftest1," I6.0 %f \n",I60);
  if(V33<2900.||V33>3500.){
    ierr++;
    fprintf(ftest1,"BAD:");
  }
  fprintf(ftest1," V3.3 %f \n",V33);
  if(V50<3900.||V50>5200.){
    ierr++;
    fprintf(ftest1,"BAD:");
  }
  fprintf(ftest1," V5.0 %f \n",V50);    
  if(V60<5000.||V60>5400.){
    ierr++;
    fprintf(ftest1,"BAD:");
  }
  fprintf(ftest1," V6.0 %f \n",V60);
  float temp1=thisDCFEB_->volt2temp(thisDCFEB_->ADC(3));
  if(temp1<60.0||temp1>105.0){
    // ierr++;
    fprintf(ftest1,"BAD:");
  }
  fprintf(ftest1,"Temperature %f \n",temp1);
  float temp2=thisDCFEB_->volt2temp(thisDCFEB_->ADC(4));
  if(temp2<60.0||temp2>105.0){
    // ierr++;
    fprintf(ftest1,"BAD:");
  }
  fprintf(ftest1,"Temperature %f \n",temp2);
  if(ierr!=0){
    // fclose(ftest1);
    passed_[0]=ierr;
    printf("Test1 LVMB: Supply Voltage or Current Draw Problem !\n");
    //system("emacs /home/dcfebcheck/config/test/dcfebs/test1.dat &");
  } 
  // shift 0x11,0x33,0x77,0xff to CFEB 
  //   return bits should be 0x11,0x33,0x77,0xff
  fprintf(ftest1,"\n SIMPLE JTAG SHIFT TEST \n");
  char expect[6]={0x11,0x33,0x77,0xff,0x00};   
  char rcvbuf[6];
  thisDCFEB_->cfeb_testjtag_shift(rcvbuf);
  fprintf(ftest1," expect 0x113377ff \n got    0x%02x%02x%02x%02x \n",
	  rcvbuf[0]&0xff,rcvbuf[1]&0xff,rcvbuf[2]&0xff,rcvbuf[3]&0xff);
  printf(" Test1: expect 0x113377ff \n got   0x%02x%02x%02x%02x \n",
	 rcvbuf[0]&0xff,rcvbuf[1]&0xff,rcvbuf[2]&0xff,rcvbuf[3]&0xff);
  for(int i=0;i<4;i++){
    if(rcvbuf[i]!=expect[i]){
      ierr=100+ierr;
      fprintf(ftest1,"Test 1 Failure: simple JTAG shift returns wrong value!\n");
    }
  } 
  if(ierr!=0){
    printf("Test 1 Failure: simple JTAG shift returns wrong value!\n");
    passed_[0] = ierr;
    char command[100];
    sprintf(command,"emacs %s &",filename.c_str());
    system(command);
  }
  fprintf(ftest1,"\n SYSMON Readback Test \n");
  std::string nam[17]={"Temp","VccIn","VccExt","DV4P3_curr","DV3P2_curr","DV3P25_curr","V3PDCOMP_volt","AV55P3_curr","AV55P5_curr","V3IO_volt","V25IO_volt","V5PACOMP_volt","V5PAMP_volt","V18PADC_volt","V33PAADC_volt","V5PAA_volt","V5PSUB_volt"};
  float mean[17]={40.,1.,2.5,580.,736.,1500.,3.3,925.,910.,3.3,2.5,5.0,5.0,1.8,3.3,5.0,5.0};
  float dmean[17]={20.,0.05,0.05,150.,500.,300.,0.05,100.,100.,0.05,0.05,0.2,0.1,0.06,0.06,0.2,0.2};
  unsigned short int mean2[4]={0x0200,0x0200,0x9000,0x3000};
  std::string nam2[4]={"status","conf0","conf1","conf2"};
  unsigned short int val2[4];
  float val[17];
  thisDCFEB_->SYSMON_Info(val2,val);
  for(int i=0;i<4;i++){
    if(val2[i]!=mean2[i]){
      ierr++;
      fprintf(ftest1,"BAD:");
    }
    fprintf(ftest1,"%s: %04x\n",nam2[i].c_str(),val2[i]);
  }
  fprintf(ftest1,"\n");
  for(int i=0;i<17;i++){
    if(abs(val[i]-mean[i])>dmean[i]){
      ierr++;
      fprintf(ftest1,"BAD:");
    }
    fprintf(ftest1,"%s: %f\n",nam[i].c_str(),val[i]);
  }



  if(ierr!=0){
    char command[100];
    sprintf(command,"emacs %s &",filename.c_str());
    system(command);
  }
  fprintf(ftest1," \n Test1: number of errors: %d \n",ierr);
  passed_[0] = ierr;
  fclose(ftest1);
  // thisODMB_->lowv_onoff(0x3f);
  // usleep(200000);
}

  

void DCFEBCheck::Test2(){
  printf("STARTING TEST 2\n");
  int ierr = 0;
  FILE *ftest2;
  char dirnam[10];
  sprintf(dirnam,"DCFEB%03d/",ibrdnum_);
  std::string sdirnam=dirnam;
  std::string filename=SAVED_DATA_PREPATH+dirnam+"test2.dat";
  ftest2 = fopen(filename.c_str(),"w");
  if (ftest2 == 0) {
    printf("Open test2.dat failed. End Test 2.\n");
    passed_[1] = 1;
    return;
  }
  fflush(stdout);
  unsigned int idcode2=thisDCFEB_->febfpgaid();
  if((idcode2&0x0fffffff)!=0x0424a093){ierr=ierr+1;fprintf(ftest2,"1 JTAG FPGA communications problem %04X \n",idcode2);}
  unsigned long int usercode=thisDCFEB_->febfpgauser();
  if((usercode&0xfffff000)!=0xdcfeb000){ierr=ierr+2;;fprintf(ftest2,"2 JTAG FPGA communications problem %04x \n",usercode);}
  fprintf(ftest2," FPGAID %04x FPGAUSER %04x \n",idcode2,usercode); 
   unsigned int idcode1=thisDCFEB_->rdbkregvirtex6(VTX6_REG_IDCODE);
  fprintf(ftest2,"Virtex 6 IDCODE: %08x \n",idcode1);
  if((idcode1&0x0fffffff)!=0x0424a093){ierr=ierr+4;fprintf(ftest2,"3 JTAG FPGA communications problem %04x \n",idcode1);}  
  unsigned int status=thisDCFEB_->rdbkregvirtex6(VTX6_REG_STAT);
  fprintf(ftest2,"Virtex 6 status: %08x \n",status);
  if(status!=0x05d07efc){ierr=ierr+4;fprintf(ftest2,"4 FPGA configuration problem %04x \n",idcode1);}
  int mode=((0x0000700&status)>>8);
  fprintf(ftest2,"     mode %02x should be 0x06 \n",mode);
  int done=((0x00004000&status)>>14);
  fprintf(ftest2,"     done %d should be 1 \n",done);
  int startup_state=((0x001c0000)>>18);
  fprintf(ftest2,"     startup_state %02d should be 0x07 \n",startup_state);

  fflush(stdout);
  unsigned int regs[9];
  std::string nam[9]={"IDCODE ","STAT   ","CTL0   ","CTL1   ","COR0   ","COR1   ","TIMER  ","WBSTAR ","BOOTSTS"};
  unsigned int expect[9]={0x0424a093,0x05d07efc,0x00001001,0x00000000,0x01443fe5,0x0c010380,0x00000000,0x00000000,0x00000001};
  thisDCFEB_->regvirtex6_info(regs);
  for(int i=0;i<9;i++){
    if(i==0)regs[i]=(regs[i]&0x0fffffff);
    if(regs[i]!=expect[i]){
      ierr++;
      printf(" BAD:");
      fprintf(ftest2," BAD:");
    }
    printf(" %s: %08x \n",nam[i].c_str(),regs[i]);
    fprintf(ftest2," %s: %08x \n",nam[i].c_str(),regs[i]);
    fflush(stdout);
  }
  fprintf(ftest2," Test2 saw %d errors. \n",ierr);
  fclose(ftest2);
  if(ierr>0){
    char command[100];
    sprintf(command,"emacs %s &",filename.c_str());
    system(command);
  }
  passed_[1]=ierr;
  printf(" Test2 saw %d errors. \n",ierr);
}
 
void DCFEBCheck::Test3(){
  printf("STARTING TEST 3\n");
  int ierr = 0;
  FILE *ftest3;
  char dirnam[10];
  sprintf(dirnam,"DCFEB%03d/",ibrdnum_);
  std::string sdirnam=dirnam;
  std::string filename=SAVED_DATA_PREPATH+dirnam+"test3.dat";

  ftest3 = fopen(filename.c_str(),"w");
  if (ftest3 == 0) {
    printf("Open test3.dat failed. End Test3 \n");
    passed_[2] = 1;
    return;
  }
  /*
  std::string DCFEBFirmware = FirmwareDir_ + DCFEB_FIRMWARE_FILENAME;
  std::cout << "open file " << DCFEBFirmware << std::endl;
  DCFEBFirmware_ = DCFEBFirmware;
  thisODMB_->epromload_mcs(thisDCFEBs_[ndcfeb_].dscamDevice(),
		     DCFEBFirmware_.toString().c_str()
		     ); 
  ierr=thisODMB_->epromread_mcs(thisDCFEBs_[ndcfeb_].dscamDevice(),
				DCFEBFirmware_.toString().c_str(),ftest3
				); 
  onoff_reset(); 
  usleep(100000); */
  fprintf(ftest3,"Virtex 6 status: %08x \n",thisDCFEB_->rdbkregvirtex6(VTX6_REG_STAT));
  fprintf(ftest3,"Virtex 6 IDCODE: %08x \n",thisDCFEB_->rdbkregvirtex6(VTX6_REG_IDCODE));
  fprintf(ftest3," FPGAID %04x FPGAUSER %04x \n",
  thisDCFEB_->febfpgaid(),
  thisDCFEB_->febfpgauser());
  char wrt[16]={0x02,0x01,0x04,0x03,0x06,0x05,0x08,0x07,0x0a,0x09,0x0c,0x0b,0x0e,0x0d,0x00,0x0f};
  thisDCFEB_->make_eprom_mcs_file(wrt);
  std::string ACTIONFirmware ="/tmp/action.mcs";
  DCFEBFirmware_=ACTIONFirmware;
  thisDCFEB_->epromload_mcs(DCFEBFirmware_.toString().c_str(),0);
  ierr=thisDCFEB_->epromread_mcs(DCFEBFirmware_.toString().c_str(),ftest3,1,0);
  fprintf(ftest3," ierr is %d on 1st pass \n",ierr);
  char wrt2[16]={0xEB,0xCF,0x01,0x00,0x00,0x00,0x00,0x00,0x09,0x04,0x12,0x20,0x00,0x00,0x00,0x00};
  thisDCFEB_->make_eprom_mcs_file(wrt2);
 
  thisDCFEB_->epromload_mcs( DCFEBFirmware_.toString().c_str(),0
		     );
  ierr=ierr+thisDCFEB_->epromread_mcs(DCFEBFirmware_.toString().c_str(),ftest3,1,0
				);
  fprintf(ftest3," ierr is %d on 2nd pass \n",ierr);
 
  if(ierr>1)
    printf("Test3 Failure: FPGA does not load!\n");


  fclose(ftest3);
  if(ierr>0){
    char command[100];
    sprintf(command,"emacs %s &",filename.c_str());
    system(command);
  }
  passed_[2]=ierr;
  printf(" Test3 saw %d errors. \n",passed_[2]);
}

void DCFEBCheck::Test4(){
  printf("STARTING TEST 4\n");
  int ierr = 0; 
  int cbits[3]={0,0,0};
  //onoff_reset();
  usleep(100000);
  FILE *ftest4;
  char dirnam[10];
  sprintf(dirnam,"DCFEB%03d/",ibrdnum_);
  std::string sdirnam=dirnam;
  std::string filename=SAVED_DATA_PREPATH+dirnam+"test4.dat";
  ftest4 = fopen(filename.c_str(),"w");
  if (ftest4 == 0) {
    ierr++;
    printf("Open test4.dat failed. End Test 4.\n");
    passed_[3] = ierr;
    return;
  }
  float val,offset;
  thisDCFEB_->set_comp_thresh(0.0);
  usleep(50000);
  int ival=thisDCFEB_->ADC(0);
  fprintf(ftest4," 3.5 value %d \n",ival);
  float val2=ival;
  val2=val2/1000.;
  float val5=val2;
  float set[25],comp[25],pul1[25],pul2[25];
  for(int i=0;i<25;i++){
    // first check comp dac and adc
    offset=3.5-val5;
    val=i*0.16;
    thisDCFEB_->set_comp_thresh_prec(val,val5);
    usleep(50000);
    ival=thisDCFEB_->ADC(0);
    val2=ival;
    val2=val2/1000.;
    // second ceck calib dac and adc
    thisDCFEB_->set_cal_dac(val,val);
    usleep(50000);
    float val3=thisDCFEB_->ADC(1)/1000.;
    float val4=thisDCFEB_->ADC(2)/1000.;
    printf(" set: %f comp: %f pulsers: %f %f  \n",val,3.5-val2-offset,val3,val4,offset);
    set[i]=val;
    comp[i]=3.5-val2-offset;
    pul1[i]=val3;
    pul2[i]=val4; 
  }
  fprintf(ftest4," Pulser DAC/ADC \n");
  fprintf(ftest4," # set pul1 pul2 \n");
  for(int i=0;i<25;i++){
    fprintf(ftest4,"%d %f ",i,set[i]);
    float test=pul1[i]-set[i];if(test<0.0)test=-1.0*test;
    if(test>0.0065)fprintf(ftest4,"bad:");
    fprintf(ftest4,"%f ",pul1[i]);
    test=pul2[i]-set[i];if(test<0.0)test=-1.0*test;
    if(test>0.0065){fprintf(ftest4,"bad:");ierr++;}
    fprintf(ftest4,"%f\n ",pul2[i]); 
  }
  fprintf(ftest4,"\n");
  fprintf(ftest4," Comparator DAC/ADC \n");
  fprintf(ftest4," # set comp \n");
  float derr=0.005;
  for(int i=0;i<25;i++){
    if(i>10&&i<14)derr=0.008;
    if(i>=14)derr=0.015;
    if(set[i]>3.5){set[i]=3.5;derr=0.04;}
    fprintf(ftest4,"%d %f ",i,set[i]);
    float test=comp[i]-set[i];if(test<0.0)test=-1.0*test;
    if(test>derr){fprintf(ftest4,"bad:");ierr++;}
    fprintf(ftest4,"%f\n",comp[i]); 
  }
  fflush(stdout);
  fclose(ftest4);
  if(ierr != 0)
    printf("Test4: DAC/ADC problems %d \n",ierr);
  if(ierr>0){
    char command[100];
    sprintf(command,"emacs %s &",filename.c_str());
    system(command);
  }
  passed_[3]=ierr;
  printf(" Test4 saw %d errors. \n",ierr);
}

void DCFEBCheck::Test5(){
  printf("STARTING TEST 5\n");
  int ierr = 0;
  // onoff_reset();
  usleep(10000);
  int trys=0;
 AGAIN:
    std::ofstream otest5;
    char dirnam[10];
    sprintf(dirnam,"DCFEB%03d/",ibrdnum_);
    std::string sdirnam=dirnam;
    std::string filename=SAVED_DATA_PREPATH+dirnam+"test5.dat";

    otest5.open(filename.c_str());
    if (otest5 == 0) {
      printf("Open test5.dat failed. End Test 5.\n");
      passed_[4] = 1;
      return;
    }
    int pass=thisODMB_->buck_shift_test();
    if(pass==0){ierr=6;otest5<<"Failed"<<std::endl;}
    if(pass==1)otest5<<"Passed"<<std::endl;
  // otest5.close();  
  // thisODMB_->RedirectOutput(&std::cout);  
  // read back output and calculate error returns
  //ierr=check_test5();
  if(ierr!=0&&trys<4){trys=trys+1;goto AGAIN;}
  if(ierr!=0)printf("Test5: Buckeye Shifts Return Wrong Value!\n");
  passed_[4]=ierr;
  if(ierr>0){
    char command[100];
    sprintf(command,"emacs %s &",filename.c_str());
    system(command);
  }
  printf(" Test5 saw %d errors. \n",passed_[4]);
}

void DCFEBCheck::Test6(){
  printf("STARTING TEST 6\n");
  int p[5][2]={{0x00000013,0x0000ffc0},{0x00000010,0x00000000},
   {0x00000012,0x0000aa80},{0x00000011,0x00005540},{0x00000011,0x00001cc0}};
  short int expect[5]={0x0fff,0x0000,0x0aaa,0x0555,0x0473};
  int ierr = 0;
  FILE *ftest6;
  char dirnam[10];
  sprintf(dirnam,"DCFEB%03d/",ibrdnum_);
  std::string sdirnam=dirnam;
  std::string filename=SAVED_DATA_PREPATH+dirnam+"test6.dat";
  ftest6 = fopen(filename.c_str(),"w");
  if (ftest6 == 0) {
    printf("Open test6.dat failed. End Test 6.\n");
    passed_[5] = 1;
    return;
  }
  thisODMB_->calctrl_global();
  usleep(20000);
  setupPulseDelays(sonoff,0);
  if(dcfeb_index>-1){
    thisDCFEB_->Set_PipelineDepth(60);
    thisDCFEB_->Pipeline_Restart();
    usleep(50000);
  }
  thisDCFEB_->set_DAQRate(0);
  usleep(100000);
  thisDCFEB_->Set_NSAMPLE(NSAMPLE);
  int totbad=0;
  for(int trial=0;trial<5;trial++){
  int digivolt=0;
  float vpulse=3.10001*digivolt/255.;
  vpulse=0.0;
  thisODMB_->set_dac(0.0,vpulse);
  thisODMB_->buck_shift(); 
  usleep(500);
  injtime_=10; 
  thisODMB_->set_cal_tim_pulse(injtime_-dinjtime_); // set pulse time
  int shift;
  for(int chip=0;chip<6;chip++){
    for(int cmd=0;cmd<2;cmd++){
      if(cmd==0){
        shift=0x00250000|p[trial][0]|((chip<<24)&0xff000000);
	thisDCFEB_->FADC_ShiftData(shift);
      }
      if(cmd==1){
        shift=0x00260000|p[trial][1]|((chip<<24)&0xff000000);
        thisDCFEB_->FADC_ShiftData(shift);
      }
      printf("SHIFTIN %08x  pattern %08x \n",shift&0xffffffff,p[trial][cmd]&0xffffffff);
    }
  }
  thisDCFEB_->FADC_Initialize();
  usleep(10000);
  vpulse=0.0;
  thisODMB_->set_dac(0.0,vpulse);
  thisODMB_->toggle_pedestal(); // pedestal
  thisODMB_->pulse(1,0xf);  // pedestal
  thisODMB_->toggle_pedestal();   // pedestal
  usleep(10000);
 
  if(DataSource==0){
    int length=daq->getCFEBDataOneEvent();
    printf(" exited getCFEBDataOneEvent %d \n",length);
    if(length<10){
	printf("*Bad -DAQMB returned only %d bytes \n",length/2);
	fprintf(ftest6,"*Bad -DAQMB returned only %d bytes \n",length/2);
    }
    if(length>10){
      OpticalUnpack();
    }

    for(int chip=0;chip<6;chip++){
      for(int chan=0;chan<16;chan++){
        fprintf(ftest6,"chip %d chan %02d :",chip,chan);
        int badval=0;
        for(int samp=0;samp<NSAMPLE;samp++){
           fprintf(ftest6," %03x",rawadc[chip][chan][samp]&0x0fff);
           if(expect[trial]!=rawadc[chip][chan][samp]&0x0fff){
             badval++;
             totbad++;
           }      
        }
        if(badval>0)fprintf(ftest6,"<-BAD");
        fprintf(ftest6,"\n");
      }
    }
  }
  }
  ierr=totbad;
  onoff_reset();
  usleep(50000);
  fclose(ftest6);
  if(ierr>0){
    char command[100];
    sprintf(command,"emacs %s &",filename.c_str());
    system(command);
  }
  passed_[5]=ierr;
  printf(" Test6 saw %d errors. \n",passed_[5]);
}

void DCFEBCheck::Test7to8(){
  int ierr=0;
  FILE *fp;
  char dirnam[10];
  sprintf(dirnam,"DCFEB%03d/",ibrdnum_);
  std::string sdirnam=dirnam;
  std::string filename=SAVED_DATA_PREPATH+dirnam+"test7to8.dat";
  fp=fopen(filename.c_str(),"w");
  if (fp == 0) {
    printf("Test7to8: Open peds7to8.dat failed. Pedestals not recorded.\n");
    ierr++;
    passed_[6] = ierr;
    passed_[7] = ierr;
  }

  // Set thresholds for pedestals and RMS and test
  printf("STARTING TEST 7 to 8\n");
  float n[6][16],x[6][16],x2[6][16];
  char dout[100];
  for(int chp=0;chp<6;chp++){
    for(int chn=0;chn<16;chn++){
      n[chp][chn]=0.0;
      x[chp][chn]=0.0;
      x2[chp][chn]=0.0;
      ped[chp][chn]=-99.;
      rms[chp][chn]=-99.;
    }
  }
  sprintf(dout,"/home/dcfebcheck/data/pedestals%s.dat",timeheader());
  printf(" %s \n",dout);
  dataout=fopen(dout,"w");
    
  thisODMB_->calctrl_global();
  usleep(50000);
  setupPulseDelays(sonoff,0);
  usleep(50000);
  daq->eth_reset();
  NSAMPLE=8;
  thisDCFEB_->Set_NSAMPLE(NSAMPLE);
  thisDCFEB_->Set_PipelineDepth(60);
  thisDCFEB_->Pipeline_Restart();
  usleep(50000);
  thisDCFEB_->set_DAQRate(0);
  usleep(100000);

  injtime_=13;    // in other tests, this is 15 
  dinjtime_=2;    // in 6.25 nsec steps
  evt=0;                                                        
  for(int k=0;k<1;k++){
    //for(int k=0; k<22; k++){
    //while(1){                                                                            
    int k=18;
    int digivolt=12*k+2;
    float vpulse=3.10001*digivolt/255.;
    vpulse=0.0;
    thisDCFEB_->set_cal_dac(0.0,0.0);
    thisODMB_->shift_all(NORM_RUN);  //pedestal
    //thisODMB_->shift_all(KILL_CHAN);  //pedestal 
    thisODMB_->buck_shift(); 
    injtime_=10; 
    thisODMB_->set_cal_tim_pulse(injtime_-dinjtime_); // set pulse time
    usleep(50000);
    for(int i=0;i<10;i++){
      thisODMB_->toggle_pedestal(); // pedestal
      thisODMB_->pulse(1,0xf);  // pedestal
      thisODMB_->toggle_pedestal();   // pedestal
      usleep(50000);
      evt++;
      //    int length=daq->getDCFEBDataOneEvent();
      int length=daq->getCFEBDataOneEvent();
      printf(" exited getDCFEBDataOneEvent %d \n",length);
      if(length<10){
        printf("*Bad -DAQMB returned only %d bytes \n",length/2);
        fprintf(fp,"*Bad -DAQMB returned only %d bytes \n",length/2);
      }
      if(length>10){
	OpticalUnpack();
      }
      // int NSAMPLE=thisODMB_->getNSample();
      if(length>10){
      for(int chip=0;chip<6;chip++){
        for(int chan=0;chan<16;chan++){
          for(int samp=0;samp<NSAMPLE;samp++){
            fprintf(dataout," %d %d %d %d %d %d %d \n",evt,k,i,chip,chan,samp,rawadc[chip][chan][samp]&0x0fff);
            n[chip][chan]=n[chip][chan]+1.0;;
            float xx=rawadc[chip][chan][samp]&0x0fff;
            x[chip][chan]=x[chip][chan]+xx;
            x2[chip][chan]=x2[chip][chan]+xx*xx;
            if(chip==0&&chan==0)printf("xx %f n %f x %f x2 %f \n",xx,n[chip][chan],x[chip][chan],x2[chip][chan]); 
          }
        }
      }
      }
      fflush(dataout);                                                                   
    }
  }
  fprintf(dataout," -1 -1 -1 -1 -1 -1 \n");
  fclose(dataout);
  for(int chp=0;chp<6;chp++){
    for(int chn=0;chn<16;chn++){
      if(n[chp][chn]>0.0){
        ped[chp][chn]=x[chp][chn]/n[chp][chn];
        rms[chp][chn]=sqrt(x2[chp][chn]/n[chp][chn]-ped[chp][chn]*ped[chp][chn]);
      }
    }
  }


  int ierr7=0;
  int ierr8=0;
  fprintf(fp," Bad Channels \n");
  for(int chp=0;chp<6;chp++){
    for(int chn=0;chn<16;chn++){
      int terr7=0;
      int terr8=0;
      if(ped[chp][chn]>test7_maxPedestal_||ped[chp][chn]<test7_minPedestal_){terr7=1;ierr7++;}
      if(rms[chp][chn]>test8_maxPedestalRms_||rms[chp][chn]<test8_minPedestalRms_){terr8=1;ierr8++;}
      if(terr7>-1||terr8>-1){
        fprintf(fp," %d %d",chp,chn); 
        if(terr7==0){
          fprintf(fp," %f",ped[chp][chn]);
        }else{
          fprintf(fp," -->%f",ped[chp][chn]);
        }
        if(terr8==0){
          fprintf(fp," %f",rms[chp][chn]);
        }else{
          fprintf(fp," -->%f",rms[chp][chn]);
        }
        fprintf(fp,"\n");
      }
    }
  }
  passed_[6] = ierr + ierr7;
  printf(" Test7 saw %d errors. \n",passed_[6]);
  passed_[7] = ierr + ierr8;
  printf(" Test8 saw %d errors. \n",passed_[7]);
  // if errors show plots
  if(ierr7+ierr8>0){
    char scmd[200];
    sprintf(scmd,"ln -s %s /home/dcfebcheck/data/data.dat ",dout);
    printf(" scmd %s \n",scmd);
    system("rm -f /home/dcfebcheck/data/data.dat");
    system(scmd);
    system("/home/dcfebcheck/RootAnalCheck/scripts/pedestalscript ");
    system("/home/dcfebcheck/RootAnalCheck/scripts/pedestalanalysis ");
    char command[100];
    sprintf(command,"emacs %s &",filename.c_str());
    system(command);
    system("/home/dcfebcheck/RootAnalCheck/scripts/pedestalshow ");
  }
  fclose(fp);
}

void DCFEBCheck::Test9(){
  printf("STARTING TEST 9\n");
  int ierr = 0;
  FILE *fp;
  char dirnam[10];
  sprintf(dirnam,"DCFEB%03d/",ibrdnum_);
  std::string sdirnam=dirnam;
  std::string filename=SAVED_DATA_PREPATH+dirnam+"test9.dat";
  fp=fopen(filename.c_str(),"w");
  if (fp == 0) {
    printf("Test9: Open test9.dat failed. Test output will not be recorded.\n");
  } 
  char dout[60];
  sprintf(dout,"/home/dcfebcheck/data/mediumcap%s.dat",timeheader());
  printf(" %s \n",dout);
  dataout=fopen(dout,"w");
  float temp=thisDCFEB_->volt2temp(thisDCFEB_->ADC(4));
  fprintf(fp," temp %f \n",temp);  
  thisODMB_->calctrl_global();
  usleep(50000);
  setupPulseDelays(sonoff,0);
  usleep(50000);
  //daq->eth_reset();
  NSAMPLE=8;
  thisDCFEB_->Set_NSAMPLE(NSAMPLE);
  thisDCFEB_->Set_PipelineDepth(61);
  thisDCFEB_->Pipeline_Restart();
  usleep(50000);
  thisDCFEB_->set_DAQRate(0);
  usleep(100000);
  evt=0;
  injtime_=9;
  thisODMB_->shift_all(MEDIUM_CAP); 
  thisODMB_->buck_shift(); // shift register fill
  thisDCFEB_->set_cal_dac(2.5,2.5); // set DAC pulse height
  int val[8][96][8];
  float tval[8][96][8];
  for(int i=0;i<8;i++){
    daq->eth_reset();
    usleep(50000);
    int inj=injtime_+i;
    thisODMB_->set_cal_tim_inject(inj);
    usleep(200000); 
    thisODMB_->inject(1,0x01);
    usleep(200000);
 
    int length=daq->getCFEBDataOneEvent();
    printf(" exited getDCFEBDataOneEvent %d \n",length);
    fflush(stdout);
    if(length<10){
      printf("*Bad -DAQMB returned only %d bytes \n",length/2);
      fprintf(fp,"*Bad -DAQMB returned only %d bytes \n",length/2);
    }
    if(length>10){
      OpticalUnpack();
    }
    if(length>10){
      int ii=0;
      int k=0;
      for(int chip=0;chip<6;chip++){
        for(int chan=0;chan<16;chan++){
          for(int samp=0;samp<NSAMPLE;samp++){
            float tsamp=samp+(7-i)*0.125;
	    //      fprintf(dataout," %d %d %d %d %d %d %d \n",evt,k,ii,chip,chan,samp,rawadc[chip][chan][samp]&0x0fff);
            tval[i][chip*16+chan][samp]=tsamp;
            val[i][chip*16+chan][samp]=rawadc[chip][chan][samp]&0x0fff;
          }
        }
      }
      //  fflush(dataout);                                  
    }
  }
  //  fprintf(dataout," -1 -1 -1 -1 -1 -1 \n");
  // fclose(dataout);
  float mtime[96],mchg[96];
  float max,qqmax,ttmax; 
  for(int chn=0;chn<96;chn++){
    printf(" chn %d \n",chn);
    max=0;
    ttmax=0;
    qqmax=-999;
    for(int j=0;j<8;j++){
    float ped=(val[j][chn][0]+val[j][chn][1])/2;
    printf(" pedestal %d %d %f\n",chn,j,ped);
    for(int smp=0;smp<8;smp++){
      if(val[j][chn][smp]-ped>qqmax){
        ttmax=j;
        max=smp;
        qqmax=val[j][chn][smp]-ped;
        mchg[chn]=qqmax;
        mtime[chn]=tval[j][chn][smp];
        printf(" chn %d %f %f \n",chn,mchg[chn],mtime[chn]);
      }
    }
    }

  }

  float qave=0.0;
  float tave=0.0;
  float nave=0.0;
  float tmax=-9999.;
  float tmin=9999.;
  float qmax=-9999.;
  float qmin=9999.;
  for(int chp=0;chp<6;chp++){
    for(int chn=0;chn<16;chn++){
      float qpeak=mchg[16*chp+chn];
      float tpeak=mtime[16*chp+chn];
      nave=nave+1;
      tave=tave+tpeak;
      qave=qave+qpeak;
      if(tpeak>tmax)tmax=tpeak;
      if(tpeak<tmin)tmin=tpeak;
      if(qpeak>qmax)qmax=qpeak;
      if(qpeak<qmin)qmin=qpeak;
    }
  }
  tave=tave/nave;
  qave=qave/nave;
  FILE *fpout;
  fpout=fopen("/home/dcfebcheck/data/standards/test9_averages.txt","a");
  fprintf(fpout,"%s  %f %f %f %f %f %f %f \n",timeheader(),temp,tave,tmin,tmax,qave,qmin,qmax);
  fclose(fpout);

  fprintf(fp," chip channel ptime peak \n");
  for(int chp=0;chp<6;chp++){
    for(int chn=0;chn<16;chn++){
      float qpeak=mchg[16*chp+chn];
      float tpeak=mtime[16*chp+chn];
      fprintf(dataout," %d %d %f %f ",chp,chn,mtime[16*chp+chn],mchg[16*chp+chn]);
      fprintf(fp," %d %d %f %f ",chp,chn,mtime[16*chp+chn],mchg[16*chp+chn]);
      if(tpeak-4.5>1.0){
        fprintf(fp,"tp too large;");
        ierr++;
      }
      if(tpeak-4.5<-1.0){
        fprintf(fp,"tp too small;");
        ierr++;
      }
      if(qpeak-test9_expectedPeak_>test9_peakVariationThresh_){
        fprintf(fp,"qp too large;");
        ierr++;
      }
      if(qpeak-test9_expectedPeak_<-1*test9_peakVariationThresh_){
        fprintf(fp,"qp too small;");
        ierr++;
      }
      fprintf(fp,"\n");
      fprintf(dataout,"\n");
    }
  } 
  fclose(dataout);
  if(ierr>0){
    char scmd[200];
    sprintf(scmd,"ln -s %s /home/dcfebcheck/data/data.dat ",dout);
    printf(" scmd %s \n",scmd);
    system("rm -f /home/dcfebcheck/data/data.dat");
    system(scmd);
    system("/home/dcfebcheck/RootAnalCheck/scripts/mediumcapscript ");
    system("/home/dcfebcheck/RootAnalCheck/scripts/mediumcapanalysis ");
    char command[100];
    sprintf(command,"emacs %s &",filename.c_str());
    system(command);
    system("/home/dcfebcheck/RootAnalCheck/scripts/mediumcapshow ");
  }

  if (fclose(fp)) printf("Close test9.dat failed.\n");
  passed_[8]=ierr;
  printf(" Test9 saw %d errors. \n",ierr);
}

void DCFEBCheck::Test10(){
  printf("STARTING TEST 10\n");
 
  FILE *fp;
  char dirnam[10];
  sprintf(dirnam,"DCFEB%03d/",ibrdnum_);
  std::string sdirnam=dirnam;
  std::string filename=SAVED_DATA_PREPATH+dirnam+"test10.dat";
  fp=fopen(filename.c_str(),"w");
  if (fp == 0) {
    printf("Test10: Open test10.dat failed. Test output will not be recorded.\n");
  }
  float temp=thisDCFEB_->volt2temp(thisDCFEB_->ADC(4));
  fprintf(fp," temp %f \n",temp);  
  int ierr=0;
  char dout[60];
  sprintf(dout,"/home/dcfebcheck/data/smallcap%s.dat",timeheader());
  printf(" %s \n",dout);
  dataout=fopen(dout,"w");
    
  thisODMB_->calctrl_global();
  usleep(50000);
  setupPulseDelays(sonoff,0);
  usleep(50000);
  daq->eth_reset();
  NSAMPLE=8;
  thisDCFEB_->Set_NSAMPLE(NSAMPLE);
  thisDCFEB_->Set_PipelineDepth(61);
  thisDCFEB_->Pipeline_Restart();
  usleep(50000);
  thisDCFEB_->set_DAQRate(0);
  usleep(100000);
  evt=0;
  injtime_=9;
  thisODMB_->shift_all(SMALL_CAP); 
  thisODMB_->buck_shift(); // shift register fill
  thisDCFEB_->set_cal_dac(2.5,2.5); // set DAC pulse height
  int val[8][96][8];
  float tval[8][96][8];
  for(int i=0;i<8;i++){
    daq->eth_reset();
    usleep(50000);
    int inj=injtime_+i;
    thisODMB_->set_cal_tim_inject(inj);
    usleep(200000); 
    thisODMB_->inject(1,0x01);
    usleep(200000);

    int length=daq->getCFEBDataOneEvent();
    printf(" exited getDCFEBDataOneEvent %d \n",length);
    fflush(stdout);
    if(length<10){
      printf("*Bad -DAQMB returned only %d bytes \n",length/2);
      fprintf(fp,"*Bad -DAQMB returned only %d bytes \n",length/2);
    }
    if(length>10){
      OpticalUnpack();
    }
    // int NSAMPLE=thisODMB_->getNSample();
    if(length>10){
      int ii=0;
      int k=0;
      for(int chip=0;chip<6;chip++){
        for(int chan=0;chan<16;chan++){
          for(int samp=0;samp<NSAMPLE;samp++){
            float tsamp=samp+(7-i)*0.125;
	    //  fprintf(dataout," %d %d %d %d %d %d %d \n",evt,k,i,chip,chan,samp,rawadc[chip][chan][samp]&0x0fff);
            tval[i][chip*16+chan][samp]=tsamp;
            val[i][chip*16+chan][samp]=rawadc[chip][chan][samp]&0x0fff;
          }
        }
      }
      //fflush(dataout);                                  
    }
  }
  //fprintf(dataout," -1 -1 -1 -1 -1 -1 \n");
  // fclose(dataout);
  float mtime[96],mchg[96];
  int max,qqmax,ttmax; 
  for(int chn=0;chn<96;chn++){
    printf(" chn %d \n",chn);
    max=0;
    ttmax=0;
    qqmax=-999;
    for(int j=0;j<8;j++){
    for(int smp=0;smp<8;smp++){
      float ped=(val[j][chn][0]+val[j][chn][1])/2;
      if(val[j][chn][smp]-ped>qqmax){
        ttmax=j;
        max=smp;
        qqmax=val[j][chn][smp]-ped;
        mchg[chn]=qqmax;
        mtime[chn]=tval[j][chn][smp];
        printf(" chn %d %f %f \n",mchg[chn],mtime[chn]);
      }
    }
    }
  }

  float qave=0.0;
  float tave=0.0;
  float nave=0.0;
  float tmax=-9999.;
  float tmin=9999.;
  float qmax=-9999.;
  float qmin=9999.;
  for(int chp=0;chp<6;chp++){
    for(int chn=0;chn<16;chn++){
      float qpeak=mchg[16*chp+chn];
      float tpeak=mtime[16*chp+chn];
      nave=nave+1;
      tave=tave+tpeak;
      qave=qave+qpeak;
      if(tpeak>tmax)tmax=tpeak;
      if(tpeak<tmin)tmin=tpeak;
      if(qpeak>qmax)qmax=qpeak;
      if(qpeak<qmin)qmin=qpeak;
    }
  }
  tave=tave/nave;
  qave=qave/nave;
  FILE *fpout;
  fpout=fopen("/home/dcfebcheck/data/standards/test10_averages.txt","a");
  fprintf(fpout,"%s %f %f %f %f %f %f %f \n",timeheader(),temp,tave,tmin,tmax,qave,qmin,qmax);
  fclose(fpout);


  fprintf(fp," chip channel ptime peak \n");
  for(int chp=0;chp<6;chp++){
    for(int chn=0;chn<16;chn++){
      float qpeak=mchg[16*chp+chn];
      float tpeak=mtime[16*chp+chn];
      fprintf(dataout," %d %d %f %f ",chp,chn,mtime[16*chp+chn],mchg[16*chp+chn]);
      fprintf(fp," %d %d %f %f ",chp,chn,mtime[16*chp+chn],mchg[16*chp+chn]);
      if(tpeak-4.5>1.0){
        fprintf(fp,"tp too large;");
        ierr++;
      }
      if(tpeak-4.5<-1.0){
        fprintf(fp,"tp too small;");
        ierr++;
      }
      if(qpeak-test10_expectedPeak_>test10_peakVariationThresh_){
        fprintf(fp,"qp too large;");
        ierr++;
      }
      if(qpeak-test10_expectedPeak_<-1*test10_peakVariationThresh_){
        fprintf(fp,"qp too small;");
        ierr++;
      }
      fprintf(fp,"\n");
      fprintf(dataout,"\n");
    }
  }
  fclose(dataout);
  if(ierr>0){
    char scmd[200];
    sprintf(scmd,"ln -s %s /home/dcfebcheck/data/data.dat ",dout);
    printf(" scmd %s \n",scmd);
    system("rm -f /home/dcfebcheck/data/data.dat");
    system(scmd);
    system("/home/dcfebcheck/RootAnalCheck/scripts/smallcapscript ");
    system("/home/dcfebcheck/RootAnalCheck/scripts/smallcapanalysis ");
    char command[100];
    sprintf(command,"emacs %s &",filename.c_str());
    system(command);
    system("/home/dcfebcheck/RootAnalCheck/scripts/smallcapshow ");
  }

  if (fclose(fp)) printf("Close test10.dat failed.\n");
  passed_[9]=ierr;
  printf(" Test10 saw %d errors. \n",ierr);
}

void DCFEBCheck::Test11to13(){
  // unpack data into local arrays
  printf("STARTING TEST 11 to 14\n");
  int ierr = 0;

   FILE *fp11;;
  char dirnam[10];
  sprintf(dirnam,"DCFEB%03d/",ibrdnum_);
  std::string sdirnam=dirnam;
  std::string filename=SAVED_DATA_PREPATH+dirnam+"test11to13.dat";
  fp11=fopen(filename.c_str(),"w");
  if (fp11 == 0) {
    printf("Test11: Open test11.dat failed. Test output will not be recorded.\n");
  }
    char dout2[100];
    sprintf(dout2,"/home/dcfebcheck/data/temperature%s.dat",timeheader());
    char dcpy2[100];
    sprintf(dout2,"/home/dcfebcheck/boards/%stemperature%s.dat",dirnam,timeheader());
    FILE *ftemp;
    ftemp=fopen(dout2,"w");
    // float thermx2=thisODMB_->readthermx(dcfeb_+1);
    float thermx2=thisDCFEB_->volt2temp(thisDCFEB_->ADC(4));
    fprintf(ftemp,"%f5.2\n",thermx2);
    fclose(ftemp); 
    char ddo[200];
    sprintf(ddo,"cp %s %s",dout2,dcpy2);
    system(ddo); 
    char dout[100];
    sprintf(dout,"/home/dcfebcheck/data/pulsefine%s.dat",timeheader());
    printf(" %s \n",dout);
    char dcpy[100];
    sprintf(dcpy,"/home/dcfebcheck/boards/%spulsefine%s.dat",dirnam,timeheader());
    dataout=fopen(dout,"w");
    setupPulseDelays(sonoff,0);
    usleep(500000);
    daq->eth_reset();
    NSAMPLE=8;
    thisDCFEB_->Set_NSAMPLE(NSAMPLE);
    thisDCFEB_->Set_PipelineDepth(61);
    thisDCFEB_->Pipeline_Restart();
    usleep(50000);
    thisDCFEB_->set_DAQRate(0);
    usleep(100000);
    /*
    thisODMB_->calctrl_global();
    usleep(1000000);
   thisCCB_->setCCBMode(CCB::VMEFPGA);
    thisCCB_->syncReset();
    thisCCB_->setCCBMode(CCB::DLOG);
    usleep(1000000); */

    int injtime_=13;    // in other tests this is 15
    evt=0;
    //    int dinjtime_=2;    // in 6.25 nsec steps
    // for(int k=0;k<1;k++){
    for(int k=0; k<22; k++){
    // int k=1;
    // while(1){
      int kk=18;
      int digivolt=12*kk+2;
      float vpulse=3.10001*digivolt/255.;
      thisDCFEB_->set_cal_dac(vpulse,vpulse);
      usleep(10000);
      for(int i=0;i<16;i++){
	printf("i = %d \n",i);
	//while(1){
	// int j=8;
	  thisODMB_->set_ext_chanx(i);  //pulser
	  thisODMB_->buck_shift();      //pulser
	  injtime_=k;
	  thisODMB_->set_cal_tim_pulse(injtime_); // set pulse time
          usleep(50000);
	  thisODMB_->pulse(1,0x01); //pulse
	  usleep(50000);
	  evt++;
          printf("Call getDFEBData \n");
          fflush(stdout); 
	  int length=daq->getCFEBDataOneEvent();
	  printf(" exited getDCFEBDataOneEvent %d \n",length);
          fflush(stdout);
	  if(length<10){
	    printf("*Bad -DAQMB returned only %d bytes \n",length/2);
            fflush(stdout);
	    fprintf(fp11,"*Bad -DAQMB returned only %d bytes \n",length/2);
	    ierr++;
	    passed_[10] = ierr;
	    passed_[11] = ierr;
	    passed_[12] = ierr;
          }
	  if(length>10){
	    OpticalUnpack();
            printf(" end optical unpack \n");
            fflush(stdout);
	  }
	  //  int NSAMPLE=thisODMB_->getNSample();
          if(length>10){
 	  for(int chip=0;chip<6;chip++){
	    for(int chan=0;chan<16;chan++){
	      for(int samp=0;samp<NSAMPLE;samp++){
		if(i==chan)fprintf(dataout," %d %d %d %d %d %d %d \n",evt,k,i,chip,chan,samp,rawadc[chip][chan][samp]&0x0fff);
	      }
	    }
	  }
          }
	  fflush(dataout);
	}
     }

     fprintf(dataout," -1 -1 -1 -1 -1 -1 \n");
     fclose(dataout);
     sprintf(ddo,"cp %s %s",dout,dcpy);
     system(ddo);
     thisDCFEB_->Set_NSAMPLE(8);
     NSAMPLE=8;

     char scmd[200];
     sprintf(scmd,"ln -s %s /home/dcfebcheck/data/data.dat",dout);
     ierr=system("rm -f /home/dcfebcheck/data/data.dat");
     ierr=system("rm -f /home/dcfebcheck/data/temperature.dat");
     printf(" removed data.dat ierr=%d \n",ierr);
     printf(" %s end \n",scmd); 
     system(scmd);
     sprintf(scmd,"ln -s %s /home/dcfebcheck/data/temperature.dat",dout2);
     system(scmd);
     system("/home/dcfebcheck/RootAnalCheck/scripts/pulsefinescript  ");
     system("/home/dcfebcheck/RootAnalCheck/scripts/pulsefineanalysis ");
     printf(" exited script \n");fflush(stdout);
     sprintf(ddo,"cp /home/dcfebcheck/data/fitshapefine.dat /home/dcfebcheck/boards/%s.",dirnam);
     system(ddo);
     sprintf(ddo,"cd /home/dcfebcheck/images;tar -cvz image_fitshapefine > imgfit.tgz;cp imgfit.tgz /home/dcfebcheck/boards/%simage_fitshapefine.tgz",dirnam);
     system(ddo);
     float norm[6][16],t0[6][16],tp[6][16],p1[6][16],z1[6][16],chi2[6][16];
     float norm_expect[6][16],norm_dpct[6][16],t0_expect[6][16],dt0[6][16],tp_expect[6][16],dtp[6][16];
     float xnorm[6],nnorm[6],xt0[6],nt0[6],xtp[6],ntp[6];
     FILE *fpin;
     fpin=fopen("/home/dcfebcheck/data/fitshapefine.dat","r");
     if (fpin == 0) {
      fprintf(fp11,"Test11: fitshapefine.dat missing, no data.\n");
     }
     for(int i=0;i<6;i++){
       xnorm[i]=0.0;
       nnorm[i]=0.0;
       xt0[i]=0.0;
       nt0[i]=0.0;
       xtp[i]=0.0;
       ntp[i]=0.0;
     }
     int chp,chn;
     for(int chip=0;chip<6;chip++){
       for(int chan=0;chan<16;chan++){
	 //  fscanf(fpin,"%d %d %f %f %f %f %f %f \n",&chp,&chn,&norm[chip][chan],&t0[chip][chan],&p0[chip][chan],&p1[chip][chan],&z1[chip][chan],&chi2[chip][chan]);
	 fscanf(fpin,"%d %d %f %f %f %f %f %f %f %f %f %f \n",&chp,&chn,&norm[chip][chan],&norm_expect[chip][chan],&norm_dpct[chip][chan],&t0[chip][chan],&t0_expect[chip][chan],&dt0[chip][chan],&tp[chip][chan],&tp_expect[chip][chan],&dtp[chip][chan],&chi2[chip][chan]);
         if(chp!=chip||chan!=chn)printf(" problem %d %d %d %d \n",chip,chp,chan,chn);
         xnorm[chip]=xnorm[chip]+norm[chip][chan];   
         nnorm[chip]=nnorm[chip]+1.0;   
         xt0[chip]=xt0[chip]+t0[chip][chan];   
         nt0[chip]=nt0[chip]+1.0;   
         xtp[chip]=xtp[chip]+tp[chip][chan];   
         ntp[chip]=ntp[chip]+1.0;   
	 // printf(" data read %d %d \n",chip,chan);
         fflush(stdout);
       }
     }
     fclose(fpin);
     for(int chip=0;chip<6;chip++){
       xnorm[chip]=xnorm[chip]/nnorm[chip];
       xt0[chip]=xt0[chip]/nt0[chip];
       xtp[chip]=xtp[chip]/ntp[chip];
     }



     printf(" closed fpint \n");fflush(stdout);
     int ierr11,ierr12,ierr12b,ierr13;
     ierr11=0;
     ierr12=0;
     ierr12b=0;
     ierr13=0;
     float ddn,ddt0,ddtp;
     fprintf(fp11," chip channel gain time chi2 \n");
     for(int chip=0;chip<6;chip++){
       for(int chan=0;chan<16;chan++){
         ddn=norm[chip][chan]-xnorm[chip];
         if(ddn<0.0)ddn=-ddn;
         ddt0=t0[chip][chan]-xt0[chip];
         if(ddt0<0.0)ddt0=-ddt0;
         ddtp=tp[chip][chan]-xtp[chip];
         if(ddtp<0.0)ddtp=-ddtp;
         int jerr11,jerr12,jerr12b,jerr13,jerr14,jerr15;
         jerr11=0;
         jerr12=0;
         jerr12b=0;
         jerr13=0;
         jerr14=0;
         jerr15=0;
         fprintf(fp11," %d %d %f %f %f %f",chip,chan,norm[chip][chan],t0[chip][chan],tp[chip][chan],chi2[chip][chan]);
         if(ddn>60.0){jerr11=1;ierr11++;}
         if(ddt0>1.5){jerr12=1;ierr12++;}
         if(ddtp>2.0){jerr12b=1;ierr12b++;}
         if(xnorm[chip]<2100.|| xnorm[chip]>2900.){jerr14=1;ierr11++;}
         if((xt0[chip]<39.||xt0[chip]>44.)&&(xt0[chip]+25.<39.||xt0[chip]+25.>44.)){jerr15=1;ierr12++;}
         if(xtp[chip]<98.||xtp[chip]>110.){jerr15=1;ierr12++;}
         if(chi2[chip][chan]>50.){jerr13=1;ierr13++;}
         if(jerr11+jerr12+jerr12b+jerr13>0)fprintf(fp11," bad:");
         if(jerr11==1)fprintf(fp11," gain");
         if(jerr12==1)fprintf(fp11," stime");
         if(jerr12b==1)fprintf(fp11," ptime");
         if(jerr13==1)fprintf(fp11," shape");
         if(jerr14==1)fprintf(fp11," avggain");
         if(jerr15==1)fprintf(fp11," avgtime");
         fprintf(fp11,"\n");
       }
     }
     ierr=ierr11+ierr12+ierr12b+ierr13;
     if(fclose(fp11))printf("Close test11.dat failed.\n");
     // float thermx2=thisODMB_->readthermx(dcfeb_+1);
     float temp=thisDCFEB_->volt2temp(thisDCFEB_->ADC(4));
     averages_vs_temperature(temp,xnorm,xt0,xtp);  
     //averages_vs_temperature(temp);
     if(ierr>0){
       char command[100];
       sprintf(command,"emacs %s &",filename.c_str());
       system(command);
       system("/home/dcfebcheck/RootAnalCheck/scripts/pulsefineshow ");
     } 
  // Test 11 gain
     passed_[10] = ierr11;
     printf(" Test11 saw %d errors. \n",ierr11);

  // Test 12 time
     passed_[11] = ierr12+ierr12b;
     printf(" Test12 saw %d errors. \n",ierr12);
  // Test 13 shape
     passed_[12] = ierr13;
     printf(" Test13 saw %d errors. \n",ierr12);

  printf("END TESTS 11 TO 14\n");
  // passed_[13]=ierr;
}

void DCFEBCheck::Test14(){
  printf("BEGIN TEST 14\n");
  int ierr=0;
  FILE *ftest14;
  char dirnam[10];
  sprintf(dirnam,"DCFEB%03d/",ibrdnum_);
  std::string sdirnam=dirnam;
  std::string filename=SAVED_DATA_PREPATH+dirnam+"test14.dat";
  ftest14 = fopen(filename.c_str(),"w");
  if (ftest14 == 0) {
    printf("Open test14.dat failed. End Test 14.\n");
    passed_[13]=99;
    return;
  }
  thisDCFEB_->Disable_VCC_hardreset();
  usleep(100000);
  for(int i=0;i<50;i++){
    thisCCB_->setCCBMode(emu::pc::CCB::VMEFPGA);
    thisCCB_->HardReset_crate();  // change now to later
    // usleep(1000000);  // get rid of send immediate for long delays 
    thisCrate_->vmeController()->sleep_vme(200000);   // in usecs
    thisDCFEB_->Reset_which_DCFEB();
    unsigned int stat=thisDCFEB_->rdbkregvirtex6(VTX6_REG_STAT);
    unsigned int user=thisDCFEB_->febfpgauser();
    fprintf(ftest14," %d: status %08x fpgauser %08x \n",i,stat,user);
    if((stat!=0x05d07efc)|(user!=0xdcfeb309))ierr++;
  } 
  fprintf(ftest14," errors %d \n",ierr);
  fclose(ftest14);
  if(ierr!=0)
    printf("Failure: HardReset Test\n");
  passed_[13] = ierr;
  printf(" Test14 saw %d errors. \n",ierr); 
  if(ierr>0){
    char command[100];
    sprintf(command,"emacs %s &",filename.c_str());
    system(command);
  }
} 
  
/*void DCFEBCheck::Test15(){
  printf("BEGIN TEST 15\n");
  int ierr=0;
  int nbad=0;
  FILE *ftest15;
  char dirnam[10];
  sprintf(dirnam,"DCFEB%03d/",ibrdnum_);
  std::string sdirnam=dirnam;
  std::string filename=SAVED_DATA_PREPATH+dirnam+"test15.dat";
  ftest15 = fopen(filename.c_str(),"w");
  if (ftest15 == 0) {
    printf("Open test15.dat failed. End Test 15.\n");
    ierr++;
    char command[100];
    sprintf(command,"emacs %s &",filename.c_str());
    system(command);
    passed_[14] = ierr;
    return;
  }
  fclose(ftest15);
  ierr=nbad;
  if(ierr!=0)
    printf("Failure: Future Test\n");
    passed_[14] = ierr;
    printf(" Test15 saw %d errors. \n",ierr);
}

void DCFEBCheck::Test16(){
  printf("BEGIN TEST 16.\n");
  int ierr=0;
  FILE *ftest16;
  char dirnam[10];
  sprintf(dirnam,"DCFEB%03d/",ibrdnum_);
  std::string sdirnam=dirnam;
  std::string filename=SAVED_DATA_PREPATH+dirnam+"test16.dat";
  printf(" filename %s \n",filename.c_str());
  ftest16 = fopen(filename.c_str(),"w");
  if (ftest16 == 0) {
    printf("Open test16.dat failed. End Test 16.\n");
    ierr++;
    passed_[15] = ierr;
    return;
  }
  // before pulsing check comparator DAC and ADC
  int pass=TestComparatorDAC(ftest16);
  if(pass==0){
    char command[100];
    sprintf(command,"emacs %s &",filename.c_str());
    system(command);
    fclose(ftest16);
    printf("Comparator DAQ Test failed. End Test 16.\n");
    ierr++;
    passed_[15] = ierr;
    return;
  }
  fclose(ftest16);
  return;

  // now check trigger primitives
  int times[32][6];
  int times2[8][6];
  for(int i=0;i<32;i++){
    for(int j=0;j<6;j++){
      times[i][j]=-99;
      if(i<8)times2[i][j]=-99;
    }
  }
  int wrong=0;
  int missing=0;
  for(int itst=0;itst<2;itst++){
  setupPulseDelays(sonoff,1);
  usleep(200000);
  thisCCB_->setCCBMode(emu::pc::CCB::VMEFPGA);
  thisCCB_->startTrigger();
  thisCCB_->bc0(); 
  thisODMB_->settrgsrc(0);
  thisTMB_->DisableCLCTInputs();
  thisTMB_->DisableALCTInputs();
 
  // Enable this TMB for this test
  thisTMB_->SetClctPatternTrigEnable(1);
  thisTMB_->WriteRegister(emu::pc::seq_trig_en_adr);

  thisTMB_->SetTmbAllowClct(1);
  thisTMB_->WriteRegister(emu::pc::tmb_trig_adr);

  thisTMB_->SetHsPretrigThresh(4);
  thisTMB_->SetMinHitsPattern(4);
  //thisTMB_->SetHsPretrigThresh(4);
  // thisTMB_->SetMinHitsPattern(4);



  thisTMB_->WriteRegister(emu::pc::seq_clct_adr);

  thisTMB_->SetIgnoreCcbStartStop(0);
  thisTMB_->WriteRegister(emu::pc::ccb_trig_adr);


  thisTMB_->SetEnableLayerTrigger(0);
  thisTMB_->WriteRegister(emu::pc::layer_trg_mode_adr);

  thisTMB_->SetFifoMode(1);
  thisTMB_->SetFifoTbins(30);
  thisTMB_->SetFifoPreTrig(5);
  thisTMB_->SetFifoNoRawHits(0);
  thisTMB_->WriteRegister(emu::pc::seq_fifo_adr);

  thisTMB_->SetL1aDelay(126);
  thisTMB_->WriteRegister(emu::pc::seq_l1a_adr);
  thisTMB_->StartTTC();
  usleep(100000);
  int TimeDelay=5;
  thisTMB_->tmb_clk_delays(TimeDelay,0) ;
  thisTMB_->tmb_clk_delays(TimeDelay,1) ;
  thisTMB_->tmb_clk_delays(TimeDelay,2) ;
  thisTMB_->tmb_clk_delays(TimeDelay,3) ;
  thisTMB_->tmb_clk_delays(TimeDelay,4) ;

  int iset = (int)pow(2,dcfeb_);
  iset=0x1f;
  //iset=0x10;
  thisTMB_->EnableCLCTInputs(iset); 

  thisODMB_->varytmbdavdelay(31);

  int comp_mode=2;
  int comp_timing=2;  
  if(itst==1)comp_timing=7;
  int comp_mode_bits = (comp_mode & 3) | ((comp_timing & 7) << 2);
  thisDCFEB_->set_comp_mode(comp_mode_bits);

  thisDCFEB_->set_comp_thresh(0.25);
  thisODMB_->set_dac(2.5,2.5);
  int imax=32;
  if(itst==1)imax=8;
  for(int i=0;i<imax;i++){  
    printf(" pulse halfstrip %d \n",i);     
    int HalfStrip=i;
    // HalfStrip=18;
    int hp[6] = {HalfStrip+1, HalfStrip, HalfStrip+1,
		 HalfStrip, HalfStrip+1, HalfStrip};       
    //
    // Set the pattern
    //
    thisODMB_->trigsetx(hp,0x1f);
    thisODMB_->buck_shift();
    usleep(100000);
    thisCCB_->setCCBMode(emu::pc::CCB::VMEFPGA);
    int pulse_l1a_delay=0xa2-16;   // 0x9f seem to work
    thisCCB_->inject_delay_l1a(pulse_l1a_delay);
    //thisCCB_->inject(1,0x01); 
   //thisODMB_->inject(1,0x01);
    usleep(100000);
//    int length = getRawDmbData(rawdata);
    int length=daq->getCFEBDataOneEvent();

    SmallUnpack *unpk;
    if(length>10){
      unpk=new SmallUnpack(length/2,(short int *)daq->rbuf,8);
      unpk->getEvent();
    }
    if (length <= 10) {
      printf("getRawDmbData failed to retreive data.\n");
      passed_[14] = 1;
      return;
    }
    int datscram[5]={0,1,3,2,4};
    int thsfeb=datscram[dcfeb_];
    printf(" thsfeb %d \n",thsfeb);
    for(int feb=thsfeb;feb<thsfeb+1;feb++){
      int nhit[6]={0,0,0,0,0,0};
      for(int lay=0;lay<6;lay++){
        for(int hit=0;hit<32;hit++){
          int igot=0;
          if(unpk->tmb->chits[feb][lay][hit]>0){
            if(itst==0)fprintf(ftest16," pln %d expect %d hit %d tim %d ",lay,i,hit,unpk->tmb->chits[feb][lay][hit]);
            if(i==hit){
               if(itst==0)fprintf(ftest16,"good \n");
               if(itst==0)times[i][lay]=unpk->tmb->chits[feb][lay][i];
               if(itst==1)times2[i][lay]=unpk->tmb->chits[feb][lay][i];
            }else{
	       if(itst==0)fprintf(ftest16,"wrong \n");
               if(itst==0)wrong=wrong+1;
            } 
            igot=1;
            nhit[lay]=nhit[lay]+1;
          }
        }
      }
      for(int tst=0;tst<6;tst++)if(nhit[tst]!=1){
        if(itst==0)fprintf(ftest16," pln %d got %d hits \n",tst,nhit[tst]);
        if(itst==0)missing=missing+1;
      }
      printf("\n");

    }
    thisODMB_->calctrl_fifomrst(); // clear any events left
    usleep(20000);
//unpackDmbData(Comparator, rawdata, length, adcdata, false);
  }
  if(itst==0){
    printf("\n Summary: wrong %d missing %d \n",wrong,missing);
    fprintf(ftest16,"\n Summary: wrong %d missing %d \n",wrong,missing);
    ierr=wrong+missing;

    for(int i=0;i<32;i++){ 
      int chk=0;  
      for(int j=0;j<6;j++){
        for(int k=0;k<6;k++){
          if(j!=k&&times[i][j]-times[0][j]!=times[i][k]-times[0][k])chk=chk+1;
        }
      }
      printf(" chk %d \n",chk);fflush(stdout);
      ierr=ierr+chk;
      if(chk>0){
        fprintf(ftest16," Timing problem strip %d plane diff: ",i);
        for(int k=0;k<6;k++)fprintf(ftest16," %3d",times[i][k]-times[0][k]);
        fprintf(ftest16,"\n");
      }
    }     
  }
  if(itst==1){
    for(int i=0;i<8;i++){ 
      printf(" loop for chk %d \n",i);fflush(stdout);
      int chk=0;  
      for(int j=0;j<6;j++){
        for(int k=0;k<6;k++){
          if(j!=k&&times2[i][j]-times[0][j]!=times2[i][k]-times[0][k])chk=chk+1;
        }
      }
      printf(" chk times2 %d \n",chk);fflush(stdout);
      ierr=ierr+chk;
      if(chk>0){
        fprintf(ftest16," comparator time set problem strip %d plane diff: ",i);
        for(int k=0;k<6;k++)fprintf(ftest16," %3d",times2[i][k]-times[0][k]);
        fprintf(ftest16,"\n");
      }
      int chk2=0;
      for(int j=0;j<6;j++){
	if(times2[i][j]-times[0][j]<1){
          chk2=chk2+1;
        }
        if(chk2>0){
          fprintf(ftest16," comparator time set problem2 strip %d plane diff: ",i);
          for(int k=0;k<6;k++)fprintf(ftest16," %3d",times2[i][k]-times[0][k]);
          fprintf(ftest16,"\n");
        }
        ierr=ierr+chk2;
      }
    }     
  }
  } // end itst loop
  fclose(ftest16);
  thisCCB_->setCCBMode(emu::pc::CCB::DLOG);
  thisODMB_->settrgsrc(1);
  if(ierr!=0){
    printf("Failure: Comparator output problem!\n");
    char command[100];
    sprintf(command,"emacs %s &",filename.c_str());
    system(command);
  }
  passed_[15]=ierr;
  printf(" Test16 saw %d errors. \n",ierr);
}

*/

void DCFEBCheck::Test15(){
  printf("BEGIN TEST 15\n");
  int ierr=0;
  int nbad=0;
  FILE *ftest15;
  char dirnam[10];
  sprintf(dirnam,"DCFEB%03d/",ibrdnum_);
  std::string sdirnam=dirnam;
  std::string filename=SAVED_DATA_PREPATH+dirnam+"test15.dat";
  ftest15 = fopen(filename.c_str(),"w");
  if (ftest15 == 0) {
    fprintf(ftest15,"Open test15.dat failed. End Test 15.\n");
    printf("Open test15.dat failed. End Test 15.\n");
    ierr++;
    char command[100];
    sprintf(command,"emacs %s &",filename.c_str());
    system(command);
    passed_[14] = ierr;
    fclose(ftest15);
    return;
  }
  printf(" enter ComparatorCheck() \n");
  fflush(stdout); 
  setupPulseDelays(sonoff,0);
  usleep(1000000);
  thisDCFEB_->Set_TMB_TX_MODE(5);
  int ifirst=0;
  for(int iloop=0;iloop<1;iloop++){
  if(ifirst==0){
  ifirst=0;
  thisCCB_->setCCBMode(emu::pc::CCB::VMEFPGA);
  thisODMB_->settrgsrc(0);
  printf(" after settrgsrc \n");
  fflush(stdout); 
  thisTMB_->DisableALCTInputs();
 
  thisTMB_->SetClctPatternTrigEnable(1);
  thisTMB_->WriteRegister(emu::pc::seq_trig_en_adr);
  
  thisTMB_->SetTmbAllowClct(1);
  thisTMB_->WriteRegister(emu::pc::tmb_trig_adr);
  
  thisTMB_->SetHsPretrigThresh(1);
  thisTMB_->SetMinHitsPattern(1);
  thisTMB_->WriteRegister(emu::pc::seq_clct_adr);

  thisTMB_->SetIgnoreCcbStartStop(0);
  thisTMB_->WriteRegister(emu::pc::ccb_trig_adr);


  thisTMB_->SetEnableLayerTrigger(0);
  thisTMB_->WriteRegister(emu::pc::layer_trg_mode_adr);

  thisTMB_->SetFifoMode(1);
  thisTMB_->SetFifoTbins(30);
  thisTMB_->SetFifoPreTrig(5);
  thisTMB_->SetFifoNoRawHits(0);
  thisTMB_->WriteRegister(emu::pc::seq_fifo_adr);  
  
 int TimeDelay;
   printf(" TimeDelay %d \n",TimeDelay);
   TimeDelay=8;
   thisTMB_->tmb_clk_delays(TimeDelay,0) ;
   thisTMB_->tmb_clk_delays(TimeDelay,1) ;
   thisTMB_->tmb_clk_delays(TimeDelay,2) ;
   thisTMB_->tmb_clk_delays(TimeDelay,3) ;
   thisTMB_->tmb_clk_delays(TimeDelay,4) ; 
   usleep(50000);  
  
  
  SetAllChannelsHot();
  int iset=0x1f;
  thisTMB_->EnableCLCTInputs(iset);

  thisTMB_->StartTTC();
  usleep(200000);


  int comp_mode=2;
  int comp_timing=2;  
  int comp_mode_bits = (comp_mode & 3) | ((comp_timing & 7) << 2);
  thisDCFEB_->set_comp_mode(comp_mode_bits);
  thisDCFEB_->set_comp_thresh(0.500);
  thisDCFEB_->set_cal_dac(0.0,0.0);
  } // end first
  int halfs[32]={0,8,16,24,1,9,17,25,2,10,18,26,3,11,19,27,4,12,20,28,5,13,21,29,6,14,22,30,7,15,23,31};

  usleep(1000000);
  for(int i=0;i<1;i++){  // 0 - 32
  daq2->eth_reset();usleep(100000);
  for(int ii=0;ii<32;ii++){
    int HalfStrip;
    HalfStrip=ii;
    int hp[6] = {HalfStrip+1, HalfStrip, HalfStrip+1,
                HalfStrip, HalfStrip+1, HalfStrip};
    char hs[6];
    thisDCFEB_->Set_TMB_TX_MODE(5);
    // char layer_mask = 0x00;
    //thisDCFEB_->Set_TMB_TX_MODE_Layer_Mask(layer_mask);
    for(int k=0;k<6;k++)hs[k]=HalfStrip&0x1f;
    thisDCFEB_->Set_TMB_TX_MODE_ShiftLayers(hs);
    usleep(50000);

    thisCCB_->setCCBMode(emu::pc::CCB::VMEFPGA);

    thisODMB_->varytmbdavdelay(30);  // 30;
    usleep(50000);
    int pulse_l1a_delay=154;//143+17;     //   =156;

    printf(" ii %d i %d \n",ii,i);fflush(stdout);
    thisCCB_->inject_delay_l1a(pulse_l1a_delay);
    usleep(50000);

    printf(" i is %d \n",i);
    thisODMB_->PrintCounters();
    int skip_readout=0;
    if(skip_readout!=1){
      int length=daq2->getCFEBDataOneEvent();
      printf(" exited getDCFEBDataOneEvent %d %d \n",length,daq->test_more_data());
      if(length>10){
        printf(" about to call unpk getEvent \n");
        NSAMPLE=8;
	unpk-> getEvent(length/2,(short int *)daq2->rbuf,NSAMPLE);
        if(unpk->tmbdatapresent()==1){
        printf(" got some data \n");
        for(int pln=0;pln<6;pln++){
          int igotpln=0;
          for(int hstrp=0;hstrp<32;hstrp++){
            int val=unpk->tmb->chits[0][pln][hstrp];
	    if(val>0){
	       if(hstrp==HalfStrip)igotpln++;
               printf(" triad %d hstrp %d expect %d tbin %d \n",pln,hstrp,HalfStrip,val);
               fprintf(ftest15," triad %d hstrp %d expect %d tbin %d ",pln,hstrp,HalfStrip,val);
               if(hstrp!=HalfStrip){
                 fprintf(ftest15," <-Bad \n");
                 nbad++;
               }else{
                 fprintf(ftest15,"\n");
               } 
	     }   
          }
          if(igotpln==0){fprintf(ftest15," Missing Plane %d \n",pln);nbad++;}
        }
        }else{
          printf(" no data read back for expected %d \n",HalfStrip);
          fprintf(ftest15," no data read back for expected %d \n",HalfStrip);
          nbad++;
        }
      }else{
        printf(" no data read back for expected %d \n",HalfStrip);
        fprintf(ftest15," no data read back for expected %d \n",HalfStrip);
        printf(" getdcfebdata length %d \n",length);
        fprintf(ftest15," getdcfebdata length %d \n",length);
        nbad++;
      }
      thisODMB_->calctrl_fifomrst(); // clear any events left
      usleep(20000);
    } //i 
  } //ii  
  }
  }
  fflush(stdout);
  thisODMB_->PrintCounters();
  fclose(ftest15);
  ierr=nbad;
  if(ierr!=0)
    printf("Failure: Comparator Direct\n");
    passed_[14] = ierr;
    printf(" Test15 saw %d errors. \n",ierr);
}




void DCFEBCheck::Test16(){
  printf("BEGIN TEST 16.\n");
  int ierr=0;
  int nbad=0;
  FILE *ftest16;
  char dirnam[10];
  sprintf(dirnam,"DCFEB%03d/",ibrdnum_);
  std::string sdirnam=dirnam;
  std::string filename=SAVED_DATA_PREPATH+dirnam+"test16.dat";
  printf(" filename %s \n",filename.c_str());
  ftest16 = fopen(filename.c_str(),"w");
  if (ftest16 == 0) {
    fprintf(ftest16,"Open test16.dat failed. End Test 16.\n");
    printf("Open test16.dat failed. End Test 16.\n");
    ierr++;
    passed_[15] = ierr;
    fclose(ftest16);
    return;
  }
  printf(" enter ComparatorCheck() \n");
  fflush(stdout); 
  setupPulseDelays(sonoff,0);
  usleep(1000000);
  thisDCFEB_->Set_TMB_TX_MODE(0);
  //daq2->eth_reset();usleep(100000);
  //  thisDCFEB_->Set_FineDelay(2);
  usleep(250000);
  int ifirst=0;
  for(int iloop=0;iloop<1;iloop++){
  if(ifirst==0){
    // ifirst=0;
  thisCCB_->setCCBMode(emu::pc::CCB::VMEFPGA);
  
  thisODMB_->settrgsrc(0);
  printf(" after settrgsrc \n");
  fflush(stdout); 
 
 thisTMB_->DisableALCTInputs();

  // Enable this TMB for this test
  
  thisTMB_->SetClctPatternTrigEnable(1);
  thisTMB_->WriteRegister(emu::pc::seq_trig_en_adr);
  
  thisTMB_->SetTmbAllowClct(1);
  thisTMB_->WriteRegister(emu::pc::tmb_trig_adr);
  
  thisTMB_->SetHsPretrigThresh(1);
  thisTMB_->SetMinHitsPattern(1);
  thisTMB_->WriteRegister(emu::pc::seq_clct_adr);

  thisTMB_->SetIgnoreCcbStartStop(0);
  thisTMB_->WriteRegister(emu::pc::ccb_trig_adr);


  thisTMB_->SetEnableLayerTrigger(0);
  thisTMB_->WriteRegister(emu::pc::layer_trg_mode_adr);

  thisTMB_->SetFifoMode(1);
  thisTMB_->SetFifoTbins(30);
  thisTMB_->SetFifoPreTrig(5);
  thisTMB_->SetFifoNoRawHits(0);
  thisTMB_->WriteRegister(emu::pc::seq_fifo_adr);  
  
  //  thisTMB_->SetL1aDelay(109);
  //thisTMB_->WriteRegister(emu::pc::seq_l1a_adr);

   int TimeDelay;
   printf(" TimeDelay %d \n",TimeDelay);
   TimeDelay=4;
   thisTMB_->tmb_clk_delays(TimeDelay,0) ;
   thisTMB_->tmb_clk_delays(TimeDelay,1) ;
   thisTMB_->tmb_clk_delays(TimeDelay,2) ;
   thisTMB_->tmb_clk_delays(TimeDelay,3) ;
   thisTMB_->tmb_clk_delays(TimeDelay,4) ; 
  
  usleep(50000);  
  
  SetAllChannelsHot();
  int iset=0x1f;
  thisTMB_->EnableCLCTInputs(iset);

  thisTMB_->StartTTC();
  usleep(200000);


  int comp_mode=2;
  int comp_timing=2;  
  int comp_mode_bits = (comp_mode & 3) | ((comp_timing & 7) << 2);
  thisDCFEB_->set_comp_mode(comp_mode_bits);
  thisDCFEB_->set_comp_thresh(0.050);
  thisDCFEB_->set_cal_dac(2.0,2.0);
  } // end first
  int halfs[32]={0,8,16,24,1,9,17,25,2,10,18,26,3,11,19,27,4,12,20,28,5,13,21,29,6,14,22,30,7,15,23,31};

  //  thisCCB_->setCCBMode(emu::pc::CCB::VMEFPGA);
  thisODMB_->varytmbdavdelay(29);
  usleep(1000000); 
  for(int i=0;i<1;i++){  // 0 - 32
  for(int ii=0;ii<32;ii++){
    //  thisTMB_->SetL1aDelay(34+17+-4);
    //thisTMB_->WriteRegister(emu::pc::seq_l1a_adr);

    int HalfStrip;
    HalfStrip=ii;
    int hp[6] = {HalfStrip, HalfStrip, HalfStrip,
                HalfStrip, HalfStrip, HalfStrip};
    //
    // Set the pattern 
    // 
    thisODMB_->trigsetx(hp,0x1f);
    thisODMB_->buck_shift();
    usleep(50000);
    thisCCB_->setCCBMode(emu::pc::CCB::VMEFPGA);

    int pulse_l1a_delay=143+17;     //   =156;

    printf(" ii %d i %d \n",ii,i);fflush(stdout);
    thisCCB_->inject_delay_l1a(pulse_l1a_delay);
    usleep(100000);

    printf(" i %d \n",i);
    thisODMB_->PrintCounters();
    int skip_readout=0;
    if(skip_readout!=1){
      int length=daq2->getCFEBDataOneEvent();
      printf(" exited getDCFEBDataOneEvent %d %d \n",length,daq->test_more_data());
      if(length>10){
        unpk->getEvent(length/2,(short int *)daq2->rbuf,NSAMPLE);         
        if(unpk->tmbdatapresent()==1){
        printf(" got some data \n");
        for(int pln=0;pln<6;pln++){
          int igotpln=0;
          for(int hstrp=0;hstrp<32;hstrp++){
            int val=unpk->tmb->chits[0][pln][hstrp];
	    if(val>0){
	       if(hstrp==HalfStrip)igotpln++;
               printf(" triad %d hstrp %d expect %d tbin %d \n",pln,hstrp,HalfStrip,val);
               fprintf(ftest16," triad %d hstrp %d expect %d tbin %d ",pln,hstrp,HalfStrip,val);
               if(hstrp!=HalfStrip){
                 fprintf(ftest16," <-Bad \n");
                 nbad++;
               }else{
                 fprintf(ftest16,"\n");
               } 
	     }   
          }
          if(igotpln==0){fprintf(ftest16," Missing Plane %d \n",pln);nbad++;}
        }
        }else{
          printf(" no data read back for expected %d \n",HalfStrip);
          fprintf(ftest16," no data read back for expected %d \n",HalfStrip);
          nbad++;
        }
      }else{
        printf(" no data read back for expected %d \n",HalfStrip);
        fprintf(ftest16," no data read back for expected %d \n",HalfStrip);
        printf(" getdcfebdata length %d \n",length);
        fprintf(ftest16," getdcfebdata length %d \n",length);
        nbad++;
      }
      thisODMB_->calctrl_fifomrst(); // clear any events left
      usleep(20000);
    } // i 
  } //ii  
  }
  }
  fflush(stdout);
  thisODMB_->PrintCounters();
  fclose(ftest16);
  ierr=nbad;
  if(ierr!=0)
    printf("Comparator Pulse Test\n");
    passed_[15] = ierr;
    printf(" Test16 saw %d errors. \n",ierr);
}

void DCFEBCheck::SetAllChannelsHot(){
  int addr[21]={0x4a,0x4c,0x4e,0x50,0x52,0x54,0x56,0x58,0x5a,0x5c,0x5e,0x60,0x62,0x64,0x66,0x16e,0x170,0x172,0x174,0x176,0x178};
  thisTMB_->WriteRegister(0x14a,0x0000);
  usleep(1000000);
  thisTMB_->WriteRegister(0x14c,0x0001);
  usleep(100000);
  for(int i=0;i<3;i++)thisTMB_->WriteRegister(addr[i],0xffff);
  usleep(50000);
  for(int i=3;i<6;i++)thisTMB_->WriteRegister(addr[i],0x0000);
  usleep(50000);
  for(int i=6;i<21;i++)thisTMB_->WriteRegister(addr[i],0x0000);
  usleep(100000);
  DumpFiberonoff();
}

void DCFEBCheck::DumpFiberonoff(){
 int val=thisTMB_->ReadRegister(0x14a);
 printf(" register 0x14a %04x \n",val);
 val=thisTMB_->ReadRegister(0x14c); printf(" register 0x14c %04x \n",val); 
 val=thisTMB_->ReadRegister(0x14e); printf(" register 0x14e %04x \n",val);
 val=thisTMB_->ReadRegister(0x150); printf(" register 0x150 %04x \n",val);
 val=thisTMB_->ReadRegister(0x152); printf(" register 0x152 %04x \n",val);
 val=thisTMB_->ReadRegister(0x154); printf(" register 0x154 %04x \n",val);
 val=thisTMB_->ReadRegister(0x156); printf(" register 0x156 %04x \n",val);
 val=thisTMB_->ReadRegister(0x158); printf(" register 0x158 %04x \n",val);

}


void DCFEBCheck::Test17(){
  printf("STARTING TEST 17\n");
  float qpeak[6][16];
  int ierr=0;
  FILE *ftest17;
  char dirnam[10];
  sprintf(dirnam,"DCFEB%03d/",ibrdnum_);
  std::string sdirnam=dirnam;
  std::string filename=SAVED_DATA_PREPATH+dirnam+"test17.dat";
  ftest17 = fopen(filename.c_str(),"w");
  if (ftest17 == 0){
    printf("Open test17.dat failed. End Test 17.\n");
    ierr++;
    passed_[16] = ierr;
    return;
  }

  char dout[100];
  sprintf(dout,"/home/dcfebcheck/data/shrtouputs%s.dat",timeheader());
  printf(" %s \n",dout);
  dataout=fopen(dout,"w");
    
  thisODMB_->calctrl_global();
  usleep(50000);
  setupPulseDelays(sonoff,0);
  usleep(50000);
  daq->eth_reset();
  NSAMPLE=8;
  thisDCFEB_->Set_NSAMPLE(NSAMPLE);
  thisDCFEB_->Set_PipelineDepth(61);
  thisDCFEB_->Pipeline_Restart();
  usleep(50000);
  thisDCFEB_->set_DAQRate(0);
  usleep(100000);


  injtime_=13;    // in other tests, this is 15 
  dinjtime_=2;    // in 6.25 nsec steps
  evt=0;                                                        
  for(int k=0;k<1;k++){
    int k=18;
    int digivolt=12*k+2;
    float vpulse=3.10001*digivolt/255.;
    vpulse=0.0;
    thisDCFEB_->set_cal_dac(2.5,2.5);
    for(int i=0;i<1;i++){
      thisODMB_->shift_all(MEDIUM_CAP);  //pedestal
      // thisODMB_->shift_all(KILL_CHAN);  //pedestal 
      thisODMB_->buck_shift(); 
      injtime_=10; 
      thisODMB_->set_cal_tim_pulse(injtime_-dinjtime_); // set pulse time
      usleep(50000);
      //thisODMB_->toggle_pedestal(); // pedestal
      thisODMB_->inject(1,0xf);  // pedestal
      //thisODMB_->toggle_pedestal();   // pedestal
      usleep(50000);
      evt++;
      //    int length=daq->getDCFEBDataOneEvent();
      int length=daq->getCFEBDataOneEvent();
      printf(" exited getDCFEBDataOneEvent %d \n",length);
      if(length<10){
        printf("*Bad -DAQMB returned only %d bytes \n",length/2);
        fprintf(ftest17,"*Bad -DAQMB returned only %d bytes \n",length/2);
      }
      if(length>10){
	OpticalUnpack();
      }
      // int NSAMPLE=thisODMB_->getNSample();
      if(length>10){
      for(int chip=0;chip<6;chip++){
        for(int chan=0;chan<16;chan++){ 
          float fmax=-1000.;
          float ped=((rawadc[chip][chan][0]&0x0fff)+(rawadc[chip][chan][0]&0x0fff))/2.;

          for(int samp=0;samp<NSAMPLE;samp++){
            fprintf(dataout," %d %d %d %d %d %d %d \n",evt,k,i,chip,chan,samp,rawadc[chip][chan][samp]&0x0fff);
            float val=(rawadc[chip][chan][samp]&0x0fff)*1.0;
            val=val-ped;
            if(val>fmax)fmax=val;
          }
          qpeak[chip][chan]=fmax; 
        }
      }
      }
      fflush(dataout);                                                                   
    }
  }
  fprintf(dataout," -1 -1 -1 -1 -1 -1 \n");
  fclose(dataout);
  
  for(int chp=0;chp<6;chp++){
    for(int chn=0;chn<16;chn++){
      fprintf(ftest17," %d %d %f ",chp,chn,qpeak[chp][chn]);
      if(qpeak[chp][chn]>500.){fprintf(ftest17,":bad\n");ierr++;}else{fprintf(ftest17,"\n");}
    }
  }
  if (fclose(ftest17))printf("Close peds17.dat failed.\n");
  if(ierr>0){
    char command[100];
    sprintf(command,"emacs %s &",filename.c_str());
    system(command);
  }
  passed_[16]=ierr;
  printf(" Test17 saw %d errors.\n",ierr);
}

void DCFEBCheck::Test18(){
  int ierr=0;
  float qpeak[6][16];
  printf("STARTING TEST 18\n");
  FILE *ftest18;
  char dirnam[10];
  sprintf(dirnam,"DCFEB%03d/",ibrdnum_);
  std::string sdirnam=dirnam;
  std::string filename=SAVED_DATA_PREPATH+dirnam+"test18.dat";
  ftest18 = fopen(filename.c_str(),"w");
  if (ftest18 == 0) {
    printf("Open test16.dat failed. End Test 16.\n");
    ierr++;
    passed_[17] = ierr;
    return;
  }
  char dout[100];
  sprintf(dout,"/home/dcfebcheck/data/killchannel%s.dat",timeheader());
  printf(" %s \n",dout);
  dataout=fopen(dout,"w");
    
  thisODMB_->calctrl_global();
  usleep(50000);
  setupPulseDelays(sonoff,0);
  usleep(50000);
  daq->eth_reset();
  NSAMPLE=8;
  thisDCFEB_->Set_NSAMPLE(NSAMPLE);
  thisDCFEB_->Set_PipelineDepth(61);
  thisDCFEB_->Pipeline_Restart();
  usleep(50000);
  thisDCFEB_->set_DAQRate(0);
  usleep(100000);

  injtime_=9;    // in other tests, this is 15 
  dinjtime_=0;    // in 6.25 nsec steps
  evt=0;                                                        
  for(int k=0;k<1;k++){
    //for(int k=0; k<22; k++){
    //while(1){                                                                            
    int k=18;
    int digivolt=12*k+2;
    float vpulse=3.10001*digivolt/255.;
    vpulse=0.0;
    thisDCFEB_->set_cal_dac(2.5,2.5);
    for(int i=0;i<1;i++){
      daq->eth_reset();
      usleep(50000);
      thisODMB_->shift_all(KILL_CHAN);
      //thisODMB_->shift_all(MEDIUM_CAP);  
      thisODMB_->buck_shift(); 
      thisODMB_->set_cal_tim_pulse(injtime_-dinjtime_); // set pulse time
      usleep(50000);
      // thisODMB_->toggle_pedestal(); // pedestal
      thisODMB_->inject(1,0xf);  // pedestal
      // thisODMB_->toggle_pedestal();   // pedestal
      usleep(50000);
      evt++;
      //    int length=daq->getDCFEBDataOneEvent();
      int length=daq->getCFEBDataOneEvent();
      printf(" exited getDCFEBDataOneEvent %d \n",length);
      if(length<10){
        printf("*Bad -DAQMB returned only %d bytes \n",length/2);
        fprintf(ftest18,"*Bad -DAQMB returned only %d bytes \n",length/2);
      }
      if(length>10){
	OpticalUnpack();
      }
      // int NSAMPLE=thisODMB_->getNSample();
      if(length>10){
      for(int chip=0;chip<6;chip++){
        for(int chan=0;chan<16;chan++){
          float fmax=-1000.;
          float ped=((rawadc[chip][chan][0]&0x0fff)+(rawadc[chip][chan][0]&0x0fff))/2.;
          for(int samp=0;samp<NSAMPLE;samp++){
            fprintf(dataout," %d %d %d %d %d %d %d \n",evt,k,i,chip,chan,samp,rawadc[chip][chan][samp]&0x0fff);
            float val=(rawadc[chip][chan][samp]&0x0fff)*1.0;
            val=val-ped;
            if(val>fmax)fmax=val;
          }
          qpeak[chip][chan]=fmax;
        }
      }
      }
      fflush(dataout);     
    }
  }
  fprintf(dataout," -1 -1 -1 -1 -1 -1 \n");
  fclose(dataout);
  for(int chp=0;chp<6;chp++){
    for(int chn=0;chn<16;chn++){
      fprintf(ftest18," %d %d %f ",chp,chn,qpeak[chp][chn]);
      if(qpeak[chp][chn]>50.){fprintf(ftest18,":bad\n");ierr++;}else{fprintf(ftest18,"\n");}
    }
  }
  if(fclose(ftest18)) printf("Close test18.dat failed.\n");
  if(ierr>0){
    char command[100];
    sprintf(command,"emacs %s &",filename.c_str());
    system(command);
  }
  passed_[17]=ierr;
  printf(" Test18 saw %d errors.\n",ierr);
}
/***************************************************
 *           BEGIN TEST HELPER METHODS
 ***************************************************/

// Used in Test3()
int DCFEBCheck::check_prom_readback(){
  FILE *exp;
  FILE *got;
  char texp[4],tgot[4];
  int bytesyes=0; 
  int bytesno=0;
  exp=fopen("/home/boardtest/config/firmware/feb_test/compare.bit","r");
  if (exp == 0) {
    printf("Open config/firmware/feb_test/compare.bit failed. "
	   "End check_prom_readback\n");
    return 1;
  }
  got = fopen("eprom.bit","r");
  if (got == 0) {
    printf("Open eprom.bit failed. End check_prom_readback.\n");
    return 1;
  }
  for(int i=0; i < 4*32768; i++){
    fscanf(exp,"%s",texp);
    fscanf(got,"%s",tgot);
    int yes=0;
    for(int j=0;j<3;j++) {
      if(texp[j]==tgot[j]) yes++;
    }
    // printf(" yes %d %s %s \n",yes,texp,tgot);
    if(yes==3){
      bytesyes++;
    } else {
      bytesno++;}
  }
  return bytesno;
}


// num_pulses will probably always be 22
int DCFEBCheck::stuckBits(float test_adcdata[][8][6][16][8],
			    int num_pulses,
			    FILE *output) {
  bool doPrint = (output == 0) ? false : true;
  int ierr = 0;
  int bit_counts[6][13];
  for(int lay=0; lay < 6; lay++){
    for (int bit = 0; bit < 13; bit++) {
      bit_counts[lay][bit] = 0;
    }
  }
  int total = 0;
  for(int pnum=0; pnum < num_pulses; pnum++){
    for (int time = 0; time < 8; time++) {
      for (int lay = 0; lay < 6; lay++) {
	for (int chan = 0; chan < 16; chan++) {
	  for (int tm = 0; tm < 8; tm++) {unsigned int test_bit = 1;
	    for (int bit = 0; bit < 13; bit++) {
	      int count = (int)test_adcdata[pnum][time][lay][chan][tm];
	      if ((count & test_bit) == test_bit)
		bit_counts[lay][bit]++;
	      test_bit<<=1;
	    }
	    total++;
	  }
	}
      }
    } 
  }
  if (doPrint) fprintf(output, "Total of %d counts per layer.\n",total/6);
  for(int lay=0; lay < 6; lay++){
    if (doPrint) fprintf(output, "Layer %d: ",lay);
    for (int bit = 0; bit < 13; bit++) {
      if (doPrint) fprintf(output, "%d ",bit_counts[lay][bit]);
      if (bit_counts[lay][bit] == total) ierr++;
      if (bit_counts[lay][bit] == 0)ierr++;
    }
    if (doPrint) fprintf(output, "\n");
  }
  return ierr;
}



void DCFEBCheck::configureDMB(){
  thisODMB_->small_configure();
} 

void DCFEBCheck::onoff_reset(){
  thisODMB_->lowv_onoff(0x00);
  usleep(500000);
  thisODMB_->lowv_onoff(sonoff);
  usleep(200000);
}

void DCFEBCheck::BrdPrmpt(xgi::Input * in, xgi::Output * out )
  throw (xgi::exception::Exception) {
  printf("Enter BrdPrmpt\n");
    cgicc::Cgicc cgi(in);
    xdata::String BrdPrmpt= cgi["BrdPrmpt"]->getValue();
    std::string brdprmpt=BrdPrmpt;
    brdnum_="DCFEB"+brdprmpt;
    prmtbox_=brdprmpt;
    cbrdnum_=brdnum_.c_str();
    printf(" cbrdnum %s \n",cbrdnum_);
    
    //ibrdnum_=atoi(cbrdnum_);
    //printf(" ibrdnum set to %d\n", ibrdnum_);
    int junk = 0;
    sscanf(cbrdnum_,"%05x%03d",&junk,&ibrdnum_);
    // check if board directory exists
    char dirnam[10];
    sprintf(dirnam,"DCFEB%03d/",ibrdnum_);
    std::string sdirnam=dirnam;
    std::string saved_data_path=SAVED_DATA_PREPATH+dirnam;
    std::cout << " path for board: " << saved_data_path << std::endl;
    DIR *dirp;
    dirp=opendir(saved_data_path.c_str());
    if(dirp==0){
      char command[100];
      sprintf(command,"mkdir %s",saved_data_path.c_str());
      system(command);
      (void)closedir(dirp);
      //virginboardsetup(ibrdnum_);
    }else{
      (void)closedir(dirp);
    } 
    printf(" passed made a new directory \n");fflush(stdout);
    //std::cout << head_ << std::endl;
    FILE *fpassbrd;
    std::string filename = SAVED_DATA_PREPATH + "passed.dat";
    fpassbrd = fopen(filename.c_str(),"r+");
    printf(" open fpassbrd %d \n",fpassbrd);fflush(stdout);
    // TODO: if the board is found in passed.dat, thetwo lines corresponding 
    // to it should be deleted so that when Finis() is called at the end of
    // testing, there is only one entry per board. In the meantime,
    // this is written to ensure only the most recent data is used.
    if (fpassbrd != 0) {
      int i=0;
      char line1[100];
      int tbrdnum;
      int tpassed[18];
      while(fgets(line1,100,fpassbrd)!=NULL){
	i++;
	if(i%2==1){
	  sscanf(line1," %04d",&tbrdnum);
          printf(" tbrdnum %d \n",tbrdnum);fflush(stdout);
	}else{
	  sscanf(line1,
		 " %04d %04d %04d %04d %04d %04d %04d %04d %04d %04d %04d %04d %04d %04d %04d %04d %04d %04d",
		 &tpassed[0],
		 &tpassed[1],
		 &tpassed[2],
		 &tpassed[3],
		 &tpassed[4],
		 &tpassed[5],
		 &tpassed[6],
		 &tpassed[7],
		 &tpassed[8],
		 &tpassed[9],
		 &tpassed[10],
		 &tpassed[11],
		 &tpassed[12],
		 &tpassed[13],
		 &tpassed[14],
		 &tpassed[15],
		 &tpassed[16],
		 &tpassed[17]);
	  printf(" tpassed[16-17] %d %d \n",tpassed[16],tpassed[17]);
	  if(ibrdnum_==tbrdnum){
	    printf(" passed match %d %d \n",ibrdnum_,tbrdnum);
	    old_=1;
	    for(int j=0;j<18;j++)passed_[j]=tpassed[j];
          }
	}
      }
      fclose(fpassbrd);
    } else {
      printf("Open passed.dat failed (file likely does not exist).\n");
    }
    //refsec_ = 3;
    refsec_ = 1000;
    sprintf(refresh_,"%d",refsec_);
    if(old_==1){
      sprintf(head_,"Old Board: %04d %s %02d/%02d/%02d %02d:%02d lsd",
	      ibrdnum_,cbrdnum_,loct_->tm_mon+1,loct_->tm_mday,
	      loct_->tm_year+1900-2000,loct_->tm_hour,loct_->tm_min);
    }else{
      sprintf(head_,"New Board: %04d %s %02d/%02d/%02d %02d:%02d lsd",
	      ibrdnum_,cbrdnum_,loct_->tm_mon+1,loct_->tm_mday,
	      loct_->tm_year+1900-2000,loct_->tm_hour,loct_->tm_min);
    }
    sprintf(cmnt_,"Board %d Chosen ",ibrdnum_);
    sprintf(cmntcolor_,"color:black;background-color:Green");

  printf(" ibrdnum_ set to %d\n", ibrdnum_);
  first_=0;
  this->Default(in,out);	
}

void DCFEBCheck::Run(xgi::Input * in, xgi::Output * out ) {
    printf("Run called: run_ %d first_ %d \n",run_,first_);
    // sprintf(cmnt_," Running ...");
    //  sprintf(cmntcolor_,"color:black;background-color:Khaki");
    if(first_==0)run_=1;
    first_=first_+1;
  this->Default(in,out);	
}   


void DCFEBCheck::Update(xgi::Input * in, xgi::Output * out ) {
    printf("Update called");
    Finis(19);
    this->Default(in,out);	
}   



void DCFEBCheck::Reset(xgi::Input * in, xgi::Output * out ) {
  // sprintf(cmnt_," ");
  //  sprintf(cmntcolor_," ");
    idone=0;
    run_=0;
    first_=0;
    for(int i=0;i<18;i++)passed_[i]=9999;
  this->Default(in,out);
}   

void DCFEBCheck::NewBoard(xgi::Input * in, xgi::Output * out ) {
    printf("NewBoard called \n");
    curt_=time(NULL);
    loct_=localtime(&curt_);
    first_=0;
    run_=0;
    next_=0;
    old_=0;
    ibrdnum_=-99;
    idone=0;
    for(int i=0;i<18;i++){
      passed_[i]=9999;
    }
    brdnum_="";
    prmtbox_="";
    refreshcount_=0;
    blastcount_ = -99;
    refsec_=1000;
    sprintf(head_," ");
    sprintf(refresh_,"%d",refsec_);
    sprintf(cmnt_,"No Board Chosen, Choose a Board First ");
    sprintf(cmntcolor_,"color:black;background-color:Green");
  this->Default(in,out);
}

void DCFEBCheck::Finis(int iv){
  printf(" Finis Called %d \n",iv);
  run_=0;
  next_=0;
  quit_ = 1;
  refsec_=1000;
  sprintf(refresh_,"%d",refsec_);
  if(iv<18){
    sprintf(cmnt_,"Board %d Failed Test%d! ",ibrdnum_,iv+1);
    sprintf(cmntcolor_,"color:yellow;background-color:red");
  }
  if (iv == 99) {
    int failCount = 0;
    for (int i = 0; i < 18; i++) {
      if (passed_[i] > passcut_[i]) {
	failCount++;
	break; // if it failed one, it failed. No need to keep counting.
      }
    }
    // Set message at top of GUI
    if (failCount == 0) {
      sprintf(cmnt_,"Successfully Finished! ");
      sprintf(cmntcolor_,"color:red;background-color:green");
    } else {
      if(old_==0){ 
	sprintf(cmnt_,"New Board %d Failed!",ibrdnum_);
	sprintf(cmntcolor_,"color:yellow;background-color:red");
      }
      if(old_==1){ 
	sprintf(cmnt_,"Old Board %d Failed!",ibrdnum_);
	sprintf(cmntcolor_,"color:yellow;background-color:red");
      }
    }
  }

  // Save test results in passed.dat.
  FILE *fpassbrd;
  std::string filename = SAVED_DATA_PREPATH + "passed.dat";
  fpassbrd = fopen(filename.c_str(),"a");
  if (fpassbrd == 0) {
    printf(" Open passed.dat failed. Unable to record test history.\n");
    return;
  } else {
    printf(" Writing test results to %s\n",filename.c_str());
    fprintf(fpassbrd," %04d %s %02d/%02d/%02d %02d:%02d LSD \n",
	    ibrdnum_,
	    cbrdnum_,
	    loct_->tm_mon+1,
	    loct_->tm_mday,
	    loct_->tm_year+1900-2000,
	    loct_->tm_hour,
	    loct_->tm_min);
    for(int i=0;i<18;i++) fprintf(fpassbrd," %04d",passed_[i]);
    fprintf(fpassbrd,"\n");  
    if (fclose(fpassbrd)) printf("Closing passed.dat failed.n");;
  }
}

void DCFEBCheck::setupPulseDelays(int von,int killbuckeyes){
  thisODMB_->lowv_onoff(0x00);
  usleep(200000);
  thisODMB_->lowv_onoff(von);
  usleep(200000);
  thisCCB_->setCCBMode(emu::pc::CCB::VMEFPGA);
  thisCCB_->hardReset(); // load fpga
  usleep(200000);
  thisDCFEB_->Reset_which_DCFEB();
  thisODMB_->small_configure();
  thisODMB_->calctrl_fifomrst();
  usleep(200000);
  daq->eth_reset();
  int xFineLatency=1;
  int inject_delay=0;
  int itim=10;
  int LCT_delay=6; //orig
  int l1acc_delay=19;
  l1acc_delay=20;
  int cfeb_dav_delay=21; // orig
  cfeb_dav_delay=23;
  if(killbuckeyes==1)cfeb_dav_delay=0;
  thisODMB_->SetFebDavDelay(cfeb_dav_delay);
  printf(" GetFEBDavDelay %d \n",thisODMB_->GetFebDavDelay());
  int dav_delay_bits =(thisODMB_->GetFebDavDelay() & 0x1F)
      | (thisODMB_->GetTmbDavDelay()  & 0X1F) << 5
      | (thisODMB_->GetPushDavDelay() & 0x1F) << 10
      | (thisODMB_->GetL1aDavDelay()  & 0x3F) << 15
      | (thisODMB_->GetAlctDavDelay() & 0x1F) << 21;
  thisODMB_->setdavdelay(dav_delay_bits);
  printf("  xFineLacency before %d \n",xFineLatency);
  thisODMB_->SetxFineLatency(xFineLatency);
  printf("  GetFineLatency %d \n",thisODMB_->GetxFineLatency());
  thisODMB_->setfebdelay(thisODMB_->GetKillFlatClk());
  thisODMB_->load_feb_clk_delay();
  thisODMB_->calctrl_global();
  usleep(100000);  
  thisODMB_->set_cal_tim_pulse(10);  // 0-22 are valid times
  int cal_delay_bits = (LCT_delay & 0xF)
    | (l1acc_delay & 0x1F) << 4
    | (itim & 0x1F) << 9
    | (inject_delay & 0x1F) << 14;
  int dword;
  dword=(cal_delay_bits)&0xfffff;
  thisODMB_->setcaldelay(dword);

}


void DCFEBCheck::virginboardsetup(int ibrd){
  char command[100];
  int ierr=0;
  FILE *fnew;
  char dirnam[50];
  sprintf(dirnam,"DCFEB%03d/",ibrd);
  std::string sdirnam=dirnam;
  std::string filename=SAVED_DATA_PREPATH+sdirnam+"newboard.dat";
  fnew = fopen(filename.c_str(),"w");
  printf(" newboard path %s %d \n",filename.c_str(),fnew);
  fflush(stdout);
  if(fnew == 0){
    printf("Open newboard.dat failed. newboardprogram.%s \n",filename.c_str());
    fflush(stdout);
    fclose(fnew);
    sprintf(command,"emacs %s",filename.c_str());
    system(command);
    return;
  }
  int nval=2;
  unsigned short int val[2];
  val[0]=0xdcfe;
  val[1]=ibrd|0xb000;
  thisDCFEB_->epromload_parameters(2,nval,val);
  int nval2=2;
  unsigned short  int val2[10];
  thisDCFEB_->epromread_parameters(2,nval2,val2);
  fprintf(fnew," Returned: ");
  for(int iv=0;iv<nval2;iv++)fprintf(fnew," val %d %04x ",iv,val2[iv]);fprintf(fnew,"\n");fflush(stdout);
  for(int iv=0;iv<nval2;iv++)printf(" val%d %04x ",iv,val2[iv]);printf("\n");fflush(stdout);
  if(val[0]!=0xdcfe||val[1]!=(ibrd|0xb000)){
    ierr++;
    fprintf(fnew," wrong eprom board information so quit \n");
    fclose(fnew);
    sprintf(command,"emacs %s",filename.c_str());
    system(command);
    return;
  } 
  std::string DCFEBFirmware = FirmwareDir_ + DCFEB_FIRMWARE_FILENAME;
  std::cout << "open file " << DCFEBFirmware << std::endl;
  DCFEBFirmware_ = DCFEBFirmware;
  thisDCFEB_->programvirtex6(DCFEBFirmware_.toString().c_str());
  unsigned int status=thisDCFEB_->rdbkregvirtex6(VTX6_REG_STAT);
  printf("programvirtex6: status: %08x \n",status);fflush(stdout);
  int mode=((0x0000700&status)>>8);
  int done=((0x00004000&status)>>14);
  int startup_state=((0x001c0000)>>18);
  fprintf(fnew,"programvirtex6: status: %08x \n",status);
  fflush(stdout);
  if(mode!=0x06||done!=1||startup_state!=7){
    fprintf(fnew,"programvirtex6 FAILED \n");
    fprintf(fnew,"Virtex 6 status: %08x \n",status);
    fprintf(fnew,"     mode %02x should be 0x06 \n",mode);
    fprintf(fnew,"     done %d should be 1 \n",done);
    fprintf(fnew,"     startup_state %02d should be 0x07 \n",startup_state);
    fclose(fnew);
    sprintf(command,"emacs %s",filename.c_str());
    system(command);
    return;
  }
  thisDCFEB_->epromload_mcs(DCFEBFirmware_.toString().c_str(),0);
  printf(" exiting epromload_mcs \n");fflush(stdout);
  usleep(50000); 
  int xonoff[5]={0x01,0x02,0x04,0x08,0x10};
  sonoff=xonoff[dcfeb_index];
  printf(" sonoff %d \n",sonoff);
  thisODMB_->lowv_onoff(sonoff);
  usleep(200000);
  thisCCB_->hardReset();
  usleep(2000000);
  thisDCFEB_->Reset_which_DCFEB();
  status=thisDCFEB_->rdbkregvirtex6(VTX6_REG_STAT);
  printf("epromload virtex6: status: %08x \n",status);
  fflush(stdout);
  mode=((0x0000700&status)>>8);
  done=((0x00004000&status)>>14);
  startup_state=((0x001c0000)>>18);
  fprintf(fnew,"epromload Virtex 6 status: %08x \n",status);
  fflush(stdout);
  if(mode!=0x06||done!=1||startup_state!=7){
    fprintf(fnew,"epromload_mcs FAILED \n");
    fprintf(fnew,"Virtex 6 status: %08x \n",status);
    fprintf(fnew,"     mode %02x should be 0x06 \n",mode);
    fprintf(fnew,"     done %d should be 1 \n",done);
    fprintf(fnew,"     startup_state %02d should be 0x07 \n",startup_state);
    fclose(fnew);
    sprintf(command,"emacs %s",filename.c_str());
    system(command);
    return;
  } 
  fclose(fnew);
}

// readin split events

int DCFEBCheck::get_split_event(){
  int newlen=0;
  printf(" next_split %d \n",next_split);
  int tstrd=0;
  for(int i=0;i<next_split;i++){
     if(((nextevt[i]&0xf000)==0xe000)&&
        ((nextevt[i+1]&0xf000)==0xe000)&&
        ((nextevt[i+2]&0xf000)==0xe000)&&
       ((nextevt[i+3]&0xf000)==0xe000)){tstrd=1;}
  }
  int length=0;
  if(tstrd==0){
    length=daq->getCFEBDataOneEvent();
    printf(" read LENGTH %d \n",length/2);
  }
  int lentot=0;
  for(int i=0;i<next_split;i++){
    if(lentot<RAWDAT){
      rawdat[lentot]=nextevt[i];
    }else{
      printf(" overflow rawdat lentot %d RAWDAT %d \n",lentot,RAWDAT);
      fflush(stdout);
    }
    lentot++;
  }
  for(int i=0;i<length/2;i++){
    if(lentot<RAWDAT){
      rawdat[lentot]=(((daq->rbuf[2*i+1]<<8)&0xff00)|(daq->rbuf[2*i]&0x00ff));
    }else{
      printf(" overflow rawdat lentot %d RAWDAT %d \n",lentot,RAWDAT);
      fflush(stdout);
    }
    lentot++;
  }
  int ifirst=0;
  for(int i=0;i<lentot;i++){
     if(((rawdat[i]&0xf000)==0xe000)&&
        ((rawdat[i+1]&0xf000)==0xe000)&&
        ((rawdat[i+2]&0xf000)==0xe000)&&
       ((rawdat[i+3]&0xf000)==0xe000)&&ifirst==0){ifirst=1;newlen=i+4;}
     //     printf(" %d %04x %04x %04x %04x \n",i,rawdat[i],rawdat[i+1],rawdat[i+2],rawdat[i+3]);
  }
  printf(" lentot %d newlen %d \n",lentot,newlen);
  if(newlen==0){
    printf(" event had no end \n");
    return 0;
  }
  next_split=0;
  for(int i=newlen;i<lentot;i++){
    if(next_split>RAWDAT){
      nextevt[next_split]=rawdat[i];
    }else{
      printf(" overflow next_split %d RAWDAT %d \n",next_split,RAWDAT);
      fflush(stdout);
    }

    next_split++;
  }
  //  printf(" **** newlen %d \n",newlen);
  // for(int i=0;i<newlen;i++){
  //  printf(" isent %d %04x \n",i,rawdat[i]&0xffff);
  //  }
  return newlen;
}

void DCFEBCheck::fastfit(float *t,float *adc,float *fitval){
   int j;
   float tdif;
   int n_fit;
   printf(" inside fastfit \n");
   for(int i=0;i<64;i++)printf(" i %d %f %f \n",i,t[i],adc[i]);

   float fpNorm   = 0.;
   float t0       = 0.;
   float chi2sav =0.;

   // How many time bins are we fitting?

   float chi_min  = 1.e10;
   float chi_last = 1.e10;
   float tt0      = 0.;
   float chi2     = 0.;
   float del_t    = 100.;

   float x[64];
   float sx2 = 0.;
   float sxy = 0.;
   float fN  = 0.;
   n_fit=64;
   while ( del_t > 0.01 ) {
     sx2 = 0.;
     sxy = 0.;
     for (j=0; j < n_fit; ++j ) {
       tdif = t[j] - tt0;
       x[j] = fivepolepolezero(tdif);
       sx2 += x[j] * x[j];
       sxy += x[j] * adc[j];
     }
     fN = sxy / sx2; // least squares fit over time bins i to adc[i] = fN * fivePoleFunction[i]

    // Compute chi^2
     chi2 = 0.0;
     for (j=0; j < n_fit; ++j) chi2 += (adc[j] - fN * x[j]) * (adc[j] - fN * x[j]);
      // Test on chi^2 to decide what to do    
       if ( chi_last > chi2 ) {
       if (chi2 < chi_min ){
         t0      = tt0;
         fpNorm  = fN;
         chi2sav=chi2;
     }
     chi_last  = chi2;
     tt0       = tt0 + del_t;
     } else {
       tt0      = tt0 - 2. * del_t;
       del_t    = del_t / 2.;
       tt0      = tt0 + del_t;
       chi_last = 1.0e10;
     }
  }
   fitval[0]=fpNorm;
   fitval[1]=t0;
   fitval[2]=chi2sav;
}

float DCFEBCheck::fivepolepolezero(double tt){
double cg;
float rt;
  if(tt>0.0){
  cg = -0.4915486701e0 * exp(-0.5300000000e-2 * tt) + 0.1007271865e-18 * exp(-0.4100000000e-1 * tt) * (0.6773434250e13 * pow(tt, 0.4e1) + 0.3700609164e14 * pow(tt, 0.3e1) + 0.4880000000e19 + 0.3109755600e16 * tt * tt + 0.1742160000e18 * tt);
  }else{
    cg=0.0;
  }
rt=cg;
return rt;
}

float DCFEBCheck::correct_for_temperature(float t){
  float val;
  if(t>40.&&t<120){
    val=(2.66121*70.0+1808.7318)/(2.66121*t+1808.7318);
  }else{
    val=1.0;
  }
  return val;
}

void DCFEBCheck::averages_vs_temperature(float t,float *xnorm,float *xt0,float *xtp){  
  for(int chip=0;chip<6;chip++){
    FILE *fpout;
    fpout=fopen("/home/dcfebcheck/data/standards/test11_averages.txt","a");
    fprintf(fpout,"%s %d %f %f %f %f \n",timeheader(),chip,t,xnorm[chip],xt0[chip],xtp[chip]);
    fclose(fpout); 
  }
}

/* void DCFEBCheck::averages_vs_temperature(float t){
  FILE *fpin;
  FILE *fpout;
  int trial,i;
  int chip,chan;
  int chp,chn;
  float norm[6][16],t0[6][16],p0[6][16],p1[6][16],z1[6][16],chi2[6][16];
  float mnorm,mt0,mp0,mp1,mz1,mchi2;
  float snorm,st0,sp0,sp1,sz1,schi2;
  float dum1,dum2,dum3,dum4,dum5,dum6;
  double x,sx,sx2,sn,mean,sig;
  fpin=fopen("/home/dcfebcheck/data/fitshapefine.dat","r");
 
  for(chip=0;chip<6;chip++){
    for(chan=0;chan<16;chan++){
      // fscanf(fpin,"%d %d %f %f %f %f %f %f \n",&chp,&chn,&norm[chip][chan],&t0[chip][chan],&p0[chip][chan],&p1[chip][chan],&z1[chip][chan],&chi2[chip][chan]);
fscanf(fpin,"%d %d %f %f %f %f %f %f %f %f %f %f n",&chp,&chn,&norm[chip][chan],&dum1,&dum2,&t0[chip][chan],&dum3,&dum4,&p0[chip][chan],&dum5,&dum6,&chi2[chip][chan]);
      //      printf("xx chp chn %d %d %f \n",chp,chn,p0[chp][chn]);
      if(chp!=chip||chan!=chn)printf(" problem %d %d %d %d \n",chip,chp,chan,chn);
    }
  }
  for(trial=0;trial<6;trial++){
    sx=0.0;sn=0.0;sx2=0.0;
    for(chip=3;chip<6;chip++){
      for(chan=0;chan<16;chan++){
        if(trial==0)x=norm[chip][chan];
        if(trial==1)x=t0[chip][chan]; 
	if(trial==2)x=p0[chip][chan];
        //if(trial==3)x=p1[chip][chan]; 
        //if(trial==4)x=z1[chip][chan];
        if(trial==3)x=chi2[chip][chan];
        sx=sx+x;
        sx2=sx2+x*x;
        sn=sn+1.0;
      }
    }
    mean=sx/sn;
    sig=sqrt(sx2/sn-mean*mean);
    if(trial==0){mnorm=mean,snorm=sig;}
    if(trial==1){mt0=mean,st0=sig;}
    if(trial==2){mp0=mean,sp0=sig;}
    // if(trial==3){mp1=mean,sp1=sig;}
    //if(trial==4){mz1=mean,sz1=sig;}
    if(trial==3){mchi2=mean,schi2=sig;}
  }
  printf(" norm %f %f \n",mnorm,snorm);
  printf(" t0 %f %f \n",mt0,st0);
  printf(" p0 %f %f \n",mp0,sp0);
  // printf(" p1 %f %f \n",mp1,sp1);
  // printf(" z1 %f %f \n",mz1,sz1);
  printf(" chi2 %f %f \n",mchi2,schi2);
  fpout=fopen("/home/dcfebcheck/data/standards/test11_averages.txt","a");
  fprintf(fpout,"%s %f: %f %f %f %f\n",timeheader(),t,mnorm,mt0,mp0,mchi2);
  fclose(fpout); 
  fclose(fpin);
}  
*/      

int DCFEBCheck::TestComparatorDAC(FILE *fp){
  int i,ierr,err2,pass;
  float v0,vout,diff,diff2;
  //
  double sn,sx,sy,sxy,sx2;
  float x[10],y[10];
  float a,b;
  ierr=0;
  //
  // check comparator DAC/ADC
  //
  fprintf(fp,"check comparator DAC/ADC\n");
  err2=0;
  for(i=0;i<10;i++){
      //
      v0=0.25*i;
      thisDCFEB_->set_comp_thresh(v0);
      usleep(20000);
      //
      vout=thisODMB_->adcplus(2,dcfeb_index);
      printf(" vout %f\n",vout);
      // if(i>0){
	x[i]=v0;
	y[i]=vout;
        fprintf(fp,"CFEB= %d %f %f \n",dcfeb_index,v0,vout);
	// }
      //
  }
  //
  sn=0.;
  sx=0.;
  sy=0.;
  sxy=0.;
  sx2=0.;
  //
  for(i=0;i<10;i++){
    sn=sn+1;
    sx=sx+x[i];
    sy=sy+y[i];
    sxy=sxy+x[i]*y[i];
    sx2=sx2+x[i]*x[i];
  }
  //
  a=(sxy*sn-sx*sy)/(sn*sx2-sx*sx);
  b=(sy*sx2-sxy*sx)/(sn*sx2-sx*sx);
  fprintf(fp," a %f b %f \n",a,b);
  //
  if(a<-1025.||a>-800.){
    ierr=1;
    fprintf(fp," slope out of range - got %f should be -1005 \n",a);
  }
  //
  if(b>3400.||b<3800.){ //changed a>3800 to b>3800
    ierr=1;
    fprintf(fp," intercept out of range- got %f  should be 3600.\n",b);
  }
  //
  for(i=0;i<9;i++){
    diff=y[i]-a*x[i]-b;
    diff2=diff;
    if(diff2<0)diff2=-diff2;
    if(diff2<2.0){
      fprintf(fp," Good - i %d %f %f %f \n",i,y[i],a*x[i]+b,diff);
    }else{
      ierr=1;
      fprintf(fp," Good - i %d %f %f %f \n",i,y[i],a*x[i]+b,diff);
    }
  }
  //
  // end comparator DAQ/ADC
  //
  //
  pass=1; 
  if(ierr!=0)pass=0;
  return pass;
}


void DCFEBCheck::playsound(int i){
std::string nam[36]={
"acess.wav","analogerr.wav","badboard.wav","board2board.wav","buckeye.wav",
"capped.wav","chanlink.wav","clap.wav","denial.wav","enter.wav","epromname.wav",
"eprom.wav","errorfile.wav","exit.wav","explode.wav","finish.wav","gain.wav",
"inputshorts.wav","input.wav","jtag.wav","kill.wav","labelerr.wav","label.wav",
"linearity.wav","medcap.wav","musicstart.wav","pedestal.wav","prescap.wav",
"rms.wav","scanerror.wav","scanit.wav","short.wav","smallcap.wav","tada.wav",
"virtex.wav","wait.wav"};
 std::string path="/home/dcfebcheck/sound/";
 std::string command="xmms ";
 std::string play=command+path+nam[i]+"&";
 std::cout << " sound command: " << play << std::endl;
 int ierr=system(play.c_str());
}

int DCFEBCheck::OpticalUnpack(){
  short int data[3200];
  int crcprob=0;
  int calcCRC;
  for(int time=0;time<NSAMPLE;time++){
   calcCRC=0;
   for(int adc=0;adc<96;adc++){ 
     int adc2=adc+time*100;
     data[adc2]=((daq->rbuf[2*adc2+1]<<8)&0xff00)|(daq->rbuf[2*adc2]&0x00ff);
     calcCRC=(data[adc2]&0x1fff)^((data[adc2]&0x1fff)<<1)^(((calcCRC&0x7ffc)>>2)|((0x0003&calcCRC)<<13))^((calcCRC&0x7ffc)>>1);
     int mod=adc%6;
     int bit=adc/6;     
     rawadc[mod][bit][time]=data[adc2]&0x1fff;
   }
   if(calcCRC!=data[96+time*100])crcprob++;
 } 
  return crcprob;
}

char * DCFEBCheck::timeheader(){
  curt_=time(NULL);
  loct_=localtime(&curt_);
  sprintf(thistime,"%s_M%02dD%02dY%04dH%02d:%02d",cbrdnum_,loct_->tm_mon+1,loct_->tm_mday,loct_->tm_year+1900,loct_->tm_hour,loct_->tm_min);
  return thistime;
}



}}
XDAQ_INSTANTIATOR_IMPL(emu::pc::DCFEBCheck)


