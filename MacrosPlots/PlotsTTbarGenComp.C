#include "Plots.h"
 
void PlotsTTbarGenComp(TString plots="4Jets", bool LogScale=false) {
  
  /****************
        Style
  ****************/
  setTDRStyle();

  gROOT->SetStyle("Plain");
  gStyle->SetOptFit(1000);
  gStyle->SetOptStat("emruo");
  gStyle->SetOptStat(kFALSE);
  gStyle->SetPadTickY(1);
  gStyle->SetPadTickX(1);
  
  
  Int_t chatch = 1756;
  TColor *color = new TColor(chatch, 0.3, 0.5, 0.5, "", 0.45); // alpha = 0.5
  TString files  = dirnameIn + fl;  
  
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
  
  /****************
       Channel
  ****************/
  TString channel[3];
  channel[0] = "mujets";
  channel[1] = "ejets";  
  channel[2] = "lepjet"; 
 
  /****************
        Data
  ****************/ 
  std::vector<histos> DataMu;
  DataMu = loadhistograms(plots, files + "_DataSingleMu");
  std::vector<histos> DataEG;
  DataEG = loadhistograms(plots, files + "_DataSingleEG");
  std::vector<histos> Data;
  Data = addhistograms(DataMu, DataEG);
  setuphistograms(Data, kBlack);

  /****************
    ttbar Signal
  ****************/ 
  // ttbar categorization 
  std::vector<histos> ttbar_0;
  ttbar_0 = loadhistograms(plots, files + "_ttbar_PowhegPythia");
  setuphistograms(ttbar_0, kOrange);

  std::vector<histos> ttbar_1;
  ttbar_1 = loadhistograms(plots, files + "_ttbar_PowhegPythiaAlphaS");
  setuphistograms(ttbar_1, kOrange);

  std::vector<histos> ttbar_2;
  ttbar_2 = loadhistograms(plots, files + "_ttbar_LepJetsPowhegPythia");
  setuphistograms(ttbar_2, kOrange);

  std::vector<histos> ttbar_3;
  ttbar_3 = loadhistograms(plots, files + "_ttbar_LepJetsPowhegPythiaAlphaS");
  setuphistograms(ttbar_3, kOrange);

  std::vector<histos> ttbar_4;
  ttbar_4 = loadhistograms(plots, files + "_ttbar_aMCatNLOPythia");
  setuphistograms(ttbar_4, kRed+2);

  std::vector<histos> ttbar_5;
  ttbar_5 = loadhistograms(plots, files + "_ttbar_MadgraphPythia");
  setuphistograms(ttbar_5, kRed+3);

  std::vector<histos> ttbar_6;
  ttbar_6 = loadhistograms(plots, files + "_ttbar_LepJetsPowhegPythiaTranche3");
  setuphistograms(ttbar_6, kRed+3);

  // ttbar Bkg
  std::vector<histos> ttbar_bkg;
  ttbar_bkg = loadhistograms(plots, files + "_ttbar_PowhegPythiaBkg");
  setuphistograms(ttbar_bkg, col_ttbarBkg);
  
  /****************
       Z+Jets
  ****************/ 
  std::vector<histos> ZJets;
  ZJets = loadhistograms(plots, files + "_ZJets_aMCatNLO");
  setuphistograms(ZJets, col_ZJets);

  /****************
         VV
  ****************/ 
  std::vector<histos> WW;
  WW = loadhistograms(plots, files + "_WW_Pythia");
  std::vector<histos> WZ;
  WZ = loadhistograms(plots, files + "_WZ_Pythia");
  std::vector<histos> ZZ;
  ZZ = loadhistograms(plots, files + "_ZZ_Pythia");
  std::vector<histos> VV;
  VV = addhistograms(WW, WZ);
  VV = addhistograms(VV, ZZ);
  setuphistograms(VV, col_VV);

  /****************
     Single Top
  ****************/ 
  std::vector<histos> tW;
  tW = loadhistograms(plots, files + "_tW_Powheg");
  std::vector<histos> tbarW;
  tbarW = loadhistograms(plots, files + "_tbarW_Powheg");
  std::vector<histos> t_tch;
  t_tch = loadhistograms(plots, files + "_t_tchannel_Powheg");
  std::vector<histos> tbar_tch;
  tbar_tch = loadhistograms(plots, files + "_tbar_tchannel_Powheg");
  std::vector<histos> Single_top;
  Single_top = addhistograms(tW, tbarW);
  Single_top = addhistograms(Single_top, t_tch);
  Single_top = addhistograms(Single_top, tbar_tch);
  setuphistograms(Single_top, col_SingleTop);

  /****************
       W+Jets
  ****************/ 
  std::vector<histos> WJets;
  WJets = loadhistograms(plots, files + "_WJets_aMCatNLO");
  //WJets = loadhistograms(plots, files + "_WJets_Madgraph");
  setuphistograms(WJets, col_WJets);

  /****************
        QCD
  ****************/ 
  std::vector<histos> QCD_Mu;
  QCD_Mu = loadhistograms(plots, files + "_QCD_MuEnr");
  std::vector<histos> QCD_EG;
  QCD_EG = loadhistograms(plots, files + "_QCD_EGEnr");
  std::vector<histos> QCD;  
  QCD = addhistograms(QCD_Mu, QCD_EG);
  setuphistograms(QCD, col_QCD);

  /****************
      ttbar+V
  ****************/ 
  std::vector<histos> ttbarW;
  ttbarW = loadhistograms(plots, files + "_ttW_Madgraph");
  std::vector<histos> ttbarZ;
  ttbarZ = loadhistograms(plots, files + "_ttZ_Madgraph");
  std::vector<histos> ttbarV;  
  ttbarV = addhistograms(ttbarW, ttbarZ);
  setuphistograms(ttbarV, col_ttbarV);

  /****************
      ttbar+H
  ****************/ 
  std::vector<histos> ttbarH;
  ttbarH = loadhistograms(plots, files + "_ttHbb_PowhegPythia");
  setuphistograms(ttbarH, col_ttbarH);

  /****************
    All Syst Unc
  ****************/ 
  std::vector<histos> MC_syst;
  MC_syst = loadhistograms(plots, files + "_MCAllSyst");
  setuphistograms(MC_syst, col_tt);


  /****************
       Stacks
  ****************/ 
  std::vector<histos> Stack_bkg;
  std::vector<histos> Stack;
  histos st_bkg;
  histos st;
  for(unsigned int h=0; h<WJets.size(); h++){
    TString variable;
    for(int ch=0; ch<3; ch++){
      st.mc[ch]     = new THStack(variable, "");
      st_bkg.mc[ch] = new THStack(variable, "");
      st.mc[ch]     -> SetHistogram( (TH1F*)WJets[h].hist[ch]->Clone());
      st_bkg.mc[ch] -> SetHistogram( (TH1F*)WJets[h].hist[ch]->Clone());      
    }
    Stack.push_back(st);
    Stack_bkg.push_back(st_bkg);
  }
  
  //-------------------------------------------------------
  // Background Stack
  Stack_bkg = addstack(Stack_bkg, WJets);
  Stack_bkg = addstack(Stack_bkg, QCD);
  Stack_bkg = addstack(Stack_bkg, Single_top);
  Stack_bkg = addstack(Stack_bkg, VV);
  Stack_bkg = addstack(Stack_bkg, ttbar_bkg);
  Stack_bkg = addstack(Stack_bkg, ZJets);
  Stack_bkg = addstack(Stack_bkg, ttbarV);
  Stack_bkg = addstack(Stack_bkg, ttbarH);
  //-------------------------------------------------------
  // Full Stack
  // Stack = addstack(Stack, ttbar_0_ttbb);
  // Stack = addstack(Stack, ttbar_0_ttb);
  // Stack = addstack(Stack, ttbar_0_ttcc);
  // Stack = addstack(Stack, ttbar_0_ttLF);
  // Stack = addstack(Stack, ttOther);
  Stack = addstack(Stack, WJets);
  Stack = addstack(Stack, QCD);
  Stack = addstack(Stack, Single_top);
  Stack = addstack(Stack, VV);
  Stack = addstack(Stack, ZJets);
  Stack = addstack(Stack, ttbarV);
  Stack = addstack(Stack, ttbarH);
  //-------------------------------------------------------
  //-------------------------------------------------------
  // other ttbar Generators
  ttbar_0 = addhistograms(ttbar_0, Stack_bkg);
  ttbar_1 = addhistograms(ttbar_1, Stack_bkg);
  ttbar_2 = addhistograms(ttbar_2, Stack_bkg);
  ttbar_3 = addhistograms(ttbar_3, Stack_bkg);
  ttbar_4 = addhistograms(ttbar_4, Stack_bkg);
  ttbar_5 = addhistograms(ttbar_5, Stack_bkg);
  ttbar_6 = addhistograms(ttbar_6, Stack_bkg);
  
  /****************
     Draw Histos
  ****************/ 
  TCanvas *histocanvas;
  histocanvas = new TCanvas("plots", "Plots");
  
  for(unsigned int h=0; h<WJets.size(); h++){
    for(int ch=0; ch<3; ch++){
      
      histocanvas->Divide(1,2);
      
      TPad    *pad[2];
      //Plot Pad
      pad[0] = (TPad*)histocanvas->GetPad(1);
      pad[0]->SetPad(0.01, 0.23, 0.99, 0.99);
      pad[0]->SetTopMargin(0.1);
      pad[0]->SetRightMargin(0.04);
      if(LogScale) pad[0]->SetLogy();
      
      //Ratio Pad
      pad[1] = (TPad*)histocanvas->GetPad(2);
      pad[1]->SetPad(0.01, 0.02, 0.99, 0.3);
      gStyle->SetGridWidth(1);
      gStyle->SetGridColor(14);
      pad[1]->SetGridx();
      pad[1]->SetGridy();
      pad[1]->SetTopMargin(0.05);
      pad[1]->SetBottomMargin(0.4);
      pad[1]->SetRightMargin(0.04);
      
      //-------------------------------------------------------
      // Stack
      pad[0]->cd();

      Stack[h].hist[ch]->GetXaxis()->SetRange(WJets[h].hist[ch]->GetXaxis()->GetFirst(), 
      					    WJets[h].hist[ch]->GetXaxis()->GetLast());
      Stack[h].hist[ch]->SetTitle("");
      Stack[h].hist[ch]->GetYaxis()->SetTitle("Events");
      Stack[h].hist[ch]->GetYaxis()->SetTitleOffset(1.2);
      Stack[h].hist[ch]->GetYaxis()->SetTitleSize(0.07);
      Stack[h].hist[ch]->GetYaxis()->SetLabelSize(0.055);
      Stack[h].hist[ch]->GetYaxis()->SetNdivisions(607);
      //Stack[h].hist[ch]->GetYaxis()->SetLabelSize(0.05);
      TGaxis *hYaxis = (TGaxis*)Stack[h].hist[ch]->GetYaxis();
      //hYaxis->SetMaxDigits(3);
      Stack[h].hist[ch]->GetXaxis()->SetLabelSize(0.0);
      Stack[h].hist[ch]->GetXaxis()->SetTitle("");

      // Produce enough vertical space for the legend 
      float MaxHisto;
      if(LogScale) MaxHisto = 100.0;
      else MaxHisto = 1.7;
      float maxh = Data[h].hist[ch]->GetMaximum();
      if(maxh < Stack[h].hist[ch]->GetMaximum()) maxh = Stack[h].hist[ch]->GetMaximum();
      Stack[h].hist[ch]->SetMaximum(MaxHisto*maxh);
      if(LogScale) Stack[h].hist[ch]->SetMinimum(0.1);

      Stack[h].hist[ch]->Draw("hist");
      
      //-------------------------------------------------------
      // Band error
      // TGraphErrors *thegraph = new TGraphErrors(HIST_syst[h].hist[ch]);  // Full set of Syst. Unc.
      TGraphErrors *thegraph = new TGraphErrors(Stack[h].hist[ch]); // Just Statistical Unc. 
      thegraph->SetName("thegraph");
      thegraph->SetFillStyle(1001);
      thegraph->SetFillColor(chatch);
      thegraph->SetLineColor(chatch);
      //thegraph->Draw("e2SAME");

      //-------------------------------------------------------
      // Other ttbar generators
      ttbar_0[h].hist[ch]->SetLineColor(kBlue);
      ttbar_0[h].hist[ch]->SetLineStyle(1);
      ttbar_0[h].hist[ch]->SetFillColor(0);
      ttbar_0[h].hist[ch]->Draw("histoSAME");

      ttbar_1[h].hist[ch]->SetLineColor(kBlue-7);
      ttbar_1[h].hist[ch]->SetLineStyle(2);
      ttbar_1[h].hist[ch]->SetFillColor(0);
      ttbar_1[h].hist[ch]->Draw("histoSAME");

      ttbar_2[h].hist[ch]->SetLineColor(kRed);
      ttbar_2[h].hist[ch]->SetLineStyle(1);
      ttbar_2[h].hist[ch]->SetFillColor(0);
      ttbar_2[h].hist[ch]->Draw("histoSAME");

      ttbar_3[h].hist[ch]->SetLineColor(kRed-7);
      ttbar_3[h].hist[ch]->SetLineStyle(2);
      ttbar_3[h].hist[ch]->SetFillColor(0);
      ttbar_3[h].hist[ch]->Draw("histoSAME");

      ttbar_4[h].hist[ch]->SetLineColor(6);
      ttbar_4[h].hist[ch]->SetLineStyle(1);
      ttbar_4[h].hist[ch]->SetFillColor(0);
      ttbar_4[h].hist[ch]->Draw("histoSAME");

      ttbar_5[h].hist[ch]->SetLineColor(8);
      ttbar_5[h].hist[ch]->SetLineStyle(1);
      ttbar_5[h].hist[ch]->SetFillColor(0);
      ttbar_5[h].hist[ch]->Draw("histoSAME");

      ttbar_6[h].hist[ch]->SetLineColor(kOrange-3);
      ttbar_6[h].hist[ch]->SetLineStyle(1);
      ttbar_6[h].hist[ch]->SetFillColor(0);
      ttbar_6[h].hist[ch]->Draw("histoSAME");
      //-------------------------------------------------------
      // Data Histogram
      Data[h].hist[ch]->SetMarkerStyle(20);
      Data[h].hist[ch]->SetMarkerSize(0.7);
      Data[h].hist[ch]->Draw("SAME");

      /***********************
             Legends
      ***********************/
      TLegend *leg;
      float legPos[4] = {0.70,  // x_o
			 0.50,  // y_o
			 0.93,  // x_f
			 0.87}; // y_f
      
      leg = new TLegend(legPos[0],legPos[1],legPos[2],legPos[3]);
      leg->SetFillColor(0);
      leg->SetLineColor(0);
      leg->SetLineWidth(0.0);
      leg->SetTextFont(62);
      leg->SetTextSize(0.03);
      //leg->SetNColumns(2);

      // leg->AddEntry(ZJets[h].hist[ch],       "Z+Jets","F");
      // leg->AddEntry(VV[h].hist[ch],          "VV","F");
      // leg->AddEntry(Single_top[h].hist[ch],  "Single t","F");
      // leg->AddEntry(QCD[h].hist[ch],         "QCD","F");
      // leg->AddEntry(WJets[h].hist[ch],       "W+Jets","F");
      // leg->AddEntry(ttbarV[h].hist[ch],      "t#bar{t}+V","F");
      // leg->AddEntry(ttbarH[h].hist[ch],      "t#bar{t}+H","F");
      // leg->AddEntry(ttOther[h].hist[ch],     "t#bar{t}+other","F");
      // leg->AddEntry(ttbar_0_ttLF[h].hist[ch],"t#bar{t}+LF","F");
      // leg->AddEntry(ttbar_0_ttcc[h].hist[ch],"t#bar{t}+cc","F");
      // leg->AddEntry(ttbar_0_ttb[h].hist[ch], "t#bar{t}+b","F");
      // leg->AddEntry(ttbar_0_ttbb[h].hist[ch],"t#bar{t}+bb","F");
      // leg->AddEntry("thegraph",              "Total Unc.","F");
      leg->AddEntry((TObject*)0,"Powheg+Pythia CUETP8M1","");
      leg->AddEntry(ttbar_0[h].hist[ch],     "Incl.","L");
      leg->AddEntry(ttbar_1[h].hist[ch],     "Incl. #alpha_{s}  Scale Up","L");
      //leg->AddEntry((TObject*)0,"","");
      leg->AddEntry(ttbar_2[h].hist[ch],     "l+Jets","L");
      leg->AddEntry(ttbar_3[h].hist[ch],     "l+Jets #alpha_{s}","L");
      leg->AddEntry((TObject*)0,"Powheg+Pythia CUETP8M2","");
      leg->AddEntry(ttbar_6[h].hist[ch],     "l+Jets Tranche3","L");
      leg->AddEntry((TObject*)0,"Other GEN CUETP8M1","");
      leg->AddEntry(ttbar_4[h].hist[ch],     "aMC@NLO FxFx","L");
      leg->AddEntry(ttbar_5[h].hist[ch],     "Madgraph","L");
      leg->AddEntry((TObject*)0,"","");
      leg->AddEntry(Data[h].hist[ch],        "Data","PL");
      leg->AddEntry(Stack[h].hist[ch],     "Bkg","F");
      leg->Draw("SAME");
      //-------------------------------------------------------
      // CMS Legends
      TString htitleCMSChannel[3];
      htitleCMSChannel[0] = "#mu^{#pm}+jets channel";
      htitleCMSChannel[1] = "e^{#pm}+jets channel";
      htitleCMSChannel[2] = "l^{#pm}+jets channel";

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
      title  = new TLatex(-20.,50.,"CMS(2016) #sqrt{s} = 13TeV, L = 15.92 fb^{-1}");
      title->SetNDC();
      title->SetTextAlign(12);
      title->SetX(0.20);
      title->SetY(0.83);
      title->SetTextFont(42);
      title->SetTextSize(0.05);
      title->SetTextSizePixels(24);
      title->Draw("SAME");
      
      TLatex *chtitle;
      chtitle  = new TLatex(-20.,50.,htitleCMSChannel[ch]);
      chtitle->SetNDC();
      chtitle->SetTextAlign(12);
      chtitle->SetX(0.20);
      chtitle->SetY(0.75);
      chtitle->SetTextFont(42);
      chtitle->SetTextSize(0.05);
      chtitle->SetTextSizePixels(24);
      chtitle->Draw("SAME");

      /***********************
               Ratio
      ***********************/    
      pad[1]->cd();
      //-------------------------------------------------------
      //Graph Ratio Clone
      TH1F *Ratio;
      Ratio = (TH1F*)Data[h].hist[ch]->Clone();
      Ratio->Divide(ttbar_0[h].hist[ch]);

      TH1F *RatioSyst;
      RatioSyst = (TH1F*)Data[h].hist[ch]->Clone();
      // RatioSyst->Divide(MC_syst[h].hist[ch]);  // Should be the histogram with the Total Syst. Unc.
      RatioSyst->Divide(Stack[h].hist[ch]); // Histogram with the total uncertainty from the STACK
      std::vector<double> ratioContent;
      for(unsigned int b_r = 1; b_r <= RatioSyst->GetNbinsX(); b_r++) RatioSyst->SetBinContent(b_r,1.0);

      // Ratio->SetMarkerStyle(20);
      Ratio->SetMarkerSize(0.);
      // Ratio->SetMarkerColor(1);
      Ratio->SetLineColor(kBlue);
      Ratio->SetFillColor(0);
      Ratio->SetLineWidth(1);
      Ratio->SetMaximum(2);
      Ratio->SetMinimum(0);
      Ratio->SetTitle("");
      
      Ratio->GetYaxis()->SetTitle("Obs/Exp");
      Ratio->GetYaxis()->CenterTitle();
      Ratio->GetYaxis()->SetTitleOffset(0.45);
      Ratio->GetYaxis()->SetTitleSize(0.16);
      Ratio->GetYaxis()->SetLabelSize(0.15);
      Ratio->GetYaxis()->SetNdivisions(402);
      Ratio->GetXaxis()->SetNdivisions(509); //(402)
      Ratio->GetXaxis()->SetTitleOffset(1.1);
      Ratio->GetXaxis()->SetLabelSize(0.20);
      Ratio->GetXaxis()->SetTitleSize(0.16);
      
      Ratio->SetMinimum(0.6);
      Ratio->SetMaximum(1.4);
      
      TGraphErrors *thegraphRatioSyst = new TGraphErrors(RatioSyst);
      thegraphRatioSyst->SetFillStyle(1001);
      thegraphRatioSyst->SetFillColor(chatch);
      thegraphRatioSyst->SetName("thegraphRatioSyst");
      
      //-------------------------------------------------------
      //Graph Ratio other ttbar generators
      TH1F *Ratio_1;
      Ratio_1 = (TH1F*)Data[h].hist[ch]->Clone();
      Ratio_1->Divide(ttbar_1[h].hist[ch]);
      Ratio_1->SetLineColor(kBlue-7);
      Ratio_1->SetLineStyle(2);
      Ratio_1->SetLineWidth(1);
      Ratio_1->SetFillColor(0);

      TH1F *Ratio_2;
      Ratio_2 = (TH1F*)Data[h].hist[ch]->Clone();
      Ratio_2->Divide(ttbar_2[h].hist[ch]);
      Ratio_2->SetLineColor(kRed);
      Ratio_2->SetLineStyle(1);
      Ratio_2->SetLineWidth(1);
      Ratio_2->SetFillColor(0);

      TH1F *Ratio_3;
      Ratio_3 = (TH1F*)Data[h].hist[ch]->Clone();
      Ratio_3->Divide(ttbar_3[h].hist[ch]);
      Ratio_3->SetLineColor(kRed-7);
      Ratio_3->SetLineStyle(2);
      Ratio_3->SetLineWidth(1);
      Ratio_3->SetFillColor(0);

      TH1F *Ratio_4;
      Ratio_4 = (TH1F*)Data[h].hist[ch]->Clone();
      Ratio_4->Divide(ttbar_4[h].hist[ch]);
      Ratio_4->SetLineColor(6);
      Ratio_4->SetLineStyle(1);
      Ratio_4->SetLineWidth(1);
      Ratio_4->SetFillColor(0);

      TH1F *Ratio_5;
      Ratio_5 = (TH1F*)Data[h].hist[ch]->Clone();
      Ratio_5->Divide(ttbar_5[h].hist[ch]);
      Ratio_5->SetLineColor(8);
      Ratio_5->SetLineStyle(1);
      Ratio_5->SetLineWidth(1);
      Ratio_5->SetFillColor(0);

      TH1F *Ratio_6;
      Ratio_6 = (TH1F*)Data[h].hist[ch]->Clone();
      Ratio_6->Divide(ttbar_6[h].hist[ch]);
      Ratio_6->SetLineColor(kOrange-3);
      Ratio_6->SetLineStyle(1);
      Ratio_6->SetLineWidth(1);
      Ratio_6->SetFillColor(0);
      //-------------------------------------------------------
      // Draw Ratios
      Ratio->Draw("hist");
      //thegraphRatioSyst->Draw("e2");
      //Ratio->Draw("histpSAME");
      Ratio_1->Draw("histSAME");
      Ratio_2->Draw("histSAME");
      Ratio_3->Draw("histSAME");
      Ratio_4->Draw("histSAME");
      Ratio_5->Draw("histSAME");
      Ratio_6->Draw("histSAME");
            
      /***********************
            Save Histos
      ***********************/    
      TString dirfigname_log;
      if(LogScale) dirfigname_log = "_log";
      else dirfigname_log = "";
      TString dirfigname_pdf;
      TString dirfigname_png;
      dirfigname_pdf = dirnameIn + "figuresTTbarComp_" + fl + "/ttbb/pdf" + dirfigname_log + "/";
      //dirfigname_png = dirnameIn + "figures_" + fl + "AlphaS/ttbb/png" + dirfigname_log + "/";
      // make a dir if it does not exist!!
      gSystem->mkdir(dirfigname_pdf,       kTRUE);
      histocanvas->SaveAs(dirfigname_pdf + WJets[h].hist[ch]->GetName() + ".pdf");
      //gSystem->mkdir(dirfigname_png,       kTRUE);
      //histocanvas->SaveAs(dirfigname_png + WJets[h].hist[ch]->GetName() + ".png");
      
      // clear Canvas
      histocanvas->Clear();    

    }
  }
  
} //end Plots.C

void setuphistograms(std::vector<histos> histos, int color){
  
  for(unsigned int h = 0; h < histos.size(); h++){
    for(unsigned int ch=0; ch<3; ch++){  
      
      // Overflows in the last bin
      histos[h].hist[ch]->SetBinContent(histos[h].hist[ch]->GetNbinsX(),
      					(histos[h].hist[ch]->GetBinContent(histos[h].hist[ch]->GetNbinsX()+1) + 
      					 histos[h].hist[ch]->GetBinContent(histos[h].hist[ch]->GetNbinsX())
      					 )
      					);
      histos[h].hist[ch]->GetXaxis()->SetCanExtend(kFALSE);
      histos[h].hist[ch]->SetBinContent(histos[h].hist[ch]->GetNbinsX()+1, 0.0);

      // Style
      // if(color == kRed+1)  histos[h].hist[ch]->SetLineColor(1); // Except ttbar
      // else histos[h].hist[ch]->SetLineColor(color);
      histos[h].hist[ch]->SetLineColor(1); 
      histos[h].hist[ch]->SetFillColor(color);
      histos[h].hist[ch]->SetFillStyle(1001);

    }    
  }    
}

std::vector<histos> addhistograms(std::vector<histos> histos_0, std::vector<histos> histos_1){

  std::vector<histos> sum;
  histos histos;

  for(unsigned int h = 0; h < histos_0.size(); h++){
    for(unsigned int ch=0; ch<3; ch++){  
      histos.hist[ch] = (TH1F*)histos_0[h].hist[ch]->Clone();
      histos.hist[ch]->Add(histos_0[h].hist[ch], histos_1[h].hist[ch]);
    }
    sum.push_back(histos);
  }

  return sum;
}

std::vector<histos> addstack(std::vector<histos> stack_0, std::vector<histos> histos_0){

  for(unsigned int h = 0; h < histos_0.size(); h++){
    for(unsigned int ch=0; ch<3; ch++){
      stack_0[h].mc[ch]->Add(histos_0[h].hist[ch]); // add histo to stack
      stack_0[h].hist[ch] = (TH1F*) stack_0[h].mc[ch]->GetStack()->Last()->Clone(); // create histo with the final stack
    }
  }
  
  return stack_0;
}

std::vector<histos> loadhistograms(TString plots, TString namefile){

  TFile *file=NULL; // new TFile(namefile);
  file = TFile::Open(namefile + ".root");
  
  if(!file){
    std::cerr << "ERROR: Could not open " <<  namefile  << " files!!!"  << std::endl;
    std::exit(0);
  }
  
  std::vector<histos> sample;
  histos histofile;
  
  TString channel[3];
  channel[0]="mujets";
  channel[1]="ejets";
  channel[2]="lepjets";
  
  std::vector<TString> histoname;
  
  // Histograms
  histoname.push_back("hPV");
  // histoname.push_back("hMET");
  // histoname.push_back("hMET_Phi");
  // histoname.push_back("hLepPt");
  // histoname.push_back("hLepEta");
  // histoname.push_back("hLepPhi");
  histoname.push_back("hJetPt_Jet-0");
  // histoname.push_back("hJetPt_Jet-1");
  // histoname.push_back("hJetPt_Jet-2");
  // histoname.push_back("hJetPt_Jet-3");
  // histoname.push_back("hJetPt_Jet-4");
  // histoname.push_back("hJetPt_Jet-5");
  histoname.push_back("hNJets");
  histoname.push_back("hNBtagJets");
  // histoname.push_back("hCSV_Jet-0");
  // histoname.push_back("hCSV_Jet-1");
  histoname.push_back("hCSV_Jet-2");
  histoname.push_back("hCSV_Jet-3");
  // histoname.push_back("hCSV_Jet-4");
  // histoname.push_back("hCSV_Jet-5");
  // histoname.push_back("hKinAdd1CSV");
  // histoname.push_back("hKinAdd2CSV");
  histoname.push_back("hKinTagAddDR");
  histoname.push_back("hKinTagAddMass");

  for(unsigned int h=0; h<histoname.size(); h++){
    for(unsigned int ch=0; ch<2; ch++) histofile.hist[ch] = (TH1F*)file->Get(plots + "/" + channel[ch] + "/" +  histoname[h] + "_" + channel[ch] + "_" + plots);
    // lep + jets
    histofile.hist[2] = (TH1F*)histofile.hist[0]->Clone();
    histofile.hist[2]->Add(histofile.hist[0], histofile.hist[1]);
    histofile.hist[2]->SetName(histoname[h] + "_" + channel[2] + "_" + plots);

    sample.push_back(histofile);
    
  }  

  cout << "All the histograms from " << namefile << ".root have been loaded successfully!!!"  << endl; 
  return sample;
}

