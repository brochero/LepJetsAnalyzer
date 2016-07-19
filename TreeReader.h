#include<string>
#include<iostream>
#include<sstream>
#include<stdio.h>
#include<stdlib.h>
#include<cmath>
#include<set>
#include<vector>
#include <sys/stat.h>


// Root
#include "TString.h"
#include "TRegexp.h"
#include "TDirectory.h"
#include "TROOT.h"
#include "TObject.h"
#include "TCanvas.h"
#include "TStopwatch.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TProfile.h"
#include "TFile.h"
#include "TChain.h"
#include "TLorentzVector.h"
#include "TSystem.h"
#include "TTree.h"
//#include "TKey.h"
//#include "TPrint.h"
//#include <exception>
#include "TError.h"

// TopCode
#include <SFIDISOTrigger.h> // SF_ID-ISO-Trigger
#include <ttbar_category.h> // Event Categorization
#include <SFLumi.h>         // Normalization SF
#include <LepJets_Fitter.h> // Kinematic Fit


// Jet Class -> Defined in LepJets_Fitter.h
//class ComJet: public TLorentzVector{
//public:
//  float CSV, CvsL, CvsB;
//  int Flavour, pTIndex, Mom;
//};

// b-tagging SF
struct btagUnc{
  enum:unsigned int{CENTRAL=0,
      JES_UP,     JES_DN,
      LF_UP,       LF_DN,
      HF_UP,       HF_DN,
      HFSTAT1_UP,  HFSTAT1_DN, 
      HFSTAT2_UP,  HFSTAT2_DN,
      LFSTAT1_UP,  LFSTAT1_DN, 
      LFSTAT2_UP,  LFSTAT2_DN,
      CFERR1_UP,   CFERR1_DN, 
      CFERR2_UP,   CFERR2_DN};
};

void print_progress(int TreeEntries, Long64_t ievt);
const TString currentDateTime();
float DiJetMassCorrection(std::vector<ComJet> &Jets, bool ReArrange);
bool IsSelectedttbarCategory(std::vector<int> *GenConeCat, TString ttbar_id);


