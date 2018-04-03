#ifndef __CINT__

#include<string>
#include<iostream>
#include<sstream>
#include<stdio.h>
#include<stdlib.h>
#include<cmath>
#include<set>
#include<vector>

#endif

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
#include <sys/stat.h>
#include "TError.h"

// TopCode
#include <ttbar_category.h> // Event Categorization
  
#ifndef __CINT__

enum ncat{ttjj,ttbb,ttbj,ttcc,ttLF,tt};
enum nch {mujets,ejets,lepjets};
TString namecat[6] = {"ttjj","ttbb","ttbj","ttcc","ttLF","tt"};
TString namech[3]  = {"mujets","ejets","lepjets"};

void display_usage()
{
  std::cout << "\033[1;37musage:\033[1;m skimfile cutindex [options]" << std::endl;
  std::cout << "" << std::endl;
  std::cout << "Options:" << std::endl;
  std::cout << "    -i   inputfile  Input file without .root" << std::endl;
  std::cout << "    -eos input files located in eos lxplus" << std::endl;
  std::cout << "    -o   name in the output file \"hSF_\"" << std::endl;
  std::cout << "    -cat ttbar categorization" << std::endl;
  std::cout << "    -d   Input file directory. Default directory: InputTrees" << std::endl;
  std::cout << "    -s create a file with the systematic uncertainty yields" << std::endl;
  std::cout << "    -h   displays this help message and exits " << std::endl;
  std::cout << "" << std::endl;
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

void print_progress(int TreeEntries, Long64_t ievt)
{
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

int main(int argc, const char* argv[]){

  gErrorIgnoreLevel = kError;

  gSystem->Load("libTree");

  bool _eos         = false;
  bool   _ttbar_cat = false;
  bool   _syst      = false;
  const char * _output   = 0;
  const char * _input    = 0;
  // TopTrees directory
  const char * _dir      = "/xrootd/store/user/brochero/v8-0-6/";
  const char * _syst_name = 0;
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
	if( strcmp(argv[i],"-cat") == 0 ){
	  _ttbar_cat = true;
	  _ttbar_id  = argv[i+1];
	  i++;
	}
	if( strcmp(argv[i],"-s") == 0 ){
	  _syst= true;
	  _syst_name = argv[i+1];
	  _syst_var  = argv[i+2];
	  i+=2;
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
  TString fdir(_dir);
  TString ttbar_id(_ttbar_id);
  TString syst_name = "";
  TString syst_var = "";
  TString syst_varname = "";
  
  if (_syst){
    syst_name = _syst_name;
    syst_var  = _syst_var;
    syst_varname = "_" + syst_name + syst_var;
  }


  if(_eos) fdir = "root://cms-xrdr.sdfarm.kr:1094///xrd/store/user/brochero/v8-0-6/";

  TChain theTree("ttbbLepJets/gentree"); 
  
  std::cout << "---------------------------------------------------------------------------------" << std::endl;
  std::cout << "Signal: ";
  std::cout << fname + ".root" << std::endl;

  theTree.Add(fdir + fname + ".root");

  int Channel;
  float GENWeight; 
  std::vector<float> *ScaleWeight=0, *PDFWeight=0, *hdampWeight=0;
  float Lep_pT, Lep_eta;
  std::vector<float> *Jet_pT=0, *Jet_eta=0, *Jet_phi=0, *Jet_E=0;

  std::vector<int>   *Jet_partonFlavour=0;

  // Categorization
  int  GenCat_ID;
  std::vector<int> *GenConeCat=0; 

 /*********************************
           Tree Branches
  **********************************/
  
  theTree.SetBranchAddress( "genweight",     &GENWeight  );
  theTree.SetBranchAddress( "scaleweight",   &ScaleWeight);
  theTree.SetBranchAddress( "hdampweight",   &hdampWeight);
  theTree.SetBranchAddress( "pdfweight",     &PDFWeight);

  theTree.SetBranchAddress( "genchannel",    &Channel    );
  theTree.SetBranchAddress( "genhiggscatid", &GenCat_ID  );
  theTree.SetBranchAddress( "genconecatid",  &GenConeCat );


  theTree.SetBranchAddress( "genlepton_pT",  &Lep_pT );
  theTree.SetBranchAddress( "genlepton_eta", &Lep_eta);
  theTree.SetBranchAddress( "genjet_pT",     &Jet_pT );
  theTree.SetBranchAddress( "genjet_eta",    &Jet_eta );
  theTree.SetBranchAddress( "genjet_phi",    &Jet_phi );
  theTree.SetBranchAddress( "genjet_E",      &Jet_E  );

  /*********************************
             Histograms
  **********************************/

  // Correct Statistical Uncertainty Treatment
  // TH1::SetDefaultSumw2(kTRUE);  
  
  TH1F *hNJets[2];
  TH1D *ScaleWeigths_Full[6][2], *ScaleWeigths_Vis[6][2];
  TH1D *PDFWeigths_Full[6][2], *PDFWeigths_Vis[6][2];
  TH1D *hdampWeigths_Full[6][2], *hdampWeigths_Vis[6][2];
  TH1D *ScaleWeigths_FullNotVis[6][2];

  TString titlenamech[2];
  titlenamech[0]="#mu+Jets";
  titlenamech[1]="e+Jets";
  
  for(int i=0; i<2; i++){ // Channel

    hNJets[i]      = new TH1F("hNJets_" + namech[i], "Jet multiplicity " + titlenamech[i],9,-0.5,8.5);
    hNJets[i]->GetXaxis()->SetTitle("Number of jets");      
    hNJets[i]->GetXaxis()->SetBinLabel(1,"0");
    hNJets[i]->GetXaxis()->SetBinLabel(2,"1");
    hNJets[i]->GetXaxis()->SetBinLabel(3,"2");
    hNJets[i]->GetXaxis()->SetBinLabel(4,"3");
    hNJets[i]->GetXaxis()->SetBinLabel(5,"4");
    hNJets[i]->GetXaxis()->SetBinLabel(6,"5");
    hNJets[i]->GetXaxis()->SetBinLabel(7,"6");
    hNJets[i]->GetXaxis()->SetBinLabel(8,"7");
    hNJets[i]->GetXaxis()->SetBinLabel(9,"#geq 8");        



    for (int icat=ttjj; icat<=tt; icat++){
      PDFWeigths_Full[icat][i] = new TH1D("PDFWeigths_Full_" + namecat[icat] + "_" + namech[i], "PDF weights for Evt in the Full Ph-Sp " + titlenamech[i],200,0.,2.); 
      PDFWeigths_Vis[icat][i]  = new TH1D("PDFWeigths_Vis_"  + namecat[icat] + "_" + namech[i], "PDF weights for Evt in the Vis Ph-Sp "  + titlenamech[i],200,0.,2.); 

      hdampWeigths_Full[icat][i] = new TH1D("hdampWeigths_Full_" + namecat[icat] + "_" + namech[i], "hdamp weights for Evt in the Full Ph-Sp " + titlenamech[i],200,0.,2.); 
      hdampWeigths_Vis[icat][i]  = new TH1D("hdampWeigths_Vis_"  + namecat[icat] + "_" + namech[i], "hdamp weights for Evt in the Vis Ph-Sp "  + titlenamech[i],200,0.,2.); 

      ScaleWeigths_Full[icat][i] = new TH1D("ScaleWeigths_Full_" + namecat[icat] + "_" + namech[i], "Scale weights for Evt in the Full Ph-Sp " + titlenamech[i],200,0.,2.); 
      ScaleWeigths_Vis[icat][i]  = new TH1D("ScaleWeigths_Vis_"  + namecat[icat] + "_" + namech[i], "Scale weights for Evt in the Vis Ph-Sp "  + titlenamech[i],200,0.,2.); 
      ScaleWeigths_FullNotVis[icat][i]  = new TH1D("ScaleWeigths_FullNotVis_"  + namecat[icat] + "_" + namech[i], "Scale weights for Evt in the Full but Not Vis Ph-Sp "  + titlenamech[i],200,0.,2.); 
    }
  }//for(i)
  
  
  TStopwatch sw;
  sw.Start(kTRUE);

  ///////////////////////////////////////
  // Please, IGNORE. Temporal solution //
  ///////////////////////////////////////
  TCanvas *mydummycanvas=new TCanvas();// 
  ///////////////////////////////////////

  // Number de events for acceptance
  //          [ttcat][Channel]
  double NEvt_full[6][3];
  double NEvt_vis [6][3];

  for(unsigned int ibinx=0; ibinx<3; ibinx++){
    for(unsigned int ibiny=0; ibiny<6; ibiny++){
      NEvt_full[ibiny][ibinx] = 0.0;
      NEvt_vis [ibiny][ibinx] = 0.0;
    }
  }

  // Uncertainties file name
  if(_syst && 
     !syst_varname.Contains("ISR") &&
     !syst_varname.Contains("FSR") &&
     !syst_varname.Contains("UE") ) fname += "_SYS" + syst_varname;

    /******************
      Scale Weights
    ******************/

    int scaleSysPar;
    if     (_syst && syst_varname.Contains("ScaleRnFUp"))   scaleSysPar = 0; // muR=Nom,  muF=Up
    else if(_syst && syst_varname.Contains("ScaleRnFDown")) scaleSysPar = 1; // muR=Nom,  muF=Down
    else if(_syst && syst_varname.Contains("ScaleRuFNom"))  scaleSysPar = 2; // muR=Up,   muF=Nom
    else if(_syst && syst_varname.Contains("ScaleRuFUp"))   scaleSysPar = 3; // muR=Up,   muF=Up
    // Name modified to fix with Combine Structure
    else if(_syst && syst_varname.Contains("ScaleRdFUp"))   scaleSysPar = 4; // muR=Down, muF=Nom
    else if(_syst && syst_varname.Contains("ScaleRdFDown")) scaleSysPar = 5; // muR=Down, muF=Down

    int hdampSysPar;
    if      (_syst && syst_varname.Contains("hdampUp"))   hdampSysPar = 18; 
    else if (_syst && syst_varname.Contains("hdampDown")) hdampSysPar = 9; 

    int pdfSysPar;
    if (_syst && syst_varname.Contains("PDF"))   pdfSysPar = syst_var.Atoi(); 
    

  /********************************
             Event Loop
  ********************************/
  std::cout << "--- Processing: " << theTree.GetEntries() << " events" << std::endl;
  
  for (Long64_t ievt=0; ievt<theTree.GetEntries();ievt++) {
  // for (Long64_t ievt=20000000; ievt<theTree.GetEntries();ievt++) {
  // for (Long64_t ievt=0; ievt<200000;ievt++) {
    
    theTree.GetEntry(ievt);  
    print_progress(theTree.GetEntries(), ievt);    
    
    /******************
        GEN Weights
    ******************/

    float EvtStep = GENWeight;

    /******************
      Scale Weights
    ******************/
    
    if (_syst && syst_varname.Contains("ScaleR"))
      EvtStep = EvtStep*(*ScaleWeight)[scaleSysPar];

    /******************
       hdamp Weights
    ******************/

    if (_syst && syst_varname.Contains("hdamp"))
      EvtStep = EvtStep*(*hdampWeight)[hdampSysPar];


    if (_syst && syst_varname.Contains("PDF"))
      EvtStep = EvtStep*(*PDFWeight)[pdfSysPar];

    /***************************
       Categorization GenTop
    ***************************/

    int cone_channel = (*GenConeCat)[0];

    // Visible Phase Space:
    // pT(jet) > 20GeV && |eta(Jet)| < 2.5
    int cone_NJets  = (*GenConeCat)[1];
    int cone_NbJets = (*GenConeCat)[2];
    int cone_NcJets = (*GenConeCat)[3];
    
    int cone_NbJetsNoTop = (*GenConeCat)[4];
    
    // Full Phase Space:
    // pT(jet) > 20GeV && |eta(Jet)| < 2.5
    int cone_NaddJets  = (*GenConeCat)[5];
    int cone_NaddbJets = (*GenConeCat)[6];
    int cone_NaddcJets = (*GenConeCat)[7];
    
    /******************
        Acceptace
    ******************/
    
    // Full Phase Space Categorization
    bool IsCat[6]={false,false,false,false,false,false};

    if(cone_NaddJets  > 1)  IsCat[ttjj] = true;
    else                    IsCat[tt]   = true;

    if     (IsCat[ttjj] && cone_NaddbJets > 1) IsCat[ttbb] = true;
    else if(IsCat[ttjj] && cone_NaddbJets > 0) IsCat[ttbj] = true;
    else if(IsCat[ttjj] && cone_NaddcJets > 1) IsCat[ttcc] = true;
    else if(IsCat[ttjj])                       IsCat[ttLF] = true;

    for (int icat=ttjj; icat<=tt; icat++){
      if(IsCat[icat]) NEvt_full[icat][Channel] += EvtStep;
    }


    float Lep_pT_CUT = 30; // Muon
    if (Channel==1) Lep_pT_CUT = 35; // Electron

    // Visible Phase Space 
    bool IsVis[6]={false,false,false,false,false,false};
    if(Lep_pT > Lep_pT_CUT && abs(Lep_eta) < 2.1){
      
      if      (IsCat[ttjj] && cone_NbJets > 1 && cone_NJets > 5) IsVis[ttjj]=true;
      else if (IsCat[tt]   && cone_NbJets > 1 && cone_NJets > 5) IsVis[tt]  =true;

      if     (IsCat[ttbb] && cone_NbJets > 3 && cone_NJets > 5)                    IsVis[ttbb]=true;
      else if(IsCat[ttbj] && cone_NbJets > 2 && cone_NJets > 5)                    IsVis[ttbj]=true;
      else if(IsCat[ttcc] && cone_NbJets > 1 && cone_NcJets > 1 && cone_NJets > 5) IsVis[ttcc]=true;
      else if(IsCat[ttLF] && cone_NbJets > 1 && cone_NJets > 5)                    IsVis[ttLF]=true;

      for (int icat=ttjj; icat<=tt; icat++){
	if(IsVis[icat]) NEvt_vis[icat][Channel] += EvtStep;
      }
      
    }

    // Weigths
    for (int icat=ttjj; icat<=tt; icat++){
      if(IsCat[icat]){
	for(unsigned int ipdf=0; ipdf<=99; ipdf++) PDFWeigths_Full[icat][Channel]->Fill((*PDFWeight)[ipdf]);

	hdampWeigths_Full[icat][Channel]->Fill(EvtStep);
	ScaleWeigths_Full[icat][Channel]->Fill(EvtStep); 
      }      
      if(IsVis[icat]){
	for(unsigned int ipdf=0; ipdf<=99; ipdf++) PDFWeigths_Vis[icat][Channel]->Fill((*PDFWeight)[ipdf]);

	hdampWeigths_Vis[icat][Channel]->Fill(EvtStep);
	ScaleWeigths_Vis[icat][Channel]->Fill(EvtStep); 
      }
      if(IsCat[icat] && !IsVis[icat]) ScaleWeigths_FullNotVis[icat][Channel]->Fill(EvtStep); 
    }

    // Jets 
    int NJets = 0;    
    
    for(int ijet=0; ijet < Jet_pT->size(); ijet++){
      
      TLorentzVector gjet;
      gjet.SetPtEtaPhiE((*Jet_pT)[ijet], (*Jet_eta)[ijet], (*Jet_phi)[ijet], (*Jet_E)[ijet]);
      
      if(gjet.Pt()>30 && std::abs(gjet.Eta())<2.5){
	
	NJets++;
	
      } // if(jet.pT > 30)
    }// for(jets)
    
    hNJets[Channel]->Fill(NJets,EvtStep);
    
  }//for(events)

  // Add mu+Jets and e+Jets
  for (int icat=ttjj; icat<=tt; icat++){
    NEvt_full[icat][2] = NEvt_full[icat][0] + NEvt_full[icat][1];
    NEvt_vis [icat][2] = NEvt_vis [icat][0] + NEvt_vis [icat][1];  
  }
    

  //Acceptance-Efficiency
  TH2D *Yields_full[6], *Yields_vis[6];

  for (int icat=ttjj; icat<=tt; icat++){
    Yields_full[icat] = new TH2D("Yields_FullPh-Sp_" + namecat[icat], "Yields in the Full Ph-Sp "    + namecat[icat],3,0,3,1,0,1);
    Yields_vis [icat] = new TH2D("Yields_VisPh-Sp_"  + namecat[icat], "Yields in the Visible Ph-Sp " + namecat[icat],3,0,3,1,0,1);
    Yields_full[icat]->SetOption("COLTEXT");
    Yields_vis [icat]->SetOption("COLTEXT");
    
    for(unsigned int ibinx=0; ibinx<3; ibinx++){
      Yields_full[icat]->GetXaxis()->SetBinLabel(ibinx+1, namech[ibinx]);
      Yields_vis [icat]->GetXaxis()->SetBinLabel(ibinx+1, namech[ibinx]);
    }

    Yields_full[icat]->GetYaxis()->SetBinLabel(1, namecat[icat]);
    Yields_vis [icat]->GetYaxis()->SetBinLabel(1, namecat[icat]);
    
    for(unsigned int ibinx=0; ibinx<3; ibinx++){
      Yields_full[icat]->SetBinContent(ibinx+1,1,NEvt_full[icat][ibinx]);
      Yields_vis [icat]->SetBinContent(ibinx+1,1,NEvt_vis [icat][ibinx]);  
    }
  
  } // for(icat)

  // Get elapsed time
  sw.Stop();
  std::cout << "==================================================] 100% " << std::endl;
  std::cout << "--- End of event loop: "; sw.Print();
  
  
  //Acceptance-Efficiency

  for(unsigned int ich=0; ich<3; ich++){
    std::cout << "--------  Acceptace Full Ph-Sp  --------" << std::endl;
    std::cout << "--------     " << namech[ich] << "     --------" << std::endl;
    std::cout << "Number of RAW events:" << std::endl;
    std::cout << "ttbb/ttjj Full Ph-Sp: " << NEvt_full[ttbb][ich]/NEvt_full[ttjj][ich] << std::endl;
    std::cout << "ttbb/ttjj Visible Ph-Sp: " << NEvt_vis[ttbb][ich]/NEvt_vis[ttjj][ich] << std::endl;
    std::cout << "ttbb Acceptance: " << NEvt_vis[ttbb][ich]/NEvt_full[ttbb][ich] << std::endl;
    std::cout << "ttbb Full: " << NEvt_full[ttbb][ich] << std::endl;
    std::cout << "ttbb Vis:  " << NEvt_vis[ttbb][ich]  << std::endl;
    std::cout << "ttbj Acceptance: " << NEvt_vis[ttbj][ich]/NEvt_full[ttbj][ich] << std::endl;
    std::cout << "ttcc Acceptance: " << NEvt_vis[ttcc][ich]/NEvt_full[ttcc][ich] << std::endl;
    std::cout << "ttLF Full: " << NEvt_full[ttLF][ich] << std::endl;
    std::cout << "ttLF Vis:  " << NEvt_vis[ttLF][ich]  << std::endl;
    std::cout << "ttLF Acceptance: " << NEvt_vis[ttLF][ich]/NEvt_full[ttLF][ich] << std::endl;
    std::cout << "ttjj Full: " << NEvt_full[ttjj][ich] << std::endl;
    std::cout << "ttjj Vis:  " << NEvt_vis[ttjj][ich]  << std::endl;
    std::cout << "ttjj Acceptance: " << NEvt_vis[ttjj][ich]/NEvt_full[ttjj][ich] << std::endl;
    std::cout << "tt Full: "   << NEvt_full[tt][ich] << std::endl;
    std::cout << "tt Vis:  "   << NEvt_vis[tt][ich] << std::endl;
    std::cout << "tt Acceptance: "   << NEvt_vis[tt][ich]/NEvt_full[tt][ich] << std::endl;
    std::cout << "-----------------------------" << std::endl;
    std::cout << "-----------------------------" << std::endl;
  }
  //Output Dir
  TString dirname = "TopResults/" + hname;   
  // make a dir if it does not exist!!
  struct stat st;
  if(stat(dirname,&st) != 0) system("mkdir " + dirname);
  
    
  // --- Write histograms
  TString outfname=dirname + "/hAcc-" + hname + "_" + fname  + ".root";
  TFile *target  = new TFile(outfname,"RECREATE" );  
  
  for (int icat=ttjj; icat<=tt; icat++){
    Yields_vis [icat]->Write();
    Yields_full[icat]->Write();
  }
  for(int i=0; i<2; i++){    

    for (int icat=ttjj; icat<=tt; icat++){
      PDFWeigths_Full[icat][i]->Write();
      PDFWeigths_Vis[icat][i]->Write();

      hdampWeigths_Full[icat][i]->Write();
      hdampWeigths_Vis[icat][i]->Write();

      ScaleWeigths_Full[icat][i]->Write();
      ScaleWeigths_Vis[icat][i]->Write();
      ScaleWeigths_FullNotVis[icat][i]->Write();
    }

    hNJets[i]->Write();

  }//for(i)
  
  target->Close();

  std::cout << "File saved as " << outfname << std::endl;
  
}


#endif

