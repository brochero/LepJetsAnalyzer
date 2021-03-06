#include "../Plots.h"

std::vector<histos> loadhistogramsSys(TString plots, TString namefile, TString SysVar);
void setuphistograms(std::vector<histos> histos, int color, int line);

void Plotter(TString plots="2btag", bool LogScale=false, TString cat = "ttbb", TString nSyst = "btagcfI"){
 
  TString files  = "../" + dirnameIn + fl;

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
  
  int col_Nom  = 1;
  int col_Up   = 2;
  int col_Down = 4;
  
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
  std::vector<histos> ttbar_Nom;
  std::vector<histos> ttbar_Up;
  std::vector<histos> ttbar_Down;

  ttbar_Nom  = loadhistogramsSys(plots, files + "_ttbar_LepJetsPowhegPythia" + cat,"central");
  ttbar_Up   = loadhistogramsSys(plots, files + "_ttbar_LepJetsPowhegPythia" + cat, nSyst + "Up");
  ttbar_Down = loadhistogramsSys(plots, files + "_ttbar_LepJetsPowhegPythia" + cat, nSyst + "Down");
  setuphistograms(ttbar_Nom,  col_Nom,   1);
  setuphistograms(ttbar_Up,   col_Up,    2);
  setuphistograms(ttbar_Down, col_Down,  3);

  
  /****************
     Draw Histos
  ****************/ 
  TCanvas *histocanvas;
  histocanvas = new TCanvas("plots", "Plots");

  for(unsigned int h=0; h<ttbar_Nom.size(); h++){
    for(int ch=0; ch<3; ch++){
      
      histocanvas->cd();
      if(LogScale) histocanvas->cd()->SetLogy();

      ttbar_Nom[h].hist[ch]->GetXaxis()->SetRange(ttbar_Nom[h].hist[ch]->GetXaxis()->GetFirst(), 
  						ttbar_Nom[h].hist[ch]->GetXaxis()->GetLast());
      //ttbar_Nom[h].hist[ch]->GetYaxis()->SetTitle("Events");
      ttbar_Nom[h].hist[ch]->GetYaxis()->SetTitleOffset(1.2);
      ttbar_Nom[h].hist[ch]->GetYaxis()->SetTitleSize(0.07);
      ttbar_Nom[h].hist[ch]->GetYaxis()->SetLabelSize(0.055);
      ttbar_Nom[h].hist[ch]->GetYaxis()->SetNdivisions(607);
      //ttbar_Nom[h].hist[ch]->GetYaxis()->SetLabelSize(0.05);
      TGaxis *hYaxis = (TGaxis*)ttbar_Nom[h].hist[ch]->GetYaxis();
      //hYaxis->SetMaxDigits(3);
      //ttbar_Nom[h].hist[ch]->GetXaxis()->SetLabelSize(0.0);
      //ttbar_Nom[h].hist[ch]->GetXaxis()->SetTitle("");
      ttbar_Nom[h].hist[ch]->GetXaxis()->SetNdivisions(509); //(402)
      ttbar_Nom[h].hist[ch]->GetXaxis()->SetTitleOffset(1.1);

      // Produce enough vertical space for the legend 
      float MaxHisto;
      if(LogScale) MaxHisto = 8.0;
      else MaxHisto = 1.4;
            float maxh;
  	    maxh = ttbar_Nom[h].hist[ch]->GetMaximum();
  	    if (maxh < ttbar_Up[h].hist[ch]->GetMaximum()) maxh = ttbar_Up[h].hist[ch]->GetMaximum();
  	    if (maxh < ttbar_Down[h].hist[ch]->GetMaximum()) maxh = ttbar_Down[h].hist[ch]->GetMaximum();
	    
      ttbar_Nom[h].hist[ch]->SetMaximum(MaxHisto*maxh);
      
      //ttbar_Nom[h].hist[ch]->SetMinimum(0.1);
      
      ttbar_Nom[h].hist[ch]->Draw("hist");
      ttbar_Up[h].hist[ch]->Draw("histSAME");
      ttbar_Down[h].hist[ch]->Draw("histSAME");
      

      //ttbar_1[h].hist[ch]->Draw("histoSAME");

      /***********************
             Legends
      ***********************/
      TLegend *leg;
      float legx1=0.70;
      float legy1=0.75;
      float legx2=0.93;
      float legy2=0.93;
      leg = new TLegend(legx1,legy1,legx2,legy2);
      leg->SetFillColor(0);
      leg->SetLineColor(1);
      leg->SetTextFont(62);
      leg->SetTextSize(0.03);

      leg->AddEntry((TObject*)0, cat + ": " + nSyst,"");
      leg->AddEntry((TObject*)0,"","");
      leg->AddEntry(ttbar_Nom[h].hist[ch],"Nom","l");
      leg->AddEntry(ttbar_Up[h].hist[ch],"Up","l");
      leg->AddEntry(ttbar_Down[h].hist[ch],"Down","l");
      //leg->AddEntry((TObject*)0,"","");
      leg->Draw("SAME");

      //-------------------------------------------------------
      // CMS Legend
      //-------------------------------------------------------
      TString htitleCMSChannel[3];
      htitleCMSChannel[0] = "#mu^{#pm}+jets channel";
      htitleCMSChannel[1] = "e^{#pm}+jets channel";
      htitleCMSChannel[2] = "l^{#pm}+jets channel";
      
      TLatex *titlePr;
      titlePr  = new TLatex(-20.,50.,"Preliminary");
      titlePr->SetNDC();
      titlePr->SetTextAlign(12);
      titlePr->SetX(0.25);
      titlePr->SetY(0.97);
      titlePr->SetTextColor(2);
      titlePr->SetTextFont(42);
      titlePr->SetTextSize(0.05);
      titlePr->SetTextSizePixels(24);
      titlePr->Draw("SAME");

      TLatex *title;
      title  = new TLatex(-20.,50.,"CMS #sqrt{s} = 13TeV, L = 36.5 fb^{-1}");
      title->SetNDC();
      title->SetTextAlign(12);
      title->SetX(0.20);
      title->SetY(0.80);
      title->SetTextFont(42);
      title->SetTextSize(0.03);
      title->SetTextSizePixels(24);
      title->Draw("SAME");
      
      TLatex *chtitle;
      chtitle  = new TLatex(-20.,50.,htitleCMSChannel[ch]);
      chtitle->SetNDC();
      chtitle->SetTextAlign(12);
      chtitle->SetX(0.20);
      chtitle->SetY(0.86);
      chtitle->SetTextFont(42);
      chtitle->SetTextSize(0.03);
      chtitle->SetTextSizePixels(24);
      chtitle->Draw("SAME");

            
      /***********************
            Save Histos
      ***********************/    
      TString dirfigname_log;
      if(LogScale) dirfigname_log = "_log";
      else dirfigname_log = "";
      TString dirfigname_pdf;
      TString dirfigname_png;
      dirfigname_pdf = "../" + dirnameIn + "figuresNomUpDown_" + fl + "/ttbbNomUpDown_" + cat + "/pdf" + dirfigname_log + "/";
      //dirfigname_png = dirnameIn + "figuresSystComp_" + fl + "/ttbb/png" + dirfigname_log + "/";
      // make a dir if it does not exist!!
      gSystem->mkdir(dirfigname_pdf,       kTRUE);
      histocanvas->SaveAs(dirfigname_pdf + nSyst + "_" + ttbar_Nom[h].hist[ch]->GetName() + ".pdf");
      //gSystem->mkdir(dirfigname_png,       kTRUE);
      //histocanvas->SaveAs(dirfigname_png + WJets[h].hist[ch]->GetName() + ".png");
      
      // clear Canvas
      histocanvas->Clear();    

    }
  }
  
} //end Plots.C

void setuphistograms(std::vector<histos> histos, int color, int line){
  
  for(unsigned int h = 0; h < histos.size(); h++){
    for(unsigned int ch=0; ch<3; ch++){  
      
      // Overflows in the last bin
      histos[h].hist[ch]->SetBinContent(histos[h].hist[ch]->GetNbinsX(),
					(histos[h].hist[ch]->GetBinContent(histos[h].hist[ch]->GetNbinsX()+1) + 
					 histos[h].hist[ch]->GetBinContent(histos[h].hist[ch]->GetNbinsX())
					 )
					);
      // !!!!!!!!!!!!!!!!!!!!!
      // Temporal Rebinning
      // !!!!!!!!!!!!!!!!!!!!!
      //histos[h].hist[ch]->Rebin(2);
      // Set overflows to 0
      histos[h].hist[ch]->SetBinContent(histos[h].hist[ch]->GetNbinsX()+1, 0);
      float Norm = histos[h].hist[ch]->Integral();
      if (Norm != 0.0) Norm = 1.0/Norm;
      else Norm = 0.0;
      //histos[h].hist[ch]->Scale(Norm);

      // Style
      // if(color == kRed+1)  histos[h].hist[ch]->SetLineColor(1); // Except ttbar
      // else 
      histos[h].hist[ch]->SetLineColor(color);
      //histos[h].hist[ch]->SetLineColor(1); 
      histos[h].hist[ch]->SetLineWidth(2); 
      histos[h].hist[ch]->SetLineStyle(line); 
      //histos[h].hist[ch]->SetFillColor(color);
      //histos[h].hist[ch]->SetFillStyle(1001);

    }    
  }    
}


std::vector<histos> loadhistogramsSys(TString plots, TString namefile, TString SysVar){

  TString htailname ="";
  TFile *file=NULL; // new TFile(namefile);
  if(SysVar == "central") file = TFile::Open(namefile + ".root");
  else{
    file = TFile::Open(namefile + "_SYS_" + SysVar + ".root");
    htailname = "_" + SysVar;
  }

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
  // histoname.push_back("hPV");
  // histoname.push_back("hMET");
  // histoname.push_back("hMET_Phi");
  // histoname.push_back("hLepPt");
  // histoname.push_back("hLepEta");
  // histoname.push_back("hLepPhi");
  // histoname.push_back("hJetPt_Jet-0");
  // histoname.push_back("hJetPt_Jet-1");
  // histoname.push_back("hJetPt_Jet-2");
  // histoname.push_back("hJetPt_Jet-3");
  // histoname.push_back("hJetPt_Jet-4");
  // histoname.push_back("hJetPt_Jet-5");
  // histoname.push_back("hNJets");
  // histoname.push_back("hNBtagJets");
  // histoname.push_back("hCSV_Jet-0");
  // histoname.push_back("hCSV_Jet-1");
  // histoname.push_back("hCSV_Jet-2");
  // histoname.push_back("hCSV_Jet-3");
  // histoname.push_back("hCSV_Jet-4");
  // histoname.push_back("hCSV_Jet-5");
  histoname.push_back("hKinAdd1CSV");
  histoname.push_back("hKinAdd2CSV");
  // histoname.push_back("hInvMassjj");
  // histoname.push_back("hmT");


  for(unsigned int h=0; h<histoname.size(); h++){
    for(unsigned int ch=0; ch<2; ch++){ 
      TString FullhName = SysVar + "/" + plots + "/" + channel[ch] + "/" +  histoname[h] + "_" + channel[ch] + "_" + plots + htailname;
      histofile.hist[ch] = (TH1D*)file->Get(FullhName);
    }

    // lep + jets
    histofile.hist[2] = (TH1D*)histofile.hist[0]->Clone();
    histofile.hist[2]->Add(histofile.hist[0], histofile.hist[1]);
    histofile.hist[2]->SetName(histoname[h] + "_" + channel[2] + "_" + plots);

    sample.push_back(histofile);
    
  }  

  cout << "All the histograms from " << namefile << ".root have been loaded successfully!!!"  << endl; 
  return sample;
}


void PlotsSystVariation(TString namesysvar = "PileUp"){
  
  Plotter("2btag", false, "ttbb", namesysvar);
  Plotter("2btag", false, "ttcc", namesysvar);
  Plotter("2btag", false, "ttbj", namesysvar);
  Plotter("2btag", false, "ttLF", namesysvar);
  Plotter("2btag", false, "ttjj", namesysvar);
}
