void SimpleChi2Plots(TString Channel="mujets"){
  gROOT->SetStyle("Plain");
  gStyle->SetOptStat(kFALSE);

  const int nh = 5;
  //TString namefile = "TopResults/hSF-EffCSVFull_Tree_LepJets_GenCone_v8-0-0_Spring16-80X_5913pb-1_ttbar_PowhegPythia";
  TString namefile = "TopResults/hSF-NuiPar-v0_Tree_LepJets_KFCSVOrder01NoSkim_v8-0-1_Spring16-80X_15920pb-1_ttbar_LepJetsPowhegPythiaTranche3FullPhSp";

  TFile *hFile=NULL;
  hFile = TFile::Open(namefile + ".root");

  if(!hFile){
    std::cerr << "ERROR: Could not open " <<  namefile  << " files!!!"  << std::endl;
    std::exit(0);
  }

  TH1F *histo[nh];
  // TEfficiency *histo[nh];
  TString histoname = "hKinChi2";
  TString Cut[nh]={"4Jets","4Jets2btag","6Jets","2btag","3btag"}; 

  cout << Cut[0] + "/" + Channel + "/" + histoname + "_" + Channel + "_" + Cut[0] << endl;
  histo[0] = (TH1F*)hFile->Get(Cut[0] + "/" + Channel + "/" + histoname + "_" + Channel + "_" + Cut[0] );
  histo[1] = (TH1F*)hFile->Get(Cut[1] + "/" + Channel + "/" + histoname + "_" + Channel + "_" + Cut[1] );
  histo[2] = (TH1F*)hFile->Get(Cut[2] + "/" + Channel + "/" + histoname + "_" + Channel + "_" + Cut[2] );
  histo[3] = (TH1F*)hFile->Get(Cut[3] + "/" + Channel + "/" + histoname + "_" + Channel + "_" + Cut[3] );
  histo[4] = (TH1F*)hFile->Get(Cut[4] + "/" + Channel + "/" + histoname + "_" + Channel + "_" + Cut[4] );

  TCanvas *histocanvas;
  histocanvas = new TCanvas("plots", "Plots");

  histocanvas->SetGrid();
  histocanvas->SetLogy();
  histocanvas->cd();

  for (int ih=0; ih<5; ih++){
    float Nhisto = histo[ih]->Integral();
    if(Nhisto!=0.0) histo[ih]->Scale(1.0/Nhisto);
  }
  histo[0]->SetTitle("#chi^{2} Distribution");
  histo[0]->SetMarkerStyle(20);
  histo[0]->SetMarkerColor(4);
  histo[0]->SetLineColor(4);
  histo[0]->GetXaxis()->SetTitle("#chi^{2}");
  histo[0]->Draw();

  histo[1]->SetMarkerStyle(21);
  histo[1]->SetMarkerColor(2);
  histo[1]->SetLineColor(2);
  histo[1]->Draw("SAME");

  histo[2]->SetMarkerStyle(22);
  histo[2]->SetMarkerColor(kGreen-2);
  histo[2]->SetLineColor(kGreen-2);
  histo[2]->Draw("SAME");

  histo[3]->SetMarkerStyle(23);
  histo[3]->SetMarkerColor(kMagenta-2);
  histo[3]->SetLineColor(kMagenta-2);
  histo[3]->Draw("SAME");

  histo[4]->SetMarkerStyle(29);
  histo[4]->SetMarkerColor(kOrange+9);
  histo[4]->SetLineColor(kOrange+9);
  histo[4]->Draw("SAME");

  TLegend *leg;
  leg = new TLegend(0.70, 0.64,
		     0.93, 0.87);
  leg->AddEntry(histo[0],"4 Jets",   "LP");
  leg->AddEntry(histo[1],"4 Jets + 2 btag",     "LP");
  leg->AddEntry(histo[2],"6 Jets", "LP");
  leg->AddEntry(histo[3],"6 Jets + 2 btag", "LP");
  leg->AddEntry(histo[4],"6 Jets + 3 btag", "LP");
  leg->Draw("SAME");

  histocanvas->SaveAs(TString(histo[0]->GetName()) + ".pdf");

}
