//***************************************************************************************
// ConsumerCanvas.hh 
//
// Description: canvas for the consumers
//
//  
// Author List: 
//   Hartmut Stadie  IEKP Karlsruhe     
//======================================================================================= 
//****************************************************************************
// RCS Current Revision Record
//-----------------------------------------------------------------------------
// $Source: /afs/cern.ch/project/cvs/reps/tridas/TriDAS/emu/dqm/common/include/emu/dqm/common/ConsumerCanvas.hh,v $
// $Revision: 1.1 $
// $Date: 2009/07/20 21:51:15 $
// $Author: barvic $
// $State: Exp $
// $Locker:  $
//*****************************************************************************


#ifndef CONSUMERCANVAS_HH
#define CONSUMERCANVAS_HH 1

#define DEF_CANVAS_WIDTH 1280 
#define DEF_CANVAS_HEIGHT 1024

#include "TCanvas.h"

class TPad;
class TPaveLabel;

class ConsumerCanvas : public TCanvas
{
public:
  ConsumerCanvas(const char* name, const char* title, const char* title2, int nx, int ny, int width = DEF_CANVAS_WIDTH, int height = DEF_CANVAS_HEIGHT);
  virtual ~ConsumerCanvas();
    
//  virtual void cd(Int_t subpadnumber = 0);
  virtual void cd(Int_t subpadnumber = 0, Int_t gridX = 0, Int_t gridY = 0);
  virtual void Streamer(TBuffer& b);
  virtual void Divide(Int_t nx = 1, Int_t ny = 1, Float_t xmargin = 0.005, Float_t ymargin = 0.005, 
		      Int_t color = 0);
  virtual TPad* GetPad(Int_t subpadnumber = 0);
  int GetNumPads() const { return _n;}
  int GetNumPadsX() const { return _nx;}
  int GetNumPadsY() const { return _ny;}
  void SetRunNumber(const char* num) {runNumber=num;}
  void Draw(); 

private:

  TPad** _pads;
  TPaveLabel *_title,*_counter;
  int _n;
  int _nx;
  int _ny;
  TString runNumber;
};

#endif
