#include "TH2.h" 

void CreateMuSF(){

  TH1::SetDefaultSumw2(kTRUE);

  // Lepton SFs: ID and ISO with stat. + syst. Errors

  TFile *MuSF_ID_BtoF  = TFile::Open("Mu_ID_Moriond17_RunBtoF.root");
  TFile *MuSF_ID_GtoH  = TFile::Open("Mu_ID_Moriond17_RunGtoH.root");

  TFile *MuSF_ISO_BtoF = TFile::Open("Mu_ISO_Moriond17_RunBtoF.root");
  TFile *MuSF_ISO_GtoH = TFile::Open("Mu_ISO_Moriond17_RunGtoH.root");

  TFile *MuSF_Tr_BtoF  = TFile::Open("Mu_Trigger_Moriond17_RunBtoF.root");
  TFile *MuSF_Tr_GtoH  = TFile::Open("Mu_Trigger_Moriond17_RunGtoH.root");

  TFile *MuSF_Tck_BtoH = TFile::Open("Tracking_EfficienciesAndSF_BCDEFGH.root");
  

  if(!MuSF_ID_BtoF  || !MuSF_ID_GtoH  ||
     !MuSF_ISO_BtoF || !MuSF_ISO_GtoH ||
     !MuSF_Tr_BtoF  || !MuSF_Tr_GtoH  || !MuSF_Tck_BtoH){
    std::cerr << "ERROR [SF]: Could not open SF files!!!"  << std::endl;
    std::exit(0);
  }


  TH2F *hMu_ID_BtoF  = (TH2F*) MuSF_ID_BtoF->Get("MC_NUM_TightID_DEN_genTracks_PAR_pt_eta/abseta_pt_ratio")->Clone("muIDBtoF");
  TH2F *hMu_ID_GtoH  = (TH2F*) MuSF_ID_GtoH->Get("MC_NUM_TightID_DEN_genTracks_PAR_pt_eta/abseta_pt_ratio")->Clone("muIDGtoH");

  TH2F *hMu_ISO_BtoF = (TH2F*) MuSF_ISO_BtoF->Get("TightISO_TightID_pt_eta/abseta_pt_ratio")->Clone("muISOBtoF");
  TH2F *hMu_ISO_GtoH = (TH2F*) MuSF_ISO_GtoH->Get("TightISO_TightID_pt_eta/abseta_pt_ratio")->Clone("muISOGtoH");

  TH2F *hMu_Tr_BtoF  = (TH2F*) MuSF_Tr_BtoF->Get("IsoMu24_OR_IsoTkMu24_PtEtaBins/abseta_pt_ratio")->Clone("muTrBtoF");
  TH2F *hMu_Tr_GtoH  = (TH2F*) MuSF_Tr_GtoH->Get("IsoMu24_OR_IsoTkMu24_PtEtaBins/abseta_pt_ratio")->Clone("muTrGtoH");

  TGraph *TGTck_eta = (TGraph*) MuSF_Tck_BtoH->Get("ratio_eff_eta3_dr030e030_corr")->Clone("muTckBtoH_eta");   
  TGraph *TGTck_vtx = (TGraph*) MuSF_Tck_BtoH->Get("ratio_eff_vtx_dr030e030_corr")->Clone("muTckBtoH_vtx");   
    
  if(!hMu_ID_BtoF      || !hMu_ID_GtoH  ||
     !hMu_ISO_BtoF     || !hMu_ISO_GtoH ||
     !hMu_Tr_BtoF      || !hMu_Tr_GtoH  ||
     !TGTck_eta || !TGTck_vtx){
    std::cerr << "ERROR [MuonSF]: Histograms not found!" << std::endl;
    std::exit(0);
  }
  
  // --------------------------------------------------------
  // ID and ISO
  // --------------------------------------------------------    
  TH2F *h2D_MuSF_IDISO;

  TH2F *hMu_IDISO_BtoF = (TH2F*) hMu_ID_BtoF->Clone("hMu_IDISO_BtoF");
  TH2F *hMu_IDISO_GtoH = (TH2F*) hMu_ID_GtoH->Clone("hMu_IDISO_GtoH");
  
  hMu_IDISO_BtoF->Multiply(hMu_ISO_BtoF);
  hMu_IDISO_GtoH->Multiply(hMu_ISO_GtoH);

  hMu_IDISO_BtoF->Scale(0.55);
  hMu_IDISO_GtoH->Scale(0.45);
  
  h2D_MuSF_IDISO = (TH2F*) hMu_IDISO_BtoF->Clone("hMu_IDISO");
  h2D_MuSF_IDISO->Add(hMu_IDISO_GtoH);
  
  // --------------------------------------------------------
  // Trigger
  // --------------------------------------------------------
  TH2F *hMu_Tr_BtoF_S = (TH2F*)hMu_Tr_BtoF->Clone("hMu_BtoF_S");
  TH2F *hMu_Tr_GtoH_S = (TH2F*)hMu_Tr_GtoH->Clone("hMu_GtoH_S");
  hMu_Tr_BtoF_S->Scale(0.55);
  hMu_Tr_GtoH_S->Scale(0.45);

  TH2F *h2D_MuSF_Tr;
  h2D_MuSF_Tr  = (TH2F*) hMu_Tr_BtoF_S->Clone("hMu_Tr_BtoF");
  h2D_MuSF_Tr->Add(hMu_Tr_GtoH_S);

  // --------------------------------------------------------
  // Tracker
  // --------------------------------------------------------
  int TckNBin_eta = TGTck_eta->GetN();
  float TckBin_eta[TckNBin_eta+1];
  float TckSF_eta [TckNBin_eta];
  
  for (unsigned int nbx_eta = 0; nbx_eta < TckNBin_eta; nbx_eta++){
    TckBin_eta[nbx_eta] = TGTck_eta->GetX()[nbx_eta] - TGTck_eta->GetEXlow()[nbx_eta];
    Double_t TGTck_x_eta, TGTck_y_eta;
    TGTck_eta->GetPoint(nbx_eta,TGTck_x_eta,TGTck_y_eta);
    TckSF_eta[nbx_eta] = TGTck_y_eta;
  }  
  TckBin_eta[TckNBin_eta] = TGTck_eta->GetX()[TckNBin_eta-1] + TGTck_eta->GetEXhigh()[TckNBin_eta-1];


  int TckNBin_vtx = TGTck_vtx->GetN();
  float TckBin_vtx[TckNBin_vtx+1];
  float TckSF_vtx [TckNBin_vtx];
  for (unsigned int nby_vtx = 0; nby_vtx < TckNBin_vtx; nby_vtx++){
    TckBin_vtx[nby_vtx] = TGTck_vtx->GetX()[nby_vtx] - TGTck_vtx->GetEXlow()[nby_vtx];
    Double_t TGTck_x_vtx, TGTck_y_vtx;
    TGTck_vtx->GetPoint(nby_vtx,TGTck_x_vtx,TGTck_y_vtx);
    TckSF_vtx[nby_vtx] = TGTck_y_vtx;
  }  
  TckBin_vtx[TckNBin_vtx] = TGTck_vtx->GetX()[TckNBin_vtx-1] + TGTck_vtx->GetEXhigh()[TckNBin_vtx-1];

  TH2F *h2D_MuSF_Tck = new TH2F("MuonTracker_eta_vtx", "Muon Tracker SF(#eta, Vtx)",
				TckNBin_eta, TckBin_eta,
				TckNBin_vtx, TckBin_vtx);


  for (unsigned int nbx_eta = 1; nbx_eta <= TckNBin_eta; nbx_eta++){
    for (unsigned int nby_vtx = 1; nby_vtx <= TckNBin_vtx; nby_vtx++){
      
      float SFTck = TckSF_eta[nbx_eta-1]*TckSF_vtx[nby_vtx-1];

      // cout << "( "<< nbx_eta << " , " << nby_vtx << " ) = " << TckSF_eta[nbx_eta-1] << " x " << TckSF_vtx[nby_vtx-1] << " = " << SFTck << endl; 

      h2D_MuSF_Tck->SetBinContent(nbx_eta,nby_vtx,SFTck);      

    }
  }

  // --------------------------------------------------------
  // --------------------------------------------------------
  
  TCanvas *CanHistID = new TCanvas("CanHistID","Combined Histogram");
  CanHistID->Divide(2,2);

  CanHistID->cd(1);
  hMu_ID_BtoF->SetTitle("ID B to F");
  hMu_ID_BtoF->Draw("LEGO2");

  CanHistID->cd(2);
  hMu_ID_BtoF->Draw("COLTEXT");

  CanHistID->cd(3);
  hMu_ID_GtoH->SetTitle("ID G to H");
  hMu_ID_GtoH->Draw("LEGO2");

  CanHistID->cd(4);
  hMu_ID_GtoH->Draw("COLTEXT");

  CanHistID->SaveAs("Mu_SFID.pdf");

  TCanvas *CanHistISO = new TCanvas("CanHistISO","Combined Histogram");
  CanHistISO->Divide(2,2);

  CanHistISO->cd(1);
  hMu_ISO_BtoF->SetTitle("ISO B to F");
  hMu_ISO_BtoF->Draw("LEGO2");

  CanHistISO->cd(2);
  hMu_ISO_BtoF->Draw("COLTEXT");

  CanHistISO->cd(3);
  hMu_ISO_GtoH->SetTitle("ISO G to H");
  hMu_ISO_GtoH->Draw("LEGO2");

  CanHistISO->cd(4);
  hMu_ISO_GtoH->Draw("COLTEXT");

  CanHistISO->SaveAs("Mu_SFISO.pdf");

  TCanvas *CanHist = new TCanvas("CanHist","Combined Histogram");
  CanHist->Divide(2,1);

  CanHist->cd(1);
  h2D_MuSF_IDISO->SetTitle("Total ID x ISO Muon Scale Factor");
  h2D_MuSF_IDISO->Draw("LEGO2");

  CanHist->cd(2);
  h2D_MuSF_IDISO->Draw("COLTEXT");

  CanHist->SaveAs("Mu_SFIDISO.pdf");

  TCanvas *CanHistTr = new TCanvas("CanHistTr","Combined Histogram");
  CanHistTr->Divide(2,1);

  CanHistTr->cd(1);
  h2D_MuSF_Tr->SetTitle("Total Single Muon Trigger Scale Factor");
  h2D_MuSF_Tr->Draw("LEGO2");

  CanHistTr->cd(2);
  h2D_MuSF_Tr->Draw("COLTEXT");

  CanHistTr->SaveAs("Mu_SFTr.pdf");


  TCanvas *CanHistTr_range = new TCanvas("CanHistTr_range","Combined Histogram");
  CanHistTr_range->Divide(2,2);

  CanHistTr_range->cd(1);
  hMu_Tr_BtoF->SetTitle("Total Single Muon Trigger Scale Factor B to F");
  hMu_Tr_BtoF->Draw("LEGO2");

  CanHistTr_range->cd(2);
  hMu_Tr_BtoF->Draw("COLTEXT");

  CanHistTr_range->cd(3);
  hMu_Tr_GtoH->SetTitle("Total Single Muon Trigger Scale Factor G to H");
  hMu_Tr_GtoH->Draw("LEGO2");

  CanHistTr_range->cd(4);
  hMu_Tr_GtoH->Draw("COLTEXT");

  CanHistTr_range->SaveAs("Mu_SFTr_Range.pdf");


  TCanvas *CanHistTck = new TCanvas("CanHistTck","Combined Histogram");
  CanHistTck->Divide(2,2);

  CanHistTck->cd(1);
  TGTck_eta->SetTitle("Tracker SF Vs #eta");
  TGTck_eta->Draw();
 
  CanHistTck->cd(2);
  TGTck_eta->SetTitle("Tracker SF Vs Vtx");
  TGTck_vtx->Draw();    

  CanHistTck->cd(3);
  h2D_MuSF_Tck->Draw("LEGO2");    

  CanHistTck->cd(4);
  h2D_MuSF_Tck->Draw("COLTEXT");    

  CanHistTck->SaveAs("Mu_SFTck.pdf");

  // --------------------------------------------------------
  // --------------------------------------------------------

  TFile *target_Mu  = new TFile("MuonSF_IDISO_Trigger_POGMoriond17.root","RECREATE" );
  target_Mu->cd();

  h2D_MuSF_Tr->SetName("muTriggerSF");
  h2D_MuSF_IDISO->SetName("muIDISOSF");
  h2D_MuSF_Tck->SetName("muTrackerSF");

  h2D_MuSF_Tr->Write();
  h2D_MuSF_IDISO->Write();
  h2D_MuSF_Tck->Write();

  CanHistTr_range->Write();
  CanHistTr->Write();
  CanHistISO->Write();
  CanHistID->Write();
  CanHist->Write();
  CanHistTck->Write();  
}
 
