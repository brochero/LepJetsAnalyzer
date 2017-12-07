#include "TFile.h"
#include "TH1.h"

TString channel[2] = {"mujets","ejets"};
TString SysVar[2]  = {"Up","Down"};

void MergeSystematics(TString Sample, std::vector<TString> SysName);
void MergeSample(std::vector<TString> GenSam, TString SystVarNam);

void PrepareCombineFiles(TString FileVersion = "Full-v0", TString FileProd = "Tree_LepJets_EGTightSkim_v8-0-6_Spring16-80X_36814pb-1"){
  
  TString basename = "TopResults/" + FileVersion + "/hSF-" + FileVersion + "_" + FileProd + "_";
  
  std::vector<TString> SysName;
  SysName.push_back("LES");SysName.push_back("IDLepSF");SysName.push_back("TrLepSF");
  SysName.push_back("PileUp");
  SysName.push_back("JER");
  SysName.push_back("btagjes");SysName.push_back("btaglf");SysName.push_back("btaghf");SysName.push_back("btaghfsI");SysName.push_back("btaghfsII");SysName.push_back("btaglfsI");SysName.push_back("btaglfsII");SysName.push_back("btagcfI");SysName.push_back("btagcfII");
  SysName.push_back("JESAbsoluteStat");SysName.push_back("JESAbsoluteScale");SysName.push_back("JESAbsoluteMPFBias");SysName.push_back("JESFragmentation");SysName.push_back("JESSinglePionECAL"); SysName.push_back("JESSinglePionHCAL");SysName.push_back("JESFlavorQCD");SysName.push_back("JESTimePtEta");SysName.push_back("JESRelativeJEREC1"); SysName.push_back("JESRelativeJEREC2"); SysName.push_back("JESRelativeJERHF");SysName.push_back("JESRelativePtBB"); SysName.push_back("JESRelativePtEC1"); SysName.push_back("JESRelativePtEC2");SysName.push_back("JESRelativePtHF");SysName.push_back("JESRelativeBal");SysName.push_back("JESRelativeFSR");SysName.push_back("JESRelativeStatFSR"); SysName.push_back("JESRelativeStatEC"); SysName.push_back("JESRelativeStatHF");SysName.push_back("JESPileUpDataMC");SysName.push_back("JESPileUpPtRef");SysName.push_back("JESPileUpPtBB");SysName.push_back("JESPileUpPtEC1");SysName.push_back("JESPileUpPtHF");
    
  
  // With Syst Var
  TString ttbarbase  = "ttbar_LepJetsPowhegPythia";
  TString ttbar[6]   = {"",
			"ttbb", "ttbj","ttcc", "ttLF", "tt"};
  TString bkg[3]     = {"ttbar_PowhegPythiaBkgtt",
			"ttbar_PowhegPythiaBkg",
			ttbarbase+"tt"};
  TString singlet[5] = {"SingleTop", 
			"tbar_tchannel_Powheg","t_tchannel_Powheg",
			"tbarW_Powheg","tW_Powheg"};
  TString diboson[4] = {"VV",
			"WW_Pythia", "WZ_Pythia", "ZZ_Pythia"};
  
  std::vector<TString> TTbarNam, BkgNam, STNam, VVNam;
  for (int is = 0 ; is < 6 ; is++) TTbarNam.push_back(basename + ttbarbase + ttbar[is]);
  for (int is = 0 ; is < 3 ; is++) BkgNam.push_back  (basename + bkg[is]);
  for (int is = 0 ; is < 5 ; is++) STNam.push_back   (basename + singlet[is]);
  for (int is = 0 ; is < 4 ; is++) VVNam.push_back   (basename + diboson[is]);
  
  // central
  MergeSample(VVNam,   "central");
  MergeSample(STNam,   "central");
  MergeSample(BkgNam,  "central");
  MergeSample(TTbarNam,"central");
  // Systematics
  for(int isys = 0 ; isys<SysName.size(); isys++){
    for(int ivar = 0; ivar<2; ivar++){
      MergeSample(VVNam,   SysName.at(isys)+SysVar[ivar]);
      MergeSample(STNam,   SysName.at(isys)+SysVar[ivar]);
      MergeSample(BkgNam,  SysName.at(isys)+SysVar[ivar]);
      MergeSample(TTbarNam,SysName.at(isys)+SysVar[ivar]);
    }
  }
  

  MergeSystematics(STNam[0],SysName);
  MergeSystematics(VVNam[0],SysName);

  // For ttbar add Theoretical Uncertainties
  std::vector<TString> SysTheName(SysName);
  SysTheName.push_back("FSR");
  SysTheName.push_back("ISR");
  SysTheName.push_back("UE");
  SysTheName.push_back("ScaleRdF");
  MergeSystematics(TTbarNam[1],SysTheName);
  MergeSystematics(TTbarNam[2],SysTheName);
  MergeSystematics(TTbarNam[3],SysTheName);
  MergeSystematics(TTbarNam[4],SysTheName);
  MergeSystematics(TTbarNam[5],SysTheName);

  MergeSystematics(BkgNam[0],SysTheName);

}


void MergeSystematics(TString Sample, std::vector<TString> SysName){


  TFile *target = new TFile(Sample+"_COMBINE.root","RECREATE" );

  // Central
  TFile *FilesSam = TFile::Open(Sample+".root");
  cout << "Open file " << Sample+".root" << endl;

  for(unsigned int ch=0; ch<2; ch++){
    TH1D *histofile     = (TH1D*)(FilesSam->Get("central/2btag/" + channel[ch] + "/hKinAddCSVUnroll_" + channel[ch] + "_2btag"))->Clone();
    TH2D *histoyields   = (TH2D*)(FilesSam->Get("central/Yields"))->Clone();
    TH2D *histoyieldsNW = (TH2D*)(FilesSam->Get("central/YieldsNoWeights"))->Clone();

    target->cd();
    target->mkdir("central/2btag/" + channel[ch]);
    target->cd   ("central/2btag/" + channel[ch]);

    histofile->Write();

    target->cd("central");

    histoyields  ->Write();
    histoyieldsNW->Write();
  }


  // Systematics  
  for(unsigned int isys=0; isys<SysName.size(); isys++){
    for(int ivar = 0; ivar<2; ivar++){
      
      TString SystVarNam = SysName.at(isys)+SysVar[ivar];
      TString SystVarNamFile = SysName.at(isys)+SysVar[ivar];
      
      if(Sample.Contains("Bkgtt")   &&
	 (SysName.at(isys) == "ISR" ||
	  SysName.at(isys) == "FSR" ||
	  SysName.at(isys) == "UE"  ||
	  SysName.at(isys) == "ScaleRdF")) SystVarNamFile = "AllTheory";  
      else if (SysName.at(isys) == "ISR" ||
	       SysName.at(isys) == "FSR" ||
	       SysName.at(isys) == "UE") SystVarNamFile = "Theory";  
      
      TFile *FilesSam = TFile::Open(Sample+"_SYS_"+SystVarNamFile+".root");
      cout << "Open file " << Sample+"_SYS_"+SystVarNamFile+".root" << endl;
      
      for(unsigned int ch=0; ch<2; ch++){
	TH1D *histofile     = (TH1D*)(FilesSam->Get(SystVarNam + "/2btag/" + channel[ch] + "/hKinAddCSVUnroll_" + channel[ch] + "_2btag_" + SystVarNam))->Clone();
	TH2D *histoyields;
	TH2D *histoyieldsNW;
	if(!SystVarNamFile.Contains("Theory") ){
	  histoyields   = (TH2D*)(FilesSam->Get(SystVarNam + "/Yields_" + SystVarNam))->Clone();
	  histoyieldsNW = (TH2D*)(FilesSam->Get(SystVarNam + "/YieldsNoWeights_" + SystVarNam))->Clone();
	}

	target->cd();
	target->mkdir(SystVarNam + "/2btag/" + channel[ch]);
	target->cd   (SystVarNam + "/2btag/" + channel[ch]);

	histofile->Write();

	target->cd(SystVarNam);
	
	if(!SystVarNamFile.Contains("Theory") ){
	  histoyields  ->Write();
	  histoyieldsNW->Write();
	}

      }  

      FilesSam->Clear();
      delete FilesSam;

    }
  }
  
  target->Close();
}

void MergeSample(std::vector<TString> GenSam, TString SystVarNam){
  
  TH1D *histofile    [2];
  TH2D *histoyields  [2];
  TH2D *histoyieldsNW[2];

  TString FileTail;
  if (SystVarNam == "central") FileTail = ".root";
  else                         FileTail = "_SYS_" + SystVarNam + ".root";
  
  for (int igs=1; igs<GenSam.size(); igs++){
        
    TFile *FilesSam = TFile::Open(GenSam.at(igs)+FileTail);
    cout << "Opening file " << GenSam.at(igs)+FileTail << endl;
    for(unsigned int ch=0; ch<2; ch++){
      
      TString HistoTail;
      if (SystVarNam == "central") HistoTail = "";
      else HistoTail = "_"+SystVarNam;
      
      if(igs == 1){
	histofile[ch]     = (TH1D*)(FilesSam->Get(SystVarNam + "/2btag/" + channel[ch] + "/hKinAddCSVUnroll_" + channel[ch] + "_2btag" + HistoTail))->Clone();
	histoyields[ch]   = (TH2D*)(FilesSam->Get(SystVarNam + "/Yields" + HistoTail))->Clone();
	histoyieldsNW[ch] = (TH2D*)(FilesSam->Get(SystVarNam + "/YieldsNoWeights" + HistoTail))->Clone();
      }
      else{
	histofile[ch]    ->Add( (TH1D*)FilesSam->Get(SystVarNam + "/2btag/" + channel[ch] + "/hKinAddCSVUnroll_" + channel[ch] + "_2btag" + HistoTail) );
	histoyields[ch]  ->Add( (TH2D*)FilesSam->Get(SystVarNam + "/Yields" + HistoTail) );
	histoyieldsNW[ch]->Add( (TH2D*)FilesSam->Get(SystVarNam + "/YieldsNoWeights" + HistoTail) );
      }
      
    }//for(ch)
    
    FilesSam->Clear();
    delete FilesSam;
  }//for(igs)
  
  
  TFile *target = new TFile(GenSam.at(0)+FileTail,"RECREATE" );
  
  for(unsigned int ch=0; ch<2; ch++){
    target->cd();
    target->mkdir(SystVarNam + "/2btag/" + channel[ch]);
    target->cd   (SystVarNam + "/2btag/" + channel[ch]);
    
    histofile[ch]->Write();

    target->cd();
    target->cd   (SystVarNam);
    
    histoyields[ch]  ->Write();
    histoyieldsNW[ch]->Write();
  }
  
  target->Close();
  
}
