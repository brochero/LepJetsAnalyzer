#include "tdrstyle.C"

#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "THStack.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TStyle.h"
#include "TSystem.h"
#include "TString.h"
#include "TDirectory.h"
#include "TROOT.h"
#include "TObject.h"
#include "TStopwatch.h"
#include "TLegend.h"
#include <vector>
#include <string>
#include <fstream>
#include <vector>
#include <sys/stat.h>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <set>

Int_t chatch = 1756;
TColor *color = new TColor(chatch, 0.3, 0.5, 0.5, "", 0.45); // alpha = 0.5

void RecoverCSVHisto (TH1D *his, TH1D *CSV1, TH1D *CSV2){

  for(int ireg=0;ireg<20;ireg++){
    TH1D *htempreg = new TH1D ("","",20,0.,1.);       
    for(int ibin=1;ibin<=20;ibin++){
      htempreg->SetBinContent(ibin,his->GetBinContent(ibin+20*ireg));	  
      htempreg->SetBinError(ibin,his->GetBinError(ibin+20*ireg));	  
    } // for(ibin)
    CSV2 -> Add(htempreg);
    double bYields, bError;
    bYields = htempreg->IntegralAndError(1,20,bError);
    CSV1 -> SetBinContent((ireg+1),bYields);
    CSV1 -> SetBinError((ireg+1),bError);
  } // for(ireg)  

}

TH1D *HistoRatio (TH1D *hisNum, TH1D *hisDen){

  //Graph Ratio Clone
  TH1D *Ratio;
  Ratio = (TH1D*)hisNum->Clone();
  Ratio->Divide(hisDen);

  for(int ibin=1;ibin<=Ratio->GetNbinsX();ibin++) {
    if (Ratio->GetBinContent(ibin) == 0.0 ) {
      Ratio->SetBinContent(ibin, 1.0);
      Ratio->SetBinError(ibin, 0.0);
    }
  }

  Ratio->SetFillColor(0);
  Ratio->SetLineColor(kGray+2);
  // Ratio->SetLineColor(kMagenta-5);
  Ratio->SetLineWidth(1);
  Ratio->SetTitle("");
  
  Ratio->GetYaxis()->SetTitle("Pre/Post(fit)");
  Ratio->GetYaxis()->CenterTitle();
  Ratio->GetYaxis()->SetTitleOffset(0.5);
  Ratio->GetYaxis()->SetTitleSize(0.10);
  Ratio->GetYaxis()->SetLabelSize(0.10);
  Ratio->GetYaxis()->SetNdivisions(402);
  Ratio->GetXaxis()->SetTitle("CSVv2"); 
  Ratio->GetXaxis()->SetNdivisions(509); //(402)
  Ratio->GetXaxis()->SetTitleOffset(1.1);
  Ratio->GetXaxis()->SetLabelSize(0.20);
  Ratio->GetXaxis()->SetTitleSize(0.16);
  
  Ratio->SetMinimum(0.4);
  Ratio->SetMaximum(1.6);

  return Ratio;  

}


void ttCatPrePostFit(TString InpDir = "FitResults_DataCardFixbtagSysVisPhSp_hSF-PreApp-v0_Tree_LepJets_NewJEC-OldKinFit_v8-0-6_Spring16-80X_36814pb-1_2btag", TString FitDir = "OBSERVED"){

  TString PlotsFileName;
  if(FitDir == "OBSERVED") PlotsFileName = "obs";
  if(FitDir == "EXPECTED") PlotsFileName = "exp";
 
  TString inputfile = "CombineResults/" + InpDir + "/" + FitDir + "/fitDiagnostics" + PlotsFileName + "MLF.root";
  
 
  setTDRStyle();

  gROOT->SetStyle("Plain");
  gStyle->SetOptFit(1000);
  gStyle->SetOptStat("emruo");
  gStyle->SetOptStat(kFALSE);
  gStyle->SetPadTickY(1);
  gStyle->SetPadTickX(1);
  
  int col_ttbb = TColor::GetColor("#660000");
  int col_ttb  = TColor::GetColor("#ffcc00");
  int col_ttcc = TColor::GetColor("#cc6600");
  int col_ttc  = TColor::GetColor("#cc6600");
  int col_ttLF = TColor::GetColor("#ff0000");
  int col_tt   = TColor::GetColor("#FF7F7F");

  int col_ttbarBkg  = TColor::GetColor("#ff6565");
  int col_SingleTop = TColor::GetColor("#ff00ff");
  int col_WJets     = TColor::GetColor("#33cc33");
  int col_ZJets     = TColor::GetColor("#3366ff");
  int col_QCD       = TColor::GetColor("#ffff00");
  int col_ttbarV    = TColor::GetColor("#e75c8d");
  int col_ttbarH    = TColor::GetColor("#e5c4f4");
  int col_VV        = TColor::GetColor("#ffffff");
 
  TFile *hfile = NULL;
  hfile = TFile::Open(inputfile);
  cout << "file loaded: " << inputfile << endl;

  std::vector<TString> hNamefile;
  std::vector<int> hColor;
  hNamefile.push_back("ttbar_LepJetsPowhegPythiattbb"); // 0
  hColor.push_back(TColor::GetColor("#660000"));
  hNamefile.push_back("ttbar_LepJetsPowhegPythiattbj"); // 1 
  hColor.push_back(TColor::GetColor("#ffcc00"));
  hNamefile.push_back("ttbar_LepJetsPowhegPythiattcc"); // 2
  hColor.push_back(TColor::GetColor("#cc6600"));
  hNamefile.push_back("ttbar_LepJetsPowhegPythiattLF"); // 3
  hColor.push_back(TColor::GetColor("#ff0000"));
  hNamefile.push_back("ttbar_PowhegPythiaBkgtt");
  hColor.push_back(TColor::GetColor("#FF7F7F"));
  hNamefile.push_back("ttHbb_PowhegPythia");
  hColor.push_back(TColor::GetColor("#e5c4f4"));
  hNamefile.push_back("ttV_Madgraph");
  hColor.push_back(TColor::GetColor("#e75c8d"));
  hNamefile.push_back("WJets_aMCatNLO");
  hColor.push_back(TColor::GetColor("#33cc33"));
  hNamefile.push_back("QCD");
  hColor.push_back(TColor::GetColor("#ffff00"));
  hNamefile.push_back("SingleTop");
  hColor.push_back(TColor::GetColor("#ff00ff"));
  hNamefile.push_back("VV");
  hColor.push_back(TColor::GetColor("#ffffff"));
  hNamefile.push_back("ZJets_aMCatNLO");
  hColor.push_back(TColor::GetColor("#3366ff"));
  hNamefile.push_back("total");
  hColor.push_back(1);
  hNamefile.push_back("total_signal");
  hColor.push_back(1);
  hNamefile.push_back("total_background");
  hColor.push_back(1);



  //TString dirname[2]     = {"Name1","Name2"};     //{mu,e}
  TString dirname[2]     = {"ch1","ch2"};     //{mu,e}
  TString titlechname[2] = {"#mu+Jets","e+Jets"}; 
  TString chname[2]      = {"mujets","ejets"};    

  std::vector<TH1D*> hInput[2][2];

  cout << "Loading histograms... " << endl;

  // MC Profiles
  for(int ich=0;ich<2;ich++){

    for(int ih=0;ih<hNamefile.size();ih++){      
      cout << "shapes/"+dirname[ich]+"/" + hNamefile.at(ih) << endl;
      TH1D *htemp1 = (TH1D*) hfile->Get("shapes_prefit/"+dirname[ich]+"/" + hNamefile.at(ih))->Clone("c" + hNamefile.at(ih));
      TH1D *htemp2 = (TH1D*) hfile->Get("shapes_fit_s/" +dirname[ich]+"/" + hNamefile.at(ih))->Clone("c" + hNamefile.at(ih));

      hInput[ich][0].push_back(htemp1);
      hInput[ich][1].push_back(htemp2);

    } // for(ih)

    
    // Recover basic CSV plots
    // -- MC
    TH1D *GlobalCSVJet[13][2][2]; // 12 components, 2 Jets, 2 pre/post fit

    for(int ih=0;ih<12;ih++){

      GlobalCSVJet[ih][0][0] = new TH1D("GlobalCSVJet1Pre_" + hNamefile.at(ih), "CSV distribution for AddJet-1", 20, 0.0, 1.0);
      GlobalCSVJet[ih][1][0] = new TH1D("GlobalCSVJet2Pre_" + hNamefile.at(ih), "CSV distribution for AddJet-2", 20, 0.0, 1.0);
      GlobalCSVJet[ih][0][1] = new TH1D("GlobalCSVJet1Post_" + hNamefile.at(ih), "CSV distribution for AddJet-1", 20, 0.0, 1.0);
      GlobalCSVJet[ih][1][1] = new TH1D("GlobalCSVJet2Post_" + hNamefile.at(ih), "CSV distribution for AddJet-2", 20, 0.0, 1.0);

      // prefit
      RecoverCSVHisto (hInput[ich][0].at(ih), GlobalCSVJet[ih][0][0], GlobalCSVJet[ih][1][0]);
      GlobalCSVJet[ih][0][0]->SetLineStyle(8);
      GlobalCSVJet[ih][1][0]->SetLineStyle(8);
      GlobalCSVJet[ih][0][0]->SetLineWidth(2);
      GlobalCSVJet[ih][1][0]->SetLineWidth(2);
      GlobalCSVJet[ih][0][0]->SetLineColor(2);
      GlobalCSVJet[ih][1][0]->SetLineColor(2);
      // postfit
      RecoverCSVHisto (hInput[ich][1].at(ih), GlobalCSVJet[ih][0][1], GlobalCSVJet[ih][1][1]);
      GlobalCSVJet[ih][0][1]->SetLineWidth(2);
      GlobalCSVJet[ih][1][1]->SetLineWidth(2);
      GlobalCSVJet[ih][0][1]->SetLineColor(kGreen-2);
      GlobalCSVJet[ih][1][1]->SetLineColor(kGreen-2);
      
      if (ih == 0){
	GlobalCSVJet[12][0][0] = (TH1D *) GlobalCSVJet[ih][0][0]->Clone("PreJet1ttjj");
	GlobalCSVJet[12][1][0] = (TH1D *) GlobalCSVJet[ih][1][0]->Clone("PreJet2ttjj");

	GlobalCSVJet[12][0][1] = (TH1D *) GlobalCSVJet[ih][0][1]->Clone("PostJet1ttjj");
	GlobalCSVJet[12][1][1] = (TH1D *) GlobalCSVJet[ih][1][1]->Clone("PostJet2ttjj");
      }
      else if (ih < 4){
	GlobalCSVJet[12][0][0] ->Add(GlobalCSVJet[ih][0][0]);
	GlobalCSVJet[12][1][0] ->Add(GlobalCSVJet[ih][1][0]);
	
	GlobalCSVJet[12][0][1] ->Add(GlobalCSVJet[ih][0][1]);
	GlobalCSVJet[12][1][1] ->Add(GlobalCSVJet[ih][1][1]);
      }
    }

    // -----------
    // Plotting
    // -----------

    
    TLatex *titlePr;      
    titlePr  = new TLatex(-20.,50.,"Preliminary");
    titlePr->SetNDC();
    titlePr->SetTextAlign(12);
    titlePr->SetX(0.25);
    titlePr->SetY(0.93);
    titlePr->SetTextColor(2);
    titlePr->SetTextFont(42);
    titlePr->SetTextSize(0.05);
    titlePr->SetTextSizePixels(24);
    titlePr->Draw("SAME");
    
    TLatex *title;
    title  = new TLatex(-20.,50.,"CMS(2016) #sqrt{s} = 13TeV, L = 35.9 fb^{-1}");
    title->SetNDC();
    title->SetTextAlign(12);
    title->SetX(0.20);
    title->SetY(0.83);
    title->SetTextFont(42);
    title->SetTextSize(0.05);
    title->SetTextSizePixels(24);
    title->Draw("SAME");
  
    TLatex *chtitle;
    chtitle  = new TLatex(-20.,50.,titlechname[ich]+" channel");
    chtitle->SetNDC();
    chtitle->SetTextAlign(12);
    chtitle->SetX(0.20);
    chtitle->SetY(0.75);
    chtitle->SetTextFont(42);
    chtitle->SetTextSize(0.05);
    chtitle->SetTextSizePixels(24);
    chtitle->Draw("SAME");

    TString dirfigname_pdf = "CombineResults/Figures_" + InpDir + "CAT" + FitDir + "/";
    // make a dir if it does not exist!!
    gSystem->mkdir(dirfigname_pdf, kTRUE);

    TCanvas *cPlotsI;
    cPlotsI = new TCanvas("cPlotsI"+dirname[ich] ,"Plots");
    cPlotsI->Divide(1,2);

    TPad    *padI[4];
    //Plot Pad
    padI[0] = (TPad*)cPlotsI->GetPad(1);
    padI[0]->SetPad(0.01, 0.23, 0.99, 0.99);
    padI[0]->SetTopMargin(0.1);
    padI[0]->SetRightMargin(0.04);
    
    //Ratio Pad
    padI[1] = (TPad*)cPlotsI->GetPad(2);
    padI[1]->SetPad(0.01, 0.02, 0.99, 0.3);
    gStyle->SetGridWidth(1);
    gStyle->SetGridColor(14);
    padI[1]->SetGridx();
    padI[1]->SetGridy();
    padI[1]->SetTopMargin(0.05);
    padI[1]->SetBottomMargin(0.4);
    padI[1]->SetRightMargin(0.04);

    TCanvas *cPlotsII;
    cPlotsII = new TCanvas("cPlotsII"+dirname[ich] ,"Plots");
    cPlotsII->Divide(1,2);
    //Plot Pad
    padI[2] = (TPad*)cPlotsII->GetPad(1);
    padI[2]->SetPad(0.01, 0.23, 0.99, 0.99);
    padI[2]->SetTopMargin(0.1);
    padI[2]->SetRightMargin(0.04);
    
    //Ratio Pad
    padI[3] = (TPad*)cPlotsII->GetPad(2);
    padI[3]->SetPad(0.01, 0.02, 0.99, 0.3);
    gStyle->SetGridWidth(1);
    gStyle->SetGridColor(14);
    padI[3]->SetGridx();
    padI[3]->SetGridy();
    padI[3]->SetTopMargin(0.05);
    padI[3]->SetBottomMargin(0.4);
    padI[3]->SetRightMargin(0.04);

    int icat = 0;  // ttbb   
    //icat = 12; // ttjj   

    TH1D *hstyleI = new TH1D ("hstyleI","",
			      GlobalCSVJet[icat][0][0]->GetNbinsX(),
			      GlobalCSVJet[icat][0][0]->GetBinLowEdge (1),
			      GlobalCSVJet[icat][0][0]->GetBinLowEdge (GlobalCSVJet[0][0][1]->GetNbinsX()+1));
    
    hstyleI -> GetYaxis()->SetTitleOffset(1.);
    hstyleI -> GetYaxis()->SetTitleSize(0.05);
    hstyleI -> GetYaxis()->SetLabelSize(0.05);
    hstyleI -> GetYaxis()->SetTitle("Events"); 
    
    padI[0]->cd();
    //padI[0]->cd()->SetLogy();
    
    hstyleI -> SetMaximum(1.5*GlobalCSVJet[icat][0][0]->GetMaximum());
    hstyleI -> SetMinimum(0.);
    hstyleI -> Draw();

    GlobalCSVJet[icat][0][0] -> Draw("SAMEHIST");
    GlobalCSVJet[icat][0][1] -> Draw("SAMEHIST");

    titlePr->Draw("SAME");
    title  ->Draw("SAME");
    chtitle->Draw("SAME");	

    TH1D *RatioFullJet1 = HistoRatio (GlobalCSVJet[icat][0][0], GlobalCSVJet[icat][0][1]);

    TLegend *legI;
    legI = new TLegend(0.70,0.64,0.88,0.87);
    legI->SetFillColor(0);
    legI->SetLineColor(0);
    legI->SetLineWidth(0.0);
    legI->SetTextFont(62);
    legI->SetTextSize(0.05);
  
    if (icat == 0 ) legI->AddEntry((TObject*)0,"t#bar{t}+bb","");
    else legI->AddEntry((TObject*)0,"t#bar{t}+jj","");
    legI->AddEntry(GlobalCSVJet[icat][0][0],  "Prefit","L");
    legI->AddEntry(GlobalCSVJet[icat][0][1],  "Postfit","L");

    legI->Draw("SAME");

    padI[1]->cd();
    RatioFullJet1 ->GetYaxis()->SetTitleOffset(0.25);
    RatioFullJet1 ->SetLineStyle(1);
    RatioFullJet1 ->Draw("HIST");

    padI[2]->cd();
    //padI[2]->cd()->SetLogy();
    
    TH1D *hstyleII = (TH1D *)hstyleI -> Clone("StyleII"); 
    hstyleII -> SetMaximum(1.5*GlobalCSVJet[icat][1][0]->GetMaximum());
    hstyleII -> SetMinimum(0.);

    hstyleII -> Draw();
    GlobalCSVJet[icat][1][0] -> Draw("HISTSAME");
    GlobalCSVJet[icat][1][1] -> Draw("HISTSAME");

    titlePr->Draw("SAME");
    title  ->Draw("SAME");
    chtitle->Draw("SAME");	

    legI->Draw("SAME");

    TH1D *RatioFullJet2 = HistoRatio (GlobalCSVJet[icat][1][0] , GlobalCSVJet[icat][1][1]);

    padI[3]->cd();
    RatioFullJet2 ->GetYaxis()->SetTitleOffset(0.25);
    RatioFullJet2 ->SetLineStyle(1);
    RatioFullJet2 ->Draw("HIST");

    if(icat == 0){
      cPlotsI ->SaveAs(dirfigname_pdf + "CSVHistosJet1_ttbb_" + dirname[ich] + "_Log.pdf");
      cPlotsII->SaveAs(dirfigname_pdf + "CSVHistosJet2_ttbb_" + dirname[ich] + "_Log.pdf");
    }
    else{
      cPlotsI ->SaveAs(dirfigname_pdf + "CSVHistosJet1_ttjj_" + dirname[ich] + "_Log.pdf");
      cPlotsII->SaveAs(dirfigname_pdf + "CSVHistosJet2_ttjj_" + dirname[ich] + "_Log.pdf");
    }
  }// for(ich)
  

}
