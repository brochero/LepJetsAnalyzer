#include "TH2.h" 

void CreateEGSF(){

  TH1::SetDefaultSumw2(kTRUE);

  // Lepton SFs: ID and ISO with stat. + syst. Errors

  TFile *EGSF_RECO  = TFile::Open("EG_RECO_Moriond17.root");
  TFile *EGSF_ID    = TFile::Open("EG_CutBasedMedium_Moriond17.root");  

  if( !EGSF_RECO  || !EGSF_ID ){
    std::cerr << "ERROR [SF]: Could not open SF files!!!"  << std::endl;
    std::exit(0);
  }

  TH2F *hEG_RECO = (TH2F*) EGSF_RECO->Get("EGamma_SF2D")->Clone("hEG_RECO");
  TH2F *hEG_ID   = (TH2F*) EGSF_ID  ->Get("EGamma_SF2D")->Clone("hEG_ID");


  if(!hEG_RECO || !hEG_ID){
    std::cerr << "ERROR [MuonSF]: Histograms not found!" << std::endl;
    std::exit(0);
  }
  
  // --------------------------------------------------------
  // ID and RECO
  // --------------------------------------------------------    

  // Binning
  int Nbinmueta_ID = hEG_ID->GetNbinsX(); // X-axis (eta)
  int NbinmupT_ID  = hEG_ID->GetNbinsY();  // Y-axis (pT)

  std::cout << "----------   ID   -------------\n " << std::endl; 
  std::cout << "Number of X-axis bins (eta) = " << Nbinmueta_ID << std::endl; 
  std::cout << "Number of Y-axis bins (pT)  = " << NbinmupT_ID << std::endl; 
  std::cout << "-------------------------------\n " << std::endl; 
    
  double Vbinmueta_ID[11]; // muon bin eta values
  double VbinmupT_ID[7];   // muon bin pT values

  // X-axis (eta)
  for(int nbx=1; nbx <= Nbinmueta_ID; nbx++)  Vbinmueta_ID[nbx-1] = ( hEG_ID->GetXaxis()->GetBinLowEdge(nbx) );
  Vbinmueta_ID[Nbinmueta_ID] = ( hEG_ID->GetXaxis()->GetBinLowEdge(Nbinmueta_ID + 1) ); // Add upper edge of the last bin

  // Y-axis (pT)
  for(int nby=1; nby <= NbinmupT_ID; nby++)  VbinmupT_ID[nby-1] = ( hEG_ID->GetYaxis()->GetBinLowEdge(nby) );
  VbinmupT_ID[NbinmupT_ID] = ( hEG_ID->GetYaxis()->GetBinLowEdge(NbinmupT_ID + 1) ); // Add Max pT for the last bin

  for(int nbx=0; nbx <= Nbinmueta_ID; nbx++) cout << Vbinmueta_ID[nbx] << endl;  
  for(int nby=0; nby <= NbinmupT_ID;  nby++) cout << VbinmupT_ID[nby] << endl;  


  // Binning
  int Nbinmueta_RECO = hEG_RECO->GetNbinsX(); // X-axis (eta)
  int NbinmupT_RECO  = hEG_RECO->GetNbinsY();  // Y-axis (pT)

  std::cout << "----------   RECO   -------------\n " << std::endl; 
  std::cout << "Number of X-axis bins (eta) = " << Nbinmueta_RECO << std::endl; 
  std::cout << "Number of Y-axis bins (pT)  = " << NbinmupT_RECO << std::endl; 
  std::cout << "-------------------------------\n " << std::endl; 
    
  double Vbinmueta_RECO[31]; // muon bin eta values
  double VbinmupT_RECO[2];   // muon bin pT values

  // X-axis (eta)
  for(int nbx=1; nbx <= Nbinmueta_RECO; nbx++)  Vbinmueta_RECO[nbx-1] = ( hEG_RECO->GetXaxis()->GetBinLowEdge(nbx) );
  Vbinmueta_RECO[Nbinmueta_RECO] = ( hEG_RECO->GetXaxis()->GetBinLowEdge(Nbinmueta_RECO + 1) ); // Add upper edge of the last bin

  // Y-axis (pT)
  for(int nby=1; nby <= NbinmupT_RECO; nby++)  VbinmupT_RECO[nby-1] = ( hEG_RECO->GetYaxis()->GetBinLowEdge(nby) );
  VbinmupT_RECO[NbinmupT_RECO] = ( hEG_RECO->GetYaxis()->GetBinLowEdge(NbinmupT_RECO + 1) ); // Add Max pT for the last bin

  for(int nbx=0; nbx <= Nbinmueta_RECO; nbx++) cout << Vbinmueta_RECO[nbx] << endl;  
  for(int nby=0; nby <= NbinmupT_RECO;  nby++) cout << VbinmupT_RECO[nby] << endl;  

  double Vbinmueta[33] = {
    -2.5,
    -2.45,
    -2.4,
    -2.3,
    -2.2,
    -2,
    -1.8,
    -1.63,
    -1.566,
    -1.444,
    -1.2,
    -1,
    -0.8,
    -0.6,
    -0.4,
    -0.2,
    0,
    0.2,
    0.4,
    0.6,
    0.8,
    1,
    1.2,
    1.444,
    1.566,
    1.63,
    1.8,
    2,
    2.2,
    2.3,
    2.4,
    2.45,
    2.5
  };

  TH2F *h2D_EG_RECO = new TH2F("h2D_EG_RECO","Electron RECO",32, Vbinmueta, NbinmupT_ID, VbinmupT_ID);

  int nbx_= 1;
  for(int nbx=1; nbx <= Nbinmueta_RECO; nbx++){
  
    float hCon = hEG_RECO->GetBinContent(nbx,1);
    float hConErr = hEG_RECO->GetBinError(nbx,1);
    for(int nby=0; nby <= NbinmupT_ID;  nby++){
      h2D_EG_RECO->SetBinContent(nbx_,nby,hCon);
      h2D_EG_RECO->SetBinError(nbx_,nby,hConErr);
    }
    if(h2D_EG_RECO->GetXaxis()->GetBinLowEdge(nbx) == -1. || h2D_EG_RECO->GetXaxis()->GetBinLowEdge(nbx) == 0.6 ){
      nbx_++;
      for(int nby=0; nby <= NbinmupT_ID;  nby++){
	h2D_EG_RECO->SetBinContent(nbx_,nby,hCon);    
	h2D_EG_RECO->SetBinError(nbx_,nby,hConErr);    
      }
    }

    nbx_++;
  }


  TH2F *h2D_EG_ID = new TH2F("h2D_EG_ID","Electron ID",32, Vbinmueta, NbinmupT_ID, VbinmupT_ID);
  TH2F *h2D_EG_Tr = new TH2F("h2D_EG_Tr","Single Electron Trigger",32, Vbinmueta, NbinmupT_ID, VbinmupT_ID);
  
  
  for(int nby=1; nby <= NbinmupT_ID; nby++){
    
    nbx_= 1;
    for(int nbx=1; nbx <= Nbinmueta_ID; nbx++){
      
      float hCon = hEG_ID->GetBinContent(nbx,nby);
      float hConErr = hEG_ID->GetBinError(nbx,nby);
      
      h2D_EG_ID->SetBinContent(nbx_,nby,hCon);
      h2D_EG_ID->SetBinError(nbx_,nby,hConErr);
     
      h2D_EG_Tr->SetBinContent(nbx_,nby,1.);
      
      float LowB  =  hEG_ID->GetXaxis()->GetBinLowEdge(nbx+1); 
      float LowB_ =  h2D_EG_ID->GetXaxis()->GetBinLowEdge(nbx_+1); 
      
      while(LowB != LowB_){
	nbx_++;
	h2D_EG_ID->SetBinContent(nbx_,nby,hCon);
	h2D_EG_ID->SetBinError(nbx_,nby,hConErr);
	h2D_EG_Tr->SetBinContent(nbx_,nby,1.);
	LowB_ =  h2D_EG_ID->GetXaxis()->GetBinLowEdge(nbx_+1);
      }
      
      nbx_++;
    }
    
  }
  

  // --------------------------------------------------------
  // --------------------------------------------------------

  TH2F *h2D_EGSF = (TH2F*) h2D_EG_ID->Clone("h2D_EGSF");
  h2D_EGSF->Multiply(h2D_EG_RECO);

  TCanvas *cEG = new TCanvas("cEG","Electron SF");
  cEG->Divide(3,2);

  cEG->cd(1);
  h2D_EG_RECO->Draw("LEGO2");
  cEG->cd(2);
  hEG_RECO->Draw("LEGO2");
  cEG->cd(3);
  hEG_RECO->Draw("COLTEXT");
  cEG->cd(4);
  h2D_EG_ID->Draw("LEGO2");
  cEG->cd(5);
  hEG_ID->Draw("LEGO2");
  cEG->cd(6);
  hEG_ID->Draw("COLTEXT");
  
  TCanvas *cEGTot = new TCanvas("cEGTot","Electron SF");
  cEGTot->Divide(2,1);

  cEGTot->cd(1);
  h2D_EGSF->Draw("LEGO2");

  cEGTot->cd(2);
  h2D_EGSF->Draw("COLTEXT");

  TCanvas *cEGTr = new TCanvas("cEGTr","Electron SF");
  cEGTr->Divide(2,1);

  cEGTr->cd(1);
  h2D_EG_Tr->Draw("LEGO2");

  cEGTr->cd(2);
  h2D_EG_Tr->Draw("COLTEXT");


  TFile *target_EG  = new TFile("ElectronSF_IDISO_Trigger_POGMoriond17.root","RECREATE" );
  target_EG->cd();

  h2D_EG_Tr->SetName("eTriggerSF");
  h2D_EGSF->SetName("eIDISOSF");

  h2D_EG_Tr->Write();
  h2D_EGSF->Write();

  cEG->Write();
  cEGTot->Write();
  cEGTr->Write();
}
 
