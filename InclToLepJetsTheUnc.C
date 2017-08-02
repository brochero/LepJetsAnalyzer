void Createhisto(TString SelCut, TString HeadFile, TString CatFile);
void RemoveNegativeBins(TH1D *histo);

enum thsys{central=0,ISR,FSR,UE};

void InclToLepJetsTheUnc(TString HeadFile = "hSF-FinalAN-v0_Tree_LepJets_Summer_v8-0-6_Spring16-80X_36814pb-1"){

  TString fdir = "TopResults/"+HeadFile;

  Createhisto("2btag", fdir, "ttbb");
  Createhisto("2btag", fdir, "ttbj");
  Createhisto("2btag", fdir, "ttcc");
  Createhisto("2btag", fdir, "ttLF");
  Createhisto("2btag", fdir, "tt");
  Createhisto("2btag", fdir, "ttjj");
  
}

void Createhisto(TString SelCut, TString HeadFile, TString CatFile){

  TString ChName[2];
  ChName[0] = "mujets";
  ChName[1] = "ejets";

  // All Systematics to be included 
  std::vector<TString> SysName;
  SysName.push_back("ISR");
  SysName.push_back("FSR");
  SysName.push_back("UE");

  TString SysNameVar[2] = {"Up","Down"};

  // Lep+Jets Sample
  std::vector<TString> namefile;
  namefile.push_back(HeadFile + "_ttbar_LepJetsPowhegPythia" + CatFile);
  namefile.push_back(HeadFile + "_ttbar_PowhegPythia" + CatFile);
  for (int isys = 0; isys < SysName.size(); isys++){
    for (int isysvar = 0; isysvar < 2; isysvar++){
      namefile.push_back(HeadFile + "_ttbar_PowhegPythia" + CatFile + "_SYS_" + SysName.at(isys)+SysNameVar[isysvar]);
    }
  }

  std::vector<TFile*> vfile;
  for (int ifile=0;ifile<namefile.size();ifile++){
    TFile *file=NULL;  
    std::cout << "Loading " << namefile.at(ifile) + ".root" << " sample..." << std::endl;
    file = TFile::Open(namefile.at(ifile) + ".root");
    if(!file){
      std::cerr << "ERROR: Could not open " <<  namefile.at(ifile)  << " files!!!"  << std::endl;
      std::exit(0);
    }
    vfile.push_back(file);    
  }
  
  // Output file with new histograms
  TString outfile;
  outfile = namefile.at(0) + "_SYS_Theory.root";  
  TFile *target  = new TFile(outfile,"RECREATE" );  
  target->cd();

  // Histogram to use in Combine
  TString HistoCombName = "hKinAddCSVUnroll";

  for(int nch = 0; nch < 2; nch++){

    TString hpath = SelCut + "/" + ChName[nch] + "/" + HistoCombName + "_" + ChName[nch] + "_" + SelCut;
    
    TH1D *hiljCentral = (TH1D*) vfile.at(0)->Get("central/" + hpath);    
    RemoveNegativeBins(hiljCentral);
    TH1D *hiinCentral = (TH1D*) vfile.at(1)->Get("central/" + hpath);    
    RemoveNegativeBins(hiinCentral);
    
    int nfile = 2;
    for (int isys = 0; isys < SysName.size(); isys++){
      for (int isysvar = 0; isysvar < 2; isysvar++){
	
  	TH1D *hiSyst = (TH1D*) vfile.at(nfile)->Get(SysName.at(isys) + SysNameVar[isysvar] + "/" + hpath + "_" + SysName.at(isys) + SysNameVar[isysvar]);
  	RemoveNegativeBins(hiSyst);
	
	TH1D *hiljSyst = (TH1D*) hiljCentral->Clone((TString)hiljCentral->GetName() + "_" + SysName.at(isys) + SysNameVar[isysvar]);
	
	// Coparison Bin/Bin
	for(int ibin=1; ibin<=hiinCentral->GetNbinsX(); ibin++){

	  float FracVar = (hiinCentral->GetBinContent(ibin) - hiSyst->GetBinContent(ibin)); 
	  float oldbin = hiljSyst->GetBinContent(ibin);
	  if(hiinCentral->GetBinContent(ibin) < 0.5)
	    // Keep the full variation if the central bin value is too low
	    hiljSyst->SetBinContent(ibin, hiSyst->GetBinContent(ibin));  
	  else{ 
	    FracVar = FracVar/hiinCentral->GetBinContent(ibin);
	    hiljSyst->SetBinContent(ibin, ((1.0-FracVar)*hiljSyst->GetBinContent(ibin)));
	  }

	} // for(ibin)
	
  	target->mkdir(SysName.at(isys) + SysNameVar[isysvar] + "/" + SelCut + "/" + ChName[nch]);
  	target->cd   (SysName.at(isys) + SysNameVar[isysvar] + "/" + SelCut + "/" + ChName[nch]);
  	hiljSyst->Write();
	target->cd();
	nfile++;
	
      }// for(nsysvar)
    } // for(nsys)
  }// for(nch)
  
  target->Close();
  
  std::cout << "All Plots from " << HeadFile << ".root have been loaded successfully!!!"  << std::endl;
  
}

void RemoveNegativeBins(TH1D *histo){
  for(int ibin=1; ibin<=histo->GetNbinsX(); ibin ++){
    float bcont = histo->GetBinContent(ibin);
    if (bcont < 0.0) {
      std::cout << "Warning!!! Negative bin entry! Setting it to 0.0" << std::endl;
      histo->SetBinContent(ibin,0.0);
    }
  }
}
