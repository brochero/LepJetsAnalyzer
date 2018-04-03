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


void PostFitCombine(TString Plots = "fit_s", TString InpDir = "FitResults_DataCardFullThVisPhSp_hSF-Full-v0_Tree_LepJets_EGTightSkim_v8-0-6_Spring16-80X_36814pb-1_2btag", TString FitDir = "OBSERVED"){

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

  std::vector<TH1D*> hInput[2];
  TH1D    *hData[2], *hData_reg[2][20];
  THStack *AllMC[2], *AllMC_reg[2][20];
  
  THStack *AllMC_CSV1[2], *AllMC_CSV2[2];

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
      cout << "shapes_"+Plots+"/"+dirname[ich]+"/" + hNamefile.at(ih) << endl;
      TH1D *htemp = (TH1D*) hfile->Get("shapes_"+Plots+"/"+dirname[ich]+"/" + hNamefile.at(ih))->Clone("c" + hNamefile.at(ih));
      htemp->SetFillColor(hColor.at(ih));
      htemp->SetLineColor(1);
      hInput[ich].push_back(htemp);
      cout << hNamefile.at(ih) << " = " << htemp->Integral()  << endl;
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
	  //if ( (ibin+20*ireg) == 181) cout << hNamefile.at(ih) << " " << ibin+20*ireg << " : " << htemp->GetBinContent(ibin+20*ireg) << " ; " << htemp->GetBinError(ibin+20*ireg) << endl;
    	  htempreg->SetBinContent(ibin,htemp->GetBinContent(ibin+20*ireg));	  
    	  htempreg->SetBinError(ibin,htemp->GetBinError(ibin+20*ireg));	  
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


    // Recover basic CSV plots
    // -- MC
    TH1D *GlobalCSVJet[12][2]; // 12 components
    AllMC_CSV1[ich] = new THStack("CSV1_"+chname[ich], "CSV-AddJet1 Distribution Post-Fit ("+titlechname[ich]+")");
    AllMC_CSV2[ich] = new THStack("CSV2_"+chname[ich], "CSV-AddJet2 Distribution Post-Fit ("+titlechname[ich]+")");

    for(int ih=0;ih<12;ih++){
      GlobalCSVJet[ih][0] = new TH1D("GlobalCSVJet1_" + hNamefile.at(ih), "CSV distribution for AddJet-1", 20, 0.0, 1.0);
      GlobalCSVJet[ih][1] = new TH1D("GlobalCSVJet2_" + hNamefile.at(ih), "CSV distribution for AddJet-2", 20, 0.0, 1.0);
      RecoverCSVHisto (hInput[ich].at(ih), GlobalCSVJet[ih][0], GlobalCSVJet[ih][1]);
      GlobalCSVJet[ih][0]->SetFillColor(hColor.at(ih));
      GlobalCSVJet[ih][1]->SetFillColor(hColor.at(ih));
      
      if(ih==0) AllMC_CSV1[ich]->SetHistogram((TH1D*)GlobalCSVJet[ih][0]->Clone("FirstStack"));
      if(ih==0) AllMC_CSV2[ich]->SetHistogram((TH1D*)GlobalCSVJet[ih][1]->Clone("FirstStack"));
      AllMC_CSV1[ich] -> Add(GlobalCSVJet[ih][0]);
      AllMC_CSV2[ich] -> Add(GlobalCSVJet[ih][1]);
    }

    // -- Data
    TH1D *GlobalCSVJet_Data[2]; // 12 components
    GlobalCSVJet_Data[0] = new TH1D("GlobalCSVJet1_Data", "CSV distribution for AddJet-1", 20, 0.0, 1.0);
    GlobalCSVJet_Data[1] = new TH1D("GlobalCSVJet2_Data", "CSV distribution for AddJet-2", 20, 0.0, 1.0);
    RecoverCSVHisto (hData[ich], GlobalCSVJet_Data[0], GlobalCSVJet_Data[1]);
    
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

    TString dirfigname_pdf = "CombineResults/Figures_" + InpDir + Plots + FitDir + "/";
    // make a dir if it does not exist!!
    gSystem->mkdir(dirfigname_pdf, kTRUE);

    cPlots->SaveAs(dirfigname_pdf + "FullHisto_" + dirname[ich] + "_NormLog.pdf");

    // -----------------------------------------------------------------------------
    // Only Log Plots
    // -----------------------------------------------------------------------------

    TCanvas *cPlotsLog;//histos                                                                                                                                                                   
    cPlotsLog = new TCanvas("cPlotsLog"+dirname[ich] ,"Plots");
    cPlotsLog->Divide(1,2);
    TPad    *padLog[2];

    //Plot Pad
    padLog[0] = (TPad*)cPlotsLog->GetPad(1);
    padLog[0]->SetPad(0.01, 0.23, 0.99, 0.99);
    padLog[0]->SetTopMargin(0.1);
    padLog[0]->SetRightMargin(0.04);
    
    //Ratio Pad
    padLog[1] = (TPad*)cPlotsLog->GetPad(2);;
    padLog[1]->SetPad(0.01, 0.02, 0.99, 0.3);
    gStyle->SetGridWidth(1);
    gStyle->SetGridColor(14);
    padLog[1]->SetGridx();
    padLog[1]->SetGridy();
    padLog[1]->SetTopMargin(0.05);
    padLog[1]->SetBottomMargin(0.4);
    padLog[1]->SetRightMargin(0.04);

    padLog[0]->cd();
    padLog[0]->cd()->SetLogy();
    
    hstyleLog -> Draw();
    
    AllMC[ich] -> Draw("HISTSAME"); 
    hData[ich] -> Draw("PSAME"); 
    titlePr->Draw("SAME");
    title->Draw("SAME");
    chtitle->Draw("SAME");

    TLegend *legLog;
    legLog = new TLegend(0.65,0.60,0.99,0.99);
    legLog->SetFillColor(0);
    legLog->SetLineColor(0);
    legLog->SetLineWidth(0.0);
    legLog->SetTextFont(62);
    legLog->SetTextSize(0.03);
    legLog->SetNColumns(3);
  
    legLog->AddEntry(hData[ich],         "Data","PL");
    legLog->AddEntry(hInput[ich].at(11), "Z+Jets","F");
    legLog->AddEntry(hInput[ich].at(10), "VV","F");
    legLog->AddEntry(hInput[ich].at(9),  "Single t","F");
    legLog->AddEntry(hInput[ich].at(8),  "QCD","F");
    legLog->AddEntry(hInput[ich].at(7),  "W+Jets","F");
    legLog->AddEntry(hInput[ich].at(6),  "t#bar{t}+V","F");
    legLog->AddEntry(hInput[ich].at(5),  "t#bar{t}+H","F");
    legLog->AddEntry(hInput[ich].at(4),  "t#bar{t}+other","F");
    legLog->AddEntry(hInput[ich].at(3),  "t#bar{t}+LF","F");
    legLog->AddEntry(hInput[ich].at(2),  "t#bar{t}+cc","F");
    legLog->AddEntry(hInput[ich].at(1),  "t#bar{t}+bj","F");
    legLog->AddEntry(hInput[ich].at(0),  "t#bar{t}+bb","F");
    legLog->AddEntry(gRatioFull,         "Stat. Unc.","F");
    legLog->Draw("SAME");

    padLog[1]->cd();
    RatioFull ->Draw("HIST");
    gRatioFull->Draw("e2");
    RatioFull ->Draw("HISTSAME");

    cPlotsLog->SaveAs(dirfigname_pdf + "FullHisto_" + dirname[ich] + "_OnlyLog.pdf");
    
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
    


    TH1D *hstyleI = new TH1D ("hstyleI","",
			      GlobalCSVJet_Data[1]->GetNbinsX(),
			      GlobalCSVJet_Data[1]->GetBinLowEdge (1),
			      GlobalCSVJet_Data[1]->GetBinLowEdge (GlobalCSVJet_Data[1]->GetNbinsX()+1));
    
    hstyleI -> SetMaximum(1.1*GlobalCSVJet_Data[1]->GetMaximum());
    hstyleI -> GetYaxis()->SetTitleOffset(0.6);
    hstyleI -> GetYaxis()->SetTitleSize(0.05);
    hstyleI -> GetYaxis()->SetLabelSize(0.05);
    hstyleI -> GetYaxis()->SetTitle("Events"); 
    
    GlobalCSVJet_Data[0] -> SetMarkerStyle(20); 
    GlobalCSVJet_Data[0] -> SetMarkerSize(0.4); 
    GlobalCSVJet_Data[0] -> SetLineWidth(1); 
    GlobalCSVJet_Data[0] -> SetTitle(""); 


    padI[0]->cd();
    padI[0]->cd()->SetLogy();
    
    hstyleI -> SetMaximum(100.0*GlobalCSVJet_Data[0]->GetMaximum());
    hstyleI -> SetMinimum(0.1);
    hstyleI -> Draw();

    AllMC_CSV1[ich] -> Draw("HISTSAME");

    GlobalCSVJet_Data[0] -> SetMarkerStyle(20); 
    GlobalCSVJet_Data[0] -> SetMarkerSize(0.6); 
    GlobalCSVJet_Data[0] -> SetLineWidth(1); 
    GlobalCSVJet_Data[0] -> SetTitle(""); 
    GlobalCSVJet_Data[0] -> Draw("SAME");

    titlePr->Draw("SAME");
    title->Draw("SAME");
    chtitle->Draw("SAME");	

    TLegend *legI;
    legI = new TLegend(0.70,0.64,0.93,0.87);
    legI->SetFillColor(0);
    legI->SetLineColor(0);
    legI->SetLineWidth(0.0);
    legI->SetTextFont(62);
    legI->SetTextSize(0.03);
    legI->SetNColumns(2);
  
    legI->AddEntry(hData[ich],         "Data","PL");
    legI->AddEntry(hInput[ich].at(11), "Z+Jets","F");
    legI->AddEntry(hInput[ich].at(10), "VV","F");
    legI->AddEntry(hInput[ich].at(9),  "Single t","F");
    legI->AddEntry(hInput[ich].at(8),  "QCD","F");
    legI->AddEntry(hInput[ich].at(7),  "W+Jets","F");
    legI->AddEntry(hInput[ich].at(6),  "t#bar{t}+V","F");
    legI->AddEntry(hInput[ich].at(5),  "t#bar{t}+H","F");
    legI->AddEntry(hInput[ich].at(4),  "t#bar{t}+other","F");
    legI->AddEntry(hInput[ich].at(3),  "t#bar{t}+LF","F");
    legI->AddEntry(hInput[ich].at(2),  "t#bar{t}+cc","F");
    legI->AddEntry(hInput[ich].at(1),  "t#bar{t}+bj","F");
    legI->AddEntry(hInput[ich].at(0),  "t#bar{t}+bb","F");
    legI->AddEntry(gRatioFull,         "Stat. Unc.","F");

    legI->Draw("SAME");

    TH1D *RatioFullJet1 = HistoRatio (GlobalCSVJet_Data[0] , (TH1D*) AllMC_CSV1[ich]->GetStack()->Last());
    TGraphErrors *gRatioFullJet1 = new TGraphErrors(RatioFullJet1);
    gRatioFullJet1->SetFillStyle(1001);
    gRatioFullJet1->SetFillColor(chatch);
    gRatioFullJet1->SetName("gRatioFullJet2");

    padI[1]->cd();
    RatioFullJet1 ->GetYaxis()->SetTitleOffset(0.25);
    RatioFullJet1 ->Draw("HIST");
    gRatioFullJet1->Draw("e2");
    RatioFullJet1 ->Draw("HISTSAME");

    padI[2]->cd();
    padI[2]->cd()->SetLogy();
    
    TH1D *hstyleII = (TH1D *)hstyleI -> Clone("StyleII"); 
    hstyleII -> SetMaximum(100.0*GlobalCSVJet_Data[1]->GetMaximum());
    hstyleII -> SetMinimum(0.1);

    hstyleII -> Draw();
    AllMC_CSV2[ich] -> Draw("HISTSAME");

    GlobalCSVJet_Data[1] -> SetMarkerStyle(20); 
    GlobalCSVJet_Data[1] -> SetMarkerSize(0.5); 
    GlobalCSVJet_Data[1] -> SetLineWidth(1); 
    GlobalCSVJet_Data[1] -> SetTitle(""); 
    GlobalCSVJet_Data[1] -> Draw("SAME");

    titlePr->Draw("SAME");
    title->Draw("SAME");
    chtitle->Draw("SAME");	

    legI->Draw("SAME");

    TH1D *RatioFullJet2 = HistoRatio (GlobalCSVJet_Data[1] , (TH1D*) AllMC_CSV2[ich]->GetStack()->Last());
    TGraphErrors *gRatioFullJet2 = new TGraphErrors(RatioFullJet2);
    gRatioFullJet2->SetFillStyle(1001);
    gRatioFullJet2->SetFillColor(chatch);
    gRatioFullJet2->SetName("gRatioFullJet2");

    padI[3]->cd();
    RatioFullJet2 ->GetYaxis()->SetTitleOffset(0.25);
    RatioFullJet2 ->Draw("HIST");
    gRatioFullJet2->Draw("e2");
    RatioFullJet2 ->Draw("HISTSAME");


    cPlotsI ->SaveAs(dirfigname_pdf + "CSVHistosJet1_" + dirname[ich] + "_Log.pdf");
    cPlotsII->SaveAs(dirfigname_pdf + "CSVHistosJet2_" + dirname[ich] + "_Log.pdf");

    // ttbb Shape

    


  }// for(ich)
  

}
