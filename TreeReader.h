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
#include "TH1D.h"
#include "TH2D.h"
#include "TProfile.h"
#include "TEfficiency.h"
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
#include <SFLumi.h>         // Normalization SF
#include <ttbar_category.h> // Event Categorization

// Jet Class
class ComJet: public TLorentzVector{
 public:
  float CSV, JES, Gen_pT, Gen_DR;
  int Flavour, pTIndex, Mom, KinMom, GenIndex;
};

struct JESUnc{
  enum:unsigned int{AbsoluteStat=0, AbsoluteScale, AbsoluteMPFBias,
      Fragmentation, 
      SinglePionECAL, SinglePionHCAL, 
      FlavorQCD,
      TimePtEta, 
      RelativeJEREC1, RelativeJEREC2, RelativeJERHF,
      RelativePtBB, RelativePtEC1, RelativePtEC2, RelativePtHF,
      RelativeBal,
      RelativeFSR,
      RelativeStatFSR, RelativeStatEC, RelativeStatHF,
      PileUpDataMC,
      PileUpPtRef, PileUpPtBB, PileUpPtEC1, PileUpPtHF};
};

const std::vector<TString> JESName = {"AbsoluteStat", "AbsoluteScale", "AbsoluteMPFBias",
				      "Fragmentation", 
				      "SinglePionECAL", "SinglePionHCAL", 
				      "FlavorQCD",
				      "TimePtEta", 
				      "RelativeJEREC1", "RelativeJEREC2", "RelativeJERHF",
				      "RelativePtBB", "RelativePtEC1", "RelativePtEC2", "RelativePtHF",
				      "RelativeBal",
				      "RelativeFSR",
				      "RelativeStatFSR", "RelativeStatEC", "RelativeStatHF",
				      "PileUpDataMC",
				      "PileUpPtRef", "PileUpPtBB", "PileUpPtEC1", "PileUpPtHF"};


// Variation
struct var {
  enum:unsigned int{Nom=0,Up,Down};
};
const std::vector<TString> vVarNames = {"Nominal","Up","Down"};


void print_progress(int TreeEntries, Long64_t ievt);
const TString currentDateTime();
float DiJetMassCorrection(std::vector<ComJet> &Jets, bool ReArrange);
bool IsSelectedttbarCategory(std::vector<int> *GenConeCat, TString ttbar_id);
void UnRoll2D(TH2D *h2D, TH1D *h1D);

// b-tag
// btag SF definition
typedef struct hbtag{
  TH1D     *HF[5];    // 5 pT bins
  TH1D     *LF[4][3]; // 4 pT 3 eta bins
  TH1D     *CF[5];    // 5 pT bins
} hbtag;

typedef struct btagCollection{
  hbtag Central;
  hbtag SystJES[25][3]; // 25 JES Variations + Nominal(Empty) Up/Down
  hbtag Syst[8][3];     // 3 Nominal (LF) - 5 HF Variations + Nominal(Empty)  Up/Down
                        // 3 LF Variations - 5 Nominal (HF CF) + Nominal(Empty) Up/Down
}btagCollection;
// btag names (SAME ORDER THAN Jetbtag.cc vectors)
const std::vector<TString> vbtgNames = {"HF","HFStats1","HFStats2",
                                        "LF","LFStats1","LFStats2","cErr1","cErr2"};

// b-tag histo structure
const int dummypT_bin = 5, dummyeta_bin = 3;
const double dummypT_binsize[dummypT_bin+1]   = {20., 30., 40., 60., 100., 10000.};
const double dummyeta_binsize[dummyeta_bin+1] = {0., 0.8, 1.6, 2.4};

