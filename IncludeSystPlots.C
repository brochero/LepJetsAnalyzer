#include "Plots.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "THStack.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TStyle.h"
#include "TSystem.h"
#include "TString.h"
#include "TDirectory.h"
#include "TROOT.h"
#include "TObject.h"
#include "TStopwatch.h"
#include "TLegend.h"
#include <vector>
#include <string>
#include <fstream>
#include <vector>
#include <sys/stat.h>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <set>

void IncludeSystPlots(TString ttbarCat = "ttbb", TString plots="2btag", TString nSample = "ttbar"){

  /****************
        Syst
  ****************/
  
  enum FileSystematic:int{PileUp,
      JER, JES,
      LES, LepSF,
      btagjes, btaglf, btaghf, btaghfsI, btaghfsII, btaglfsI, btaglfsII, btagcfI, btagcfII};
  
  TString SystNam[14]= {"PileUp",
			"JER", "JES",
			"LES", "LepSF",
			"btagjes", "btaglf", "btaghf", "btaghfsI", "btaghfsII", "btaglfsI", "btaglfsII", "btagcfI", "btagcfII"};
  
  enum FileVariation:int{Nom, Down, Up};
  TString VarNam[3]= {"Nom", "Down", "Up"};
  
  
  /****************
       Channel
  ****************/
  TString channel[3];
  channel[0] = "mujets";
  channel[1] = "ejets";  
  channel[2] = "lepjet"; 

  TString files   = dirnameIn + fl;
  TString nttbarS = "ttbar_LepJetsPowhegPythiaTranche3";
  
  /****************
    ttbar Signal
  ****************/ 
  // ttbar categorization 
  std::vector<histos> Sample;

  TString SampleFullName;
  if (nSample == "ttbar") SampleFullName = nttbarS + ttbarCat + "FullPhSp";
  else SampleFullName = nSample;

  Sample = loadhistograms(plots, files + "_" + SampleFullName);

  // PileUp
  std::vector<histos> SampleSys[14][3]; //[syst][var]

  for(int isys = 0; isys<14; isys++){

    if (isys == JER)  SampleSys[JER][Nom] = loadhistograms(plots, files + "_" + SampleFullName + "_SYS_" + SystNam[isys] + "_" + VarNam[Nom]);      
    
    for(int ivar = 1; ivar<3; ivar++)
      SampleSys[isys][ivar] = loadhistograms(plots, files + "_" + SampleFullName + "_SYS_" + SystNam[isys] + "_" + VarNam[ivar]);      

  }// for(isys)

  
  for(unsigned int h = 0; h < Sample.size(); h++){
    for(unsigned int ch=0; ch<2; ch++){// Only mu+Jets and e+Jets  
      for(int ibin = 1; ibin <= Sample[h].hist[ch]->GetNbinsX()+1; ibin++){      
	float central = Sample[h].hist[ch]->GetBinContent(ibin);

	float SysVar[14][3];
	for(int isys = 0; isys<14; isys++){
	  if (isys == JER) SysVar[JER][Nom] = SampleSys[JER][Nom][h].hist[ch]->GetBinContent(ibin);
	  for(int ivar = 1; ivar<3; ivar++)
	    SysVar[isys][ivar] = SampleSys[isys][ivar][h].hist[ch]->GetBinContent(ibin);	 
	}


	float btagTot = 0;
	for(int ibtag = btaglf; ibtag<btagcfII; ibtag++){
	  float btagF;
	  if (central != 0.0)
	    btagF = max(fabs(central - SysVar[ibtag][Up])/central, fabs(central - SysVar[ibtag][Down])/central);
	  else btagF = 0.0;
	  btagTot += btagF*btagF;
	}
	btagTot = sqrt(btagTot);

	
	float PileUpTot = 0.0;
	if(central != 0.0) PileUpTot = max(abs(central - SysVar[PileUp][Up])/central,  fabs(central - SysVar[PileUp][Down])/central);
	float JESTot = 0.0;
	if(central != 0.0) JESTot = max(fabs(central - SysVar[JES][Up])/central,  fabs(central - SysVar[JES][Down])/central);
	float JERTot = 0.0;
	if(SysVar[JER][Nom] != 0.0) JERTot = max(fabs(SysVar[JER][Nom] - SysVar[JER][Up])/SysVar[JER][Nom],  fabs(SysVar[JER][Nom] - SysVar[JER][Down])/SysVar[JER][Nom]);
	
	
	float Stat  = Sample[h].hist[ch]->GetBinError(ibin);
	float Syst  = sqrt(PileUpTot*PileUpTot + JESTot*JESTot + JERTot*JERTot + btagTot*btagTot)*Sample[h].hist[ch]->GetBinContent(ibin);
	float Total = sqrt(Syst*Syst + Stat*Stat); 
	
	if (Sample[h].hist[ch]->GetBinContent(ibin) == 0.0) Sample[h].hist[ch]->SetBinError(ibin, 0.0);
	else Sample[h].hist[ch]->SetBinError(ibin, Total);

	cout << "Content = " << Sample[h].hist[ch]->GetBinContent(ibin) << " Stat = " << Stat << " Syst = " << Syst << " Total Error = " << Total << endl;	
      }// for(bins)
    } // for(channel)    
  }  // for(histograms)  
  
  overwritehistograms(Sample, plots, files + "_" + SampleFullName);

} //end Plots.C

std::vector<histos> loadhistograms(TString plots, TString namefile){

  TFile *file=NULL; // new TFile(namefile);
  file = TFile::Open(namefile + ".root");
  
  if(!file){
    std::cerr << "ERROR: Could not open " <<  namefile  << " files!!!"  << std::endl;
    std::exit(0);
  }
  
  std::vector<histos> sample;
  histos histofile;
  
  TString channel[3];
  channel[0]="mujets";
  channel[1]="ejets";
  channel[2]="lepjets";
  
  std::vector<TString> histoname;
  
  // Histograms
  histoname.push_back("hPV");
  histoname.push_back("hMET");
  histoname.push_back("hMET_Phi");
  histoname.push_back("hLepPt");
  histoname.push_back("hLepEta");
  histoname.push_back("hLepPhi");
  histoname.push_back("hJetPt_Jet-0");
  histoname.push_back("hJetPt_Jet-1");
  histoname.push_back("hJetPt_Jet-2");
  histoname.push_back("hJetPt_Jet-3");
  histoname.push_back("hJetPt_Jet-4");
  histoname.push_back("hJetPt_Jet-5");
  histoname.push_back("hNJets");
  histoname.push_back("hNBtagJets");
  histoname.push_back("hCSV_Jet-0");
  histoname.push_back("hCSV_Jet-1");
  histoname.push_back("hCSV_Jet-2");
  histoname.push_back("hCSV_Jet-3");
  histoname.push_back("hCSV_Jet-4");
  histoname.push_back("hCSV_Jet-5");
  histoname.push_back("hKinAdd1CSV");
  histoname.push_back("hKinAdd2CSV");
  histoname.push_back("hInvMassjj");
  histoname.push_back("hmT");

  for(unsigned int h=0; h<histoname.size(); h++){
    for(unsigned int ch=0; ch<2; ch++) histofile.hist[ch] = (TH1D*)file->Get(plots + "/" + channel[ch] + "/" +  histoname[h] + "_" + channel[ch] + "_" + plots);    

    sample.push_back(histofile);
    
  }  

  cout << "All the histograms from " << namefile << ".root have been loaded successfully!!!"  << endl; 
  return sample;
}

void overwritehistograms(std::vector<histos> newhistos, TString plots, TString namefile){

  TFile *file=NULL; // new TFile(namefile);
  file = TFile::Open(namefile + plots + "SystError.root", "RECREATE");
  
  if(!file){
    std::cerr << "ERROR: Could not open " <<  namefile  << " files!!!"  << std::endl;
    std::exit(0);
  }
  TString namech     [3] = {"mujets","ejets","lepjets"};
  TString namecut    [7]   = {"lepton","6Jets","2btag","3btag","4Jets","4Jets2btag","Only2btag"};

  for (int ic=0;ic<7;ic++) {
    file->mkdir(namecut[ic]);
    file->mkdir(namecut[ic] + "/mujets");
    file->mkdir(namecut[ic] + "/ejets");
  }

  for(unsigned int h = 0; h < newhistos.size(); h++){
    for(unsigned int ch=0; ch<2; ch++){// Only mu+Jets and e+Jets  
      file->cd   (plots + "/" + namech[ch]);
      //newhistos[h].hist[ch]->Write("",TObject::kOverwrite);
      newhistos[h].hist[ch]->Write();
    }
  }
  
  cout << "All the histograms from " << namefile << ".root have been overwrote successfully!!!"  << endl; 
}

