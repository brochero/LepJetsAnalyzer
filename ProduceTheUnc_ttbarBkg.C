
void ProduceTheUnc_ttbarBkg(TString FileVersion = "PreApp-v0"){
  
  TString HeadFile = "hSF-" + FileVersion + "_Tree_LepJets_NewJEC-OldKinFit_v8-0-6_Spring16-80X_36814pb-1";
  TString InOutDir = "TopResults/" + FileVersion + "/";

  TString ChName[2];
  ChName[0] = "mujets";
  ChName[1] = "ejets";
  TString SelCut = "2btag";
  TString  HistoCombName = "hKinAddCSVUnroll";
  
  TFile *Infile=NULL;
  Infile = TFile::Open(InOutDir + HeadFile + "_ttbar_PowhegPythiaBkg.root");
  if(!Infile){
    std::cerr << "ERROR: Could not open the input file!!!"  << std::endl;
    std::exit(0);
  }

  TFile *target = new TFile(InOutDir + HeadFile + "_ttbar_PowhegPythiaBkg_SYS_AllTheory.root","recreate");

  std::vector<TString> SysName;
  SysName.push_back("ISR");
  SysName.push_back("FSR");
  SysName.push_back("UE");
  SysName.push_back("ScaleRdF");
  SysName.push_back("ScaleRuF");

  TString SysNameVar[2] = {"Up","Down"};
  
  for(int nch = 0; nch < 2; nch++){
    for (int isys = 0; isys < SysName.size(); isys++){
      for (int isysvar = 0; isysvar < 2; isysvar++){
	
	TString hpath = SelCut + "/" + ChName[nch] + "/" + HistoCombName + "_" + ChName[nch] + "_" + SelCut;
	TH1D *chis = (TH1D*)Infile->Get("central/"+hpath);

	TH1D *Systhis = (TH1D*) chis->Clone((TString)chis->GetName() + "_" + SysName.at(isys) + SysNameVar[isysvar]);
	target->mkdir(SysName.at(isys) + SysNameVar[isysvar] + "/" + SelCut + "/" + ChName[nch]);
        target->cd   (SysName.at(isys) + SysNameVar[isysvar] + "/" + SelCut + "/" + ChName[nch]);
        Systhis->Write();
        target->cd();

      }
    }
  }

  target->Close();
  cout << "Files for ttbar background with sytematic variation (ScaleRdF, ISR/FSR and UE) have been produced." << endl;
}
 
