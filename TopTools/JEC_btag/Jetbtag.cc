// New b-tag method (includes JES correlations)
#include "../../TreeReader.h"

// ---------
// Jet SF
// ---------
float GetbtagSF(hbtag SF, ComJet jet){
  TH1F dummyPTh("dummyPTh", "dummyPTh",
		dummypT_bin, dummypT_binsize);
  TH1F dummyETAh("dummyETAh","dummyETAh",
		 dummyeta_bin, dummyeta_binsize);
  
  int hadronFlavour = jet.Flavour;
  
  int ipT_bin = dummyPTh.GetXaxis()->FindBin(jet.Pt()) - 1; 
  int icsv_bin = SF.HF[0]->GetXaxis()->FindBin(jet.CSV);
  
  float SFbtag = 1.;
  
  if(std::abs(jet.Eta()) > 2.4 || jet.CSV < -1) return SFbtag;

  // b-Flavor
  if (std::abs(hadronFlavour) == 5)
    SFbtag = SF.HF[ipT_bin]->GetBinContent(icsv_bin);
  // c-Flavor is 1
  else if(std::abs(hadronFlavour) == 4 )
    SFbtag = SF.CF[ipT_bin]->GetBinContent(icsv_bin);            
  // light-Flavor
  else if(hadronFlavour == 0 ){ 
    int ieta_bin = dummyETAh.GetXaxis()->FindBin(std::abs(jet.Eta())) - 1; 
    if (ipT_bin == 4) ipT_bin = 3;
    SFbtag = SF.LF[ipT_bin][ieta_bin]->GetBinContent(icsv_bin);
  }
  
  return SFbtag;
 
}


// ---------
// Event SF
// ---------
float GetbtagEvtWeight(hbtag SF, std::vector<ComJet> jets){
  
  float EvtWeight = 1.;  
  for (auto itjet = jets.begin(); itjet != jets.end(); itjet++ )
    EvtWeight *= GetbtagSF(SF,(*itjet));    
  
  return EvtWeight;

}

// ---------
// Load SF
// ---------
btagCollection SetupbtagSF(TString FileName_HF, TString FileName_LF){

  TH1::AddDirectory(kFALSE);
    
  btagCollection FullSetOfSF;
  // 25 JES Variations + Up/Down
  // 3 Nominal (LF) - 5 HF Variations + Up/Down
  // 3 LF Variations - 5 Nominal (HF CF) + Up/Down
  
  std::cout << "Loading b-tag file " << FileName_HF << std::endl;
  std::cout << "Loading b-tag file " << FileName_LF << std::endl;
    
  std::vector<TString> btgnamesLF = {"HF","Stats1","Stats2"};
  std::vector<TString> btgnamesHF = {"LF","Stats1","Stats2"};
  std::vector<TString> btgnamesCF = {"cErr1","cErr2"};
  // ORDER in vbtgNames: btgnamesLF + btgnamesHF
  
  TFile * csv_hf = TFile::Open(FileName_HF);
  TFile * csv_lf = TFile::Open(FileName_LF);
  
  // --------------
  // CENTRAL
  // --------------

  // HF
  for (int iipt=0; iipt<5; iipt++){ // pT bins

    char histName[50]; 
    sprintf(histName,"h_csv_ratio_Pt%i_Eta0_final",iipt);
    
    csv_hf->cd();
    // default HF
    if ( csv_hf->GetListOfKeys()->Contains(histName) )
      FullSetOfSF.Central.HF[iipt] = (TH1D*)csv_hf->Get(histName)->Clone(histName); 
    else exit(EXIT_FAILURE);

    // default CF
    sprintf(histName,"c_csv_ratio_Pt%i_Eta0_final",iipt);
    if ( csv_hf->GetListOfKeys()->Contains(histName) )
      FullSetOfSF.Central.CF[iipt] = (TH1D*)csv_hf->Get(histName)->Clone(histName); 
    else exit(EXIT_FAILURE);

    if (iipt<=3){ 
      
      csv_lf->cd();
    
      for (int iieta=0; iieta<3; iieta++) {
	// Central LF
	sprintf(histName,"h_csv_ratio_Pt%i_Eta%i_final",iipt,iieta);      
	if ( csv_lf ->GetListOfKeys()->Contains(histName) )
	  FullSetOfSF.Central.LF[iipt][iieta] = (TH1D*)csv_lf->Get(histName)->Clone(histName);
	else exit(EXIT_FAILURE);
      }// for(iieta)
    }// if(iieta)
  }// for(iipt)

  //--------------------------------------------------------------------------------------------------------

  // --------------
  // b-tag sys HF
  // --------------
  for (int ibtg = 0; ibtg < btgnamesLF.size(); ibtg ++){
    for (int ivar = 1; ivar < vVarNames.size(); ivar ++){      
      for (int iipt=0; iipt<5; iipt++){ // pT bins
	
	char histName[50]; 
    	TString sbtagName = btgnamesLF.at(ibtg) + vVarNames.at(ivar);
	
	csv_hf->cd();
	
	// Default HF
    	sprintf(histName,"DEFAULTFor_h_csv_ratio_Pt%i_Eta0_final_%s",iipt,sbtagName.Data() );
    	FullSetOfSF.Syst[ibtg][ivar].HF[iipt] = (TH1D*)FullSetOfSF.Central.HF[iipt]->Clone(histName);
	
	// Default CF
	sprintf(histName,"DEFAULTFor_c_csv_ratio_Pt%i_Eta0_final_%s",iipt,sbtagName.Data() );
	FullSetOfSF.Syst[ibtg][ivar].CF[iipt] = (TH1D*)FullSetOfSF.Central.CF[iipt]->Clone(histName); 
	
	if (iipt<=3){ 
	  
	  csv_lf->cd();
	  
	  for (int iieta=0; iieta<3; iieta++) {
	    // Syst LF
	    sprintf(histName,"h_csv_ratio_Pt%i_Eta%i_final_%s",iipt,iieta,sbtagName.Data());
	    if ( csv_lf ->GetListOfKeys()->Contains(histName) )
	      FullSetOfSF.Syst[ibtg][ivar].LF[iipt][iieta] = (TH1D*)csv_lf->Get(histName)->Clone(histName);
	    else exit(EXIT_FAILURE);
	  }// for(iieta)
	}// if(iieta)
	
      } //for(iipt)
    }// for(ivar) 
  }// for(ibtg) 
  
  
  // --------------
  // b-tag sys LF
  // --------------
  for (int ibtg = btgnamesLF.size(); 
       ibtg < (btgnamesLF.size() + btgnamesHF.size()); 
       ibtg ++){
    for (int ivar = 1; ivar < vVarNames.size(); ivar ++){      
      for (int iipt=0; iipt<5; iipt++){ // pT bins
	
	char histName[50]; 
	TString sbtagName = btgnamesHF.at(ibtg-btgnamesLF.size()) + vVarNames.at(ivar);
	
	csv_hf->cd();
	
	// Syst HF
    	sprintf(histName,"h_csv_ratio_Pt%i_Eta0_final_%s",iipt,sbtagName.Data() );
	if ( csv_hf->GetListOfKeys()->Contains(histName) )
    	  FullSetOfSF.Syst[ibtg][ivar].HF[iipt] = (TH1D*)csv_hf->Get(histName)->Clone(histName); 
    	else exit(EXIT_FAILURE);
	
	// Default CF
	sprintf(histName,"DEFAULTFor_c_csv_ratio_Pt%i_Eta0_final_%s",iipt,sbtagName.Data() );
	FullSetOfSF.Syst[ibtg][ivar].CF[iipt] = (TH1D*)FullSetOfSF.Central.CF[iipt]->Clone(histName); 
	
	if (iipt<=3){ 
	  
	  csv_lf->cd();
	  
	  for (int iieta=0; iieta<3; iieta++) {
	    // Default LF
	    sprintf(histName,"DEFAULTFor_h_csv_ratio_Pt%i_Eta%i_final_%s",iipt,iieta,sbtagName.Data());
	    FullSetOfSF.Syst[ibtg][ivar].LF[iipt][iieta] = (TH1D*)FullSetOfSF.Central.LF[iipt][iieta]->Clone(histName);
	  }// for(iieta)
	}// if(iieta)
      
      } //for(iipt)
    }// for(ivar) 
  }// for(ibtg) 

  // --------------
  // b-tag sys CF
  // --------------
  for (int ibtg = (btgnamesLF.size() + btgnamesHF.size()); 
       ibtg < (btgnamesLF.size() + btgnamesHF.size() + btgnamesCF.size()); 
       ibtg ++){
    for (int ivar = 1; ivar < vVarNames.size(); ivar ++){      
      for (int iipt=0; iipt<5; iipt++){ // pT bins
	
	char histName[50]; 
	TString sbtagName = btgnamesCF.at(ibtg-(btgnamesLF.size() + btgnamesHF.size())) + vVarNames.at(ivar);
	
	csv_hf->cd();
	
	// Default HF
    	sprintf(histName,"DEFAULTFor_h_csv_ratio_Pt%i_Eta0_final_%s",iipt,sbtagName.Data() );
    	FullSetOfSF.Syst[ibtg][ivar].HF[iipt] = (TH1D*)FullSetOfSF.Central.HF[iipt]->Clone(histName);
	
	// Default CF
    	sprintf(histName,"c_csv_ratio_Pt%i_Eta0_final_%s",iipt,sbtagName.Data() );
	if ( csv_hf->GetListOfKeys()->Contains(histName) )
    	  FullSetOfSF.Syst[ibtg][ivar].CF[iipt] = (TH1D*)csv_hf->Get(histName)->Clone(histName); 
    	else exit(EXIT_FAILURE);
	
	if (iipt<=3){ 
	  
	  csv_lf->cd();
	  
	  for (int iieta=0; iieta<3; iieta++) {
	    // Syst LF
	    sprintf(histName,"DEFAULTFor_h_csv_ratio_Pt%i_Eta%i_final_%s",iipt,iieta,sbtagName.Data());
	    FullSetOfSF.Syst[ibtg][ivar].LF[iipt][iieta] = (TH1D*)FullSetOfSF.Central.LF[iipt][iieta]->Clone(histName);
	  }// for(iieta)
	}// if(iieta)
      
      } //for(iipt)
    }// for(ivar) 
  }// for(ibtg) 

  //--------------------------------------------------------------------------------------------------------
  
  // JES sys (25)
  for (int ijes = 0; ijes < JESName.size(); ijes ++){ 
    for (int ivar = 1; ivar < vVarNames.size(); ivar ++){
      for (int iipt=0; iipt<5; iipt++){ // pT bins
	
    	char histJESName[50];
    	TString sJESName = JESName.at(ijes)+vVarNames.at(ivar);	
	
    	sprintf(histJESName,"h_csv_ratio_Pt%i_Eta0_final_JES%s",iipt, sJESName.Data() );

	csv_hf->cd();
	// HF
    	if ( csv_hf ->GetListOfKeys()->Contains(histJESName) )
    	  FullSetOfSF.SystJES[ijes][ivar].HF[iipt] = (TH1D*)csv_hf->Get(histJESName)->Clone(histJESName);
    	else{ // Default
    	  sprintf(histJESName,"DEFAULTFor_h_csv_ratio_Pt%i_Eta0_final_JES%s",iipt, sJESName.Data() );
	  FullSetOfSF.SystJES[ijes][ivar].HF[iipt] = (TH1D*)FullSetOfSF.Central.HF[iipt]->Clone(histJESName);
    	}
	// Default CF 
	sprintf(histJESName,"DEFAULTFor_c_csv_ratio_Pt%i_Eta0_final_JES%s",iipt, sJESName.Data() );
	FullSetOfSF.SystJES[ijes][ivar].CF[iipt] = (TH1D*)FullSetOfSF.Central.CF[iipt]->Clone(histJESName);
	
	if (iipt<=3){ 
	  
	  csv_lf->cd();
	  
	  for (int iieta=0; iieta<3; iieta++) {
	    // Syst LF
	    sprintf(histJESName,"h_csv_ratio_Pt%i_Eta%i_final_JES%s",iipt,iieta,sJESName.Data());
	    if ( csv_lf ->GetListOfKeys()->Contains(histJESName) )
	      FullSetOfSF.SystJES[ijes][ivar].LF[iipt][iieta] = (TH1D*)csv_lf->Get(histJESName)->Clone(histJESName);
	    else{ // Default
	      sprintf(histJESName,"DEFAULTFor_h_csv_ratio_Pt%i_Eta%i_final_JES%s",iipt,iieta,sJESName.Data());
	      FullSetOfSF.SystJES[ijes][ivar].LF[iipt][iieta] = (TH1D*)FullSetOfSF.Central.LF[iipt][iieta]->Clone(histJESName);
	    }
	  }// for(iieta)
	}// if(iieta)
      }	// for(iipt)
	
    } // for(ivar) 
  } // for(ijes)
    
  std::cout << "Histograms for b-tagging SF loaded successfully." << std::endl;

  csv_hf->Close();
  csv_lf->Close();

  delete csv_hf;
  delete csv_lf;

  return FullSetOfSF; 

}
