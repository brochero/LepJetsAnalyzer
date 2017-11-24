#include "tdrstyle.C"

void TheoSystVar(TString FileVersion, TString ttbarcat = "ttbb", TString SysName = ""){

  TString InpBaseFile = "hSF-" + FileVersion + "_Tree_LepJets_FallSkim_v8-0-6_Spring16-80X_36814pb-1_ttbar_LepJetsPowhegPythia";

  TString dirname[3]     = {"central",
			    SysName + "Up",
			    SysName + "Down"};

  TString gFileName = "TopResults/" + FileVersion + "/" + InpBaseFile + ttbarcat;
  TString inputfile[3] = {gFileName, 
			  gFileName + "_SYS_" + dirname[1],
			  gFileName + "_SYS_" + dirname[2]}; 
  
  if (SysName=="ISR" || SysName=="FSR" || SysName=="UE"){
    inputfile[1] = gFileName + "_SYS_Theory";
    inputfile[2] = gFileName + "_SYS_Theory";
  }

  setTDRStyle();

  gROOT->SetStyle("Plain");
  gStyle->SetOptFit(1000);
  gStyle->SetOptStat("emruo");
  gStyle->SetOptStat(kFALSE);
  gStyle->SetPadTickY(1);
  gStyle->SetPadTickX(1);

  int sysvarcolor[3] = {1,2,4};   

  TString chname[2]      = {"mujets","ejets"};    
  TString titlechname[2] = {"#mu+Jets","e+Jets"}; 

  std::vector<TH1D*> histo[2][3];

  TFile *hfile[3] = {NULL,NULL,NULL};
  cout << "Loading histograms... " << endl;


  for(int ich=0;ich<2;ich++){
    
    // Histograms for each region
    for(int is=0;is<3;is++){
      
      hfile[is] = TFile::Open(inputfile[is] + ".root");
      if (!hfile[is]) {
	cout << "File not found!" << endl;
	exit(0);
      }      
      cout << dirname[is] + "/2btag/" + chname[ich] + "/" + "hKinAddCSVUnroll_" + chname[ich] + "_2btag" << endl;
      TString HistoPathName;
      HistoPathName = dirname[is] + "/2btag/" + chname[ich] + "/" + "hKinAddCSVUnroll_" + chname[ich] + "_2btag";
      if (is > 0) HistoPathName += "_" + dirname[is]; 
      TH1D *htemp = (TH1D*) hfile[is]->Get(HistoPathName)->Clone("c_reg");
      
      for(int ireg=0;ireg<20;ireg++){
	TH1D *htempreg = new TH1D ("","",20,0,20);       
	for(int ibin=1;ibin<=20;ibin++){
	  htempreg->SetBinContent(ibin,htemp->GetBinContent(ibin+20*ireg));	  
	  htempreg->SetLineColor(sysvarcolor[is]);
	} // for(ibin)
	histo[ich][is].push_back(htempreg);
      } // for(ireg)
    } // for(ih)
    
    
    // -----------
    // Plotting
    // -----------
        
    TLegend *leg;
    float legPos[4] = {0.70,  // x_o
		       0.64,  // y_o
		       0.89,  // x_f
		       0.87}; // y_f
    
    leg = new TLegend(legPos[0],legPos[1],legPos[2],legPos[3]);
    leg->SetFillColor(0);
    leg->SetLineColor(0);
    leg->SetLineWidth(0.0);
    leg->SetTextFont(62);
    leg->SetTextSize(0.08);
    
    leg->SetHeader(SysName + " variation");
    leg->AddEntry(histo[ich][0].at(0),       "Nominal","L");
    leg->AddEntry(histo[ich][1].at(0),       "Up",     "L");
    leg->AddEntry(histo[ich][2].at(0),       "Down",   "L");
    
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
    
    TLatex *title;
    title  = new TLatex(-20.,50.,"CMS(2016) #sqrt{s} = 13TeV, L = 36.8 fb^{-1}");
    title->SetNDC();
    title->SetTextAlign(12);
    title->SetX(0.20);
    title->SetY(0.83);
    title->SetTextFont(42);
    title->SetTextSize(0.05);
    title->SetTextSizePixels(24);
  
    TLatex *chtitle;
    chtitle  = new TLatex(-20.,50.,titlechname[ich]+" channel");
    chtitle->SetNDC();
    chtitle->SetTextAlign(12);
    chtitle->SetX(0.20);
    chtitle->SetY(0.75);
    chtitle->SetTextFont(42);
    chtitle->SetTextSize(0.05);
    chtitle->SetTextSizePixels(24);


    TString dirfigname_pdf = "TopResults/FiguresSystVariationsUnroll/" + FileVersion + "/" + InpBaseFile + ttbarcat + "/" + SysName  + "/";
    // make a dir if it does not exist!!
    gSystem->mkdir(dirfigname_pdf, kTRUE);

    // Plots by Regions
    int ireg = 0;
    for(int ican=0;ican<4;ican++){

      TString CanNum;
      CanNum.Form("%i",ican); 
      TCanvas *cPlots_reg;//histos
      cPlots_reg = new TCanvas("cPlots_reg" ,"Plots By regions");
      cPlots_reg->Divide(2,5);
      
      for(int icr=1;icr<=10;icr+=2){
             
	TH1D *hstyle_reg = (TH1D*)histo[0][0].at(0)->Clone("style"); 
	hstyle_reg -> Reset();
	hstyle_reg -> SetMaximum(1.6*histo[ich][0].at(ireg)->GetMaximum());

	cPlots_reg->cd(icr);
	hstyle_reg->Draw();
	histo[ich][0].at(ireg) -> Draw("SAME"); 
	histo[ich][1].at(ireg) -> Draw("SAME"); 
	histo[ich][2].at(ireg) -> Draw("SAME"); 
	titlePr->Draw("SAME");
	title->Draw("SAME");
	chtitle->Draw("SAME");
	leg->Draw("SAME");
 	
	TH1D *hstyleLog_reg = (TH1D *)hstyle_reg->Clone();
      	hstyleLog_reg -> SetMaximum(5.0*histo[ich][0].at(ireg)->GetMaximum());
	hstyleLog_reg -> SetMinimum(1);

	cPlots_reg->cd(icr+1);
	cPlots_reg->cd(icr+1)->SetLogy();      
	hstyleLog_reg -> Draw();      
	histo[ich][0].at(ireg) -> Draw("SAME"); 
	histo[ich][1].at(ireg) -> Draw("SAME"); 
	histo[ich][2].at(ireg) -> Draw("SAME"); 
	titlePr->Draw("SAME");
	title->Draw("SAME");
	chtitle->Draw("SAME");

	ireg++;
      } // for(ireg) 

      cPlots_reg->SaveAs(dirfigname_pdf + "RegionHisto_" + CanNum + "_" + chname[ich] + "_NormLog.pdf");
    } // for(ican) 

  }// for(ich)
  
}

void UnRollTheSystVar(TString FileVersion="JESCom-v0", TString SysName="ScaleRdF"){

  TheoSystVar(FileVersion, "ttbb", SysName);
  TheoSystVar(FileVersion, "ttbj", SysName);
  TheoSystVar(FileVersion, "ttcc", SysName);
  TheoSystVar(FileVersion, "ttLF", SysName);
  TheoSystVar(FileVersion, "ttjj", SysName);

}
