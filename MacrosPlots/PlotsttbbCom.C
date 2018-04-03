#include "../Plots.h"
void Plotter(TString plots="2btag", bool LogScale=false, TString cat = "ttbb"){
  
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
  
  int col_GEN1  = 1;
  int col_GEN2   = 2;
  int col_GEN3 = 4;
    
  /****************
       Channel
  ****************/
  TString channel[3];
  channel[0] = "mujets";
  channel[1] = "ejets";  
  channel[2] = "lepjet"; 
 
  /****************
    ttbar Signal
  ****************/ 
  std::vector<histos> ttbar_GEN1;
  std::vector<histos> ttbar_GEN2;
  std::vector<histos> ttbar_GEN3;

  ttbar_GEN1  = loadhistograms(plots, files + "_ttbar_LepJetsPowhegPythia" + cat);
  ttbar_GEN2   = loadhistograms(plots, files + "_ttbb_aMCatNLO" + cat);
  ttbar_GEN3 = loadhistograms(plots, files + "_ttbb_Sherpa" + cat);
  setuphistograms(ttbar_GEN1,  col_GEN1);
  setuphistograms(ttbar_GEN2,   col_GEN2);
  setuphistograms(ttbar_GEN3, col_GEN3);
  
  /****************
     Draw Histos
  ****************/ 
  TCanvas *histocanvas;
  histocanvas = new TCanvas("plots", "Plots");
  
  for(unsigned int h=0; h<ttbar_GEN1.size(); h++){
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
      // HISTOGRAM
      pad[0]->cd();

      ttbar_GEN1[h].hist[ch]->GetXaxis()->SetRange(ttbar_GEN1[h].hist[ch]->GetXaxis()->GetFirst(), 
      					    ttbar_GEN1[h].hist[ch]->GetXaxis()->GetLast());
      //ttbar_GEN1[h].hist[ch]->GetYaxis()->SetTitle("Events");
      ttbar_GEN1[h].hist[ch]->GetYaxis()->SetTitleOffset(1.2);
      ttbar_GEN1[h].hist[ch]->GetYaxis()->SetTitleSize(0.07);
      ttbar_GEN1[h].hist[ch]->GetYaxis()->SetLabelSize(0.055);
      ttbar_GEN1[h].hist[ch]->GetYaxis()->SetNdivisions(607);
      //ttbar_GEN1[h].hist[ch]->GetYaxis()->SetLabelSize(0.05);
      TGaxis *hYaxis = (TGaxis*)ttbar_GEN1[h].hist[ch]->GetYaxis();
      //hYaxis->SetMaxDigits(3);
      ttbar_GEN1[h].hist[ch]->GetXaxis()->SetLabelSize(0.0);
      //ttbar_GEN1[h].hist[ch]->GetXaxis()->SetTitle("");
      ttbar_GEN1[h].hist[ch]->SetTitle("");

      // Produce enough vertical space for the legend 
      float MaxHisto;
      if(LogScale) MaxHisto = 100.0;
      else MaxHisto = 1.7;
      float maxh = ttbar_GEN1[h].hist[ch]->GetMaximum();
      if(maxh < ttbar_GEN1[h].hist[ch]->GetMaximum()) maxh = ttbar_GEN1[h].hist[ch]->GetMaximum();
      ttbar_GEN1[h].hist[ch]->SetMaximum(MaxHisto*maxh);
      if(LogScale) ttbar_GEN1[h].hist[ch]->SetMinimum(0.1);
      
      ttbar_GEN1 [h].hist[ch]->Draw("hist");
      ttbar_GEN2  [h].hist[ch]->Draw("histSAME");
      ttbar_GEN3[h].hist[ch]->Draw("histSAME");

      /***********************
             Legends
      ***********************/
      TLegend *leg;
      float legPos[4] = {0.60,  // x_o
			 0.64,  // y_o
			 0.93,  // x_f
			 0.87}; // y_f
      
      leg = new TLegend(legPos[0],legPos[1],legPos[2],legPos[3]);
      leg->SetFillColor(0);
      leg->SetLineColor(0);
      leg->SetLineWidth(0.0);
      leg->SetTextFont(62);
      leg->SetTextSize(0.03);
      //leg->SetNColumns(2);

      leg->AddEntry((TObject*)0, "MC Comparisons: " + cat,"");
      leg->AddEntry((TObject*)0,"","");
      leg->AddEntry(ttbar_GEN1[h].hist[ch],"t#bar{t} Powheg+Pythia8","l");
      leg->AddEntry(ttbar_GEN2[h].hist[ch],"t#bar{t}b#bar{b} aMC@NLO Madspin+Pythia8","l");
      leg->AddEntry(ttbar_GEN3[h].hist[ch],"t#bar{t}b#bar{b} OpenLoops Sherpa ","l");
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
      title  = new TLatex(-20.,50.,"CMS(2016) #sqrt{s} = 13TeV");
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
      TH1D *RatioUp, *RatioDown;
      RatioDown = (TH1D*)ttbar_GEN1[h].hist[ch]->Clone();
      RatioUp   = (TH1D*)ttbar_GEN1[h].hist[ch]->Clone();
      RatioDown->Divide(ttbar_GEN3[h].hist[ch]);
      RatioUp  ->Divide(ttbar_GEN2[h].hist[ch]);

      RatioUp->GetYaxis()->SetTitle("Obs/Exp");
      RatioUp->GetYaxis()->CenterTitle();
      RatioUp->GetYaxis()->SetTitleOffset(0.45);
      RatioUp->GetYaxis()->SetTitleSize(0.16);
      RatioUp->GetYaxis()->SetLabelSize(0.15);
      RatioUp->GetYaxis()->SetNdivisions(402);
      RatioUp->GetXaxis()->SetNdivisions(509); //(402)
      RatioUp->GetXaxis()->SetTitleOffset(1.1);
      RatioUp->GetXaxis()->SetLabelSize(0.20);
      RatioUp->GetXaxis()->SetTitleSize(0.16);      
      RatioUp->SetMinimum(0.0);
      RatioUp->SetMaximum(2.0);
 
      RatioUp->SetLineColor(col_GEN2);
      RatioUp->SetLineWidth(1);
      RatioUp->SetTitle("");
      
      RatioDown->SetLineColor(col_GEN3);
      RatioDown->SetLineWidth(1);
     
      //-------------------------------------------------------
      // Draw Ratios
      RatioUp  ->Draw();
      RatioDown->Draw("SAME");
            
      /***********************
            Save Histos
      ***********************/    
      TString dirfigname_log;
      if(LogScale) dirfigname_log = "_log";
      else dirfigname_log = "";
      TString dirfigname_pdf;
      TString dirfigname_png;
      dirfigname_pdf = "../" + dirnameIn + "figures_" + fl + "/ttbbGenComp/pdf_" + cat + dirfigname_log + "/";
      // make a dir if it does not exist!!
      gSystem->mkdir(dirfigname_pdf,       kTRUE);
      histocanvas->SaveAs(dirfigname_pdf + ttbar_GEN1[h].hist[ch]->GetName() + ".pdf");      
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

      // Normalization
      float Norm = histos[h].hist[ch]->Integral();
      if (Norm != 0.0) Norm = 1.0/Norm;
      else Norm = 0.0;
      //histos[h].hist[ch]->Scale(Norm);

      // Style
      // if(color == kRed+1)  histos[h].hist[ch]->SetLineColor(1); // Except ttbar
      // else histos[h].hist[ch]->SetLineColor(color);
      histos[h].hist[ch]->SetLineColor(color); 
      histos[h].hist[ch]->SetLineWidth(2);
      //histos[h].hist[ch]->SetFillColor(color);
      //histos[h].hist[ch]->SetFillStyle(1001);

    }    
  }    
}

std::vector<histos> addhistograms(std::vector<histos> histos_0, std::vector<histos> histos_1){

  std::vector<histos> sum;
  histos histos;

  for(unsigned int h = 0; h < histos_0.size(); h++){
    for(unsigned int ch=0; ch<3; ch++){  
      histos.hist[ch] = (TH1D*)histos_0[h].hist[ch]->Clone();
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
      stack_0[h].hist[ch] = (TH1D*) stack_0[h].mc[ch]->GetStack()->Last()->Clone(); // create histo with the final stack
    }
  }
  
  return stack_0;
}

std::vector<histos> loadhistograms(TString plots, TString namefile){

  TFile *file=NULL; // new TFile(namefile);
  file = TFile::Open("../" + namefile + ".root");
  
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
  histoname.push_back("hMET");
  histoname.push_back("hMET_Phi");
  histoname.push_back("hLepPt");
  histoname.push_back("hLepEta");
  histoname.push_back("hLepPhi");
  histoname.push_back("hJetPt_Jet-0");
  histoname.push_back("hJetPt_Jet-1");
  histoname.push_back("hJetPt_Jet-2");
  histoname.push_back("hJetPt_Jet-3");
  histoname.push_back("hJetPt_Jet-4");
  histoname.push_back("hJetPt_Jet-5");
  histoname.push_back("hNJets");
  histoname.push_back("hNBtagJets");
  histoname.push_back("hCSV_Jet-0");
  histoname.push_back("hCSV_Jet-1");
  histoname.push_back("hCSV_Jet-2");
  histoname.push_back("hCSV_Jet-3");
  histoname.push_back("hCSV_Jet-4");
  histoname.push_back("hCSV_Jet-5");
  histoname.push_back("hKinAdd1CSV");
  histoname.push_back("hKinAdd2CSV");
  histoname.push_back("hGenTagAdd1CSV");
  histoname.push_back("hGenTagAdd2CSV");
  histoname.push_back("hInvMassjj");
  histoname.push_back("hmT");

  for(unsigned int h=0; h<histoname.size(); h++){
    for(unsigned int ch=0; ch<2; ch++) histofile.hist[ch] = (TH1D*)file->Get("central/" + plots + "/" + channel[ch] + "/" +  histoname[h] + "_" + channel[ch] + "_" + plots);
    // lep + jets
    histofile.hist[2] = (TH1D*)histofile.hist[0]->Clone();
    histofile.hist[2]->Add(histofile.hist[0], histofile.hist[1]);
    histofile.hist[2]->SetName(histoname[h] + "_" + channel[2] + "_" + plots);

    sample.push_back(histofile);
    
  }  

  cout << "All the histograms from " << namefile << ".root have been loaded successfully!!!"  << endl; 
  return sample;
}

void PlotsttbbCom(){

  Plotter("2btag", false, "ttbb");
  Plotter("2btag", true,  "ttbb");

}
