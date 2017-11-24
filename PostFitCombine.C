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
  
  Ratio->GetYaxis()->SetTitle("Obs/Exp");
  Ratio->GetYaxis()->CenterTitle();
  Ratio->GetYaxis()->SetTitleOffset(0.15);
  Ratio->GetYaxis()->SetTitleSize(0.16);
  Ratio->GetYaxis()->SetLabelSize(0.15);
  Ratio->GetYaxis()->SetNdivisions(402);
  Ratio->GetXaxis()->SetTitle("CSV bin"); 
  Ratio->GetXaxis()->SetNdivisions(509); //(402)
  Ratio->GetXaxis()->SetTitleOffset(1.1);
  Ratio->GetXaxis()->SetLabelSize(0.20);
  Ratio->GetXaxis()->SetTitleSize(0.16);
  
  Ratio->SetMinimum(0.4);
  Ratio->SetMaximum(1.6);

  return Ratio;  

}


void PostFitCombine(TString Plots = "fit_s", TString InpDir = "FitResults_DataCard_FinalAN-v1VisPhSp_hSF-FinalAN-v1_Tree_LepJets_Summer_v8-0-6_Spring16-80X_36814pb-1_2btag"){

  TString inputfile = "CombineResults/StableResult-v1/" + InpDir + "/OBSERVED/mlfitobsMLF.root";
  
 
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
  hNamefile.push_back("ttbar_LepJetsPowhegPythiattbb");
  hColor.push_back(TColor::GetColor("#660000"));
  hNamefile.push_back("ttbar_LepJetsPowhegPythiattbj");
  hColor.push_back(TColor::GetColor("#ffcc00"));
  hNamefile.push_back("ttbar_LepJetsPowhegPythiattcc");
  hColor.push_back(TColor::GetColor("#cc6600"));
  hNamefile.push_back("ttbar_LepJetsPowhegPythiattLF");
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


  TString dirname[2]     = {"Name1","Name2"};     //{mu,e}
  TString titlechname[2] = {"#mu+Jets","e+Jets"}; 
  TString chname[2]      = {"mujets","ejets"};    

  std::vector<TH1D*> hInput[2];
  TH1D    *hData[2], *hData_reg[2][20];
  THStack *AllMC[2], *AllMC_reg[2][20];

  // THStack Initialization
  for(int ich=0;ich<2;ich++){
    AllMC[ich] = new THStack("PostFit_"+chname[ich], "CSV Distribution Post-Fit ("+titlechname[ich]+")");  
    for(int ireg=0;ireg<20;ireg++){
      TString RegNum;
      RegNum.Form("%i",ireg); 
      AllMC_reg[ich][ireg] = new THStack("PostFit_"+chname[ich]+RegNum, "CSV Distribution Post-Fit ("+titlechname[ich]+") for "+RegNum);        
    } // for(ireg)
  } // for(ich)
 
  cout << "Loading histograms... " << endl;

  // MC Profiles
  for(int ich=0;ich<2;ich++){

    for(int ih=0;ih<hNamefile.size();ih++){      
      TH1D *htemp = (TH1D*) hfile->Get("shapes_"+Plots+"/"+dirname[ich]+"/" + hNamefile.at(ih))->Clone("c" + hNamefile.at(ih));
      htemp->SetFillColor(hColor.at(ih));
      htemp->SetLineColor(1);
      hInput[ich].push_back(htemp);
      if(ih==0) AllMC[ich]->SetHistogram((TH1D*)htemp->Clone("FirstStack"));
      if (!hNamefile.at(ih).Contains("total")) AllMC[ich] -> Add(htemp);
      // Data
      // Clone Histo Structure only once
      if(ih==0){
	hData[ich] = (TH1D *) htemp->Clone("data_"+dirname[ich]);
	hData[ich]->Reset();
      }
    } // for(ih)

    // Data TGraph
    TGraphAsymmErrors *DataFull = (TGraphAsymmErrors *) hfile->Get("shapes_"+Plots+"/"+dirname[ich]+"/data")->Clone("data_"+dirname[ich]);
    for(int ibin=1;ibin<=hData[ich]->GetNbinsX();ibin++){ 
      double igb, EvtBinData, EvtErrBinData;
      DataFull->GetPoint((ibin-1),igb,EvtBinData); 
      hData[ich]->SetBinContent(ibin,EvtBinData);
      EvtErrBinData = DataFull->GetErrorY((ibin-1));
      hData[ich]->SetBinError(ibin,EvtErrBinData);
    }
    
    // Histograms for each region
    for(int ih=0;ih<hNamefile.size();ih++){

      TH1D *htemp = (TH1D*) hfile->Get("shapes_"+Plots+"/"+dirname[ich]+"/" + hNamefile.at(ih))->Clone("c_reg" + hNamefile.at(ih));
      
      for(int ireg=0;ireg<20;ireg++){
	TH1D *htempreg = new TH1D ("","",20,0,20);       
    	for(int ibin=1;ibin<=20;ibin++){
    	  htempreg->SetBinContent(ibin,htemp->GetBinContent(ibin+20*ireg));	  
	  htempreg->SetFillColor(hColor.at(ih));
    	} // for(ibin)
	if(ih==0) AllMC_reg[ich][ireg]->SetHistogram((TH1D*)htempreg->Clone("FirstStack"));
	if (!hNamefile.at(ih).Contains("total")) AllMC_reg[ich][ireg] -> Add(htempreg);
      } // for(ireg)
    } // for(ih)

    // Data
    for(int ireg=0;ireg<20;ireg++){
      TString RegNum;
      RegNum.Form("%i",ireg);
      hData_reg[ich][ireg] = new TH1D ("hData_"+chname[ich]+RegNum,"Data Histogram "+titlechname[ich]+" for "+RegNum,20,0,20);       

      // Data
      for(int ibin=1;ibin<=20;ibin++){ 
	double igb, EvtBinData, EvtErrBinData;
	DataFull->GetPoint(((ibin-1)+(20*ireg)),igb,EvtBinData); 
	hData_reg[ich][ireg]->SetBinContent(ibin,EvtBinData);	  
	EvtErrBinData = DataFull->GetErrorY((ibin-1));
	hData_reg[ich][ireg]->SetBinError(ibin,EvtErrBinData);

	hData_reg[ich][ireg]->SetMarkerStyle(20);
	hData_reg[ich][ireg]->SetMarkerSize(0.5);
      } // for(ibin)
    } // for(ireg)


    // -----------
    // Plotting
    // -----------

    TH1D *hstyle = new TH1D ("","",
			     hData[ich]->GetNbinsX(),
			     hData[ich]->GetBinLowEdge (1),
			     hData[ich]->GetBinLowEdge (hData[ich]->GetNbinsX()+1));
    
    hstyle -> SetMaximum(1.1*hInput[ich].at(12)->GetMaximum());
    hstyle -> GetYaxis()->SetTitleOffset(0.6);
    hstyle -> GetYaxis()->SetTitleSize(0.05);
    hstyle -> GetYaxis()->SetLabelSize(0.05);
    hstyle -> GetYaxis()->SetTitle("Events"); 
    
    
    hData[ich] -> SetMarkerStyle(20); 
    hData[ich] -> SetMarkerSize(0.4); 
    hData[ich] -> SetLineWidth(1); 
    hData[ich] -> SetTitle(""); 
    
    
    TCanvas *cPlots;//histos
    cPlots = new TCanvas("cPlots"+dirname[ich] ,"Plots");
    cPlots->Divide(1,2);
    TPad    *pad[4], *glpad[2];
    // Global Pad
    glpad[0] = (TPad*)cPlots->GetPad(1);
    glpad[0]->Divide(1,2);
    glpad[1] = (TPad*)cPlots->GetPad(2);
    glpad[1]->Divide(1,2);

    //Plot Pad
    pad[0] = (TPad*)glpad[0]->GetPad(1);
    pad[0]->SetPad(0.01, 0.23, 0.99, 0.99);
    pad[0]->SetTopMargin(0.1);
    pad[0]->SetRightMargin(0.04);
    
    //Ratio Pad
    pad[1] = (TPad*)glpad[0]->GetPad(2);
    pad[1]->SetPad(0.01, 0.02, 0.99, 0.3);
    gStyle->SetGridWidth(1);
    gStyle->SetGridColor(14);
    pad[1]->SetGridx();
    pad[1]->SetGridy();
    pad[1]->SetTopMargin(0.05);
    pad[1]->SetBottomMargin(0.4);
    pad[1]->SetRightMargin(0.04);

    //Plot Pad
    pad[2] = (TPad*)glpad[1]->GetPad(1);
    pad[2]->SetPad(0.01, 0.23, 0.99, 0.99);
    pad[2]->SetTopMargin(0.1);
    pad[2]->SetRightMargin(0.04);
    
    //Ratio Pad
    pad[3] = (TPad*)glpad[1]->GetPad(2);
    pad[3]->SetPad(0.01, 0.02, 0.99, 0.3);
    gStyle->SetGridWidth(1);
    gStyle->SetGridColor(14);
    pad[3]->SetGridx();
    pad[3]->SetGridy();
    pad[3]->SetTopMargin(0.05);
    pad[3]->SetBottomMargin(0.4);
    pad[3]->SetRightMargin(0.04);
    
    pad[0]->cd();

    hstyle->Draw();
    AllMC[ich] -> Draw("HISTSAME"); 
    hData[ich] -> Draw("PSAME"); 
    
    TH1D *RatioFull = HistoRatio (hData[ich] , (TH1D*) AllMC[ich]->GetStack()->Last());
    TGraphErrors *gRatioFull = new TGraphErrors(RatioFull);
    gRatioFull->SetFillStyle(1001);
    gRatioFull->SetFillColor(chatch);
    gRatioFull->SetName("gRatioFull");


    TLegend *leg;
    float legPos[4] = {0.70,  // x_o
		       0.40,  // y_o
		       0.94,  // x_f
		       0.87}; // y_f
    
    leg = new TLegend(legPos[0],legPos[1],legPos[2],legPos[3]);
    leg->SetFillColor(0);
    leg->SetLineColor(0);
    leg->SetLineWidth(0.0);
    leg->SetTextFont(62);
    leg->SetTextSize(0.03);
    leg->SetNColumns(2);
  
    leg->AddEntry(hData[ich],         "Data","PL");
    leg->AddEntry(hInput[ich].at(11), "Z+Jets","F");
    leg->AddEntry(hInput[ich].at(10), "VV","F");
    leg->AddEntry(hInput[ich].at(9),  "Single t","F");
    leg->AddEntry(hInput[ich].at(8),  "QCD","F");
    leg->AddEntry(hInput[ich].at(7),  "W+Jets","F");
    leg->AddEntry(hInput[ich].at(6),  "t#bar{t}+V","F");
    leg->AddEntry(hInput[ich].at(5),  "t#bar{t}+H","F");
    leg->AddEntry(hInput[ich].at(4),  "t#bar{t}+other","F");
    leg->AddEntry(hInput[ich].at(3),  "t#bar{t}+LF","F");
    leg->AddEntry(hInput[ich].at(2),  "t#bar{t}+cc","F");
    leg->AddEntry(hInput[ich].at(1),  "t#bar{t}+bj","F");
    leg->AddEntry(hInput[ich].at(0),  "t#bar{t}+bb","F");
    leg->AddEntry(gRatioFull,         "Stat. Unc.","F");

    leg->Draw("SAME");
    
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
    title  = new TLatex(-20.,50.,"CMS(2016) #sqrt{s} = 13TeV, L = 36.8 fb^{-1}");
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

    pad[2]->cd();
    pad[2]->cd()->SetLogy();
    
    TH1D *hstyleLog = (TH1D *)hstyle->Clone();
    hstyleLog -> SetMaximum(5.0*hInput[ich].at(12)->GetMaximum());
    hstyleLog -> SetMinimum(1);
    hstyleLog -> Draw();
    
    AllMC[ich] -> Draw("HISTSAME"); 
    hData[ich] -> Draw("PSAME"); 
    titlePr->Draw("SAME");
    title->Draw("SAME");
    chtitle->Draw("SAME");

    pad[1]->cd();
    RatioFull ->Draw("HIST");
    gRatioFull->Draw("e2");
    RatioFull ->Draw("HISTSAME");

    pad[3]->cd();
    RatioFull ->Draw("HIST");
    gRatioFull->Draw("e2");
    RatioFull ->Draw("HISTSAME");

    TString dirfigname_pdf = "CombineResults/Figures_" + InpDir + Plots + "/";
    // make a dir if it does not exist!!
    gSystem->mkdir(dirfigname_pdf, kTRUE);

    cPlots->SaveAs(dirfigname_pdf + "FullHisto_" + dirname[ich] + "_NormLog.pdf");
    
    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------
    // Plots by Regions
    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------
    // -----------------------------------------------------------------------------
    int ireg = 0;
    for(int ican=0;ican<5;ican++){

      TString CanNum;
      CanNum.Form("%i",ican); 
      TCanvas *cPlots_reg;//histos
      cPlots_reg = new TCanvas("cPlots_reg" + CanNum + dirname[ich] ,"Plots By regions");
      cPlots_reg->Divide(2,2);
      
      for(int icr=1;icr<=4;icr++){

        //cPlots_reg->cd(icr);

	TPad *glpad_reg = (TPad*)cPlots_reg->cd(icr);
	glpad_reg->Divide(2,1);
	TPad *pad_reg[2];
	//Plot Pad
	pad_reg[0] = (TPad*)glpad_reg->GetPad(1);
	pad_reg[0]->SetPad(0.01, 0.23, 0.99, 0.99);
	pad_reg[0]->SetTopMargin(0.1);
	pad_reg[0]->SetRightMargin(0.04);	
	//Ratio Pad
	pad_reg[1] = (TPad*)glpad_reg->GetPad(2);
	pad_reg[1]->SetPad(0.01, 0.02, 0.99, 0.3);
	gStyle->SetGridWidth(1);
	gStyle->SetGridColor(14);
	pad_reg[1]->SetGridx();
	pad_reg[1]->SetGridy();
	pad_reg[1]->SetTopMargin(0.05);
	pad_reg[1]->SetBottomMargin(0.4);
	pad_reg[1]->SetRightMargin(0.04);

	pad_reg[0]->cd();
	pad_reg[0]->SetLogy();
	TH1D *hstyle_reg = (TH1D*)AllMC_reg[ich][ireg]->GetHistogram(); 
	hstyle_reg -> Reset();
	hstyle_reg -> SetMaximum(5.0*hData_reg[ich][ireg]->GetMaximum());
	hstyle_reg -> GetYaxis()->SetTitleOffset(0.8);
	hstyle_reg -> GetYaxis()->SetTitleSize(0.05);
	hstyle_reg -> GetYaxis()->SetLabelSize(0.05);
	hstyle_reg -> GetYaxis()->SetTitle("Events"); 

	pad_reg[0]->cd();
	hstyle_reg->Draw();
	AllMC_reg[ich][ireg] -> Draw("HISTSAME"); 
	hData_reg[ich][ireg] -> Draw("E1SAME"); 
	titlePr->Draw("SAME");
	title->Draw("SAME");
	chtitle->Draw("SAME");	

	TH1D *RatioFull_reg = HistoRatio (hData_reg[ich][ireg] , (TH1D*) AllMC_reg[ich][ireg]->GetStack()->Last());
	TGraphErrors *gRatioFull_reg = new TGraphErrors(RatioFull_reg);
	gRatioFull_reg->SetFillStyle(1001);
	gRatioFull_reg->SetFillColor(chatch);
	gRatioFull_reg->SetName("gRatioFull");
	
	pad_reg[1]->cd();
	RatioFull_reg->GetYaxis()->SetTitleOffset(0.25);	
	RatioFull_reg->Draw("HIST");
	gRatioFull_reg->Draw("e2");
	RatioFull_reg->Draw("HISTSAME");
	
	ireg++;
      } // for(ireg) 

      cPlots_reg->SaveAs(dirfigname_pdf + "RegionHisto_" + CanNum + "_" + dirname[ich] + "_NormLog.pdf");
    } // for(ican) 

  }// for(ich)
  

}
