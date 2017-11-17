#include "Plots.h"

typedef struct histos2D{
  TH2D     *hist[3]; 
} histos2D;

std::vector<histos2D> loadhistograms2D(TString plots, TString namefile);
 
void TwoDimPlots(TString plots="2btag", TString Cat = "ttbb") {
  
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
  
  
  TString files  = dirnameIn + fl;  
  
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
  // ttbar categorization 
  std::vector<histos2D> ttbarCat;
  ttbarCat = loadhistograms2D(plots, files + "_ttbar_LepJetsPowhegPythia" + Cat);  
  
  /****************
     Draw Histos
  ****************/ 
  TCanvas *histocanvas;
  histocanvas = new TCanvas("plots", "Plots");
  
  for(unsigned int h=0; h<ttbarCat.size(); h++){
    for(int ch=0; ch<3; ch++){
      
      ttbarCat[h].hist[ch]->SetTitle("");
      ttbarCat[h].hist[ch]->GetYaxis()->SetTitle("CSVv2 for the second additional jet");
      ttbarCat[h].hist[ch]->GetXaxis()->SetTitle("CSVv2 for the first additional jet");
      ttbarCat[h].hist[ch]->SetContour(30);
      ttbarCat[h].hist[ch]->Draw("COLZ20");

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
      chtitle  = new TLatex(-20.,50.,Cat + " in " + htitleCMSChannel[ch]);
      chtitle->SetNDC();
      chtitle->SetTextAlign(12);
      chtitle->SetX(0.20);
      chtitle->SetY(0.75);
      chtitle->SetTextFont(42);
      chtitle->SetTextSize(0.05);
      chtitle->SetTextSizePixels(24);
      chtitle->Draw("SAME");

      /***********************
            Save Histos
      ***********************/    
      TString dirfigname_pdf;
      dirfigname_pdf = dirnameIn + "figures_" + fl + "/ttbb/pdf2D/";
      gSystem->mkdir(dirfigname_pdf,       kTRUE);
      histocanvas->SaveAs(dirfigname_pdf + ttbarCat[h].hist[ch]->GetName() + "_" + Cat + ".pdf");
      
      // clear Canvas
      histocanvas->Clear();    

    }
  }
  
} //end 2DPlots.C

std::vector<histos2D> loadhistograms2D(TString plots, TString namefile){

  TFile *file=NULL; // new TFile(namefile);
  file = TFile::Open(namefile + ".root");
  
  if(!file){
    std::cerr << "ERROR: Could not open " <<  namefile  << " files!!!"  << std::endl;
    std::exit(0);
  }
  
  std::vector<histos2D> sample;
  histos2D histofile;
  
  TString channel[3];
  channel[0]="mujets";
  channel[1]="ejets";
  channel[2]="lepjets";
  
  std::vector<TString> histoname;
  
  // Histograms
  histoname.push_back("h2DKinAddCSV");

  for(unsigned int h=0; h<histoname.size(); h++){
    for(unsigned int ch=0; ch<2; ch++) histofile.hist[ch] = (TH2D*)file->Get("central/" + plots + "/" + channel[ch] + "/" +  histoname[h] + "_" + channel[ch] + "_" + plots);
    // lep + jets
    histofile.hist[2] = (TH2D*)histofile.hist[0]->Clone();
    histofile.hist[2]->Add(histofile.hist[0], histofile.hist[1]);
    histofile.hist[2]->SetName(histoname[h] + "_" + channel[2] + "_" + plots);

    sample.push_back(histofile);
    
  }  

  cout << "All the histograms from " << namefile << ".root have been loaded successfully!!!"  << endl; 
  return sample;
}

