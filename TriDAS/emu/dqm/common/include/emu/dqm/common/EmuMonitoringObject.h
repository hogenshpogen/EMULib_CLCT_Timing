#ifndef EmuMonitoringObject_h
#define EmuMonitoringObject_h


#include <iostream>
#include <string>
#include <map>
#include <string>
#include <iomanip>
#include <set>
#include <sstream>
#include <algorithm>

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOMNodeList.hpp>

using namespace XERCES_CPP_NAMESPACE;

// ==  ROOT Section
#include <TROOT.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TProfile.h>
#include <TProfile2D.h>
#include <TString.h>
#include <TFile.h>
#include <TRandom.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TPaveStats.h>
#include <TColor.h>
#include <TPaletteAxis.h>
#include <TClass.h>
#include <TPRegexp.h>

#include "DQM/RenderPlugins/src/CSCRenderPlugin_ChamberMap.h"
#include "DQM/RenderPlugins/src/CSCRenderPlugin_SummaryMap.h"
#include "DQM/RenderPlugins/src/CSCRenderPlugin_EventDisplay.h"

//#include "EmuMonitorElement.h"
//class MonitorElement: public TH1 {};
//typedef EmuMonitorElement MonitorElement;

typedef TH1 MonitorElement;
class EmuMonitoringObject;

typedef std::map<std::string, EmuMonitoringObject*> ME_List;
typedef ME_List::iterator ME_List_iterator;
typedef ME_List::const_iterator ME_List_const_iterator;

#define DEF_HISTO_COLOR 48
#define REMATCH(pat, str)              (TPRegexp(pat).MatchB(str))
#define REREPLACE(pat, str, rep)       { TString s(str); TPRegexp(pat).Substitute(s, rep); str = s; }
#define REREPLACEM(pat, str, rep, mod) { TString s(str); TPRegexp(pat).Substitute(s, rep, mod); str = s; }

class EmuMonitoringObject
{

public:
  EmuMonitoringObject();
  // == Copy constructor
  EmuMonitoringObject(const EmuMonitoringObject&);

//	EmuMonitoringObject();
  EmuMonitoringObject(DOMNode *info);
  ~EmuMonitoringObject();
  EmuMonitoringObject& operator=(const EmuMonitoringObject&);
  bool operator<( const EmuMonitoringObject& s1)
  {
    return (getFullName()<s1.getFullName());
  };
  bool operator>(const EmuMonitoringObject& s1)
  {
    return (getFullName()>s1.getFullName());
  };
  bool operator==(const EmuMonitoringObject& s1)
  {
    return (getFullName()==s1.getFullName());
  };

  int Book();
  int Book(DOMNode *info);
  int Fill(double);
  // can be used with 2D (x,y) or 1D (x, w) histograms
  int Fill(double, double);
  // can be used with 3D (x, y, z) or 2D (x, y, w) histograms
  int Fill(double, double, double);
  // can be used with 3D (x, y, z, w) histograms
  int Fill(double, double, double, double);

  void Reset();

  MonitorElement* getObject()
  {
    return object;
  }
  void setPrefix(std::string);
  std::string getPrefix() const
  {
    return prefix;
  }
  std::string getName() const
  {
    return name;
  }
  void setName(std::string);
  std::string getTitle() const
  {
    return title;
  }
  void setTitle(std::string);
  std::string getFolder() const
  {
    return folder;
  }
  void setFolder(std::string new_folder)
  {
    folder=new_folder;
  }
  int setParameter(std::string, std::string);
  std::string getParameter(std::string);
  int setParameters(std::map<std::string, std::string>, bool resetParams = true);
  std::map<std::string, std::string>getParameters() const
  {
    return params;
  }
  std::string getFullName() const
  {
    return type+"_"+prefix+"_"+name;
  }

  time_t getTimeStamp() const
  {
    return timestamp;
  }

  void setTimeStamp()
  {
    timestamp = time(NULL);
  }
  void setTimeStamp(time_t tstamp) 
  {
    timestamp = tstamp;
  }  

  void SetEntries(double);
  void SetBinContent(int, double);
  void SetBinContent(int, int, double);
  double GetBinContent(int);
  double GetBinContent(int, int);
  void SetAxisRange(double, double, std::string);

  void Write()
  {
    if (object!=NULL)
      {
        object->Write();
      }
  }

  void applyPadProperties();
  void Draw();// {if (object!=NULL) object->Draw();}

  int setObject(MonitorElement* hist);
  int applyParameters();
  int Add(MonitorElement* hist);

//	DOMNode * getDOMInfo();
// 	void setDOMInfo(DOMNode *info);

private:

  int parseDOMNode(DOMNode* info);

protected:

  MonitorElement* object;
  std::map<std::string, std::string>params;
  std::string type;
  std::string prefix;
  std::string name;
  std::string title;
  std::string folder;
  time_t timestamp;
};

/*
bool operator<(const EmuMonitoringObject& s1, const EmuMonitoringObject& s2)
		{return (s1.getFullName()<s2.getFullName());};
bool operator>(const EmuMonitoringObject& s1, const EmuMonitoringObject& s2)
                {return (s1.getFullName()>s2.getFullName());};
bool operator==(const EmuMonitoringObject& s1, const EmuMonitoringObject& s2)
                {return (s1.getFullName()==s2.getFullName());};
*/


#endif
