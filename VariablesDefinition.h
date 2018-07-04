
// Luminosity per channel (due to trigger PreScale)
float LuminNorm_Mu = 35870; //[pb-1]
float LuminNorm_El = 35870; //[pb-1]

// Output Dir
TString dirname="TopResults";
// Number of Histograms
const unsigned int Nhcuts = 3;
const unsigned int Nhch   = 2;
const unsigned int NhJets = 6;
// Channel and cut names 
TString namech     [Nhch + 1] = {"mujets","ejets","lepjets"};
TString titlenamech[Nhch]     = {"#mu+Jets","e+Jets"};
TString namecut    [Nhcuts]   = {"4Jets","6Jets","2btag"};
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
// Global Variables
Histos hPV, hMET, hMET_Phi;
// PDF (and Alpha_s)
Histos hWPDF, hWPDFAlphaUp, hWPDFAlphaDown, hWhdamp;
// SF(ID,ISO & Trigger)
Histos   hSFIDISOTr, hSFIDISOTrError, hSFIDISO, hSFIDISOError, hSFTrigger, hSFTriggerError;
// SF(b-tag)
Histos   hSFbtag_Global;
Prof     pSFCSVVsCSV, pSFCSVVsCSVAdd;
// Lepton Variables
Histos hLepPt, hLepEta, hLepPhi, hmT;
// Jets Variables
HistosJet hJetPt, hCSV;
// Jet Multiplicity
Histos hNJets, hNBtagJets;
/***************************
 Kinematic Reconstruction
***************************/
// Global
Histos hKinChi2,hKinProb;
// Jets
HistosJet hKinJetPt;
// W leptonic
Histos hKinWlTransMass, hKinWlMass, hKinWlpT;
// W hadronic
Histos hKinWhMass, hKinWhpT;
// Top leptonic
Histos hKintlMass, hKintlpT;
// Top hadronic
Histos hKinthMass, hKinthpT;
// Additional Jets
Histos hKinTagAddMass, hKinTagAddDR;
// CSV for Additional Jets 
Histos hKinAdd1CSV, hKinAdd2CSV, hKinAddCSVUnroll;
Histos2D  h2DKinAddCSV;

//----------------------------------------
//           Tree branches
//----------------------------------------
// Global event
int Event,Run,Channel, GoodPV;
float PUWeight, GENWeight, Rho;
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
std::vector<int>   *Jet_partonFlavour=0, *Jet_hadronFlavour=0;
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
std::vector <int>   *GenCone_Mom=0; // From GenCone
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
// hdamp Syst. Unc.
std::vector<float> *hdampWeight=0;


