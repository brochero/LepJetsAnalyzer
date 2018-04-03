void ttHConeCatPlots(TString Cut="2btag", TString Channel="mujets"){
  gROOT->SetStyle("Plain");
  gStyle->SetOptStat(kFALSE);

  TString bname = "hSF-ttbbMeetingPlots_Tree_LepJets_TrMC_v8-0-4_Spring16-80X_36814pb-1_ttbar_LepJetsPowhegPythia" ; 

  const int nh = 3;
  TString namefile[6] = 
    {"../TopResults/" + bname + "ttbb", 
     "../TopResults/" + bname + "ttbj", 
     "../TopResults/" + bname + "ttcc",
     "../TopResults/" + bname + "ttLF",
     "../TopResults/" + bname + "tt"};

  TFile *hFile[6]={NULL,NULL, NULL, NULL, NULL, NULL};
  TH1D *histo[6];
  for(int fi = 0; fi < 5; fi++){
    hFile[fi] = TFile::Open(TString(namefile[fi] + ".root"));
    
    if(!hFile[fi]){
      std::cerr << "ERROR: Could not open " <<  namefile  << " files!!!"  << std::endl;
      std::exit(0);
    }
    
    TString histoname = "effCatHiggs";
    
    cout << "Loading: " << Cut + "/" + Channel + "/" + histoname + "_" + Channel + "_" + Cut << endl;
    
    histo[fi] = (TH1D*)hFile[fi]->Get(Cut + "/" + Channel + "/" + histoname + "_" + Channel + "_" + Cut ) ->Clone();
  }


  TCanvas *histocanvas;
  histocanvas = new TCanvas("plots", "Plots");

  histocanvas->SetGrid();
  histocanvas->cd();

  int col_ttbb = TColor::GetColor("#660000");
  int col_ttb  = TColor::GetColor("#ffcc00");
  int col_ttcc = TColor::GetColor("#cc6600");
  int col_ttc  = TColor::GetColor("#cc6600");
  int col_ttLF = TColor::GetColor("#ff0000");
  int col_tt   = TColor::GetColor("#FF7F7F");

  histo[0]->SetTitle("tt Categorization");
  // -----

  TH1F *hStyle = new TH1F("hStyle", "tt Categorization; ttH Categories; Eff",5,0,5);
  hStyle->GetXaxis()->SetBinLabel(1,"ttbb");
  hStyle->GetXaxis()->SetBinLabel(2,"ttbj");
  hStyle->GetXaxis()->SetBinLabel(3,"tt2b");
  hStyle->GetXaxis()->SetBinLabel(4,"ttcc");
  hStyle->GetXaxis()->SetBinLabel(5,"ttLF/tt");
  // -----

  histo[0]->SetMarkerStyle(20);
  histo[0]->SetLineWidth(2);
  histo[0]->SetMarkerColor(col_ttbb);
  histo[0]->SetLineColor(col_ttbb);

  histo[1]->SetMarkerColor(col_ttb);
  histo[1]->SetLineColor(col_ttb);
  histo[1]->SetMarkerStyle(21);
  histo[1]->SetLineWidth(2);

  histo[2]->SetMarkerStyle(22);
  histo[2]->SetLineWidth(2);
  histo[2]->SetMarkerColor(col_ttcc);
  histo[2]->SetLineColor(col_ttcc);

  histo[3]->SetMarkerStyle(23);
  histo[3]->SetLineWidth(2);
  histo[3]->SetMarkerColor(col_ttLF);
  histo[3]->SetLineColor(col_ttLF);

  histo[4]->SetMarkerStyle(29);
  histo[4]->SetLineWidth(2);
  histo[4]->SetMarkerColor(col_tt);
  histo[4]->SetLineColor(col_tt);

  hStyle->Draw();
  histo[0]->Draw("SAME");
  histo[1]->Draw("SAME");
  histo[2]->Draw("SAME");
  histo[3]->Draw("SAME");
  histo[4]->Draw("SAME");

  gPad->Update();
  // histo[0]->GetPaintedGraph()->SetMaximum(0.9);
  // histo[0]->GetPaintedGraph()->SetMinimum(0.);

  TLegend *leg;
  leg = new TLegend(0.78, 0.45,
		    0.88, 0.7);
  //leg->AddEntry(histo[0],"All t#bar{t}","LP");
  leg->AddEntry(histo[0],"t#bar{t}bb","LP");
  leg->AddEntry(histo[1],"t#bar{t}bj","LP");
  leg->AddEntry(histo[2],"t#bar{t}cc","LP");
  leg->AddEntry(histo[3],"t#bar{t}LF","LP");
  leg->AddEntry(histo[4],"t#bar{t}",  "LP");
  leg->Draw("SAME");

  histocanvas->SaveAs("../TopResults/FiguresAlone_PurEff/CatCompttHCone_" + bname + "_" + TString(histo[0]->GetName()) + ".pdf");

}
