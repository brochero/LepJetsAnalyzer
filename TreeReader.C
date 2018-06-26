#ifndef __CINT__
#include "TreeReader.h"
#include "VariablesDefinition.h"
#endif

#ifndef __CINT__

// JER
#include "JER/JER_806/JetResolution.h"
#include "JER/JER_806/JetResolutionObject.h"

void JERCorrection(JME::JetResolutionScaleFactor res_sf, JME::JetResolution resolution, float rho, std::vector<ComJet> *jets, int nSystVar);
void JESCorrection(std::vector<ComJet> *jets);
// btag
btagCollection SetupbtagSF(TString FileName_HF, TString FileName_LF);
float GetbtagSF(hbtag SF, ComJet jet);
float GetbtagEvtWeight(hbtag SF, std::vector<ComJet> jets);
// Id for Systematics
int FindSysID(const std::vector<TString> Names, TString Name);

void display_usage()
{
  std::cout << "\033[1;37musage:\033[1;m skimfile cutindex [options]" << std::endl;
  std::cout << "" << std::endl;
  std::cout << "Options:" << std::endl;
  std::cout << "    -i inputfile  Input file without .root" << std::endl;
  std::cout << "    -eos input files located in eos lxplus" << std::endl;
  std::cout << "    -o name in the output file \"h_\"" << std::endl;
  std::cout << "    -n number of events" << std::endl;
  std::cout << "    -s create a file with the systematic uncertainty yields: \"variation name\"" << std::endl;
  // std::cout << "    -JES create a file with the JEX systematic uncertainty yields: \"variation name\"" << std::endl;
  std::cout << "    -cat ttbar categorization" << std::endl;
  std::cout << "    -d Input file directory. Default directory: InputTrees" << std::endl;
  std::cout << "    -h                 displays this help message and exits " << std::endl;
  std::cout << "" << std::endl;
}


int main(int argc, const char* argv[]){

  gErrorIgnoreLevel = kError;
  
  gSystem->Load("libTree");
  gROOT->ProcessLine("#include <vector>");
  
  bool _eos       = false;
  bool _ttbar_cat = false;
  bool _syst      = false;
  int  _NUserEvt  = 0;
  const char * _output   = 0;
  const char * _input    = 0;
  // TopTrees directory
  const char * _dir      = "/xrootd/store/user/brochero/v8-0-6/";
  const char * _syst_name = 0;
  const char * _syst_var  = 0;
  const char * _ttbar_id = 0;
  
  // Arguments used
  if(argc == 1){
    display_usage();
    return -1;
  }
  
  else{
    //Argumet 1 must be a valid input fileName
    for (int i = 1; i < argc; i++){
      if( strcmp(argv[i],"-i") == 0 ){
	_input = argv[i+1];
	i++;
      }
      if( strcmp(argv[i],"-eos") == 0 ){
	_eos = true;
      }
      if( strcmp(argv[i],"-d") == 0 ){
	_dir = argv[i+1];
	i++;
      }
      if( strcmp(argv[i],"-o") == 0 ){
	_output= argv[i+1];
	i++;
      }
      if( strcmp(argv[i],"-n") == 0 ){
	_NUserEvt= atoi(argv[i+1]);
	i++;
      }
      if( strcmp(argv[i],"-s") == 0 ){
	_syst= true;
	_syst_name = argv[i+1];
	_syst_var  = argv[i+2];
	i+=2;
      }
      if( strcmp(argv[i],"-cat") == 0 ){
	_ttbar_cat = true;
	_ttbar_id  = argv[i+1];
	i++;
      }
      if( strcmp(argv[i],"-h") == 0 ||
	  strcmp(argv[i],"--help") == 0 ){
	display_usage();
	return 0;
      }
    }
  }//else
  if( _input ==0 ){
    std::cerr << "\033[1;31mskimfile ERROR:\033[1;m The '-i' option is mandatory!"
	      << std::endl;
    display_usage();
    return -1;
  }
  
  // reassigning
  TString fname(_input);
  TString hname(_output);
  TString syst_name = "";
  TString syst_var = "";
  TString syst_varname = "";
  TString fdir(_dir);
  TString ttbar_id(_ttbar_id);


  if (_syst){
    syst_name = _syst_name;
    syst_var  = _syst_var;
    syst_varname = "_" + syst_name + syst_var;
  }

  // eos directory
  if(_eos) fdir = "root://cms-xrdr.sdfarm.kr:1094///xrd/store/user/brochero/v8-0-6/";
  //make a dir if output does not exist!!
  struct stat st;
  dirname += "/" + hname;
  if(stat(dirname,&st) != 0) system("mkdir " + dirname);

  TChain theTree("ttbbLepJets/tree"); 
  
  std::cout << "---------------------------------------------------------------------------------" << std::endl;
  std::cout << "Signal: ";
  std::cout << fname + ".root" << std::endl;

  theTree.Add(fdir + fname + ".root");

  if(!theTree.GetFile()){
    std::cerr << "Input file not found!!!"  << std::endl;
    std::exit(0);
  }
   
 /*********************************
           Tree Branches
  **********************************/  
  theTree.SetBranchAddress( "event",    &Event );
  theTree.SetBranchAddress( "run",      &Run );
  theTree.SetBranchAddress( "channel",  &Channel );
  theTree.SetBranchAddress( "GoodPV",   &GoodPV );

  theTree.SetBranchAddress( "PUWeight", &PUWeight_sys );

  theTree.SetBranchAddress( "MET",      &MET );
  theTree.SetBranchAddress( "MET_phi",  &MET_Phi );

  theTree.SetBranchAddress( "lepton_pT",  &Lep_pT );
  theTree.SetBranchAddress( "lepton_eta", &Lep_eta );
  theTree.SetBranchAddress( "lepton_phi", &Lep_phi );
  theTree.SetBranchAddress( "lepton_E",   &Lep_E );
  theTree.SetBranchAddress( "lepton_LES", &Lep_LES );

  theTree.SetBranchAddress( "jet_pT",    &Jet_pT );
  theTree.SetBranchAddress( "jet_eta",   &Jet_eta );
  theTree.SetBranchAddress( "jet_phi",   &Jet_phi );
  theTree.SetBranchAddress( "jet_E",     &Jet_E );
  theTree.SetBranchAddress( "jet_index", &Jet_pTIndex );

  theTree.SetBranchAddress( "jet_partonFlavour", &Jet_partonFlavour );
  theTree.SetBranchAddress( "jet_hadronFlavour", &Jet_hadronFlavour );
  theTree.SetBranchAddress( "jet_CSV",           &Jet_CSV );
  
  if(!fname.Contains("DataSingle")){
    theTree.SetBranchAddress( "jet_JESCom_Up",  &Jet_JESCom_Up );  
    theTree.SetBranchAddress( "jet_JESCom_Down",&Jet_JESCom_Down );  
    // Rho for JER
    theTree.SetBranchAddress( "jet_JER_Up",     &Jet_JER_Up );
    theTree.SetBranchAddress( "jet_JER_Nom",    &Jet_JER_Nom );
    theTree.SetBranchAddress( "jet_JER_Down",   &Jet_JER_Down );
  }
  
  if(fname.Contains("ttbar") && !fname.Contains("Bkg")){
    
    // PDF Uncertainty
    theTree.SetBranchAddress("pdfweight",   &PDFWeight);
    // hdamp Uncertainty
    theTree.SetBranchAddress("hdampweight", &hdampWeight);
    // Scale Uncertainties
    theTree.SetBranchAddress("scaleweight", &ScaleWeight );
    theTree.SetBranchAddress("rho",         &Rho);
    // Jet Origin
    theTree.SetBranchAddress("jet_gencone_mom",       &Jet_Mom);
    theTree.SetBranchAddress("jet_MatchedGenJetIndex",&Jet_GENmatched);
    
    theTree.SetBranchAddress("genjet_pT",             &GenJet_pT);
    theTree.SetBranchAddress("genjet_eta",            &GenJet_eta);
    theTree.SetBranchAddress("genjet_phi",            &GenJet_phi);
    theTree.SetBranchAddress("genjet_E",              &GenJet_E);
  }
  
  // Kinematic Reconstruction Variables
  theTree.SetBranchAddress("kin_chi2",  &Kin_Chi2);
  // Neutrino
  theTree.SetBranchAddress("kinnu_pT",  &KinNu_pT);
  theTree.SetBranchAddress("kinnu_eta", &KinNu_eta);
  theTree.SetBranchAddress("kinnu_phi", &KinNu_phi);
  theTree.SetBranchAddress("kinnu_E",   &KinNu_E);
  // Jets
  theTree.SetBranchAddress("kinjet_pT",    &KinJet_pT);
  theTree.SetBranchAddress("kinjet_eta",   &KinJet_eta);
  theTree.SetBranchAddress("kinjet_phi",   &KinJet_phi);
  theTree.SetBranchAddress("kinjet_E",     &KinJet_E);
  theTree.SetBranchAddress("kinjet_index", &KinJet_Index);
  
  // Number of Events and Weights (MC@NLO) from Histograms
  TFile *fileEntries = NULL;
  fileEntries = TFile::Open(fdir + fname + ".root");
  TH1D *h_NumberEvt, *h_ScaleWeights;
  h_NumberEvt    = (TH1D*)fileEntries->Get("ttbbLepJets/EventInfo");
  h_ScaleWeights = (TH1D*)fileEntries->Get("ttbbLepJets/ScaleWeights");

  float NTotal_Event, NTotal_Weight, nNorm_Event, NTotal_ScalemuRF_Weight[6];
  NTotal_Event  = h_NumberEvt->GetBinContent(1);
  NTotal_Weight = h_NumberEvt->GetBinContent(2);
  for (unsigned int ibin = 1; ibin< 7; ibin++) NTotal_ScalemuRF_Weight[ibin-1]= h_ScaleWeights->GetBinContent(ibin);

  // Number of events
  int MaxEvt = theTree.GetEntries();
  if(_NUserEvt > 0) MaxEvt = std::min(MaxEvt, _NUserEvt);

  // MCatNLO Weights
  if(fname.Contains("aMCatNLO") || fname.Contains("Sherpa")){
    theTree.SetBranchAddress( "genweight", &GENWeight );
    nNorm_Event = NTotal_Weight;    
    if(_NUserEvt > 0) nNorm_Event = MaxEvt*(NTotal_Weight/theTree.GetEntries());
  }
  else{
    GENWeight = 1.0;
    nNorm_Event = NTotal_Event;
    if(_NUserEvt > 0) nNorm_Event = MaxEvt*(NTotal_Event/theTree.GetEntries());
  }

  /*********************************
             Histograms
  **********************************/
  //Correct Statistical Uncertainty Treatment
  TH1::SetDefaultSumw2(kTRUE);  
    
  for(int j=0; j<Nhcuts; j++){   // Cut
    for(int i=0; i<Nhch; i++){ // Channel
      // Global Variables
      hPV[j][i]      = new TH1D("hPV_"     +namech[i]+"_"+namecut[j]+syst_varname, "PV Distribution "     +titlenamech[i]+";PV",                       30,0,60);
      hMET[j][i]     = new TH1D("hMET_"    +namech[i]+"_"+namecut[j]+syst_varname, "#slash{E}_{T} "       +titlenamech[i]+";#slash{E}_{T}[GeV]",       10,0,200);
      hMET_Phi[j][i] = new TH1D("hMET_Phi_"+namech[i]+"_"+namecut[j]+syst_varname, "#Phi_{#slash{E}_{T}} "+titlenamech[i]+";#Phi_{#slash{E}_{T}}[rad]",16,0,3.2);
      // PDF (and Alpha_s)
      hWPDF[j][i]          = new TH1D("hWPDF_"         +namech[i]+"_"+namecut[j]+syst_varname, "W_{PDF} the 100 weights/Evt "  +titlenamech[i],160,0.6,1.4);    
      hWPDFAlphaUp[j][i]   = new TH1D("hWPDFAlphaUp_"  +namech[i]+"_"+namecut[j]+syst_varname, "W_{#alpha_{s}^{Up}} per Evt "  +titlenamech[i],160,0.6,1.4);    
      hWPDFAlphaDown[j][i] = new TH1D("hWPDFAlphaDown_"+namech[i]+"_"+namecut[j]+syst_varname, "W_{#alpha_{s}^{Down}} per Evt "+titlenamech[i],160,0.6,1.4);    
      hWhdamp[j][i]        = new TH1D("hWhdamp_"       +namech[i]+"_"+namecut[j]+syst_varname, "W_{hdamp} "                    +titlenamech[i],200,0.,2.);    
      // SF(ID,ISO & Trigger)
      hSFIDISOTr[j][i]      = new TH1D("hSFIDISOTr_"     +namech[i]+"_"+namecut[j]+syst_varname, "SF_{ID,ISO}^{Tr} "       +titlenamech[i],80,0.8,1.2);    
      hSFIDISOTrError[j][i] = new TH1D("hSFIDISOTrError_"+namech[i]+"_"+namecut[j]+syst_varname, "#Delta SF_{ID,ISO}^{Tr} "+titlenamech[i],20,0,0.02); 
      hSFIDISO[j][i]        = new TH1D("hSFIDISO_"       +namech[i]+"_"+namecut[j]+syst_varname, "SF_{ID,ISO} "            +titlenamech[i],80,0.8,1.2);    
      hSFIDISOError[j][i]   = new TH1D("hSFIDISOError_"  +namech[i]+"_"+namecut[j]+syst_varname, "#Delta SF_{ID,ISO} "     +titlenamech[i],20,0,0.02); 
      hSFTrigger[j][i]      = new TH1D("hSFTrigger_"     +namech[i]+"_"+namecut[j]+syst_varname, "SF^{Trigger} "           +titlenamech[i],80,0.8,1.2);    
      hSFTriggerError[j][i] = new TH1D("hSFTriggerError_"+namech[i]+"_"+namecut[j]+syst_varname, "#Delta SF^{Trigger} "    +titlenamech[i],20,0,0.02);
      // SF(b-tag)
      hSFbtag_Global[j][i]  = new TH1D("hSFbtag_Global_"+namech[i]+"_"+namecut[j]+syst_varname, "Global SF_{b-tag} " + titlenamech[i] + ";SF_{b-tag}",40,0.0,4.0);

      pSFCSVVsCSV[j][i]     = new TProfile("pSFCSVVsCSV_"   +namech[i]+"_"+namecut[j]+syst_varname, "Global SF_{b-tag}"           +titlenamech[i]+";CSV;SF_{b-tag}",20,0.0,1.0,0.0,2.0);
      pSFCSVVsCSVAdd[j][i]  = new TProfile("pSFCSVVsCSVAdd_"+namech[i]+"_"+namecut[j]+syst_varname, "Global SF_{b-tag} Add. Jets "+titlenamech[i]+";CSV;SF_{b-tag}",20,0.0,1.0,0.0,2.0);
      // Lepton Variables
      hLepPt [j][i] = new TH1D("hLepPt_" +namech[i]+"_"+namecut[j]+syst_varname, "Lepton p_{T} "              +titlenamech[i]+";Lepton p_{T}[GeV]",20,0.0,200.0);
      hLepEta[j][i] = new TH1D("hLepEta_"+namech[i]+"_"+namecut[j]+syst_varname, "#eta_{Lep} "                +titlenamech[i]+";Lepton #eta",      12,0.0,2.2);
      hLepPhi[j][i] = new TH1D("hLepPhi_"+namech[i]+"_"+namecut[j]+syst_varname, "#phi_{Lep} "                +titlenamech[i]+";Lepton #Phi[rad]", 16,0.0,3.2);
      hmT[j][i]     = new TH1D("hmT_"    +namech[i]+"_"+namecut[j]+syst_varname, "transverse Mass Lepton/MET "+titlenamech[i]+";m_{T}[GeV]",       40,0,160);
      // Jets Variables 
      for(int ij=0; ij<NhJets; ij++){
	TString jetn;
	std::ostringstream jni;
	jni << ij;
	jetn = "Jet-" + jni.str();	
	
	hJetPt[ij][j][i] = new TH1D("hJetPt_" +jetn+"_" +namech[i]+"_"+namecut[j]+syst_varname, "p_{T}^{Jet} "+jetn+" "+titlenamech[i]+";p_{T}[GeV]",40,0,200);		
	hCSV[ij][j][i]   = new TH1D("hCSV_"   +jetn+"_" +namech[i]+"_"+namecut[j]+syst_varname, "CSV "        +jetn+" "+titlenamech[i]+";CSVv2",     20,0,1);
      }
      // Jet Multiplicity
      hNJets[j][i] = new TH1D("hNJets_"+namech[i]+"_"+namecut[j]+syst_varname, "Jet multiplicity "+titlenamech[i]+";Number of jets",9,-0.5,8.5);
      hNJets[j][i]->GetXaxis()->SetBinLabel(1,"0");
      hNJets[j][i]->GetXaxis()->SetBinLabel(2,"1");
      hNJets[j][i]->GetXaxis()->SetBinLabel(3,"2");
      hNJets[j][i]->GetXaxis()->SetBinLabel(4,"3");
      hNJets[j][i]->GetXaxis()->SetBinLabel(5,"4");
      hNJets[j][i]->GetXaxis()->SetBinLabel(6,"5");
      hNJets[j][i]->GetXaxis()->SetBinLabel(7,"6");
      hNJets[j][i]->GetXaxis()->SetBinLabel(8,"7");
      hNJets[j][i]->GetXaxis()->SetBinLabel(9,"#geq 8");
      if(namecut[j] != "lepton") hNJets[j][i]->GetXaxis()->SetRange(7,9);
      if(namecut[j].Contains("4Jets")) hNJets[j][i]->GetXaxis()->SetRange(5,9);
      // b-Jet Multiplicuty
      hNBtagJets[j][i] = new TH1D("hNBtagJets_"+namech[i]+"_"+namecut[j]+syst_varname, "b-tag jet multiplicity "+titlenamech[i]+";Number of b-jets",6,-0.5,5.5);
      hNBtagJets[j][i]->GetXaxis()->SetBinLabel(1,"0");
      hNBtagJets[j][i]->GetXaxis()->SetBinLabel(2,"1");
      hNBtagJets[j][i]->GetXaxis()->SetBinLabel(3,"2");
      hNBtagJets[j][i]->GetXaxis()->SetBinLabel(4,"3");
      hNBtagJets[j][i]->GetXaxis()->SetBinLabel(5,"4");
      hNBtagJets[j][i]->GetXaxis()->SetBinLabel(6,"#geq 5");
      if(namecut[j].Contains("2btag")) hNBtagJets[j][i]->GetXaxis()->SetRange(3,6);
      if(namecut[j].Contains("3btag")) hNBtagJets[j][i]->GetXaxis()->SetRange(4,6);

      /***************************
        Kinematic Reconstruction
      ***************************/
      // Global
      hKinChi2 [j][i] = new TH1D("hKinChi2_"+namech[i]+"_"+namecut[j]+syst_varname, "#chi^{2} for Kin. RECO "   +titlenamech[i]+";#chi^{2}",40,0,20);
      hKinProb [j][i] = new TH1D("hKinProb_"+namech[i]+"_"+namecut[j]+syst_varname, "Probability for Kin. RECO "+titlenamech[i]+";P",       20,0,1.0);
      // Jets 
      TString kinJetname[4];
      kinJetname[0] = "bFromH"; kinJetname[1] = "W1"; kinJetname[2] = "W2";  kinJetname[3] = "bFromL";
      for(unsigned int ikj=0; ikj<4; ikj++)
	hKinJetPt[ikj][j][i] = new TH1D ("hKinJetPt_"+kinJetname[ikj]+"_" +namech[i]+"_"+namecut[j]+syst_varname, "pT KIN Assignment "+titlenamech[i]+"; p_{T}^{j}[GeV]", 50,0,150);
      // W leptonic
      hKinWlTransMass[j][i] = new TH1D("hKinWlTransMass_"+namech[i]+"_"+namecut[j]+syst_varname, "Inv. Trans. Mass of W(lep) from Kin Reco "+titlenamech[i]+"; M_{T}^{W#to l#nu} [GeV]",50,0,150);
      hKinWlMass[j][i]      = new TH1D("hKinWlMass_"     +namech[i]+"_"+namecut[j]+syst_varname, "Inv. Mass of W(lep) from Kin Reco "       +titlenamech[i]+"; M_{W#to l#nu} [GeV]",    80,40,120);
      hKinWlpT[j][i]        = new TH1D("hKinWlpT_"       +namech[i]+"_"+namecut[j]+syst_varname, "p_{T} of W(lep) from Kin Reco "           +titlenamech[i]+"; p_{T} [GeV]",            30,0,300);
      // W hadronic
      hKinWhMass[j][i] = new TH1D("hKinWhMass_"+namech[i]+"_"+namecut[j]+syst_varname, "Inv. Mass of W(had) from Kin Reco "+titlenamech[i]+"; M_{W#to jj} [GeV]",80,40,120);
      hKinWhpT[j][i]   = new TH1D("hKinWhpT_"  +namech[i]+"_"+namecut[j]+syst_varname, "p_{T} of W(had) from Kin Reco "    +titlenamech[i]+"; p_{T} [GeV]",      30,0,300);
      // Top leptonic 
      hKintlMass[j][i] = new TH1D("hKintlMass_"+namech[i]+"_"+namecut[j]+syst_varname, "Inv. Mass of Top(lep) from Kin Reco "+titlenamech[i]+"; M_{t_{l}} [GeV]",100,100,300);
      hKintlpT[j][i]   = new TH1D("hKintlpT_"  +namech[i]+"_"+namecut[j]+syst_varname, "p_{T} of Top(lep) from Kin Reco "    +titlenamech[i]+"; p_{T} [GeV]",    30,0,300);
      // Top hadronic
      hKinthMass[j][i] = new TH1D("hKinthMass_"+namech[i]+"_"+namecut[j]+syst_varname, "Inv. Mass of Top(had) from Kin Reco "+titlenamech[i]+"; M_{t_{h}} [GeV]",100,100,300);
      hKinthpT[j][i]   = new TH1D("hKinthpT_"  +namech[i]+"_"+namecut[j]+syst_varname, "p_{T} of Top(had) from Kin Reco "    +titlenamech[i]+"; p_{T} [GeV]",    30,0,300);
      // Additional Jets
      hKinTagAddMass[j][i] = new TH1D("hKinTagAddMass_"+namech[i]+"_"+namecut[j]+syst_varname, "Inv. Mass of Additional Jets "+titlenamech[i]+"; M [GeV]", 150,0,300);
      hKinTagAddDR  [j][i] = new TH1D("hKinTagAddDR_"  +namech[i]+"_"+namecut[j]+syst_varname, "#Delta R of Additional Jets " +titlenamech[i]+"; #Delta R",25,0,5);
      // CSV for Additional Jets
      hKinAdd1CSV[j][i]      = new TH1D("hKinAdd1CSV_"     +namech[i]+"_"+namecut[j]+syst_varname, "CSV For add Jet-1 from KinFit "            +titlenamech[i]+";CSVv2",  20,0.,1.);
      hKinAdd2CSV[j][i]      = new TH1D("hKinAdd2CSV_"     +namech[i]+"_"+namecut[j]+syst_varname, "CSV For add Jet-2 from KinFit "            +titlenamech[i]+";CSVv2",  20,0.,1.);
      hKinAddCSVUnroll[j][i] = new TH1D("hKinAddCSVUnroll_"+namech[i]+"_"+namecut[j]+syst_varname, "CSV For add Jets from KinFit (Unrolled) "  +titlenamech[i]+";CSV Bin",400,0.,400);

      h2DKinAddCSV[j][i]     = new TH2D("h2DKinAddCSV_"    +namech[i]+"_"+namecut[j]+syst_varname, "CSVv2 Discriminant for the Add (kin) Jets "+titlenamech[i],           20,0.,1.,20,0.,1.);
    }//for(i->channel)
  }//for(j->cut)
  
  TStopwatch sw;
  sw.Start(kTRUE);

  ///////////////////////////////////////
  // Please, IGNORE. Temporal solution //
  ///////////////////////////////////////
  TCanvas *mydummycanvas=new TCanvas();// 
  ///////////////////////////////////////

  /************************
  Acceptance and Efficiency
  *************************/
  for(unsigned int iAEc=0; iAEc<Nhcuts; iAEc++){
    for(unsigned int iAEch=0; iAEch<(Nhch+1); iAEch++){ AccEvent[iAEc][iAEch] = 0 ; EffEvent[iAEc][iAEch] = 0.0; }
  }
  
  /************************
     SF Parametrization
  *************************/
  TH2F *hmuIDISOSF, *hmuTriggerSF, *hmuTrackerSF;
  TH2F *heIDISOSF,  *heTriggerSF;

  GetSFHistogram ("TopTools/SF_ID-ISO-Trigger/", 
		  "IDISO_Trigger_POGMoriond17",
		  &hmuIDISOSF, &hmuTriggerSF, &hmuTrackerSF, 
		  &heIDISOSF, &heTriggerSF);

  /*******************************
      Theoretical Uncertainties
  *******************************/

  if(_syst && 
     syst_name!="UE"  && 
     syst_name!="ISR" &&
     syst_name!="FSR") fname += "_SYS" + syst_varname;

  // Scale Uncertainty
  // From: https://indico.cern.ch/event/494682/contribution/2/attachments/1223578/1800218/mcaod-Feb15-2016.pdf
  int scaleSysPar;
  if     (_syst && syst_varname.Contains("ScaleRnFUp"))   scaleSysPar = 0; // muR=Nom,  muF=Up
  else if(_syst && syst_varname.Contains("ScaleRnFDown")) scaleSysPar = 1; // muR=Nom,  muF=Down
  else if(_syst && syst_varname.Contains("ScaleRuFNom"))  scaleSysPar = 2; // muR=Up,   muF=Nom
  else if(_syst && syst_varname.Contains("ScaleRuFUp"))   scaleSysPar = 3; // muR=Up,   muF=Up
  // else if(_syst && syst_varname.Contains("ScaleRdFNom"))  scaleSysPar = 4; // muR=Down, muF=Nom
  // Name modified to fix with Combine Structure
  else if(_syst && syst_varname.Contains("ScaleRdFUp"))   scaleSysPar = 4; // muR=Down, muF=Nom
  else if(_syst && syst_varname.Contains("ScaleRdFDown")) scaleSysPar = 5; // muR=Down, muF=Down
  // Normalization for Scale Weights:
  if (_syst && syst_varname.Contains("ScaleR")){
    if(scaleSysPar < 6){
      std::cout << "Scale normalization: Weighted events = " << NTotal_ScalemuRF_Weight[scaleSysPar] << std::endl;
      nNorm_Event = NTotal_ScalemuRF_Weight[scaleSysPar];
      if(_NUserEvt > 0) nNorm_Event = MaxEvt*(NTotal_ScalemuRF_Weight[scaleSysPar]/theTree.GetEntries());

    }
    else{
      std::cerr << "No entry for Scale normalization! Check HISTO!"  << std::endl;
      std::exit(0);
    }

  }

  // h_damp Uncertainty
  int hdampSysPar=0;
  if      (_syst && syst_varname.Contains("hdampUp"))   hdampSysPar = 18;
  else if (_syst && syst_varname.Contains("hdampDown")) hdampSysPar = 9;
  // Normalization for hdamp Weights:
  if (_syst && syst_varname.Contains("hdamp")){
    TH1D *h_hdampWeights;
    h_hdampWeights = (TH1D*)fileEntries->Get("ttbbLepJets/hdampWeights");
    if(hdampSysPar!=0){
      int ihdampbin = 1;
      if(hdampSysPar==9)  ihdampbin=2;
      if(hdampSysPar==18) ihdampbin=3;
      std::cout << "hdamp normalization: Weighted events = " << h_hdampWeights->GetBinContent(ihdampbin) << std::endl;
      nNorm_Event = h_hdampWeights->GetBinContent(ihdampbin);
      if(_NUserEvt > 0) nNorm_Event = MaxEvt*(h_hdampWeights->GetBinContent(ihdampbin)/theTree.GetEntries());

    }
    else{
      std::cerr << "No entry for hdamp normalization! Check HISTO!"  << std::endl;
      std::exit(0);
    }

  }

  /*****************************************
    Jet uncertainties (btag, JES and JER)
  ******************************************/
  TString GlobalJetSFPath = "/afs/cern.ch/user/b/brochero/brochero_WorkArea/LepJetsAnalyzer/";
  // Jet corrections
  TString JetResPath   = GlobalJetSFPath + "TopTools/JEC_btag/JER/JER_806/Spring16_25nsV10_MC_PtResolution_AK4PFchs.txt";
  TString JetResSFPath = GlobalJetSFPath + "TopTools/JEC_btag/JER/JER_806/Spring16_25nsV10_MC_SF_AK4PFchs.txt";
  std::cout << "-------- Correction Files -------- " << std::endl;
  std::cout <<  JetResPath << "\n" << JetResSFPath << std::endl;
  JME::JetResolution resolution        = JME::JetResolution((std::string)JetResPath);
  JME::JetResolutionScaleFactor res_sf = JME::JetResolutionScaleFactor((std::string)JetResSFPath);
  // b-tag
  // New WP for 80X: https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation80XReReco
  // float CSV_WP = 0.8484; // Medium Moriond-17
  float CSV_WP = 0.9535; // Tight Moriond-17

  btagCollection SF_btag;
  TString btagHFPath   = GlobalJetSFPath + "TopTools/JEC_btag/bTag/bTag_2017_6_7/csv_rwt_fit_hf_v2_final_2017_6_7_all.root";
  TString btagLFPath   = GlobalJetSFPath + "TopTools/JEC_btag/bTag/bTag_2017_6_7/csv_rwt_fit_lf_v2_final_2017_6_7_all.root";
  SF_btag = SetupbtagSF(btagHFPath, btagLFPath);

  // JES Variations 
  bool sysJES = false;
  int  nJES = -99, nJESVar = -99;
  if(_syst && syst_varname.Contains("JES")){
    // JES Components
    nJES    = FindSysID(JESName,  syst_name);
    nJESVar = FindSysID(vVarNames, syst_var );
    
    if (nJES != -99 && nJESVar != -99){
      sysJES = true;
      std::cout << "JES UNCERTAINTY: CODE set to estimate " << JESName.at(nJES) << " + " << vVarNames.at(nJESVar) << std::endl;
    }
    else{
	std::cout << "JES variation NOT found." << std::endl;
	std::exit (EXIT_FAILURE);
      }
  } // if(JES)
  
  // b-tag Variations
  bool sysbtag = false;
  int  nbtag = -99, nbtagVar = -99;
  if (syst_name.Contains("btag_")){
    nbtag    = FindSysID(vbtgNames, syst_name);
    nbtagVar = FindSysID(vVarNames, syst_var);
    
    if (nbtag != -99 && nbtagVar != -99) {
      sysbtag = true;
      std::cout << "b-TAG UNCERTAINTY: CODE set to estimate " << vbtgNames.at(nbtag) << " + " << vVarNames.at(nbtagVar) << std::endl;
    }
    else{      
      std::cout << "btag variation NOT found." << std::endl;
      std::exit (EXIT_FAILURE);
    }
  }

  // -- btag SF
  hbtag hSF_btag;
  if (sysbtag)     hSF_btag = SF_btag.Syst[nbtag][nbtagVar];
  else if (sysJES) hSF_btag = SF_btag.SystJES[nJES][nJESVar];
  else             hSF_btag = SF_btag.Central;


  /************************
     PileUp Uncertainty  
  ************************/
  int pileupSysPar;
  if     (_syst && syst_varname.Contains("PileUpUp"))   pileupSysPar = 1; // Up
  else if(_syst && syst_varname.Contains("PileUpDown")) pileupSysPar = 2; // Down
  
  /************************
    Normalization Weights
  *************************/  
  float NormWeight[2] = {0.0, 0.0}; // mu, e
  // It is estimated as ==> NormWeight = Lumi*(1.0/N_Gen_events)*(Xsec)
  NormWeight[0] = SFLumi(fname, LuminNorm_Mu, nNorm_Event);  
  NormWeight[1] = SFLumi(fname, LuminNorm_El, nNorm_Event);  
  
  std::cout << "-----------------------                                 -------------------------" << std::endl;
  std::cout << "Number of Events         = " << nNorm_Event << std::endl;
  std::cout << "Normalization Factor[Mu] = " << NormWeight[0]  << std::endl;
  std::cout << "Normalization Factor[EG] = " << NormWeight[1]  << std::endl;
  std::cout << "---------------------------------------------------------------------------------" << std::endl;

  /********************************
             Event Loop
  ********************************/
  std::cout << "--- Processing: " << MaxEvt << " events" << std::endl;
  
  for (Long64_t ievt=0; ievt<MaxEvt; ievt++) {
    
    theTree.GetEntry(ievt);  
    print_progress(MaxEvt, ievt);

    // PU reweight: Includes Syst. Unc.
    if (_syst && (syst_varname.Contains("PileUpUp") || syst_varname.Contains("PileUpDown")) )
      PUWeight = (*PUWeight_sys)[pileupSysPar]; // Up
    else PUWeight = (*PUWeight_sys)[0];
        
    // Luminosity Weight + MCatNLO GEN Weights (For MC@NLO) 
    PUWeight = PUWeight * NormWeight[Channel] * GENWeight;
    
    // Scale reweight: Syst. Unc.
    if (_syst && syst_varname.Contains("ScaleR"))
      PUWeight = PUWeight*(*ScaleWeight)[scaleSysPar];
    
    // hdamp reweight: Syst. Unc.
    if (_syst && syst_varname.Contains("hdamp"))
      PUWeight = PUWeight*(*hdampWeight)[hdampSysPar];
    
    /*********************
            Lepton 
     *********************/
    TLorentzVector Lep;    
    if (_syst && syst_varname.Contains("LESUp")) 
      Lep.SetPtEtaPhiE(Lep_pT + (Lep_pT*Lep_LES),
		       Lep_eta,
		       Lep_phi,
		       Lep_E);
    else if (_syst && syst_varname.Contains("LESDown")) 
      Lep.SetPtEtaPhiE(Lep_pT - (Lep_pT*Lep_LES),
		       Lep_eta,
		       Lep_phi,
		       Lep_E);
    else Lep.SetPtEtaPhiE(Lep_pT,Lep_eta,Lep_phi,Lep_E);    
    
    // Lep pT_mu > 30GeV
    if(Channel == 0 && Lep.Pt() < 30)  continue; 
    // Lep pT_e  > 35GeV
    if(Channel == 1 && Lep.Pt() < 35)  continue; 
    
    // Transverse W Mass
    TLorentzVector METv;
    METv.SetPtEtaPhiE(MET,0.0,MET_Phi,MET);
    float mT = sqrt(2*MET*Lep.Pt()*(1.0-cos( Lep.DeltaPhi(METv) )));
    
    /*******************************************
    Kinematic Reconstruction (at least 4 Jets)
    ********************************************/
    // Order for Jets: 
    // [0] b from hadronic leg 
    // [1] j from W
    // [2] j from W
    // [3] b from leptonic leg
    double KinProb = std::exp(-0.5*Kin_Chi2);

    int KinJetIndex[4];
    TLorentzVector KinNu;
    ComJet KinJet[4];
    KinNu.SetPtEtaPhiE(KinNu_pT, KinNu_eta, KinNu_phi, KinNu_E);
    for(unsigned int ikj=0; ikj<4; ikj++){
      KinJetIndex[ikj] = (*KinJet_Index)[ikj];
      KinJet[ikj].SetPtEtaPhiE((*KinJet_pT)[ikj],(*KinJet_eta)[ikj],(*KinJet_phi)[ikj],(*KinJet_E)[ikj]);
    }

    /*********************
             Jets
    *********************/
    std::vector<ComJet> PreJets;    
      
    for(int ijet=0; ijet < Jet_pT->size(); ijet++){
      
      ComJet jet;
      jet.SetPtEtaPhiE((*Jet_pT)[ijet], 
		       (*Jet_eta)[ijet],
		       (*Jet_phi)[ijet],
		       (*Jet_E)[ijet]);
      
      jet.Flavour = (*Jet_hadronFlavour)[ijet]; 
      jet.CSV     = (*Jet_CSV)[ijet];
      if      (jet.CSV < 0.0)  jet.CSV = 0.0000;
      else if (jet.CSV >= 1.0) jet.CSV = 0.9999;
      jet.Mom      = -1;
      jet.KinMom   = -1;
      jet.GenIndex = -1;
      jet.Gen_pT   = -1;
      jet.Gen_DR   = -1;
      jet.pTIndex  = (*Jet_pTIndex)[ijet];
      if(sysJES){
	if     (nJESVar ==var::Up)   jet.JES = 1.0 + std::abs((*Jet_JESCom_Up)  [ijet][nJES]);
	else if(nJESVar ==var::Down) jet.JES = 1.0 - std::abs((*Jet_JESCom_Down)[ijet][nJES]);
      }
      if((fname.Contains("ttbar")  && !fname.Contains("Bkg"))){
	jet.Mom      = (*Jet_Mom)[ijet];
	jet.GenIndex = (*Jet_GENmatched)[ijet];
	if(jet.GenIndex == -999){
	  jet.Gen_pT = 0.0;
	  jet.Gen_DR = 0.0;
	}
	else{
	  TLorentzVector GenJet;
	  GenJet.SetPtEtaPhiE((*GenJet_pT)[jet.GenIndex],(*GenJet_eta)[jet.GenIndex],(*GenJet_phi)[jet.GenIndex],(*GenJet_E)[jet.GenIndex] );
	  jet.Gen_pT   = (*GenJet_pT) [jet.GenIndex];	
	  jet.Gen_DR   = jet.DeltaR(GenJet);	
	}
      }// if(ttbar)
      
      // Kin Mother (All events)
      // [0] b from hadronic leg 
      // [1] j from W
      // [2] j from W
      // [3] b from leptonic leg
      if (ijet == KinJetIndex[0] || ijet == KinJetIndex[3] ) jet.KinMom = 6;
      if (ijet == KinJetIndex[1] || ijet == KinJetIndex[2] ) jet.KinMom = 24;
      
      PreJets.push_back(jet);	
    }// for(jets)
    
    
    unsigned int NJets=0, NBtagJets=0;
    
    if (!fname.Contains("DataSingle")){
      
      // Measurements show that the jet energy resolution (JER) in data is worse than in the simulation and the jets in MC need to be smeared to describe the data.
      // https://twiki.cern.ch/twiki/bin/viewauth/CMS/JetResolution
      int JERVar = var::Nom;
      if     (_syst && syst_varname.Contains("JERUp"))   JERVar = var::Up;
      else if(_syst && syst_varname.Contains("JERDown")) JERVar = var::Down;
      // JER      
      JERCorrection(res_sf, resolution, Rho, &PreJets, JERVar);
      
      // JES
      if(sysJES) JESCorrection(&PreJets);      
      
    }// if(!data)
    
    std::vector<ComJet> Jets;
    for(int ijet=0; ijet < PreJets.size(); ijet++){
      ComJet jet = PreJets.at(ijet);
      
      if(jet.Pt() > 30){ // Jet pT Cut
	Jets.push_back(jet);
	// b-tagging
	if(jet.CSV > CSV_WP) NBtagJets++; // Number of b-tagged jets	
      } // if(Jet_pT)
      
    }//for(jets)
    
    NJets = Jets.size();
        
    /*******************************************
              b-tagging Scale Factors
    *******************************************/     
    
    // Shape Method
    // From: https://twiki.cern.ch/twiki/bin/view/CMS/BTagShapeCalibration
    float SFevt_btag = GetbtagEvtWeight(hSF_btag, Jets);

    PUWeight = PUWeight * SFevt_btag;
    

    /*******************************************
     Trigger,ID & ISO Scale Factors/bin(Pt,Eta)
    *******************************************/    
    std::vector<float> SF_ID_ISO_Tr;    
    if (fname.Contains("DataSingle")){
      SF_ID_ISO_Tr.push_back(1.0); // SF_ID_ISO_Tr      [0] 
      SF_ID_ISO_Tr.push_back(1.0); // SF_ID_ISO_Tr_Up   [1] 
      SF_ID_ISO_Tr.push_back(1.0); // SF_ID_ISO_Tr_Down [2] 
      SF_ID_ISO_Tr.push_back(1.0); // SF_ID_ISO         [3] 
      SF_ID_ISO_Tr.push_back(1.0); // SF_ID_ISO_Error   [4] 
      SF_ID_ISO_Tr.push_back(1.0); // SF_Tr             [5] 
      SF_ID_ISO_Tr.push_back(1.0); // SF_Tr_Error       [6] 
    }    
    else {
      // Taking SF from root file
      SFIDISOTrigger(SF_ID_ISO_Tr,
      		     Lep, Channel, GoodPV,
      		     hmuIDISOSF, hmuTriggerSF, hmuTrackerSF,
      		     heIDISOSF,  heTriggerSF);      
      
      if(_syst && syst_varname.Contains("LepSF")){
	float SFSystUnc;
	if(syst_varname.Contains("IDLepSF")){
	  SFSystUnc = sqrt(pow(SF_ID_ISO_Tr[3]*0.01,2) + pow(SF_ID_ISO_Tr[4],2)); //Additional 1%
	  if(syst_varname.Contains("Up"))   PUWeight = PUWeight * (SF_ID_ISO_Tr[3] + SFSystUnc);
	  if(syst_varname.Contains("Down")) PUWeight = PUWeight * (SF_ID_ISO_Tr[3] - SFSystUnc);
	}
	else if(syst_varname.Contains("TrLepSF")){
	  SFSystUnc = sqrt(pow(SF_ID_ISO_Tr[5]*0.01,2) + pow(SF_ID_ISO_Tr[6],2)); //Additional 1%
	  if(syst_varname.Contains("Up"))   PUWeight = PUWeight * (SF_ID_ISO_Tr[5] + SFSystUnc);
	  if(syst_varname.Contains("Down")) PUWeight = PUWeight * (SF_ID_ISO_Tr[5] - SFSystUnc);
	}
      }
      
      else PUWeight = PUWeight * SF_ID_ISO_Tr[0]; // Central     

    }// else(Contain("Data"))
    

    /***************************
            Selection
    ***************************/
    
    bool JumpCutEvent[Nhcuts];
    for(unsigned int bcut=0; bcut<Nhcuts; bcut++) JumpCutEvent[bcut] = true;

    JumpCutEvent[0] = false;                                // Single Lepton (from Tree)
    if(NJets > 5)                   JumpCutEvent[1]= false; // lep + 6 Jets 
    if(NJets > 5 && NBtagJets > 1)  JumpCutEvent[2]= false; // lep + 6 Jets + 2 b-tag

    bool JumpEvent = false;
    
    // Exclusive Data and QCD Samples
    if (fname.Contains("DataSingleMu") && Channel==1)  JumpEvent = true;
    if (fname.Contains("DataSingleEG") && Channel==0)  JumpEvent = true;
    if (fname.Contains("QCD_MuEnr")    && Channel==1)  JumpEvent = true;
    if (fname.Contains("QCD_EGEnr")    && Channel==0)  JumpEvent = true;
    
    if (JumpEvent) continue;

    /***************************
          Loop over cuts
    ***************************/
    for(int icut = 0; icut < Nhcuts; icut++){
      
      if(JumpCutEvent[icut]) continue;
      
      /******************
          Acc. / Eff. 
      ******************/
      AccEvent[icut][Channel]++;
      EffEvent[icut][Channel]+=  PUWeight;

      /*******************
        Fill Histograms
      *******************/
      // ID, ISO and Trigger SF
      hSFIDISOTr[icut][Channel]->Fill(SF_ID_ISO_Tr[0],PUWeight);
      hSFIDISOTrError[icut][Channel]->Fill((SF_ID_ISO_Tr[1] - SF_ID_ISO_Tr[0]),PUWeight);
      hSFIDISO[icut][Channel]->Fill(SF_ID_ISO_Tr[3],PUWeight);
      hSFIDISOError[icut][Channel]->Fill(SF_ID_ISO_Tr[4],PUWeight);
      hSFTrigger[icut][Channel]->Fill(SF_ID_ISO_Tr[5],PUWeight);
      hSFTriggerError[icut][Channel]->Fill(SF_ID_ISO_Tr[6],PUWeight);
    
      // PDF Weights 
      if(_syst && syst_varname.Contains("PDF")){
	for(unsigned int ipdf=0; ipdf<=99; ipdf++) hWPDF [icut][Channel]->Fill((*PDFWeight)[ipdf], PUWeight);
	hWPDFAlphaUp   [icut][Channel]->Fill((*PDFWeight)[101], PUWeight);
	hWPDFAlphaDown [icut][Channel]->Fill((*PDFWeight)[100], PUWeight);
      }
      
      // hdamp Weights 
      if(_syst && syst_varname.Contains("hdamp"))  hWhdamp[icut][Channel]->Fill((*hdampWeight)[hdampSysPar], PUWeight);
      
      // Kinematic Variables
      hPV     [icut][Channel]->Fill(GoodPV,             PUWeight);
      hMET    [icut][Channel]->Fill(MET,                PUWeight);
      hMET_Phi[icut][Channel]->Fill(fabs(MET_Phi),      PUWeight);
      hLepPt  [icut][Channel]->Fill(Lep.Pt(),           PUWeight);
      hLepEta [icut][Channel]->Fill(std::abs(Lep.Eta()),PUWeight);
      hLepPhi [icut][Channel]->Fill(std::abs(Lep.Phi()),PUWeight);
      hmT     [icut][Channel]->Fill(mT,                 PUWeight);      
      // Jets Multiplicity
      hNJets    [icut][Channel]->Fill(NJets,    PUWeight); 
      hNBtagJets[icut][Channel]->Fill(NBtagJets,PUWeight);
      // Global btag SF
      hSFbtag_Global[icut][Channel]->Fill(SFevt_btag, PUWeight);

      // Jet Variables
      bool fKinAddjj = true;
      for(int ijet=0; ijet < Jets.size(); ijet++){
	ComJet jet = Jets[ijet];
	// SF_btag Profile
	pSFCSVVsCSV[icut][Channel]->Fill(jet.CSV, SFevt_btag, PUWeight);
	
	// Jet variables
	if (ijet < NhJets){
	  hJetPt[ijet][icut][Channel]->Fill(jet.Pt(), PUWeight);
	  hCSV  [ijet][icut][Channel]->Fill(jet.CSV,  PUWeight);
	}
	
	//Dijet Invariant Mass 
	int jbmax = std::min(NhJets,NJets);
	for(int jjet=ijet+1; jjet < jbmax; jjet++){
	  ComJet jet_ = Jets[jjet];
	  float DijetInvMass = (jet+jet_).M(); 
	  float DijetDR = jet.DeltaR(jet_); 
	  // Kin Mass
	  if(jet.KinMom != 24 && jet_.KinMom !=24 &&
	     jet.KinMom != 6  && jet_.KinMom != 6 &&
	     fKinAddjj){
	    hKinTagAddMass[icut][Channel]->Fill(DijetInvMass, PUWeight);
	    hKinTagAddDR  [icut][Channel]->Fill(DijetDR,      PUWeight);

	    hKinAdd1CSV   [icut][Channel]->Fill(jet.CSV,      PUWeight);
	    hKinAdd2CSV   [icut][Channel]->Fill(jet_.CSV,     PUWeight);

	    h2DKinAddCSV  [icut][Channel]->Fill(jet.CSV, jet_.CSV, PUWeight);

	    pSFCSVVsCSVAdd[icut][Channel]->Fill(jet.CSV,  SFevt_btag, PUWeight);
	    pSFCSVVsCSVAdd[icut][Channel]->Fill(jet_.CSV, SFevt_btag, PUWeight);

	    fKinAddjj = false; // FIRST Pair of additional jets! That means the pair with the highest CSV  

	  } // if(AddJet)
	}// for(jjet)
	
      }//for(ijet)     
      
      // GenCone Vs Kinematic Reconstruction 
      if(NJets > 3){ 	
	// Kinematic Reconstruction
	TLorentzVector KinWl, KinWh, Kintl, Kinth;
	hKinChi2 [icut][Channel]->Fill(Kin_Chi2,PUWeight);
	hKinProb [icut][Channel]->Fill(KinProb, PUWeight);
	
	KinWl = Lep + KinNu;
	KinWh = KinJet[1] + KinJet[2];
	
	Kintl = KinWl + KinJet[3];
	Kinth = KinWh + KinJet[0];
	
	hKinWlMass     [icut][Channel]->Fill(KinWl.M());
	hKinWlTransMass[icut][Channel]->Fill(KinWl.Mt());
	hKinWlpT       [icut][Channel]->Fill(KinWl.Pt());
	
	hKinWhMass[icut][Channel]->Fill(KinWh.M());
	hKinWhpT  [icut][Channel]->Fill(KinWh.Pt());

	hKintlMass[icut][Channel]->Fill(Kintl.M());
	hKintlpT  [icut][Channel]->Fill(Kintl.Pt());
	
	hKinthMass[icut][Channel]->Fill(Kinth.M());
	hKinthpT  [icut][Channel]->Fill(Kinth.Pt());
	
	for(int ikj=0; ikj < 4; ikj++)
	  hKinJetPt[ikj][icut][Channel]->Fill(KinJet[ikj].Pt(), PUWeight);
	
      } //if(NJet > 3)

    }//for(icuts)     
    
    Jets.clear();

  }//for(events)

  /********************************
         Event Loop END
  ********************************/
  
  // Get elapsed time
  sw.Stop();
  std::cout << "==================================================] 100% " << std::endl;
  std::cout << "--- End of event loop: "; sw.Print();
  std::cout << std::endl;

  //Acceptance-Efficiency
  TH1D *Yields, *YieldsNoWeights;
  Yields = new TH1D("Yields" + syst_varname, "Yields",(Nhcuts)*(Nhch+1)+1,0,(Nhcuts)*(Nhch+1)+1);
  YieldsNoWeights = new TH1D("YieldsNoWeights" + syst_varname, "Yields W/O Weights",(Nhcuts)*(Nhch+1)+1,0,(Nhcuts)*(Nhch+1)+1);

  int nbin = 1;  
  for(int nc = 0; nc < Nhcuts; nc++){
    AccEvent[nc][2] = AccEvent[nc][0] + AccEvent[nc][1];
    EffEvent[nc][2] = EffEvent[nc][0] + EffEvent[nc][1];
    
    for(int nch = 0; nch < Nhch+1; nch++){            
      float AccError, EffError;
      AccError =  sqrt(AccEvent[nc][nch]);
      if (AccEvent[nc][nch] != 0.0) EffError = sqrt(AccEvent[nc][nch])*EffEvent[nc][nch]/AccEvent[nc][nch];
      else EffError = 0.0;
      
      Yields->GetXaxis()->SetBinLabel(nbin, namecut[nc] + " " + namech[nch]);
      Yields->SetBinContent(nbin, EffEvent[nc][nch]);
      Yields->SetBinError  (nbin, EffError);

      YieldsNoWeights->GetXaxis()->SetBinLabel(nbin, namecut[nc] + " " + namech[nch]);
      YieldsNoWeights->SetBinContent(nbin, AccEvent[nc][nch]);
      YieldsNoWeights->SetBinError  (nbin, AccError);

      nbin++;
      
      std::cout << "-- Acceptace  " << namecut[nc] << " " << namech[nch] << ": " << AccEvent[nc][nch] << std::endl;
      std::cout << "-- Efficiency " << namecut[nc] << " " << namech[nch] << ": " << EffEvent[nc][nch] << " +/- " << EffError << std::endl;
      std::cout << std::endl;

      // Unroll TH2F in 1D histogram
      if (nch < 2)
	UnRoll2D(h2DKinAddCSV    [nc][nch], hKinAddCSVUnroll[nc][nch]);
      
    }// for(nch)

    std::cout << "-----------------------------" << std::endl;

  }// for (ncut)

  Yields         ->GetXaxis()->SetBinLabel(nbin, "Norm. Factor");
  YieldsNoWeights->GetXaxis()->SetBinLabel(nbin, "Norm. Factor");
  Yields         ->SetBinContent(nbin, NormWeight[0]);
  YieldsNoWeights->SetBinContent(nbin, NormWeight[0]);
    
  /********************************
         Write histograms
  ********************************/
  TString outfname=dirname + "/hSF-" + hname + "_" + fname + ".root";
  TFile *target  = new TFile(outfname,"RECREATE" );  
  
  target->cd();
  TString dirhistosname = "central";
  if(_syst) dirhistosname = syst_name + syst_var;  

  target->mkdir(dirhistosname);
  target->cd   (dirhistosname);
  
  Yields         ->Write();
  YieldsNoWeights->Write();

  for(int j=0; j<Nhcuts; j++){    
    for(int i=0; i<Nhch; i++){
      
      target->mkdir(dirhistosname + "/" + namecut[j] + "/" + namech[i]);
      target->cd   (dirhistosname + "/" + namecut[j] + "/" + namech[i]);
      
      // Global Variables
      hPV     [j][i]->Write();
      hMET    [j][i]->Write();
      hMET_Phi[j][i]->Write();
      // PDF (and Alpha_s)
      hWPDF         [j][i]->Write();
      hWPDFAlphaUp  [j][i]->Write();
      hWPDFAlphaDown[j][i]->Write();      
      hWhdamp       [j][i]->Write();
      // SF(ID,ISO & Trigger)
      hSFIDISOTr     [j][i]->Write();
      hSFIDISOTrError[j][i]->Write();
      hSFIDISO       [j][i]->Write();
      hSFIDISOError  [j][i]->Write();
      hSFTrigger     [j][i]->Write();
      hSFTriggerError[j][i]->Write();
      // SF(b-tag)
      hSFbtag_Global[j][i]->Write();
      pSFCSVVsCSV   [j][i]->Write();
      pSFCSVVsCSVAdd[j][i]->Write();
      // Lepton Variables
      hLepPt [j][i]->Write();
      hLepEta[j][i]->Write();
      hLepPhi[j][i]->Write();
      hmT    [j][i]->Write();
      // Jets Variables 
      for(int ij=0; ij<NhJets; ij++){
  	hJetPt[ij][j][i]->Write();
  	hCSV  [ij][j][i]->Write();
      }
      // Jet Multiplicity     
      hNJets    [j][i]->Write();
      hNBtagJets[j][i]->Write();            
      /***************************
        Kinematic Reconstruction
      ***************************/
      // Global
      hKinChi2[j][i]->Write();
      hKinProb[j][i]->Write();
      // Jets
      for(int ikj=0; ikj<4;ikj++) hKinJetPt[ikj][j][i]->Write();
      // W leptonic
      hKinWlTransMass[j][i]->Write();
      hKinWlMass     [j][i]->Write();
      hKinWlpT       [j][i]->Write();
      // W hadronic
      hKinWhMass     [j][i]->Write();
      hKinWhpT       [j][i]->Write();
      // Top leptonic
      hKintlMass[j][i]->Write();
      hKintlpT  [j][i]->Write();
      // Top hadronic
      hKinthMass[j][i]->Write();
      hKinthpT  [j][i]->Write();
      // Additional Jets
      hKinTagAddMass[j][i]->Write();
      hKinTagAddDR  [j][i]->Write();
      // CSV for Additional Jets      
      hKinAdd1CSV     [j][i]->Write();
      hKinAdd2CSV     [j][i]->Write();
      hKinAddCSVUnroll[j][i]->Write();

      h2DKinAddCSV    [j][i]->Write();
    }//for(i)

  }//for(j)

  target->cd(dirhistosname);
    
  target->Close();

  std::cout << "File saved as " << outfname << std::endl;

}

// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
const TString currentDateTime() {
  time_t     now = time(0);
  struct tm  tstruct;
  char       buf[80];
  tstruct = *localtime(&now);
  // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
  // for more information about date/time format
  strftime(buf, sizeof(buf), "%Y-%m-%d at %X", &tstruct);

  return buf;
}

void print_progress(int TreeEntries, Long64_t ievt){
  int step = TreeEntries/50;
  if (ievt%(step) == 0){ 
    float progress=(ievt)/(TreeEntries*1.0);
    int barWidth = 50;
    
    std::cout << "[";
    int pos = barWidth * progress;
    
    for (int i = 0; i < barWidth; ++i) {
      if (i < pos) std::cout << "=";
      else if (i == pos) std::cout << ">";
      else std::cout << " ";
    }
    
    std::cout << "] " << int(progress * 100.0) << " %\r";
    std::cout.flush();
  }  
}

void UnRoll2D(TH2D *h2D, TH1D *h1D){
  int UnRollbin = 1;
  for (int ibinx = 1; ibinx<=h2D->GetXaxis()->GetNbins(); ibinx++){ 
    for (int ibiny = 1; ibiny<=h2D->GetYaxis()->GetNbins(); ibiny++){      
      double bc = h2D->GetBinContent(ibinx,ibiny);
      double be = h2D->GetBinError  (ibinx,ibiny);
      h1D->SetBinContent(UnRollbin, bc);
      h1D->SetBinError  (UnRollbin, be);
      UnRollbin ++;
    } // for(ibinx) 
  } // for(ibinx)     

}

// Find Systematic Uncertainties
int FindSysID(const std::vector<TString> Names, TString Name){
  for(int iName = 0; iName < Names.size(); iName++){
    if (Name.Contains(Names.at(iName))) return iName;
  }
  return -99;
}

#endif

