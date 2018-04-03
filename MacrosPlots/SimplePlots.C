void SimplePlots(TString Cut="2btag", TString Channel="mujets"){
  gROOT->SetStyle("Plain");
  gStyle->SetOptStat(kFALSE);

  const int nh = 3;
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
  TString histoname[nh] = {"hTJetPosition","hWJetPosition","hOJetPosition"};

  cout << "Loading: " << Cut + "/" + Channel + "/" + histoname[0] + "_" + Channel + "_" + Cut << endl;

  histo[0] = (TH1F*)hFile->Get(Cut + "/" + Channel + "/" + histoname[0] + "_" + Channel + "_" + Cut );
  histo[1] = (TH1F*)hFile->Get(Cut + "/" + Channel + "/" + histoname[1] + "_" + Channel + "_" + Cut );
  histo[2] = (TH1F*)hFile->Get(Cut + "/" + Channel + "/" + histoname[2] + "_" + Channel + "_" + Cut );
  //histo[0] = (TEfficiency*)hFile->Get(Cut + "/" + Channel + "/" + histoname[0] + "_" + Channel + "_" + Cut ) ->Clone(histoname[0] + Channel + Cut);

  TCanvas *histocanvas;
  histocanvas = new TCanvas("plots", "Plots");

  histocanvas->SetGrid();
  histocanvas->cd();

  for (int ih=0; ih<3; ih++){
    float Nhisto = histo[ih]->Integral();
    if(Nhisto!=0.0) histo[ih]->Scale(1.0/Nhisto);
  }
  histo[0]->SetTitle("CSV Jet Positions");
  histo[0]->SetMarkerStyle(20);
  histo[0]->SetMarkerColor(4);
  histo[0]->SetLineColor(4);
  histo[0]->Draw();

  histo[1]->SetMarkerStyle(21);
  histo[1]->SetMarkerColor(2);
  histo[1]->SetLineColor(2);
  histo[1]->Draw("SAME");

  histo[2]->SetMarkerStyle(22);
  histo[2]->SetMarkerColor(kGreen-2);
  histo[2]->SetLineColor(kGreen-2);
  histo[2]->Draw("SAME");

  TLegend *leg;
  leg = new TLegend(0.70, 0.64,
		     0.93, 0.87);
  leg->AddEntry(histo[0],"Jets From Top",   "LP");
  leg->AddEntry(histo[1],"Jets From W",     "LP");
  leg->AddEntry(histo[2],"Additional Jets", "LP");
  leg->Draw("SAME");

  histocanvas->SaveAs(TString(histo[0]->GetName()) + ".pdf");

}
