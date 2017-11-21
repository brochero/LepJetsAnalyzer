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

TString JESName[25] = {"AbsoluteStat", "AbsoluteScale", "AbsoluteMPFBias",
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


void print_progress(int TreeEntries, Long64_t ievt);
const TString currentDateTime();
float DiJetMassCorrection(std::vector<ComJet> &Jets, bool ReArrange);
bool IsSelectedttbarCategory(std::vector<int> *GenConeCat, TString ttbar_id);
void UnRoll2D(TH2D *h2D, TH1D *h1D);

// Luminosity per channel (due to trigger PreScale)
float LuminNorm_Mu = 35870; //[pb-1]
float LuminNorm_El = 35870; //[pb-1]

// Output Dir
TString dirname="TopResults";
// Number of Histograms
const unsigned int Nhcuts = 7;
const unsigned int Nhch   = 2;
const unsigned int NhJets = 6;
// Channel and cut names 
TString namech     [Nhch + 1] = {"mujets","ejets","lepjets"};
TString titlenamech[Nhch]     = {"#mu+Jets","e+Jets"};
TString namecut    [Nhcuts]   = {"lepton","6Jets","2btag","3btag","4Jets","4Jets2btag","Only2btag"};
// Acceptancies and Efficiencies
int    AccEvent[Nhcuts][Nhch+1];
double EffEvent[Nhcuts][Nhch+1];
// Histograms definitions
typedef TH1D *HistosJet    [NhJets][Nhcuts][Nhch];
typedef TH2D *HistosJet2D  [NhJets][Nhcuts][Nhch];
typedef TH1D *HistosDiJet  [NhJets][NhJets-1][Nhcuts][Nhch];
typedef TH2D *HistosDiJet2D[NhJets][NhJets-1][Nhcuts][Nhch];
typedef TH1D *Histos       [Nhcuts][Nhch];
typedef TH2D *Histos2D     [Nhcuts][Nhch];
typedef TEfficiency *Eff   [Nhcuts][Nhch];
typedef TProfile    *Prof  [Nhcuts][Nhch];

//----------------------------------------
//            Histograms
//----------------------------------------
// Event variables
Histos hPV;
Histos hMET, hMET_Phi, hHT;
Histos hmT;
Histos hNJets, hNBtagJets;
// Categorization
Eff effCatHiggs;
// Lepton
Histos hLepPt, hLepEta, hLepPhi;
// Jets
HistosJet hCSV, hJetPt, hJetpTUncVar;
HistosJet hCvsL, hCvsB;
HistosDiJet2D h2DCSV;
HistosDiJet hMassJet, hDRJet;
Histos hInvMassjj;
// Lepton, trigger SF and b-tag eff.
Histos   hSFIDISOTr, hSFIDISOTrError, hSFIDISO, hSFIDISOError, hSFTrigger, hSFTriggerError;
Histos2D h2DSFbtag_Global;
Histos   hSFbtag_Global, hSFbtag_Global_var;
Histos2D h2DSFbtag_b, h2DSFbtag_c, h2DSFbtag_l, h2DSFbtag_btag_b, h2DSFbtag_btag_c, h2DSFbtag_btag_l;
Prof     pSFCSVVsCSVAll, pSFCSVErrorVsCSVAll;
// PDF Weights
Histos hWPDF, hWPDFAlphaUp, hWPDFAlphaDown;
// Kinematic Fitter
Histos    hKinChi2;
Histos2D  h2DKinChi2_JetMatch;
Histos    hKinWlTransMass, hKinWlMass, hKinWlpT, hKintlMass, hKintlpT;
Histos    hKinWhMass, hKinWhpT, hKinthMass, hKinthpT;
Histos    hKinWMass, hKinWpT, hKinTagWMass, hKinTagAddMass, hKinTagAddDR;
Eff       effKinGenIndex, purKinGenIndex, effKinGenIndexVsChi2;
HistosJet hKinJetPt, hGENJetPt;
Histos    hKinAdd1CSV, hKinAdd2CSV, hKinAdd12CSV, hKinAddCSVUnroll;
Histos    hKinAdd1CSV_30, hKinAdd2CSV_30, hKinAdd12CSV_30, hKinAddCSVUnroll_30;
Histos    hKinAdd1CSV_40, hKinAdd2CSV_40, hKinAdd12CSV_40, hKinAddCSVUnroll_40;
Histos2D  h2DKinAddCSV, h2DKinAddCSV_30, h2DKinAddCSV_40;
// GenCone
Histos   hTJetPosition, hWJetPosition, hOJetPosition;
Histos   hGenTagWMass, hGenTagAddMass, hGenTagAddDR, hGenTagAdd1CSV, hGenTagAdd2CSV, hGenTagAdd12CSV;
Histos2D h2DGenTagAddCSV;
Eff    effTagCSV, purTagCSV;
Prof pSFCSVVsCSVAdd, pSFCSVUpVsCSVAdd, pSFCSVDownVsCSVAdd;
TH2D *h2DTJetPosition, *h2DWJetPosition, *h2DttbarNGenJets;
//----------------------------------------
//           Tree branches
//----------------------------------------
// Global event
int Event,Run,Channel, GoodPV;
float PUWeight, GENWeight;
std::vector<float> *PUWeight_sys=0;
// PDF
std::vector<float> *PDFWeight=0;
// MET
float MET,MET_Phi;
// Leptons
float Lep_pT, Lep_eta, Lep_phi, Lep_E;
float Lep_LES=0;
// Jets
std::vector<float> *Jet_pT=0, *Jet_eta=0, *Jet_phi=0, *Jet_E=0;
std::vector<int>   *Jet_partonFlavour=0;
std::vector<int>   *Jet_pTIndex=0;
std::vector<int>   *Jet_GENmatched=0;
std::vector<int>   *Jet_Mom=0; // From GenCone
std::vector<float> *Jet_CSV=0;
std::vector<float> *Jet_SF_CSV=0;
std::vector<float> *Jet_SF_CSVg=0;
std::vector<float> *Jet_CvsB=0, *Jet_CvsL=0;
std::vector<float> *Jet_JER_Up=0, *Jet_JER_Nom=0, *Jet_JER_Down=0;
std::vector<float> *Jet_JES_Up=0, *Jet_JES_Down=0;
std::vector< std::vector<float> > *Jet_JESCom_Up=0, *Jet_JESCom_Down=0;

// ttH Categorization
int  GenttHCat;
// GenCone Info
std::vector<int>    *GenConeCat=0;
std::vector <float> *GenCone_pT=0;
std::vector <float> *GenCone_eta=0;
std::vector <float> *GenCone_phi=0;
std::vector <float> *GenCone_E=0;
std::vector <int>   *GenCone_gjetMom=0; // From GenCone
int GenCone_NgjetsW;
float DRAddJets;
// Gen Info
float GenLep_pT;
std::vector<float> *GenJet_pT=0, *GenJet_eta=0, *GenJet_phi=0, *GenJet_E=0;
float GenNu_pT, GenNu_eta, GenNu_phi, GenNu_E;
// Scale Syst. Unc.
std::vector<float> *ScaleWeight=0;
// Kinematic Reconstruction
float Kin_Chi2;
std::vector<int>   *KinJet_Index=0;
std::vector<float> *KinJet_pT=0, *KinJet_eta=0, *KinJet_phi=0, *KinJet_E=0;
float KinNu_pT, KinNu_eta, KinNu_phi, KinNu_E;
