void SimpleChi2CatPlots(TString Cut="2btag", TString Channel="mujets"){
  gROOT->SetStyle("Plain");
  gStyle->SetOptStat(kFALSE);

  //TString bname = "hSF-NuiPar10M-v0_Tree_LepJets_PileUp17_v8-0-4_Spring16-80X_36814pb-1_ttbar_LepJetsPowhegPythia" ; 
  TString bname = "hSF-SFHisto-v0_Tree_LepJets_TrMC_v8-0-4_Spring16-80X_36814pb-1_ttbar_LepJetsPowhegPythia" ; 
  //TString bname = "Addhisto_Tree_LepJets_GenCone" ; 

  const int nh = 3;
  //TString namefile = "TopResults/hSF-0to3CSVKinFitter_v8-0-0_Spring16-80X_5913pb-1_ttbar_PowhegPythia";
  TString namefile[6] = 
    {"../TopResults/" + bname,
     "../TopResults/" + bname + "ttbb", 
     "../TopResults/" + bname + "ttbj", 
     "../TopResults/" + bname + "ttcc",
     "../TopResults/" + bname + "ttLF",
     "../TopResults/" + bname + "tt"};

  TFile *hFile[6]={NULL,NULL, NULL, NULL, NULL, NULL};
  TH1D *histo[6];
  for(int fi = 0; fi < 6; fi++){
    hFile[fi] = TFile::Open(TString(namefile[fi] + ".root"));
    
    if(!hFile[fi]){
      std::cerr << "ERROR: Could not open " <<  namefile  << " files!!!"  << std::endl;
      std::exit(0);
    }
    
    TString histoname = "hKinChi2";
    
    cout << "Loading: " << Cut + "/" + Channel + "/" + histoname + "_" + Channel + "_" + Cut << endl;
    
    histo[fi] = (TH1D*)hFile[fi]->Get(Cut + "/" + Channel + "/" + histoname + "_" + Channel + "_" + Cut ) ->Clone();
    histo[fi]->Rebin(4);

    histo[fi]->Scale(1./histo[fi]->Integral());
  }


  TCanvas *histocanvas;
  histocanvas = new TCanvas("plots", "Plots");

  histocanvas->SetGrid();
  histocanvas->cd();
  //histocanvas->cd()->SetLogy();

  int col_ttbb = TColor::GetColor("#660000");
  int col_ttb  = TColor::GetColor("#ffcc00");
  int col_ttcc = TColor::GetColor("#cc6600");
  int col_ttc  = TColor::GetColor("#cc6600");
  int col_ttLF = TColor::GetColor("#ff0000");
  int col_tt   = TColor::GetColor("#FF7F7F");

  //histo[0]->SetTitle("Efficiency");
  //histo[0]->SetTitle("Purity");
  //histo[0]->SetTitle("Kin. Fitter Purity");
  histo[4]->SetTitle("#chi^{2} Distribution");
  histo[4]->SetMarkerStyle(20);
  histo[4]->SetMarkerColor(1);
  histo[4]->SetLineColor(1);
  //histo[0]->Draw();

  histo[1]->SetMarkerStyle(21);
  histo[1]->SetMarkerColor(col_ttbb);
  histo[1]->SetLineColor(col_ttbb);

  histo[2]->SetMarkerStyle(21);
  histo[2]->SetMarkerColor(col_ttb);
  histo[2]->SetLineColor(col_ttb);

  histo[3]->SetMarkerStyle(21);
  histo[3]->SetMarkerColor(col_ttcc);
  histo[3]->SetLineColor(col_ttcc);

  histo[4]->SetMarkerStyle(21);
  histo[4]->SetMarkerColor(col_ttLF);
  histo[4]->SetLineColor(col_ttLF);

  histo[5]->SetMarkerStyle(21);
  histo[5]->SetMarkerColor(col_tt);
  histo[5]->SetLineColor(col_tt);

  histo[4]->Draw("HIST");
  histo[1]->Draw("SAMEHIST");
  histo[2]->Draw("SAMEHIST");
  histo[3]->Draw("SAMEHIST");
  histo[5]->Draw("SAMEHIST");
  //histo[5]->Draw("SAME");

  gPad->Update();
  // histo[0]->GetPaintedGraph()->SetMaximum(0.9);
  // histo[0]->GetPaintedGraph()->SetMinimum(0.);

  TLegend *leg;
  leg = new TLegend(0.78, 0.8,
		    0.93, 0.99);
  //leg->AddEntry(histo[0],"All t#bar{t}","LP");
  leg->AddEntry(histo[1],"t#bar{t}bb","LP");
  leg->AddEntry(histo[2],"t#bar{t}bj","LP");
  leg->AddEntry(histo[3],"t#bar{t}cc","LP");
  leg->AddEntry(histo[4],"t#bar{t}LF","LP");
  leg->AddEntry(histo[5],"t#bar{t}","LP");
  leg->Draw("SAME");


  //histocanvas->SaveAs("TopResults/FiguresAlone_PurEff/PurEff_0to3CSVKinFitter_" + TString(histo->GetName()) + ".pdf");
  histocanvas->SaveAs("../TopResults/FiguresAlone_PurEff/Chi2Com_" + bname + "_" + TString(histo[0]->GetName()) + ".pdf");

}
