void SimpleWPlots(TString Cut="6Jets", TString Channel="mujets"){
  gROOT->SetStyle("Plain");
  gStyle->SetOptStat(kFALSE);

  const int nh = 3;
  TString ttcat = "ttbb";
  //TString namefile = "TopResults/hSF-Addhisto_Tree_LepJets_GenCone_v8-0-0_Spring16-80X_5913pb-1_ttbar_PowhegPythia" + ttcat;
  TString namefile = "TopResults/hSF-Addhisto_Tree_LepJets_CSVKinFitter_v8-0-0_Spring16-80X_5913pb-1_ttbar_PowhegPythia" + ttcat;

  TFile *hFile=NULL;
  hFile = TFile::Open(namefile + ".root");

  if(!hFile){
    std::cerr << "ERROR: Could not open " <<  namefile  << " files!!!"  << std::endl;
    std::exit(0);
  }

  TH1F *histo[nh];
  // TEfficiency *histo[nh];
  //TString histoname[nh] = {"hKinTagWMass", "hGenTagWMass", "hMassJet_Jet45"};
  //TString histoname[nh] = {"hKinTagAddMass", "hGenTagAddMass", "hMassJet_Jet23"};
  TString histoname[nh] = {"hKinTagAddDR", "hGenTagAddDR", "hDRJet_Jet23"};

  cout << "Loading: " << Cut + "/" + Channel + "/" + histoname[0] + "_" + Channel + "_" + Cut << endl;

  histo[0] = (TH1F*)hFile->Get(Cut + "/" + Channel + "/" + histoname[0] + "_" + Channel + "_" + Cut );
  histo[1] = (TH1F*)hFile->Get(Cut + "/" + Channel + "/" + histoname[1] + "_" + Channel + "_" + Cut );
  histo[2] = (TH1F*)hFile->Get(Cut + "/" + Channel + "/" + histoname[2] + "_" + Channel + "_" + Cut );
  //histo[0] = (TEfficiency*)hFile->Get(Cut + "/" + Channel + "/" + histoname[0] + "_" + Channel + "_" + Cut ) ->Clone(histoname[0] + Channel + Cut);

  TCanvas *histocanvas;
  histocanvas = new TCanvas("plots", "Plots");

  histocanvas->SetGrid();
  histocanvas->cd();

  //histo[0]->Rebin(10);
  //histo[1]->Rebin(10);
  //histo[2]->Rebin(10);

  for (int ih=0; ih<3; ih++){
    float Nhisto = histo[ih]->Integral();
    if(Nhisto!=0.0) histo[ih]->Scale(1.0/Nhisto);
  }



  //histo[0]->SetTitle("W hadronic Mass");
  histo[0]->SetTitle("Add Dijet #Delta R");
  histo[0]->SetMaximum(0.1);
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
  leg->AddEntry(histo[1],"GEN",   "LP");
  leg->AddEntry(histo[0],"Kinematic Fit",     "LP");
  leg->AddEntry(histo[2],"CSV Order (3,4)", "LP");
  //leg->AddEntry(histo[2],"CSV Order (5,6)", "LP");
  leg->Draw("SAME");

  if (ttcat != "") ttcat = "_" + ttcat;
  histocanvas->SaveAs("TopResults/FiguresAlone_PurEff/PurEff" + ttcat + "_CSVKinFitter_" + TString(histo[0]->GetName()) + ".pdf");
  //histocanvas->SaveAs("TopResults/FiguresAlone_PurEff/PurEff" + ttcat + "_GenCone_" + TString(histo[0]->GetName()) + ".pdf");

}
