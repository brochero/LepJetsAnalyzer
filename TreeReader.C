#ifndef __CINT__
#include "TreeReader.h"
#endif

#ifndef __CINT__


void display_usage()
{
  std::cout << "\033[1;37musage:\033[1;m skimfile cutindex [options]" << std::endl;
  std::cout << "" << std::endl;
  std::cout << "Options:" << std::endl;
  std::cout << "    -i inputfile  Input file without .root" << std::endl;
  std::cout << "    -eos input files located in eos lxplus" << std::endl;
  std::cout << "    -o name in the output file \"h_\"" << std::endl;
  std::cout << "    -n number of events" << std::endl;
  std::cout << "    -s create a file with the systematic uncertainty yields" << std::endl;
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
  const char * _dir      = "../v8-0-1/";
  const char * _syst_var = 0;
  const char * _ttbar_id = 0;
  
  // Arguments used
  //std::set<int> usedargs;
  //Parsing input options
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
	_syst_var = argv[i+1];
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
  TString syst_varname(_syst_var);
  TString fdir(_dir);
  TString ttbar_id(_ttbar_id);

  // eos directory
  if(_eos) fdir = "root://eoscms.cern.ch//eos/cms/store/user/brochero/" + fdir + "/";
  //make a dir if output does not exist!!
  struct stat st;
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
  theTree.SetBranchAddress( "PUWeight", &PUWeight_sys );
  theTree.SetBranchAddress( "channel",  &Channel );
  theTree.SetBranchAddress( "GoodPV",   &GoodPV );
  theTree.SetBranchAddress( "MET",      &MET );
  theTree.SetBranchAddress( "MET_phi",  &MET_Phi );

  theTree.SetBranchAddress( "lepton_pT",  &Lep_pT );
  theTree.SetBranchAddress( "lepton_eta", &Lep_eta );
  theTree.SetBranchAddress( "lepton_phi", &Lep_phi );
  theTree.SetBranchAddress( "lepton_E",   &Lep_E );
  theTree.SetBranchAddress( "lepton_SF",  &Lep_SF );
  theTree.SetBranchAddress( "lepton_LES", &Lep_LES );

  theTree.SetBranchAddress( "jet_pT",    &Jet_pT );
  theTree.SetBranchAddress( "jet_eta",   &Jet_eta );
  theTree.SetBranchAddress( "jet_phi",   &Jet_phi );
  theTree.SetBranchAddress( "jet_E",     &Jet_E );
  theTree.SetBranchAddress( "jet_index", &Jet_pTIndex );

  theTree.SetBranchAddress( "jet_CSV",           &Jet_CSV );
  theTree.SetBranchAddress( "jet_SF_CSV",        &Jet_SF_CSV );
  theTree.SetBranchAddress( "jet_partonFlavour", &Jet_partonFlavour );
  theTree.SetBranchAddress( "jet_CvsB",          &Jet_CvsB );
  theTree.SetBranchAddress( "jet_CvsL",          &Jet_CvsL );

  if(!fname.Contains("Data")){
    theTree.SetBranchAddress( "jet_JES_Up",  &Jet_JES_Up );
    theTree.SetBranchAddress( "jet_JES_Down",&Jet_JES_Down );  
    theTree.SetBranchAddress( "jet_JER_Up",  &Jet_JER_Up );
    theTree.SetBranchAddress( "jet_JER_Nom", &Jet_JER_Nom );
    theTree.SetBranchAddress( "jet_JER_Down",&Jet_JER_Down );
  }
  // ttbar event categorization
  if(fname.Contains("ttbar") && !fname.Contains("Bkg")){
    theTree.SetBranchAddress("scaleweight",       &ScaleWeight );

    theTree.SetBranchAddress("genconecatid",      &GenConeCat);
    theTree.SetBranchAddress("gencone_gjet_pT",   &GenCone_pT);
    theTree.SetBranchAddress("gencone_gjet_eta",  &GenCone_eta);
    theTree.SetBranchAddress("gencone_gjet_phi",  &GenCone_phi);
    theTree.SetBranchAddress("gencone_gjet_E",    &GenCone_E);
    theTree.SetBranchAddress("gencone_gjetIndex", &GenCone_gjetMom);
    theTree.SetBranchAddress("gencone_NgjetsW",   &GenCone_NgjetsW);
    theTree.SetBranchAddress("draddjets",             &DRAddJets);

    theTree.SetBranchAddress("genlepton_pT",          &GenLep_pT);
    theTree.SetBranchAddress("jet_MatchedGenJetIndex",&Jet_GENmatched);
    theTree.SetBranchAddress("jet_gencone_mom",       &Jet_Mom);
    theTree.SetBranchAddress("genjet_pT",             &GenJet_pT);
    theTree.SetBranchAddress("genjet_eta",            &GenJet_eta);
    theTree.SetBranchAddress("genjet_phi",            &GenJet_phi);
    theTree.SetBranchAddress("genjet_E",              &GenJet_E);
    theTree.SetBranchAddress("gennu_pT",              &GenNu_pT);
    theTree.SetBranchAddress("gennu_eta",             &GenNu_eta);
    theTree.SetBranchAddress("gennu_phi",             &GenNu_phi);
    theTree.SetBranchAddress("gennu_E",               &GenNu_E);
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
  TH1F *h_NumberEvt, *h_ScaleWeights;
  h_NumberEvt    = (TH1F*)fileEntries->Get("ttbbLepJets/EventInfo");
  h_ScaleWeights = (TH1F*)fileEntries->Get("ttbbLepJets/ScaleWeights");

  float NTotal_Event, NTotal_Weight, nNorm_Event, NTotal_ScalemuRF_Weight[6];
  NTotal_Event  = h_NumberEvt->GetBinContent(1);
  NTotal_Weight = h_NumberEvt->GetBinContent(2);
  for (unsigned int ibin = 1; ibin< 7; ibin++) NTotal_ScalemuRF_Weight[ibin]= h_ScaleWeights->GetBinContent(ibin);

  // MCatNLO Weights
  if(fname.Contains("aMCatNLO")){
    theTree.SetBranchAddress( "genweight", &GENWeight );
    nNorm_Event = NTotal_Weight;    
  }
  else{
    GENWeight = 1.0;
    nNorm_Event = NTotal_Event;
  }

  /*********************************
             Histograms
  **********************************/
  //Correct Statistical Uncertainty Treatment
  TH1::SetDefaultSumw2(kTRUE);  
    
  for(int j=0; j<Nhcuts; j++){   // Cut
    for(int i=0; i<Nhch; i++){ // Channel
      hPV[j][i]         = new TH1F("hPV_"+namech[i]+"_"+namecut[j],"PV Distribution  " + titlenamech[i] + ";PV",15,0,30);
      hMET[j][i]        = new TH1F("hMET_"+ namech[i]+"_"+namecut[j],"#slash{E}_{T} " + titlenamech[i] + ";#slash{E}_{T}[GeV]",10,0,200);
      hMET_Phi[j][i]    = new TH1F("hMET_Phi_"+ namech[i]+"_"+namecut[j],"#Phi_{#slash{E}_{T}} " + titlenamech[i] + ";#Phi_{#slash{E}_{T}}[rad]",16,0,3.2);
      hmT[j][i]         = new TH1F("hmT_"+ namech[i]+"_"+namecut[j],"transverse Mass Lepton/MET " + titlenamech[i] + ";m_{T}[GeV]",40,0,160);
      hHT[j][i]         = new TH1F("hHT_"+namech[i]+"_"+namecut[j],"H_{T} " + titlenamech[i] + ";HT[GeV]",100,0,600);
      
      hLepPt [j][i]    = new TH1F("hLepPt_"  + namech[i] + "_" + namecut[j], "Lepton p_{T} " + titlenamech[i] + ";Lepton p_{T}[GeV]",20,0.0,200.0);
      hLepEta[j][i]    = new TH1F("hLepEta_" + namech[i] + "_" + namecut[j], "#eta_{Lep} "   + titlenamech[i] + ";Lepton #eta",12,0.0,2.2);
      hLepPhi[j][i]    = new TH1F("hLepPhi_" + namech[i] + "_" + namecut[j], "#phi_{Lep} "   + titlenamech[i] + ";Lepton #Phi[rad]",16,0.0,3.2);
      
      hNJets[j][i]     = new TH1F("hNJets_" + namech[i] + "_" + namecut[j], "Jet multiplicity " + titlenamech[i] + ";Number of jets",9,-0.5,8.5);
      hNJets[j][i]->GetXaxis()->SetBinLabel(1,"0");
      hNJets[j][i]->GetXaxis()->SetBinLabel(2,"1");
      hNJets[j][i]->GetXaxis()->SetBinLabel(3,"2");
      hNJets[j][i]->GetXaxis()->SetBinLabel(4,"3");
      hNJets[j][i]->GetXaxis()->SetBinLabel(5,"4");
      hNJets[j][i]->GetXaxis()->SetBinLabel(6,"5");
      hNJets[j][i]->GetXaxis()->SetBinLabel(7,"6");
      hNJets[j][i]->GetXaxis()->SetBinLabel(8,"7");
      hNJets[j][i]->GetXaxis()->SetBinLabel(9,"#geq 8");
      if(j>0) hNJets[j][i]->GetXaxis()->SetRange(7,9);

      hNBtagJets[j][i]  = new TH1F("hNBtagJets_"+namech[i]+"_"+namecut[j],"b-tag jet multiplicity " + titlenamech[i] + ";Number of b-jets",6,-0.5,5.5);
      hNBtagJets[j][i]->GetXaxis()->SetBinLabel(1,"0");
      hNBtagJets[j][i]->GetXaxis()->SetBinLabel(2,"1");
      hNBtagJets[j][i]->GetXaxis()->SetBinLabel(3,"2");
      hNBtagJets[j][i]->GetXaxis()->SetBinLabel(4,"3");
      hNBtagJets[j][i]->GetXaxis()->SetBinLabel(5,"4");
      hNBtagJets[j][i]->GetXaxis()->SetBinLabel(6,"#geq 5");
      if(j == 2) hNBtagJets[j][i]->GetXaxis()->SetRange(3,6);
      if(j == 3) hNBtagJets[j][i]->GetXaxis()->SetRange(5,6);
      
      /***************************
          SF(ID,ISO & Trigger)
      ***************************/
      hSFIDISOTr[j][i]       = new TH1F("hSFIDISOTr_"+namech[i]+"_"+namecut[j],"SF_{ID,ISO}^{Tr} " + titlenamech[i],400,0.8,1.2);    
      hSFIDISOTrError[j][i]  = new TH1F("hSFIDISOTrError_"+namech[i]+"_"+namecut[j],"#Delta SF_{ID,ISO}^{Tr} " + titlenamech[i],400,0,0.1); 
      hSFIDISO[j][i]         = new TH1F("hSFIDISO_"+namech[i]+"_"+namecut[j],"SF_{ID,ISO} " + titlenamech[i],400,0.8,1.2);    
      hSFIDISOError[j][i]    = new TH1F("hSFIDISOError_"+namech[i]+"_"+namecut[j],"#Delta SF_{ID,ISO} " + titlenamech[i],400,0,0.1); 
      hSFTrigger[j][i]       = new TH1F("hSFTrigger_"+namech[i]+"_"+namecut[j],"SF^{Trigger} " + titlenamech[i],400,0.8,1.2);    
      hSFTriggerError[j][i]  = new TH1F("hSFTriggerError_"+namech[i]+"_"+namecut[j],"#Delta SF^{Trigger} " + titlenamech[i],400,0,0.1);

      /***************************
              SF(b-tag)
      ***************************/
      h2DSFbtag_Global[j][i]   = new TH2F("h2DSFbtag_Global_"+namech[i]+"_"+namecut[j], "Global SF_{b-tag} Vs  #Delta SF_{b-tag} " + titlenamech[i], 40, 0.0, 4.0, 50, 0.0, 0.5);
      hSFbtag_Global[j][i]     = new TH1F("hSFbtag_Global_"+namech[i]+"_"+namecut[j], "Global SF_{b-tag} " + titlenamech[i],40, 0.0, 4.0);
      hSFbtag_Global_var[j][i] = new TH1F("hSFbtag_Global_var_"+namech[i]+"_"+namecut[j], "Global #Delta SF_{b-tag} " + titlenamech[i], 20, 0.0, 0.10);

      // B-tag efficiency histograms
      h2DSFbtag_b[j][i]    = new TH2F("hSFbtag_b_"+namech[i]+"_"+namecut[j], "N^{b}(p_{T} vs #eta) " + titlenamech[i] + ";p_{T}[GeV];#eta",7,0.0,140.0,4,0.0,2.4);
      h2DSFbtag_c[j][i]    = new TH2F("hSFbtag_c_"+namech[i]+"_"+namecut[j], "N^{c}(p_{T} vs #eta) " + titlenamech[i] + ";p_{T}[GeV];#eta",7,0.0,140.0,4,0.0,2.4);
      h2DSFbtag_l[j][i]    = new TH2F("hSFbtag_l_"+namech[i]+"_"+namecut[j], "N^{l}(p_{T} vs #eta) " + titlenamech[i] + ";p_{T}[GeV];#eta",7,0.0,140.0,4,0.0,2.4);

      h2DSFbtag_btag_b[j][i]    = new TH2F("hSFbtag_btag_b_"+namech[i]+"_"+namecut[j], "N_{btag}^{b}(p_{T} vs #eta) " + titlenamech[i] + ";p_{T}[GeV];#eta",7,0.0,140.0,4,0.0,2.4);
      h2DSFbtag_btag_c[j][i]    = new TH2F("hSFbtag_btag_c_"+namech[i]+"_"+namecut[j], "N_{btag}^{c}(p_{T} vs #eta) " + titlenamech[i] + ";p_{T}[GeV];#eta",7,0.0,140.0,4,0.0,2.4);
      h2DSFbtag_btag_l[j][i]    = new TH2F("hSFbtag_btag_l_"+namech[i]+"_"+namecut[j], "N_{btag}^{l}(p_{T} vs #eta) " + titlenamech[i] + ";p_{T}[GeV];#eta",7,0.0,140.0,4,0.0,2.4);
      
      for(int ij=0; ij<NhJets; ij++){
	TString jetn;
	std::ostringstream jni;
	jni << ij;
	jetn = "Jet-" + jni.str();	
	
	hCSV[ij][j][i]   = new TH1F("hCSV_"   + jetn + "_" + namech[i] + "_" + namecut[j],"CSV " + jetn + " " + titlenamech[i] + ";CSVv2",10,0,1);
	hCvsL[ij][j][i]  = new TH1F("hCvsL_"  + jetn + "_" + namech[i] + "_" + namecut[j],"CvsL " + jetn + " " + titlenamech[i] + ";CvsL",20,-1,1);
	hCvsB[ij][j][i]  = new TH1F("hCvsB_"  + jetn + "_" + namech[i] + "_" + namecut[j],"CvsB " + jetn + " " + titlenamech[i] + ";CvsB",20,-1,1);
	hJetPt[ij][j][i] = new TH1F("hJetPt_" + jetn + "_" + namech[i] + "_" + namecut[j],"p_{T}^{Jet} " + jetn + " " + titlenamech[i] + ";p_{T}[GeV]",10,0,200);

	hJetpTUncVar[ij][j][i] = new TH1F("hJetpTUncVar_" + jetn + "_" + namech[i] + "_" + namecut[j], "#Delta pT^{Jet} " + jetn + " " + titlenamech[i], 20.0, 0.0, 2.0);
	
	for(int jj=ij+1; jj<NhJets; jj++){
	  TString jetMassn;
	  std::ostringstream jni, jnj;
	  jni << ij;
	  jnj << jj;
	  jetMassn = "Jet" + jni.str() + jnj.str();
	  
	  hMassJet[ij][jj][j][i] = new TH1F("hMassJet_" + jetMassn + "_" + namech[i] + "_" + namecut[j],"Mass of Dijets "+ jetMassn + " " + titlenamech[i] + "; M_{jj}[GeV]",150,0,300);
	  hDRJet[ij][jj][j][i]   = new TH1F("hDRJet_"   + jetMassn + "_" + namech[i] + "_" + namecut[j],"#Delta R of Dijets "+ jetMassn + " " + titlenamech[i] + "; #Delta R_{jj}",25,0,5);
	  h2DCSV[ij][jj][j][i]   = new TH2F("h2DCSV_"   + jetMassn + "_" + namech[i] + "_" + namecut[j], "CSVv2 Discriminant for 3rd and 4th Jets " + titlenamech[i], 20,0,1,20,0,1);
	}
	
      }
      
      hInvMassjj[j][i]  = new TH1F("hInvMassjj_" + namech[i] + "_"+namecut[j], "Compatible Inv. Mass " + titlenamech[i],80,40,120);

      // Kinematic Reconstruction
      hKinChi2 [j][i] = new TH1F("hKinChi2_" + namech[i] + "_" + namecut[j], "#chi^{2} for Kin. RECO " + titlenamech[i] + ";#chi^{2}", 100,0,20);
      h2DKinChi2_JetMatch[j][i] = new TH2F("hKinChi2_JetMatch_" + namech[i] + "_" + namecut[j], "#chi^{2} Vs # of Jet Matches for Kin. RECO " + titlenamech[i], 100,0,20,5,0,5);
      effKinGenIndex [j][i] = new TEfficiency("effKinGenIndex_" + namech[i] + "_" + namecut[j], "Kin. RECO vs GEN " + titlenamech[i] + "; [0]->All 4 jets, [1]->Top, [2]->W, [3]->Add; Match Eff.", 4,0,4);
      purKinGenIndex [j][i] = new TEfficiency("purKinGenIndex_" + namech[i] + "_" + namecut[j], "Kin. RECO vs GEN Purity " + titlenamech[i] + "; [0]->Top, [1]->W, [2]->Add; Purity", 3,0,3);

      hKinWlTransMass[j][i] = new TH1F("hKinWlTransMass_" + namech[i] + "_" + namecut[j], "Inv. Trans. Mass of W(lep) from Kin Reco " + titlenamech[i] + "; M_{T}^{W#to l#nu} [GeV]", 50,0,150);
      hKinWlMass[j][i]      = new TH1F("hKinWlMass_" + namech[i] + "_" + namecut[j], "Inv. Mass of W(lep) from Kin Reco " + titlenamech[i] + "; M_{W#to l#nu} [GeV]", 80,40,120);
      hKinWlpT[j][i]        = new TH1F("hKinWlpT_"   + namech[i] + "_" + namecut[j], "p_{T} of W(lep) from Kin Reco " + titlenamech[i] + "; p_{T} [GeV]", 30,0,300);

      hKinWhMass[j][i] = new TH1F("hKinWhMass_" + namech[i] + "_" + namecut[j], "Inv. Mass of W(had) from Kin Reco " + titlenamech[i] + "; M_{W#to jj} [GeV]", 80, 40, 120);
      hKinWhpT[j][i]   = new TH1F("hKinWhpT_"   + namech[i] + "_" + namecut[j], "p_{T} of W(had) from Kin Reco " + titlenamech[i] + "; p_{T} [GeV]", 30,0,300);

      hKinWMass[j][i] = new TH1F("hKinWMass_" + namech[i] + "_" + namecut[j], "Inv. Mass of W from Kin Reco " + titlenamech[i] + "; M_{W} [GeV]", 80, 40, 120);
      hKinWpT[j][i]   = new TH1F("hKinWpT_"   + namech[i] + "_" + namecut[j], "p_{T} of W from Kin Reco " + titlenamech[i] + "; p_{T} [GeV]", 30,0,300);

      hKinTagWMass [j][i]  = new TH1F("hKinTagWMass_" + namech[i] + "_" + namecut[j], "Inv. Mass of W_{jj} boson " + titlenamech[i] + "; M_{W} [GeV]", 80, 40, 120);
      hKinTagAddMass[j][i] = new TH1F("hKinTagAddMass_" + namech[i] + "_" + namecut[j], "Inv. Mass of Additional Jets " + titlenamech[i] + "; M [GeV]", 150, 0, 300);
      hKinTagAddDR  [j][i] = new TH1F("hKinTagAddDR_" + namech[i] + "_" + namecut[j], "#Delta R of Additional Jets " + titlenamech[i] + "; #Delta R", 25, 0, 5);

      hKintlMass[j][i] = new TH1F("hKintlMass_" + namech[i] + "_" + namecut[j], "Inv. Mass of Top(lep) from Kin Reco " + titlenamech[i] + "; M_{t_{l}} [GeV]", 100, 100, 300);
      hKintlpT[j][i]   = new TH1F("hKintlpT_"   + namech[i] + "_" + namecut[j], "p_{T} of Top(lep) from Kin Reco " + titlenamech[i] + "; p_{T} [GeV]", 30,0,300);

      hKinthMass[j][i] = new TH1F("hKinthMass_" + namech[i] + "_" + namecut[j], "Inv. Mass of Top(had) from Kin Reco " + titlenamech[i] + "; M_{t_{h}} [GeV]", 100, 100, 300);
      hKinthpT[j][i]   = new TH1F("hKinthpT_"   + namech[i] + "_" + namecut[j], "p_{T} of Top(had) from Kin Reco " + titlenamech[i] + "; p_{T} [GeV]", 30,0,300);

      TString kinJetname[4];
      kinJetname[0] = "bFromH";
      kinJetname[1] = "W1";
      kinJetname[2] = "W2";
      kinJetname[3] = "bFromL";
      for(unsigned int ikj=0; ikj<4; ikj++){
	hKinJetPt[ikj][j][i] = new TH1F ("hKinJetPt_" + kinJetname[ikj] + "_"  + namech[i] + "_" + namecut[j], "pT KIN Assignment " + titlenamech[i] + "; p_{T}^{j}[GeV]", 50,0,150);
	hGENJetPt[ikj][j][i] = new TH1F ("hGENJetPt_" + kinJetname[ikj] + "_"  + namech[i] + "_" + namecut[j], "pT GEN Assignment " + titlenamech[i] + "; p_{T}^{j}[GeV]", 50,0,150);
      }
      
      hTJetPosition[j][i] = new TH1F ("hTJetPosition_" + namech[i] + "_" + namecut[j], "CSV Position for jets from Top "   + titlenamech[i] + "; CSV Jet Index", 7,0,7);
      hWJetPosition[j][i] = new TH1F ("hWJetPosition_" + namech[i] + "_" + namecut[j], "CSV Position for jets from W "     + titlenamech[i] + "; CSV Jet Index", 7,0,7);
      hOJetPosition[j][i] = new TH1F ("hOJetPosition_" + namech[i] + "_" + namecut[j], "CSV Position for additional jets " + titlenamech[i] + "; CSV Jet Index", 7,0,7);
      effTagCSV    [j][i] = new TEfficiency ("effTagCSV_" + namech[i] + "_" + namecut[j], "CSV Tag Eff. " + titlenamech[i] + "; [0]->Top, [1]->W, [2]->Add; Match Eff.", 3,0,3);
      purTagCSV    [j][i] = new TEfficiency ("purTagCSV_" + namech[i] + "_" + namecut[j], "CSV Purity " + titlenamech[i] + "; [0]->Top, [1]->W, [2]->Add; Purity", 3,0,3);
      hGenTagWMass  [j][i] = new TH1F("hGenTagWMass_" + namech[i] + "_" + namecut[j], "Inv. Mass of W boson " + titlenamech[i] + "; M_{W} [GeV]", 80, 40, 120);
      hGenTagAddMass[j][i] = new TH1F("hGenTagAddMass_" + namech[i] + "_" + namecut[j], "Inv. Mass of Add Jets " + titlenamech[i] + "; M [GeV]", 150, 0, 300);
      hGenTagAddDR  [j][i] = new TH1F("hGenTagAddDR_" + namech[i] + "_" + namecut[j], "#Delta R of Add Jets " + titlenamech[i] + "; #Delta R", 25, 0, 5);

    }//for(i->channel)
  }//for(j->cut)
  
  // BEST escenario plots  
  h2DTJetPosition    = new TH2F("h2DTJetPosition","CSV Position for jets from Top Vs Dijet Rank",  7,0,7,7,0,7);
  h2DWJetPosition    = new TH2F("h2DWJetPosition","CSV Position for jets from W Vs Dijet Rank",    7,0,7,7,0,7);
  
  h2DttbarNGenJets   = new TH2F("h2DttbarNGenJets","Number of b and W jets per event",    3,0,3,4,0,4);
  h2DttbarNGenJets->GetXaxis()->SetBinLabel(1,"b-jets from top");
  h2DttbarNGenJets->GetXaxis()->SetBinLabel(2,"jets from W");
  h2DttbarNGenJets->GetXaxis()->SetBinLabel(3,"Nj(Kin/Gen) matched");

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
  TH2F *hmuIDISOSF, *hmuTriggerSF;
  TH2F *heIDISOSF,  *heTriggerSF;

  GetSFHistogram ("ScaleFactors/", 
		  "IDISO_Trigger_POG25ns",
		  hmuIDISOSF, hmuTriggerSF, heIDISOSF,  heTriggerSF);

  // Jet uncertainties (btag, JES and JER)
  if(_syst) fname += "_SYS_" + syst_varname;

  /***************************
     ttbar Categorization
  ***************************/
  if(_ttbar_cat) fname += ttbar_id; // add in the sample name the ttbar category

  // New WP for 76X: https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation76X
  float CSV_WP = 0.800; // Medium
  //float CSV_WP = 0.935; // Tight
  int btagSysPar = 0;

  // Global SF uncertainty: 18 Components
  if     (_syst && syst_varname.Contains("btagjes_Up"))     btagSysPar = btagUnc::JES_UP;
  else if(_syst && syst_varname.Contains("btagjes_Down"))   btagSysPar = btagUnc::JES_DN;
  else if(_syst && syst_varname.Contains("btaglf_Up"))      btagSysPar = btagUnc::LF_UP;
  else if(_syst && syst_varname.Contains("btaglf_Down"))    btagSysPar = btagUnc::LF_DN;
  else if(_syst && syst_varname.Contains("btaghf_Up"))      btagSysPar = btagUnc::HF_UP;
  else if(_syst && syst_varname.Contains("btaghf_Down"))    btagSysPar = btagUnc::HF_DN;
  else if(_syst && syst_varname.Contains("btaghfsI_Up"))    btagSysPar = btagUnc::HFSTAT1_UP;
  else if(_syst && syst_varname.Contains("btaghfsI_Down"))  btagSysPar = btagUnc::HFSTAT1_DN;
  else if(_syst && syst_varname.Contains("btaghfsII_Up"))   btagSysPar = btagUnc::HFSTAT2_UP;
  else if(_syst && syst_varname.Contains("btaghfsII_Down")) btagSysPar = btagUnc::HFSTAT2_DN;
  else if(_syst && syst_varname.Contains("btaglfsI_Up"))    btagSysPar = btagUnc::LFSTAT1_UP;
  else if(_syst && syst_varname.Contains("btaglfsI_Down"))  btagSysPar = btagUnc::LFSTAT1_DN;
  else if(_syst && syst_varname.Contains("btaglfsII_Up"))   btagSysPar = btagUnc::LFSTAT2_UP;
  else if(_syst && syst_varname.Contains("btaglfsII_Down")) btagSysPar = btagUnc::LFSTAT2_DN;
  else if(_syst && syst_varname.Contains("btagcfI_Up"))     btagSysPar = btagUnc::CFERR1_UP;
  else if(_syst && syst_varname.Contains("btagcfI_Down"))   btagSysPar = btagUnc::CFERR1_DN;
  else if(_syst && syst_varname.Contains("btagcfII_Up"))    btagSysPar = btagUnc::CFERR2_UP;
  else if(_syst && syst_varname.Contains("btagcfII_Down"))  btagSysPar = btagUnc::CFERR2_DN;

  // Scale Uncertainty
  // From: https://indico.cern.ch/event/494682/contribution/2/attachments/1223578/1800218/mcaod-Feb15-2016.pdf
  int scaleSysPar;
  if     (_syst && syst_varname.Contains("ScaleRnF_Up"))   scaleSysPar = 0; // muR=Nom,  muF=Up
  else if(_syst && syst_varname.Contains("ScaleRnF_Down")) scaleSysPar = 1; // muR=Nom,  muF=Down
  else if(_syst && syst_varname.Contains("ScaleRuF_Nom"))  scaleSysPar = 2; // muR=Up,   muF=Nom
  else if(_syst && syst_varname.Contains("ScaleRuF_Up"))   scaleSysPar = 3; // muR=Up,   muF=Up
  else if(_syst && syst_varname.Contains("ScaleRdF_Nom"))  scaleSysPar = 4; // muR=Down, muF=Nom
  else if(_syst && syst_varname.Contains("ScaleRdF_Down")) scaleSysPar = 5; // muR=Down, muF=Down
  // Normalization for Scale Weights:
  if (_syst && syst_varname.Contains("Scale")){
    if(scaleSysPar < 6) nNorm_Event = NTotal_ScalemuRF_Weight[scaleSysPar];
    else{
      std::cerr << "No entry for Scale normalization! Check HISTO!"  << std::endl;
      std::exit(0);
    }
  }
  // PileUp Uncertainty  
  int pileupSysPar;
  if     (_syst && syst_varname.Contains("PileUp_Up"))   pileupSysPar = 1; // Up
  else if(_syst && syst_varname.Contains("PileUp_Down")) pileupSysPar = 2; // Down

  /************************
    Normalization Weights
  *************************/  
  float NormWeight = 0.0;
  // NormWeight = Lumi*(1.0/N_Gen_events)*(Xsec)
  NormWeight = SFLumi(fname, LuminNorm, nNorm_Event);  

  std::cout << "-----------------------                                 -------------------------" << std::endl;
  std::cout << "Number of Events     = " << nNorm_Event << std::endl;
  std::cout << "Normalization Factor = " << NormWeight  << std::endl;
  std::cout << "---------------------------------------------------------------------------------" << std::endl;

  /********************************
             Event Loop
  ********************************/
  // Number of events
  int MaxEvt = theTree.GetEntries();
  if(_NUserEvt > 0) MaxEvt = std::min(MaxEvt, _NUserEvt);

  std::cout << "--- Processing: " << MaxEvt << " events" << std::endl;
  
  for (Long64_t ievt=0; ievt<MaxEvt; ievt++) {
    
    theTree.GetEntry(ievt);  
    print_progress(theTree.GetEntries(), ievt);

    // PU reweight: Includes Syst. Unc.
    if (_syst && syst_varname.Contains("PileUp"))
      PUWeight = (*PUWeight_sys)[pileupSysPar]; // Up
    else PUWeight = (*PUWeight_sys)[0];
    
    // Luminosity Weight + MCatNLO GEN Weights (For MC@NLO) 
    PUWeight = PUWeight * NormWeight * GENWeight;
    
    // Scale reweight: Syst. Unc.
    if (_syst && syst_varname.Contains("ScaleR"))
      PUWeight = PUWeight*(*ScaleWeight)[scaleSysPar];
    
    unsigned int NJets, NBtagJets;
    
    TLorentzVector Lep;
    
    if (_syst && syst_varname.Contains("LES_Up"))        Lep.SetPtEtaPhiE(Lep_pT + (Lep_pT*Lep_LES),
									  Lep_eta,
									  Lep_phi,
									  Lep_E);
    else if (_syst && syst_varname.Contains("LES_Down")) Lep.SetPtEtaPhiE(Lep_pT - (Lep_pT*Lep_LES),
									  Lep_eta,
									  Lep_phi,
									  Lep_E);
    else Lep.SetPtEtaPhiE(Lep_pT,Lep_eta,Lep_phi,Lep_E);    

    // Lep pT >30GeV
    if(Lep.Pt() < 30)  continue; 
    
    // Transverse W Mass
    TLorentzVector METv;
    METv.SetPtEtaPhiE(MET,0.0,MET_Phi,MET);
    float mT = sqrt(2*MET*Lep.Pt()*(1.0-cos( Lep.DeltaPhi(METv) )));
    
    // Jets     
    NJets      = 0;
    NBtagJets  = 0;
    
    // Global SF_b-tag
    // From: https://twiki.cern.ch/twiki/bin/view/CMS/BTagShapeCalibration
    float btagUnc_val = 0.0;
    if (!fname.Contains("Data")){
      if(_syst && syst_varname.Contains("btag_Up"))
	btagUnc_val = 1.0 * (*Jet_SF_CSV)[btagSysPar];
      else if(_syst && syst_varname.Contains("btag_Down")) 
	btagUnc_val = -1.0 * (*Jet_SF_CSV)[btagSysPar];      
      // SF estimated for jets with pT > 25GeV
      PUWeight = PUWeight * ((*Jet_SF_CSV)[btagUnc::CENTRAL] + btagUnc_val);
    }// if(!data)
    
    // Kinematic Reconstruction (at least 4 Jets)
    // Order for Jets: 
    // [0] b from hadronic leg 
    // [1] j from W
    // [2] j from W
    // [3] b from leptonic leg
    int KinJetIndex[4];
    TLorentzVector KinNu;
    ComJet KinJet[4];
    KinNu.SetPtEtaPhiE(KinNu_pT, KinNu_eta, KinNu_phi, KinNu_E);
    for(unsigned int ikj=0; ikj<4; ikj++){
      KinJetIndex[ikj] = (*KinJet_Index)[ikj];
      KinJet[ikj].SetPtEtaPhiE((*KinJet_pT)[ikj],(*KinJet_eta)[ikj],(*KinJet_phi)[ikj],(*KinJet_E)[ikj]);
    }
    bool isKinEvt = true; // TEMPORAL SOLUTION -> Change default indexes from 0 to -999
    if (KinJetIndex[0] == 0 && KinJetIndex[1] == 0 &&
	KinJetIndex[2] == 0 && KinJetIndex[3] == 0 ) isKinEvt = false;
 
    std::vector<ComJet> Jets;    
    for(int ijet=0; ijet < Jet_pT->size(); ijet++){

      float JetSystVar = 1.0;
      if(_syst){
	if(syst_varname.Contains("JES_Up")){
	  JetSystVar = (*Jet_JES_Up)[ijet];
	}
	else if(syst_varname.Contains("JES_Down")){
	  JetSystVar = (*Jet_JES_Down)[ijet];
	}
	else if(syst_varname.Contains("JER_Up")){
	  JetSystVar = (*Jet_JER_Up)[ijet];
	}
	else if(syst_varname.Contains("JER_Nom")){
	  JetSystVar = (*Jet_JER_Nom)[ijet];
	}
	else if(syst_varname.Contains("JER_Down")){
	  JetSystVar = (*Jet_JER_Down)[ijet];
	}
      }
      
      ComJet jet;
      jet.SetPtEtaPhiE(JetSystVar * (*Jet_pT)[ijet], // Syst. Var affects only pT
		       (*Jet_eta)[ijet],
		       (*Jet_phi)[ijet],
		       (*Jet_E)[ijet]);
      jet.Flavour = (*Jet_partonFlavour)[ijet];
      jet.CSV     = (*Jet_CSV)[ijet];
      jet.CvsL    = (*Jet_CvsL)[ijet];
      jet.CvsB    = (*Jet_CvsB)[ijet];
      jet.Mom     = -1;
      jet.KinMom  = -1;
      
      // Jet Mother
      if((fname.Contains("ttbar")  && !fname.Contains("Bkg"))) jet.Mom = (*Jet_Mom)[ijet];
      // Kin Mother
      // [0] b from hadronic leg 
      // [1] j from W
      // [2] j from W
      // [3] b from leptonic leg
      if ( isKinEvt ){
	if (ijet == KinJetIndex[0] || ijet == KinJetIndex[3] ) jet.KinMom = 6;
	if (ijet == KinJetIndex[1] || ijet == KinJetIndex[2] ) jet.KinMom = 24;
      }

      if(jet.Pt() > 25){ // Jet pT Cut

	Jets.push_back(jet);
	
	/*******************************************
                       b-tagging
	*******************************************/    
	// OLD b-tagging Method: 
	// b-tagging WP from https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideBTagging
	// Not recommended for analysis with observables that depends ontagging.
	// if (fname.Contains("Data")) btagDisc = fBTagSF->IsTagged((*Jet_CSV)[ijet], -999999, jet.Pt(), jet.Eta());
	// else btagDisc = fBTagSF->IsTagged((*Jet_CSV)[ijet], JetFlav, jet.Pt(), jet.Eta()); 
	// New Method (Event SF from tth group)
	// https://twiki.cern.ch/twiki/bin/view/CMS/BTagShapeCalibration
	if(jet.CSV > CSV_WP) NBtagJets++; // Number of b-tagged jets
	
      } // if(Jet_pT)
    }// for(jets)
    
    NJets = Jets.size();
    
    float Mjj    = 0.0; // Dijet Inv. Mass   
    // Only Final Cut Level
    if(NJets > 5 && NBtagJets > 1){ 
      // Estimation of the DiJet invariant mass closest to the W mass
      bool ReArrangeMjj = false; // Jets keep the same order
      Mjj = DiJetMassCorrection(Jets, ReArrangeMjj);            
    } //if (6jets && 1btag)    
    
    /*******************************************
                Number of GENJets
    *******************************************/    
    int NGenJets = 0;
    if (_ttbar_cat){
      for(int igenjet=0; igenjet < GenJet_pT->size(); igenjet++){
	TLorentzVector TLGenJet((*GenJet_pT) [igenjet],
				(*GenJet_eta)[igenjet],
				(*GenJet_phi)[igenjet],
				(*GenJet_E)  [igenjet]); 
	if( TLGenJet.Pt() > 20 ) NGenJets++;
      }
    }

  /*******************************************
   Trigger,ID & ISO Scale Factors/bin(Pt,Eta)
  *******************************************/    
    std::vector<float> SF_ID_ISO_Tr;
    
    if (fname.Contains("Data")){
      SF_ID_ISO_Tr.push_back(1.0); // SF_ID_ISO_Tr      [0] 
      SF_ID_ISO_Tr.push_back(1.0); // SF_ID_ISO_Tr_Up   [1] 
      SF_ID_ISO_Tr.push_back(1.0); // SF_ID_ISO_Tr_Down [2] 
      SF_ID_ISO_Tr.push_back(1.0); // SF_ID_ISO         [3] 
      SF_ID_ISO_Tr.push_back(1.0); // SF_ID_ISO_Error   [4] 
      SF_ID_ISO_Tr.push_back(1.0); // SF_Tr             [5] 
      SF_ID_ISO_Tr.push_back(1.0); // SF_Tr_Error       [6] 
    }
    
    else {
      // Second Method: Taking SF from root file
      // SFIDISOTrigger(SF_ID_ISO_Tr,
      // 		     Lep, Channel,
      // 		     hmuIDISOSF, hmuTriggerSF,
      // 		     heIDISOSF,  heTriggerSF);
      
      // From CATTuple
      SF_ID_ISO_Tr = (*Lep_SF);
      
      if(_syst && syst_varname.Contains("LepSF")){
	float SFSystUnc = SF_ID_ISO_Tr[0]*0.015; //Additional??
	if(syst_varname.Contains("Up"))   PUWeight = PUWeight * (SF_ID_ISO_Tr[1] + SFSystUnc) ;
	if(syst_varname.Contains("Down")) PUWeight = PUWeight * (SF_ID_ISO_Tr[2] - SFSystUnc);
      }
      else PUWeight = PUWeight * SF_ID_ISO_Tr[0]; // Central
     
    }// else(Contain("Data"))
    

    /***************************
            Selection
    ***************************/
    
    bool JumpCutEvent[Nhcuts];
    for(unsigned int bcut=0; bcut<Nhcuts; bcut++) JumpCutEvent[bcut] = true;

    JumpCutEvent[0] = false;                               // Single Lepton (from Tree)
    if(NJets > 5)                  JumpCutEvent[1]= false; // lep + 6 Jets 
    if(NJets > 5 && NBtagJets > 1) JumpCutEvent[2]= false; // lep + 6 Jets + 2 b-tag
    if(NJets > 5 && NBtagJets > 2) JumpCutEvent[3]= false; // lep + 6 Jets + 3 b-tag
    if(NJets > 3)                  JumpCutEvent[4]= false; // lep + 4 Jets 
    if(NJets > 3 && NBtagJets > 1) JumpCutEvent[5]= false; // lep + 4 Jets + 2 b-tag 
    if(NBtagJets > 1)              JumpCutEvent[6]= false; // lep + 2 b-tag 


    bool JumpEvent = false;
    
    /***************************
        ttbar Categorization
    ***************************/
    if (_ttbar_cat) JumpEvent = !IsSelectedttbarCategory(GenConeCat, ttbar_id);
    
    /********************************
      Exclusive Data and QCD Samples
    *********************************/
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
      
      /*******************
        Fill Histograms
      *******************/
      hSFIDISOTr[icut][Channel]->Fill(SF_ID_ISO_Tr[0],PUWeight);
      hSFIDISOTrError[icut][Channel]->Fill((SF_ID_ISO_Tr[1] - SF_ID_ISO_Tr[0]),PUWeight);
      //hSFIDISO[icut][Channel]->Fill(SF_ID_ISO_Tr[1],PUWeight);
      //hSFIDISOError[icut][Channel]->Fill(SF_ID_ISO_Tr[2],PUWeight);
      //hSFTrigger[icut][Channel]->Fill(SF_ID_ISO_Tr[3],PUWeight);
      //hSFTriggerError[icut][Channel]->Fill(SF_ID_ISO_Tr[4],PUWeight);
    
      /******************
          Acc. / Eff. 
      ******************/
      AccEvent[icut][Channel]++;
      EffEvent[icut][Channel]= EffEvent[icut][Channel] + PUWeight;

      /******************
        Kinematic Var.
      ******************/      
      hPV     [icut][Channel]->Fill(GoodPV,             PUWeight);
      hMET    [icut][Channel]->Fill(MET,                PUWeight);
      hMET_Phi[icut][Channel]->Fill(fabs(MET_Phi),      PUWeight);
      hLepPt  [icut][Channel]->Fill(Lep.Pt(),           PUWeight);
      hLepEta [icut][Channel]->Fill(std::abs(Lep.Eta()),PUWeight);
      hLepPhi [icut][Channel]->Fill(std::abs(Lep.Phi()),PUWeight);
      hmT     [icut][Channel]->Fill(mT,                 PUWeight);
      
      /******************
          Jets Var.
      ******************/
      hNJets    [icut][Channel]->Fill(NJets,    PUWeight); 
      hNBtagJets[icut][Channel]->Fill(NBtagJets,PUWeight);
      // Dijet InvMass
      hInvMassjj[icut][Channel]->Fill(Mjj, PUWeight);
      // Global btag SF
      hSFbtag_Global    [icut][Channel]->Fill((*Jet_SF_CSV)[btagUnc::CENTRAL], PUWeight);
      hSFbtag_Global_var[icut][Channel]->Fill(btagUnc_val,                     PUWeight);
      h2DSFbtag_Global  [icut][Channel]->Fill((*Jet_SF_CSV)[btagUnc::CENTRAL], btagUnc_val, PUWeight);
      
      // Jet Variables
      int kinGenConeMatch = 0, TopkinGenConeMatch = 0, WkinGenConeMatch = 0, OkinGenConeMatch = 0;
      bool fKinAddjj = true;
      bool fGenAddjj = true;
      for(int ijet=0; ijet < Jets.size(); ijet++){
	ComJet jet = Jets[ijet];
	// b-Jet Efficiencies
	if(jet.Flavour == 5){
	  h2DSFbtag_b[icut][Channel]->Fill(jet.Pt(), fabs(jet.Eta()), PUWeight); // b-Flavour
	  if(jet.CSV > CSV_WP) h2DSFbtag_btag_b[icut][Channel]->Fill(jet.Pt(), fabs(jet.Eta()), PUWeight);  
	}	
	else if(jet.Flavour == 4){
	  h2DSFbtag_c[icut][Channel]->Fill(jet.Pt(), fabs(jet.Eta()), PUWeight); // c-Flavour
	  if(jet.CSV > CSV_WP) h2DSFbtag_btag_c[icut][Channel]->Fill(jet.Pt(), fabs(jet.Eta()), PUWeight);  
	}
	else if(jet.Flavour == 1 || jet.Flavour == 2 || jet.Flavour == 3){
	  h2DSFbtag_l[icut][Channel]->Fill(jet.Pt(), fabs(jet.Eta()), PUWeight); // l-Flavour
	  if(jet.CSV > CSV_WP) h2DSFbtag_btag_l[icut][Channel]->Fill(jet.Pt(), fabs(jet.Eta()), PUWeight);  
	}
	
	if (ijet < NhJets){
	  hJetPt[ijet][icut][Channel]->Fill(jet.Pt(), PUWeight);
	  hCSV  [ijet][icut][Channel]->Fill(jet.CSV,  PUWeight);
	  hCvsL [ijet][icut][Channel]->Fill(jet.CvsL, PUWeight);
	  hCvsB [ijet][icut][Channel]->Fill(jet.CvsB, PUWeight);
	}

	//Dijet Invariant Mass 
	int jbmax = std::min(NhJets,NJets);
	for(int jjet=ijet+1; jjet < jbmax; jjet++){
	  ComJet jet_ = Jets[jjet];
	  float DijetInvMass = (jet+jet_).M(); 
	  float DijetDR = jet.DeltaR(jet_); 
	  hMassJet[ijet][jjet][icut][Channel]->Fill(DijetInvMass, PUWeight);
	  hDRJet  [ijet][jjet][icut][Channel]->Fill(DijetDR,      PUWeight);
	  // 2D CSV discriminant plot for all DiJets system	  
	  h2DCSV[ijet][jjet][icut][Channel]->Fill(jet.CSV, jet_.CSV, PUWeight);
	  // Gen Mass
	  if(jet.Mom == 24 && jet_.Mom ==24)        hGenTagWMass[icut][Channel]->Fill(DijetInvMass, PUWeight);
	  if(jet.Mom != 24 && jet_.Mom !=24 &&
	     jet.Mom != 6  && jet_.Mom != 6 &&
	     fGenAddjj){
	    hGenTagAddMass[icut][Channel]->Fill(DijetInvMass, PUWeight);
	    hGenTagAddDR  [icut][Channel]->Fill(DijetDR,      PUWeight);
	    fGenAddjj = false;
	  }
	  // Kin Mass
	  if(jet.KinMom == 24 && jet_.KinMom == 24) hKinTagWMass[icut][Channel]->Fill(DijetInvMass, PUWeight);
	  if(jet.KinMom != 24 && jet_.KinMom !=24 &&
	     jet.KinMom != 6  && jet_.KinMom != 6 &&
	     fKinAddjj){
	    hKinTagAddMass[icut][Channel]->Fill(DijetInvMass, PUWeight);
	    hKinTagAddDR  [icut][Channel]->Fill(DijetDR,      PUWeight);
	    fKinAddjj = false;
	  }
	}// for(jjet)

	// Jet Tag Efficiencies
	if(NJets > 3){ 
	  // -- CSV purity
	  // Jets from Top
	  if(ijet < 2)                   purTagCSV[icut][Channel]-> Fill(jet.Mom == 6, 0.5);
	  // Add jets
	  else if(ijet > 1 && ijet < 4)  purTagCSV[icut][Channel]-> Fill((jet.Mom != 24 && jet.Mom != 6), 1.5);
	  // Jets from W
	  else if(ijet > 3)              purTagCSV[icut][Channel]-> Fill(jet.Mom == 24, 2.5);
	  // -- Kin. fitter purity
	  // Jets from Top
	  if(jet.KinMom == 6)  purKinGenIndex[icut][Channel]->Fill(jet.Mom == 6,  0.5);
	  // Jets from W
	  if(jet.KinMom == 24) purKinGenIndex[icut][Channel]->Fill(jet.Mom == 24, 1.5);
	  // Add Jets
	  if(jet.KinMom != 24 && jet.KinMom != 6) purKinGenIndex[icut][Channel]->Fill(jet.Mom != 24 && jet.Mom != 6, 2.5);


	  // GenCone Vs Kinematic Reconstruction 
	  if (jet.Mom == 6 || jet.Mom == 24){ 
	    
	    if(jet.Mom == jet.KinMom) kinGenConeMatch++; 
	    
	    if(jet.Mom == 6) { effKinGenIndex[icut][Channel]->Fill(jet.KinMom == 6,  1.5); TopkinGenConeMatch++;}
	    if(jet.Mom == 24){ effKinGenIndex[icut][Channel]->Fill(jet.KinMom == 24, 2.5); WkinGenConeMatch++;}
	  } // if ((jet.Mom)
	  else if(jet.Mom != 6 && jet.Mom != 24){ effKinGenIndex[icut][Channel]->Fill((jet.KinMom != 24 && jet.KinMom != 6), 3.5); OkinGenConeMatch++;}

	  // CSV Jet Postion Eff (No weights)
	  bool isGoodCSVOrder = false;
	  int CSVBin;
	  if(jet.Mom == 6){
	    CSVBin = 0.5;
	    if(ijet < 2) isGoodCSVOrder = true; 
	    hTJetPosition[icut][Channel]->Fill(ijet);
	  }
	  else if(jet.Mom == 24){
	    CSVBin = 1.5;
	    if(ijet > 3) isGoodCSVOrder = true;
	    hWJetPosition[icut][Channel]->Fill(ijet);
	  }
	  else{
	    CSVBin = 2.5;
	    if(ijet > 1 && ijet < 4) isGoodCSVOrder = true;
	    hOJetPosition[icut][Channel]->Fill(ijet);
	  }

	  effTagCSV[icut][Channel]-> Fill(isGoodCSVOrder, CSVBin);

	} // if(NJets > 3)
	
      }//for(ijet)     
       
      // GenCone Vs Kinematic Reconstruction 
      if(NJets > 3){ 
	// Kinematic Reconstruction
	TLorentzVector KinWl, KinWh, Kintl, Kinth;
	hKinChi2 [icut][Channel]->Fill(Kin_Chi2,PUWeight);
	h2DKinChi2_JetMatch[icut][Channel]->Fill(Kin_Chi2, kinGenConeMatch, PUWeight);
	
	KinWl = Lep + KinNu;
	KinWh = KinJet[1] + KinJet[2];
	
	Kintl = KinWl + KinJet[3];
	Kinth = KinWh + KinJet[0];
	
	hKinWlMass     [icut][Channel]->Fill(KinWl.M());
	hKinWlTransMass[icut][Channel]->Fill(KinWl.Mt());
	hKinWlpT       [icut][Channel]->Fill(KinWl.Pt());
	
	hKinWhMass[icut][Channel]->Fill(KinWh.M());
	hKinWhpT  [icut][Channel]->Fill(KinWh.Pt());

	hKinWMass[icut][Channel]->Fill(KinWl.M()); // Leptonic
	hKinWMass[icut][Channel]->Fill(KinWh.M()); // Hadronic
	hKinWpT  [icut][Channel]->Fill(KinWl.Pt());// Leptonic
	hKinWpT  [icut][Channel]->Fill(KinWh.Pt());// Hadronic
	
	hKintlMass[icut][Channel]->Fill(Kintl.M());
	hKintlpT  [icut][Channel]->Fill(Kintl.Pt());
	
	hKinthMass[icut][Channel]->Fill(Kinth.M());
	hKinthpT  [icut][Channel]->Fill(Kinth.Pt());
	
	bool IsKinGenMatch = false;
	if ( kinGenConeMatch == (TopkinGenConeMatch + WkinGenConeMatch) ) IsKinGenMatch = true;
	effKinGenIndex[icut][Channel]->Fill(IsKinGenMatch, 0.5, PUWeight);
	for(int ikj=0; ikj < 4; ikj++){
	  hKinJetPt[ikj][icut][Channel]->Fill(KinJet[ikj].Pt(), PUWeight);
	}

      } //if(NJet > 3)
    }//for(icuts)     
    
    Jets.clear();

  }//for(events)
  
  // Get elapsed time
  sw.Stop();
  std::cout << "==================================================] 100% " << std::endl;
  std::cout << "--- End of event loop: "; sw.Print();
  std::cout << std::endl;

  //Acceptance-Efficiency
  TH1F *Yields;
  Yields = new TH1F("Yields", "Yields",(Nhcuts)*(Nhch+1),0,(Nhcuts)*(Nhch+1));
  int nbin = 1;
  
  for(int nc = 0; nc < Nhcuts; nc++){
    AccEvent[nc][2] = AccEvent[nc][0] + AccEvent[nc][1];
    EffEvent[nc][2] = EffEvent[nc][0] + EffEvent[nc][1];
    
    for(int nch = 0; nch < Nhch+1; nch++){            
      float EffError;
      if (AccEvent[nc][nch] != 0.0) EffError = sqrt(AccEvent[nc][nch])*EffEvent[nc][nch]/AccEvent[nc][nch];
      else EffError = 0.0;
      
      Yields->GetXaxis()->SetBinLabel(nbin, namecut[nc] + " " + namech[nch]);
      Yields->SetBinContent(nbin, EffEvent[nc][nch]);
      Yields->SetBinError  (nbin, EffError);
      nbin++;
      
      std::cout << "-- Acceptace  " << namecut[nc] << " " << namech[nch] << ": " << AccEvent[nc][nch] << std::endl;
      std::cout << "-- Efficiency " << namecut[nc] << " " << namech[nch] << ": " << EffEvent[nc][nch] << " +/- " << EffError << std::endl;
      std::cout << std::endl;
    }
    std::cout << "-----------------------------" << std::endl;
  }
  
  
  // --- Write histograms
  TString outfname=dirname + "/hSF-" + hname + "_" + fname + ".root";
  TFile *target  = new TFile(outfname,"RECREATE" );  
  
  target->cd();
  
  Yields->Write();

  for(int j=0; j<Nhcuts; j++){
    
    target->mkdir(namecut[j]);

    for(int i=0; i<Nhch; i++){
      
      target->mkdir(namecut[j] + "/" + namech[i]);
      target->cd   (namecut[j] + "/" + namech[i]);
      
      hPV[j][i]     ->Write();
      hMET[j][i]    ->Write();
      hMET_Phi[j][i]->Write();
      hmT[j][i]     ->Write();

      
      hLepPt[j][i]->Write();
      hLepEta[j][i]->Write();
      hLepPhi[j][i]->Write();
      
      hNJets[j][i]->Write();
      hNBtagJets[j][i]->Write();            

      h2DSFbtag_Global[j][i]->Write();
      hSFbtag_Global[j][i]->Write();
      hSFbtag_Global_var[j][i]->Write();

      for(int ij=0; ij<NhJets; ij++){
  	hJetPt[ij][j][i]->Write();
  	hCSV  [ij][j][i]->Write();
  	hCvsL [ij][j][i]->Write();
  	hCvsB [ij][j][i]->Write();
      }

      for(int ja=0; ja<NhJets; ja++){
  	for(int jb=ja+1; jb<NhJets; jb++){
  	  hMassJet[ja][jb][j][i]->Write();
  	  hDRJet  [ja][jb][j][i]->Write();
	  h2DCSV  [ja][jb][j][i]->Write();
  	}
      }   
   
      hInvMassjj[j][i]->Write();
      
      hSFIDISOTr[j][i]->Write();
      hSFIDISOTrError[j][i]->Write();
      hSFIDISO[j][i]->Write();
      hSFIDISOError[j][i]->Write();
      hSFTrigger[j][i]->Write();
      hSFTriggerError[j][i]->Write();
      
      h2DSFbtag_b[j][i]->Write();
      h2DSFbtag_c[j][i]->Write();
      h2DSFbtag_l[j][i]->Write();

      h2DSFbtag_btag_b[j][i]->Write();
      h2DSFbtag_btag_c[j][i]->Write();
      h2DSFbtag_btag_l[j][i]->Write();

      // Kinematic Reconstruction
      hKinChi2       [j][i]->Write();
      h2DKinChi2_JetMatch[j][i]->Write();

      hKinWlMass     [j][i]->Write();
      hKinWlTransMass[j][i]->Write();
      hKinWlpT       [j][i]->Write();
      hKinWhMass     [j][i]->Write();
      hKinWhpT       [j][i]->Write();
      hKinWMass     [j][i]->Write();
      hKinWpT       [j][i]->Write();

      hKinTagWMass  [j][i]->Write();
      hKinTagAddMass[j][i]->Write();
      hKinTagAddDR  [j][i]->Write();

      hKintlMass     [j][i]->Write();
      hKintlpT       [j][i]->Write();

      hKinthMass     [j][i]->Write();
      hKinthpT       [j][i]->Write();

      for(int ikj=0; ikj<4;ikj++) hKinJetPt[ikj][j][i]->Write();

      hTJetPosition [j][i]->Write();
      hWJetPosition [j][i]->Write();
      hOJetPosition [j][i]->Write();

      hGenTagWMass  [j][i]->Write();
      hGenTagAddMass[j][i]->Write();
      hGenTagAddDR  [j][i]->Write();

      // Purities and Efficiencies
      effKinGenIndex [j][i]->Write();
      purKinGenIndex [j][i]->Write();
      
      effTagCSV     [j][i]->Write();
      purTagCSV     [j][i]->Write();
      
    }//for(i)

  }//for(j)
  target->cd();

  h2DttbarNGenJets->Write();
    
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


float DiJetMassCorrection(std::vector<ComJet>  &Jets, bool ReArrange = false){

  /*******************************************
            Dijet Invariant Mass 
  *******************************************/
  int InvMassIndex[2];
  float minDeltaMjj = 9999.;
  float Mjj = 0.0;
  
  for(int ijet=0; ijet < Jets.size(); ijet++){
    
    ComJet jet_i = Jets[ijet];
    
    for(int jjet=ijet+1; jjet < Jets.size(); jjet++){
      ComJet jet_j = Jets[jjet];
      
      float DijetInvMass = (jet_i+jet_j).M(); 
      float DeltaMjj = std::abs(DijetInvMass-80.3);
      
      if(minDeltaMjj > DeltaMjj){
	minDeltaMjj = DeltaMjj;
	Mjj = DijetInvMass;
	InvMassIndex[0] = ijet;
	InvMassIndex[1] = jjet;
      }
      
    }// for(jjets)

  }// for(ijet)

  // Change of Jet Ordering
  if(ReArrange && (InvMassIndex[0] > 1 || InvMassIndex[1] > 1)){    
    ComJet Mjj_a = Jets[InvMassIndex[0]];
    ComJet Mjj_b = Jets[InvMassIndex[1]];
  
    // Delete the Mjj Jets: First the last one!
    Jets.erase(Jets.begin()+InvMassIndex[1]);
    Jets.erase(Jets.begin()+InvMassIndex[0]);

    // Move them to the end
    Jets.push_back(Mjj_a);
    Jets.push_back(Mjj_b);
  } // if(ReArrange)

  return Mjj;
}


bool IsSelectedttbarCategory(std::vector<int> *GenConeCat, TString ttbar_id){
  
  // Categorization using Cone DeltaR
  // Visible Phase Space: 
  int cone_NJets  = (*GenConeCat)[1];
  int cone_NbJets = (*GenConeCat)[2];
  int cone_NcJets = (*GenConeCat)[3];
  
  int cone_NbJetsNoTop = (*GenConeCat)[4];
  
  // Full Phase Space:
  int cone_NaddJets  = (*GenConeCat)[5];
  int cone_NaddbJets = (*GenConeCat)[6];
  int cone_NaddcJets = (*GenConeCat)[7];
  
  bool Isttjj = false;
  bool Isttbb = false;
  bool Isttcc = false;
  bool Isttb  = false;
  bool IsttLF = false;
  bool Istt   = false;
  
  if(cone_NbJets > 1 && cone_NJets > 5) Isttjj = true;
  
  // Categorization based in the Visible Ph-Sp
  if      (cone_NbJets > 3  && cone_NJets > 5) Isttbb = true;
  else if (cone_NbJets > 2  && cone_NJets > 5) Isttb  = true;
  else if (cone_NbJets > 1  && cone_NJets > 5 && cone_NcJets > 1) Isttcc = true;
  else if (cone_NbJets > 1  && cone_NJets > 5) IsttLF = true;
  else Istt = true;
  
  // Categorization based in the Full Ph-Sp
  // if      (cone_NaddJets > 1) Isttjj = true;
  
  // if      (cone_NaddbJets > 1) Isttbb = true;
  // else if (cone_NaddbJets > 0) Isttb  = true;
  // else if (cone_NaddcJets > 1) Isttcc = true;
  // else if (cone_NaddJets  > 0) IsttLF = true;
  // else Istt = true;
  
  bool IsttbarCat = true;
  if(ttbar_id == "ttjj" && !Isttjj) IsttbarCat = false;
  if(ttbar_id == "ttbb" && !Isttbb) IsttbarCat = false;
  if(ttbar_id == "ttb"  && !Isttb ) IsttbarCat = false;
  if(ttbar_id == "ttcc" && !Isttcc) IsttbarCat = false;
  if(ttbar_id == "ttLF" && !IsttLF) IsttbarCat = false;
  if(ttbar_id == "tt"   && !Istt)   IsttbarCat = false;
  
  return IsttbarCat;
}

#endif

