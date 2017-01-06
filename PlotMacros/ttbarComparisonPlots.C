void ttbarComparisonPlots(TString Cut="6Jets", TString Channel="mujets"){
  gROOT->SetStyle("Plain");
  gStyle->SetOptStat(kFALSE);

  TString bname   = "v0_Tree_LepJets_KFCSVOrder01" ; 
  TString catname = "" ; 

  //TString bname = "Addhisto_Tree_LepJets_GenCone" ; 

  TString namefile[8] = 
    {"TopResults/hSF-" + bname + "_v8-0-1_Spring16-80X_15920pb-1_ttbar_PowhegPythia" + catname,
     "TopResults/hSF-" + bname + "_v8-0-1_Spring16-80X_15920pb-1_ttbar_PowhegPythia" + catname + "AlphaS",
     "TopResults/hSF-" + bname + "_v8-0-1_Spring16-80X_15920pb-1_ttbar_LepJetsPowhegPythia"  + catname, 
     "TopResults/hSF-" + bname + "_v8-0-1_Spring16-80X_15920pb-1_ttbar_LepJetsPowhegPythia" + catname + "AlphaS", 
     "TopResults/hSF-" + bname + "_v8-0-1_Spring16-80X_15920pb-1_ttbar_aMCatNLOPythia",
     "TopResults/hSF-" + bname + "_v8-0-1_Spring16-80X_15920pb-1_ttbar_MadgraphPythia",
     "TopResults/hSF-" + bname + "_v8-0-1_Spring16-80X_15920pb-1_ttbb_aMCatNLOPythia",
     "TopResults/hSF-" + bname + "_v8-0-1_Spring16-80X_15920pb-1_DataSingleMu"};
  
  TFile *hFile[8]={NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
  TH1F *histo[8];
  for(int fi = 0; fi < 8; fi++){
    hFile[fi] = TFile::Open(TString(namefile[fi] + ".root"));
    
    if(!hFile[fi]){
      std::cerr << "ERROR: Could not open " <<  namefile  << " files!!!"  << std::endl;
      std::exit(0);
    }
    
    TString histoname = "hNJets";
    
    cout << "Loading: " << Cut + "/" + Channel + "/" + histoname + "_" + Channel + "_" + Cut << endl;
    
    histo[fi] = (TH1F*)hFile[fi]->Get(Cut + "/" + Channel + "/" + histoname + "_" + Channel + "_" + Cut ) ->Clone();
    
  }


  TCanvas *histocanvas;
  histocanvas = new TCanvas("plots", "Plots");

  histocanvas->SetGrid();
  histocanvas->cd();
  
  histo[7]->SetTitle("t#bar{t} Comparison");
  histo[7]->SetMarkerStyle(20);
  histo[7]->SetMarkerColor(1);
  histo[7]->SetLineColor(1);
  //histo[0]->->SetMaximum(0.);
  //histo[0]->->SetMinimum(0.);
  histo[7]->Draw();

  //histo[0]->SetTitle("Efficiency");
  //histo[0]->SetTitle("Purity");
  //histo[0]->SetTitle("Kin. Fitter Purity");
  histo[0]->SetTitle("t#bar{t} Comparison");
  histo[0]->SetMarkerStyle(20);
  histo[0]->SetMarkerColor(1);
  histo[0]->SetLineColor(1);
  //histo[0]->->SetMaximum(0.);
  //histo[0]->->SetMinimum(0.);
  histo[0]->Draw("SAMEHIST");


  histo[1]->SetMarkerStyle(21);
  histo[1]->SetMarkerColor(2);
  histo[1]->SetLineColor(2);
  histo[1]->Draw("SAMEHIST");

  histo[2]->SetMarkerStyle(22);
  histo[2]->SetMarkerColor(3);
  histo[2]->SetLineColor(3);
  histo[2]->Draw("SAMEHIST");

  histo[3]->SetMarkerStyle(23);
  histo[3]->SetMarkerColor(4);
  histo[3]->SetLineColor(4);
  histo[3]->Draw("SAMEHIST");

  histo[4]->SetMarkerStyle(29);
  histo[4]->SetMarkerColor(5);
  histo[4]->SetLineColor(5);
  histo[4]->Draw("SAMEHIST");

  histo[5]->SetMarkerStyle(33);
  histo[5]->SetMarkerColor(6);
  histo[5]->SetLineColor(6);
  histo[5]->Draw("SAMEHIST");

  histo[6]->SetMarkerStyle(34);
  histo[6]->SetMarkerColor(7);
  histo[6]->SetLineColor(7);
  histo[6]->Draw("SAMEHIST");


  TLegend *leg;
  leg = new TLegend(0.78, 0.8,
		    0.93, 0.99);
  leg->AddEntry((TObject*)0,"t#bar{t} Samples","");
  leg->AddEntry(histo[0],"Powheg Inclusive","L");
  leg->AddEntry(histo[1],"Powheg Inclusive #alpha_{s}","L");
  leg->AddEntry(histo[2],"Powheg LepJets","L");
  leg->AddEntry(histo[3],"Powheg LepJets #alpha_{s}","L");
  leg->AddEntry(histo[4],"aMC@NLO","L");
  leg->AddEntry(histo[5],"MadGraph","L");
  leg->AddEntry(histo[6],"t#bar{t}b#bar{b} aMC@NLO","L");
  leg->AddEntry(histo[7],"Data","LP");
  leg->Draw("SAME");


  //histocanvas->SaveAs("TopResults/FiguresAlone_PurEff/PurEff_0to3CSVKinFitter_" + TString(histo->GetName()) + ".pdf");
  histocanvas->SaveAs("TopResults/FiguresAlone_ttbarComp/ttbarSamples_" + bname + "_" + TString(histo[0]->GetName()) + ".pdf");

}
