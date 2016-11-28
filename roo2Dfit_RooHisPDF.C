#include "roo2Dfit_RooHisPDF.h"

void roo2Dfit_RooHisPDF(TString nModel = "RttbCon"){

  //setTDRStyle();
  gSystem->Load("libRooFit") ;
  using namespace RooFit;

  HistoFit InFile[18];
  float ratio[15];

  // Data
  InFile[data] = LoadSample("DataSingleLep");

  // MC: Signal
  InFile[ttbb]   = LoadSample("ttbar_LepJetsPowhegPythiaTranche3ttbb");
  InFile[ttb]    = LoadSample("ttbar_LepJetsPowhegPythiaTranche3ttbj");
  InFile[ttcc]   = LoadSample("ttbar_LepJetsPowhegPythiaTranche3ttcc");
  InFile[ttLF]   = LoadSample("ttbar_LepJetsPowhegPythiaTranche3ttLF"); // Includes ttc
  InFile[ttccLF] = LoadSample("ttbar_LepJetsPowhegPythiaTranche3ttccLF");

  // MC: Backgrounds
  InFile[Bkgtt]    = LoadSample("ttbar_PowhegPythiaBkgtt"); // ttbarBkg + tt
  InFile[BkgOther] = LoadSample("BkgOther"); 

  TString dirfigname_pdf;
  dirfigname_pdf = dirnameIn + "figures_" + fl + "/ttbbFITNuisance" + nModel + "/pdf/";
  // make a dir if it does not exist!!
  gSystem->mkdir(dirfigname_pdf,       kTRUE);
  TString Rfile =  dirnameIn + "figures_" + fl + "/ttbbFITNuisance" + nModel + "/FitResultImp.log";
  FILE* fResult = fopen(Rfile, "w");


  // ----------------------------------------------------------------------------------
  // Initial Parameters
  // ----------------------------------------------------------------------------------
  // Cross Sections from MC 
  RooRealVar *Vis_Xsecttjj   = new RooRealVar("Vis_Xsecttjj",  "ttjj cross section Vis Ph-Sp",    5.10, 1.0, 10.0);
  RooRealVar *Vis_Xsecttbb   = new RooRealVar("Vis_Xsecttbb",  "ttbb cross section Vis Ph-Sp",    0.07, 0.001, 0.5);
  RooRealVar *Full_Xsecttjj  = new RooRealVar("Full_Xsecttjj", "ttjj cross section Full Ph-Sp", 257.0,  150.0, 350.0);
  RooRealVar *Full_Xsecttbb  = new RooRealVar("Full_Xsecttbb", "ttbb cross section Full Ph-Sp",   3.2,  2.0, 5.0);
  // Ratios Xsecttbb/Xsecttjj    
  RooFormulaVar *Vis_Xsecttbb_Xsecttjj  = new RooFormulaVar("Vis_Xsecttbb_Xsecttjj",  "Xsecttbb/Xsecttjj Vis-PhSp",  
							    "Vis_Xsecttbb/Vis_Xsecttjj",   RooArgList(*Vis_Xsecttbb,*Vis_Xsecttjj));
  RooFormulaVar *Full_Xsecttbb_Xsecttjj = new RooFormulaVar("Full_Xsecttbb_Xsecttjj", "Xsecttbb/Xsecttjj Full-PhSp", 
							    "Full_Xsecttbb/Full_Xsecttjj", RooArgList(*Full_Xsecttbb,*Full_Xsecttjj));

  // Efficiencies ttjj
  RooRealVar *Effttjj_nom[3];
  Effttjj_nom[0] = new RooRealVar("Effttjj_nom_" + name_ch[0],  "Nom. ttjj Efficiency for "  + name_ch[0],  0.4359);
  Effttjj_nom[1] = new RooRealVar("Effttjj_nom_" + name_ch[2],  "Nom. ttjj Efficiency for "  + name_ch[1],  0.3710);
  Effttjj_nom[2] = new RooRealVar("Effttjj_nom_" + name_ch[3],  "Nom. ttjj Efficiency for "  + name_ch[2],  0.4034);
  // Efficiencies ttbb
  RooRealVar *Effttbb_nom[3];
  Effttbb_nom[0] = new RooRealVar("Effttbb_nom_" + name_ch[0],  "Nom. ttbb Efficiency for "  + name_ch[0],  0.1885);
  Effttbb_nom[1] = new RooRealVar("Effttbb_nom_" + name_ch[2],  "Nom. ttbb Efficiency for "  + name_ch[1],  0.1583);
  Effttbb_nom[2] = new RooRealVar("Effttbb_nom_" + name_ch[3],  "Nom. ttbb Efficiency for "  + name_ch[2],  0.1734);

  // Aceptancies ttjj
  RooRealVar *Accttjj_nom[3];
  Accttjj_nom[0] = new RooRealVar("Accttjj_nom_" + name_ch[0],  "Nom. ttjj Acceptancy for "  + name_ch[0],  0.322);
  Accttjj_nom[1] = new RooRealVar("Accttjj_nom_" + name_ch[2],  "Nom. ttjj Acceptancy for "  + name_ch[1],  0.320);
  Accttjj_nom[2] = new RooRealVar("Accttjj_nom_" + name_ch[3],  "Nom. ttjj Acceptancy for "  + name_ch[2],  0.320);
  // Aceptancies ttbb
  RooRealVar *Accttbb_nom[3];
  Accttbb_nom[0] = new RooRealVar("Accttbb_nom_" + name_ch[0],  "Nom. ttbb Acceptancy for "  + name_ch[0],  0.276);
  Accttbb_nom[1] = new RooRealVar("Accttbb_nom_" + name_ch[2],  "Nom. ttbb Acceptancy for "  + name_ch[1],  0.275);
  Accttbb_nom[2] = new RooRealVar("Accttbb_nom_" + name_ch[3],  "Nom. ttbb Acceptancy for "  + name_ch[2],  0.275);

  // Luminosity
  RooRealVar *Lumi_nom  = new RooRealVar("Lumi_nom",  "Luminosity",  15000);


  for (unsigned int ch=0; ch<1; ch++){
    
    // Variables CSV2 and CSV3
    // + 0.1 to avoid empty bins
    RooRealVar *CSV2 = new RooRealVar("CSV2", "CSV for Jet 3", InFile[ttbb].hist1D[0][0]->GetXaxis()->GetXmin()+0.1, InFile[ttbb].hist1D[0][0]->GetXaxis()->GetXmax()); 
    RooRealVar *CSV3 = new RooRealVar("CSV3", "CSV for Jet 4", InFile[ttbb].hist1D[1][0]->GetXaxis()->GetXmin()+0.1, InFile[ttbb].hist1D[1][0]->GetXaxis()->GetXmax());  

    float n_ttjj = InFile[ttbb].events[ch] + 
      InFile[ttcc].events[ch] + 
      InFile[ttb].events[ch]  + 
      InFile[ttLF].events[ch];    

    ratio[ttbb]    = InFile[ttbb].events[ch]/n_ttjj; 
    ratio[ttb]     = InFile[ttb].events[ch]/n_ttjj; 
    ratio[ttccLF]  = InFile[ttccLF].events[ch]/n_ttjj; 

    cout << "----- Ratios from MC -----" << endl;
    cout << "ttbb/ttjj   = " << ratio[ttbb]   << endl;
    cout << "ttbj/ttjj   = " << ratio[ttb]    << endl;
    cout << "ttccLF/ttjj = " << ratio[ttccLF] << endl;
    cout << "--------------------------" << endl;
    
   
    // Ratio ttbj/ttbb
    RooRealVar *CRatio_ttbjttbb = new RooRealVar("CRatio_ttbjttbb", "MC ratio ttbj/ttbb",  InFile[ttb].events[ch]/InFile[ttbb].events[ch],InFile[ttb].events[ch]/InFile[ttbb].events[ch],InFile[ttb].events[ch]/InFile[ttbb].events[ch]);
    // Fitted ratio
    RooRealVar *FRatio_ttbbttjj = new RooRealVar("FRatio_ttbbttjj", "FITTED ratio ttbb/ttjj",   ratio[ttbb], 0.0, 0.2); 

    // Signal: Nttjj
    RooRealVar *n_ttjj_var = new RooRealVar("n_ttjj_var",   "number of ttjj events", n_ttjj, n_ttjj, n_ttjj);    
    // Background
    RooRealVar *n_Bkgtt_var    = new RooRealVar("n_Bkgtt_var",    "number of tt background events",    InFile[Bkgtt].events[ch]);
    RooRealVar *n_BkgOther_var = new RooRealVar("n_BkgOther_var", "number of Other background events", InFile[BkgOther].events[ch]);

    // 2D Arguments
    RooArgList *arg_CSV = new RooArgList(*CSV2, *CSV3);

    // ----------------------------------------------------------------------------------
    // ----------------------------------------------------------------------------------

    RooDataHist *DataHis[18][3];

    //////////
    // CSV2 //
    //////////
    // histograms: ttjj    
    DataHis[ttbb][0]   = new RooDataHist("ttbb_his2",   "ttbb Histogram",   
					 *CSV2, InFile[ttbb].hist1D[0][ch]);
    DataHis[ttb][0]    = new RooDataHist("ttb_his2",    "ttb Histogram",    
					 *CSV2, InFile[ttb].hist1D[0][ch]);
    DataHis[ttccLF][0] = new RooDataHist("ttccLF_his2", "ttccLF Histogram", 
					 *CSV2, InFile[ttccLF].hist1D[0][ch]);
    // histograms: Bkg    
    DataHis[Bkgtt][0]    = new RooDataHist("Bkgtt_his2",    "Bkgtt Histogram",     
					   *CSV2, InFile[Bkgtt].hist1D[0][ch]);
    DataHis[BkgOther][0] = new RooDataHist("BkgOther_his2", "BkgOther Histogram",    
					   *CSV2, InFile[BkgOther].hist1D[0][ch]);
    
    //////////
    // CSV3 //
    //////////
    // histograms: ttjj    
    DataHis[ttbb][1]   = new RooDataHist("ttbb_his3",   "ttbb Histogram",   
					 *CSV3, InFile[ttbb].hist1D[1][ch]);
    DataHis[ttb][1]    = new RooDataHist("ttb_his3",    "ttb Histogram",    
					 *CSV3, InFile[ttb].hist1D[1][ch]);
    DataHis[ttccLF][1] = new RooDataHist("ttccLF_his3", "ttccLF Histogram", 
					 *CSV3, InFile[ttccLF].hist1D[1][ch]);
    // histograms: Bkg    
    DataHis[Bkgtt][1]    = new RooDataHist("Bkgtt_his3",    "Bkgtt Histogram",     
					   *CSV3, InFile[Bkgtt].hist1D[1][ch]);
    DataHis[BkgOther][1] = new RooDataHist("BkgOther_his3", "BkgOther Histogram",    
					   *CSV3, InFile[BkgOther].hist1D[1][ch]);
    
    ///////////////
    // CSV2-CSV3 //
    ///////////////
    // histograms: ttjj    
    DataHis[ttbb][2]   = new RooDataHist("ttbb_his23",   "ttbb Histogram",   
					 *arg_CSV, InFile[ttbb].hist2D[ch]);
    DataHis[ttb][2]    = new RooDataHist("ttb_his23",    "ttb Histogram",    
					 *arg_CSV, InFile[ttb].hist2D[ch]);
    DataHis[ttccLF][2] = new RooDataHist("ttccLF_his23", "ttccLF Histogram", 
					 *arg_CSV, InFile[ttccLF].hist2D[ch]);
    // histograms: Bkg    
    DataHis[Bkgtt][2]    = new RooDataHist("Bkgtt_his23",    "Bkgtt Histogram",     
					   *arg_CSV, InFile[Bkgtt].hist2D[ch]);
    DataHis[BkgOther][2] = new RooDataHist("BkgOther_his23", "BkgOther Histogram",    
					   *arg_CSV, InFile[BkgOther].hist2D[ch]);
    
    // ----------------------------------------------------------------------------------
    // ----------------------------------------------------------------------------------
    
    RooHistPdf *HisPdf[18][3];

    //////////
    // CSV2 //
    //////////
    // pdf: ttjj 
    HisPdf[ttbb][0]   = new RooHistPdf("ttbb_hispdf2",   "PDF for ttbb",
				       RooArgSet(*CSV2), *DataHis[ttbb][0]);
    HisPdf[ttb][0]    = new RooHistPdf("ttb_hispdf2",   "PDF for ttb",
				       RooArgSet(*CSV2), *DataHis[ttb][0]);
    HisPdf[ttccLF][0] = new RooHistPdf("ttccLF_hispdf2",   "PDF for ttccLF",
				       RooArgSet(*CSV2), *DataHis[ttccLF][0]);
    // pdf: Bkg
    HisPdf[Bkgtt][0]    = new RooHistPdf("Bkgtt_hispdf2",    "PDF for ttbar Bkg", 
					 RooArgSet(*CSV2), *DataHis[Bkgtt][0]);
    HisPdf[BkgOther][0] = new RooHistPdf("BkgOther_hispdf2", "PDF for Other Bkg", 
					 RooArgSet(*CSV2), *DataHis[BkgOther][0]);
    
    //////////
    // CSV3 //
    //////////
    // pdf: ttjj 
    HisPdf[ttbb][1]   = new RooHistPdf("ttbb_hispdf3",   "PDF for ttbb",
				       RooArgSet(*CSV3), *DataHis[ttbb][1]);
    HisPdf[ttb][1]    = new RooHistPdf("ttb_hispdf3",   "PDF for ttb",
				       RooArgSet(*CSV3), *DataHis[ttb][1]);
    HisPdf[ttccLF][1] = new RooHistPdf("ttccLF_hispdf3",   "PDF for ttccLF",
				       RooArgSet(*CSV3), *DataHis[ttccLF][1]);
    // pdf: Bkg
    HisPdf[Bkgtt][1]    = new RooHistPdf("Bkgtt_hispdf3",    "PDF for ttbar Bkg", 
					 RooArgSet(*CSV3), *DataHis[Bkgtt][1]);
    HisPdf[BkgOther][1] = new RooHistPdf("BkgOther_hispdf3", "PDF for Other Bkg", 
					 RooArgSet(*CSV3), *DataHis[BkgOther][1]);
    
    ///////////////
    // CSV2-CSV3 //
    ///////////////
    // pdf: ttjj 
    HisPdf[ttbb][2]   = new RooHistPdf("ttbb_hispdf23",   "PDF for ttbb",
				       RooArgSet(*arg_CSV), *DataHis[ttbb][2]);
    HisPdf[ttb][2]    = new RooHistPdf("ttb_hispdf23",   "PDF for ttb",
				       RooArgSet(*arg_CSV), *DataHis[ttb][2]);
    HisPdf[ttccLF][2] = new RooHistPdf("ttccLF_hispdf23",   "PDF for ttccLF",
				       RooArgSet(*arg_CSV), *DataHis[ttccLF][2]);
    // pdf: Bkg
    HisPdf[Bkgtt][2]    = new RooHistPdf("Bkgtt_hispdf23",    "PDF for ttbar Bkg", 
					 RooArgSet(*arg_CSV), *DataHis[Bkgtt][2]);
    HisPdf[BkgOther][2] = new RooHistPdf("BkgOther_hispdf23", "PDF for Other Bkg", 
					 RooArgSet(*arg_CSV), *DataHis[BkgOther][2]);

    // ----------------------------------------------------------------------------------
    // ----------------------------------------------------------------------------------

    /////////////////////////////////////
    // WorkSpace 1: Standard Fit Model //
    /////////////////////////////////////

    RooWorkspace *WS_StMo = new RooWorkspace("Fit ttbb/ttjj");

    WS_StMo->import(*CSV2);
    WS_StMo->import(*CSV3);
    WS_StMo->import(*arg_CSV);

    for(int idim = 0; idim<3; idim++){
      // Histograms
      WS_StMo->import(*DataHis[ttbb][idim]);
      WS_StMo->import(*DataHis[ttb][idim]);
      WS_StMo->import(*DataHis[ttccLF][idim]);

      WS_StMo->import(*DataHis[Bkgtt][idim]);
      WS_StMo->import(*DataHis[BkgOther][idim]);
      
      // P.D.F.
      WS_StMo->import(*HisPdf[ttbb][idim]);
      WS_StMo->import(*HisPdf[ttb][idim]);
      WS_StMo->import(*HisPdf[ttccLF][idim]);

      WS_StMo->import(*HisPdf[Bkgtt][idim]);
      WS_StMo->import(*HisPdf[BkgOther][idim]);
    }
    
    // Parameters
    WS_StMo->import(*FRatio_ttbbttjj);
    WS_StMo->import(*CRatio_ttbjttbb);

    WS_StMo->import(*n_ttjj_var);
    WS_StMo->import(*n_Bkgtt_var);
    WS_StMo->import(*n_BkgOther_var);

    // Model Implementation
    WS_StMo->factory("prod::kn_ttjj_var(k[1.0,0.1,2.0],n_ttjj_var)");

    // CSV2 
    WS_StMo->factory("SUM::ttjj2(FRatio_ttbbttjj*ttbb_hispdf2,prod(FRatio_ttbbttjj,CRatio_ttbjttbb)*ttb_hispdf2, ttccLF_hispdf2)");
    WS_StMo->factory("SUM::TotModel2(kn_ttjj_var*ttjj2,n_Bkgtt_var*Bkgtt_hispdf2,n_BkgOther_var*BkgOther_hispdf2)");
    // CSV3
    WS_StMo->factory("SUM::ttjj3(FRatio_ttbbttjj*ttbb_hispdf3,prod(FRatio_ttbbttjj,CRatio_ttbjttbb)*ttb_hispdf3, ttccLF_hispdf3)");
    WS_StMo->factory("SUM::TotModel3(kn_ttjj_var*ttjj3,n_Bkgtt_var*Bkgtt_hispdf3,n_BkgOther_var*BkgOther_hispdf3)");
    // CSV23
    WS_StMo->factory("SUM::ttjj_hispdf23(FRatio_ttbbttjj*ttbb_hispdf23,prod(FRatio_ttbbttjj,CRatio_ttbjttbb)*ttb_hispdf23, ttccLF_hispdf23)");
    WS_StMo->factory("SUM::TotModel23(kn_ttjj_var*ttjj_hispdf23, n_Bkgtt_var*Bkgtt_hispdf23, n_BkgOther_var*BkgOther_hispdf23)");

    // ----------------------------------------------------------------------------------
    // ----------------------    Systematic Uncertainties   -----------------------------
    // ----------------------------------------------------------------------------------

    RooDataHist *DataHisSys[18][3][14][3]; // [sample][case][syst][var]
    RooHistPdf  *HisPdfSys[18][3][14][3];  // [sample][case][syst][var]
    RooArgList  *arg_PdfVar[18][3][3];     // [sample][case][var]

    RooRealVar *AlphaSys[14][3]; // [syst][var]   
    RooArgList *arg_AlphaSys[3]; // [var]

    PiecewiseInterpolation *PInterSyst[18][3]; // [sample][case]
    
    for(int is=0;is<18;is++) {
      for(int ic=0;ic<3;ic++) {
	for(int iv=0;iv<3;iv++) arg_PdfVar[is][ic][iv]= new RooArgList();
      }
    }    

    for(int iv=0;iv<3;iv++) arg_AlphaSys[iv]= new RooArgList();

    // Set of Nuisance parameters    
    for(int isys=0; isys<14; isys++){      
      AlphaSys[isys][0] = new RooRealVar("AlphaSys2"  + SystNam[isys], "Alpha Syst " + SystNam[isys], -5.0, 5.0);
      AlphaSys[isys][1] = new RooRealVar("AlphaSys3"  + SystNam[isys], "Alpha Syst " + SystNam[isys], -5.0, 5.0);
      AlphaSys[isys][2] = new RooRealVar("AlphaSys23" + SystNam[isys], "Alpha Syst " + SystNam[isys], -5.0, 5.0);

      if(isys == PileUp || isys == JES){
	arg_AlphaSys[0]->add(*AlphaSys[isys][0]);
	arg_AlphaSys[1]->add(*AlphaSys[isys][1]);
	arg_AlphaSys[2]->add(*AlphaSys[isys][2]);
      }
    }
    
    for(int isam=ttbb; isam<=BkgOther; isam++){
      for(int isys=0; isys<14; isys++){
    	for(int ivar=0; ivar<3; ivar++){
	  
    	  //////////
    	  // CSV2 //
    	  //////////
    	  // histograms
    	  DataHisSys[isam][0][isys][ivar] = new RooDataHist(SamNam[isam] + "_his2" + SystNam[isys] + VarNam[ivar],   SamNam[isam] + " Histogram",
    							    *CSV2, InFile[isam].hsyst1D[isys][ivar][0][ch]);
	  // PDF
	  HisPdfSys[isam][0][isys][ivar]   = new RooHistPdf(SamNam[isam] + "_hisPdf2" + SystNam[isys] + VarNam[ivar],   SamNam[isam] + " PDF",
							    RooArgSet(*CSV2), *DataHisSys[isam][0][isys][ivar]);
	  
    	  //////////
    	  // CSV3 //
    	  //////////
    	  // histograms
    	  DataHisSys[isam][1][isys][ivar] = new RooDataHist(SamNam[isam] + "_his3" + SystNam[isys] + VarNam[ivar],   SamNam[isam] + " Histogram",
    							    *CSV3, InFile[isam].hsyst1D[isys][ivar][1][ch]);
	  // PDF
	  HisPdfSys[isam][1][isys][ivar]  = new RooHistPdf(SamNam[isam] + "_hisPdf3" + SystNam[isys] + VarNam[ivar],   SamNam[isam] + " PDF",
							   RooArgSet(*CSV3), *DataHisSys[isam][1][isys][ivar]);
	  
    	  ///////////////
    	  // CSV2-CSV3 //
    	  ///////////////
    	  // histograms    
    	  DataHisSys[isam][2][isys][ivar] = new RooDataHist(SamNam[isam] + "_his23" + SystNam[isys] + VarNam[ivar],  SamNam[isam] +  " Histogram",   
    							    *arg_CSV, InFile[isam].hsyst2D[isys][ivar][ch]);
	  // PDF
	  HisPdfSys[isam][2][isys][ivar]  = new RooHistPdf(SamNam[isam] + "_hisPdf23" + SystNam[isys] + VarNam[ivar],   SamNam[isam] + " PDF",
	  						   RooArgSet(*arg_CSV), *DataHisSys[isam][2][isys][ivar]);
	  if(isys == PileUp || isys == JES){
	    arg_PdfVar[isam][0][ivar]->add(*HisPdfSys[isam][0][isys][ivar]);
	    arg_PdfVar[isam][1][ivar]->add(*HisPdfSys[isam][1][isys][ivar]);
	    arg_PdfVar[isam][2][ivar]->add(*HisPdfSys[isam][2][isys][ivar]);	    
	  }
    	} // for(ivar)
      } // for(isys)
      
      // ----------------------------------------------------------------------------------
      // ----------------------------------------------------------------------------------
      
      //////////
      // CSV2 //
      //////////      
      PInterSyst[isam][0] = new PiecewiseInterpolation(SamNam[isam] + "_PInterSyst2", "Interpolation for "  + SamNam[isam],
						       *HisPdfSys [isam][0][0][Nom],// Nom [sample][Scen][syst][var] 
      						       *arg_PdfVar[isam][0][Down],  // Down[sample][Scen][var]  
      						       *arg_PdfVar[isam][0][Up],    // Up  [sample][Scen][var]  
      						       *arg_AlphaSys[0]);           // Nuisance[Scen]
      
      PInterSyst[isam][0]->setPositiveDefinite(kTRUE);
      
      //////////
      // CSV3 //
      //////////
      PInterSyst[isam][1] = new PiecewiseInterpolation(SamNam[isam] + "_PInterSyst3", "Interpolation for "  + SamNam[isam],
						       *HisPdfSys [isam][1][0][Nom],// Nom [sample][Scen][syst][var] 
      						       *arg_PdfVar[isam][1][Down],  // Down[sample][Scen][var]  
      						       *arg_PdfVar[isam][1][Up],    // Up  [sample][Scen][var]  
      						       *arg_AlphaSys[1]);           // Nuisance[Scen]
      
      PInterSyst[isam][1]->setPositiveDefinite(kTRUE);
      
      ///////////////
      // CSV2-CSV3 //
      ///////////////
      PInterSyst[isam][2] = new PiecewiseInterpolation(SamNam[isam] + "_PInterSyst23", "Interpolation for "  + SamNam[isam],
						       *HisPdfSys [isam][2][0][Nom],// Nom [sample][Scen][syst][var] 
      						       *arg_PdfVar[isam][2][Down],  // Down[sample][Scen][var]  
      						       *arg_PdfVar[isam][2][Up],    // Up  [sample][Scen][var]  
      						       *arg_AlphaSys[2]);           // Nuisance[Scen]
      
      PInterSyst[isam][2]->setPositiveDefinite(kTRUE);
      
    }// for(isam)
    
    // ----------------------------------------------------------------------------------
    // ----------------------------------------------------------------------------------

    /////////////////////////////////////
    // WorkSpace 2: Fit with Nuisance  //
    /////////////////////////////////////

    RooWorkspace *WS_NuMo = new RooWorkspace("Fit ttbb/ttjj with Nuisance Parameters");

    for(int idim = 0; idim<3; idim++){
      for(int isam=ttbb; isam<=BkgOther; isam++) WS_NuMo->import(*PInterSyst[isam][idim]);
    }

    // Parameters
    WS_NuMo->import(*FRatio_ttbbttjj);
    WS_NuMo->import(*CRatio_ttbjttbb);

    WS_NuMo->import(*n_ttjj_var);
    WS_NuMo->import(*n_Bkgtt_var);
    WS_NuMo->import(*n_BkgOther_var);

    WS_NuMo->factory("prod::kn_ttjj_var(k[1.0,0.1,2.0],n_ttjj_var)");

    // CSV2
    WS_NuMo->factory("ASUM::ttjj_PInterSyst2(FRatio_ttbbttjj*ttbb_PInterSyst2,prod(FRatio_ttbbttjj,CRatio_ttbjttbb)*ttb_PInterSyst2,ttccLF_PInterSyst2)");
    WS_NuMo->factory("ASUM::TotModel2_sys(kn_ttjj_var*ttjj_PInterSyst2,n_Bkgtt_var*Bkgtt_PInterSyst2,n_BkgOther_var*BkgOther_PInterSyst2)");

    // CSV3
    WS_NuMo->factory("ASUM::ttjj_PInterSyst3(FRatio_ttbbttjj*ttbb_PInterSyst3,prod(FRatio_ttbbttjj,CRatio_ttbjttbb)*ttb_PInterSyst3,ttccLF_PInterSyst3)");
    WS_NuMo->factory("ASUM::TotModel3_sys(kn_ttjj_var*ttjj_PInterSyst3,n_Bkgtt_var*Bkgtt_PInterSyst3,n_BkgOther_var*BkgOther_PInterSyst3)");

    // CSV23
    WS_NuMo->factory("ASUM::ttjj_PInterSyst23(FRatio_ttbbttjj*ttbb_PInterSyst23,prod(FRatio_ttbbttjj,CRatio_ttbjttbb)*ttb_PInterSyst23,ttccLF_PInterSyst23)");
    WS_NuMo->factory("ASUM::TotModel23_sys(kn_ttjj_var*ttjj_PInterSyst23,n_Bkgtt_var*Bkgtt_PInterSyst23,n_BkgOther_var*BkgOther_PInterSyst23)");

    // Constrain the models
    TString ModelSysName[3] = {"TotModel2_SysCons",
			       "TotModel3_SysCons",
			       "TotModel23_SysCons"};
 
    TString sTotModel2_sys  = "PROD::" + ModelSysName[0] + "(TotModel2_sys"; 
    TString sTotModel3_sys  = "PROD::" + ModelSysName[1] + "(TotModel3_sys"; 
    TString sTotModel23_sys = "PROD::" + ModelSysName[2] + "(TotModel23_sys"; 
    
    for(int isys=0; isys<14; isys++){
      if(isys == PileUp || isys == JES){
	sTotModel2_sys  += ",Gaussian(0.0,AlphaSys2"  + SystNam[isys] + ",1.0)"; 
	sTotModel3_sys  += ",Gaussian(0.0,AlphaSys3"  + SystNam[isys] + ",1.0)"; 
	sTotModel23_sys += ",Gaussian(0.0,AlphaSys23" + SystNam[isys] + ",1.0)"; 
      }
    }
    // To close the model definition
    sTotModel2_sys  += ")"; 
    sTotModel3_sys  += ")"; 
    sTotModel23_sys += ")"; 
    
    cout << sTotModel2_sys << endl;
    cout << sTotModel3_sys << endl;
    cout << sTotModel23_sys << endl;

    WS_NuMo->factory(sTotModel2_sys);
    WS_NuMo->factory(sTotModel3_sys);
    WS_NuMo->factory(sTotModel23_sys);


    WS_NuMo->Print();

    // ----------------------------------------------------------------------------------
    // ----------------------------------------------------------------------------------

    // histograms: Data 
    DataHis[data][0]   = new RooDataHist("data_his2",   "data Histogram",   
    					 *CSV2, InFile[data].hist1D[0][ch]);
    DataHis[data][1]   = new RooDataHist("data_his3",   "data Histogram",   
    					 *CSV3, InFile[data].hist1D[1][ch]);
    DataHis[data][2]   = new RooDataHist("data_his23",   "data Histogram",   
    					 *arg_CSV, InFile[data].hist2D[ch]); 


    // ----------------------------------------------------------------------------------
    // Fits (NLL Minimization)
    // ----------------------------------------------------------------------------------

    RooAbsReal *nll_StMo = WS_StMo->pdf("TotModel23")->createNLL(*DataHis[data][2]);
    RooMinimizer m_StMo(*nll_StMo);
    m_StMo.migrad();
    m_StMo.hesse();

    RooAbsReal *nll_NuMo = WS_NuMo->pdf("TotModel23_SysCons")->createNLL(*DataHis[data][2]);
    RooMinimizer m_NuMo(*nll_NuMo);
    m_NuMo.migrad();
    m_NuMo.hesse();

    // ----------------------------------------------------------------------------------
    // Plots
    // ----------------------------------------------------------------------------------
    TLegend *CSV_leg;
    float legPos[4] = {0.64,  // x_o
		       0.70,  // y_o
		       0.88,  // x_f
		       0.89}; // y_f
    CSV_leg = new TLegend(legPos[0],legPos[1],legPos[2],legPos[3]);
    RooPlot *plot_CSV2_StMo = PlotPDFModel(CSV2, (RooHistPdf*)WS_StMo->pdf("TotModel23"), DataHis[data][2], true, CSV_leg);
    RooPlot *plot_CSV3_StMo = PlotPDFModel(CSV3, (RooHistPdf*)WS_StMo->pdf("TotModel23"), DataHis[data][2], true, CSV_leg);

    TCanvas *canvas_com_StMo = new TCanvas("canvas_com_StMo",   "Model Plots");
    canvas_com_StMo->Divide(2,2);

    canvas_com_StMo->cd(1);
    plot_CSV2_StMo->SetTitle("CSV for Additional Jet 1");
    plot_CSV2_StMo->Draw();

    canvas_com_StMo->cd(2);
    plot_CSV3_StMo->SetTitle("CSV for Additional Jet 2");
    plot_CSV3_StMo->Draw();
    CSV_leg->Draw("SAME");

    canvas_com_StMo->cd(3);
    canvas_com_StMo->cd(3)->SetLogy();
    plot_CSV2_StMo->Draw();

    canvas_com_StMo->cd(4);
    canvas_com_StMo->cd(4)->SetLogy();
    plot_CSV3_StMo->Draw();
    CSV_leg->Draw("SAME");

    canvas_com_StMo->SaveAs("Canvas_com_StMo.pdf");

    RooRealVar *k_StMo = WS_StMo->var("k");
    float vk_StMo =  k_StMo->getVal();
    float vk_error_StMo =  k_StMo->getError();

    RooRealVar *FRatio_ttbbttjj_StMo = WS_StMo->var("FRatio_ttbbttjj");
    float vFRatio_ttbbttjj_StMo =  FRatio_ttbbttjj_StMo->getVal();
    float vFRatio_ttbbttjj_error_StMo =  FRatio_ttbbttjj_StMo->getError();

    RooPlot *plot_NLL_FRatio_ttbbttjj_StMo = FRatio_ttbbttjj_StMo->frame();
    nll_StMo->plotOn(plot_NLL_FRatio_ttbbttjj_StMo, ShiftToZero());

    RooPlot *plot_NLL_k_StMo = k_StMo->frame();
    nll_StMo->plotOn(plot_NLL_k_StMo, ShiftToZero());

    TCanvas *canvas_par_StMo = new TCanvas("canvas_par_StMo", "Parameters vs NLL Plots");
    canvas_par_StMo->Divide(2,1);

    float ErrorRange = 10.;
    canvas_par_StMo->cd(1);
    plot_NLL_FRatio_ttbbttjj_StMo->GetXaxis()->SetRangeUser(vFRatio_ttbbttjj_StMo - ErrorRange*vFRatio_ttbbttjj_error_StMo, 
							    vFRatio_ttbbttjj_StMo + ErrorRange*vFRatio_ttbbttjj_error_StMo);
    plot_NLL_FRatio_ttbbttjj_StMo->SetMinimum(0.0);
    plot_NLL_FRatio_ttbbttjj_StMo->SetMaximum(50.0);
    plot_NLL_FRatio_ttbbttjj_StMo->Draw();

    canvas_par_StMo->cd(2);
    plot_NLL_k_StMo->GetXaxis()->SetRangeUser(vk_StMo - ErrorRange*vk_error_StMo, 
					      vk_StMo + ErrorRange*vk_error_StMo);
    plot_NLL_k_StMo->SetMinimum(0.0);
    plot_NLL_k_StMo->SetMaximum(50.0);
    plot_NLL_k_StMo->Draw();

    canvas_par_StMo->SaveAs("Canvas_par_StMo.pdf");


    RooRealVar *k_NuMo  = WS_NuMo->var("k");
    float vk_NuMo       = k_NuMo->getVal();
    float vk_error_NuMo = k_NuMo->getError();

    RooRealVar *FRatio_ttbbttjj_NuMo  = WS_NuMo->var("FRatio_ttbbttjj");
    float vFRatio_ttbbttjj_NuMo       = FRatio_ttbbttjj_NuMo->getVal();
    float vFRatio_ttbbttjj_error_NuMo = FRatio_ttbbttjj_NuMo->getError();

    RooRealVar *AlphaSys23PileUp_NuMo  = WS_NuMo->var("AlphaSys23PileUp");
    float vAlphaSys23PileUp_NuMo       = AlphaSys23PileUp_NuMo->getVal();
    float vAlphaSys23PileUp_error_NuMo = AlphaSys23PileUp_NuMo->getError();

    RooRealVar *AlphaSys23JES_NuMo  = WS_NuMo->var("AlphaSys23JES");
    float vAlphaSys23JES_NuMo       = AlphaSys23JES_NuMo->getVal();
    float vAlphaSys23JES_error_NuMo = AlphaSys23JES_NuMo->getError();

    TLegend *CSV_leg_NuMo;

    CSV_leg_NuMo = new TLegend(legPos[0],legPos[1],legPos[2],legPos[3]);
    RooPlot *plot_CSV2_NuMo = PlotPDF_NuMo(CSV2, (RooHistPdf*)WS_NuMo->function("TotModel23_SysCons"), DataHis[data][2], true, CSV_leg_NuMo, k_NuMo, FRatio_ttbbttjj_NuMo);
    RooPlot *plot_CSV3_NuMo = PlotPDF_NuMo(CSV3, (RooHistPdf*)WS_NuMo->function("TotModel23_SysCons"), DataHis[data][2], true, CSV_leg_NuMo, k_NuMo, FRatio_ttbbttjj_NuMo);
      
    TH1 *hModel[3];
    hModel[0] = WS_NuMo->pdf(ModelSysName[0])->createHistogram("CSV2");
    hModel[1] = WS_NuMo->pdf(ModelSysName[1])->createHistogram("CSV3");
    hModel[2] = WS_NuMo->pdf(ModelSysName[2])->createHistogram("CSV2,CSV3");
       
    // TH1 *hModel2D_PileUp     = WS_NuMo->pdf("TotModel2_SysCons")->createHistogram("CSV2,AlphaSys2PileUp", 20, 100);
    // TH1 *hModel_alpha_PileUp = WS_NuMo->pdf("TotModel2_SysCons")->createHistogram("AlphaSys2PileUp",100);
    // TH1 *hInter_ttbb_PileUp  = WS_NuMo->function("ttbb_PInterSyst2")->createHistogram("CSV2,AlphaSys2PileUp");

    TCanvas *canvas_com_NuMo = new TCanvas("canvas_com_NuMo",   "Model Plots");
    canvas_com_NuMo->Divide(2,3);
    
    canvas_com_NuMo->cd(1);
    canvas_com_NuMo->cd(1)->SetTheta(50);
    canvas_com_NuMo->cd(1)->SetPhi(45);
    hModel[2]->Draw("LEGO");

    canvas_com_NuMo->cd(2);
    canvas_com_NuMo->cd(2)->SetTheta(50);
    canvas_com_NuMo->cd(2)->SetPhi(-75);
    hModel[2]->Draw("LEGO");

    canvas_com_NuMo->cd(3);
    plot_CSV2_NuMo->Draw();

    canvas_com_NuMo->cd(4);
    plot_CSV3_NuMo->Draw();
    CSV_leg_NuMo->Draw("SAME");

    canvas_com_NuMo->cd(5);
    canvas_com_NuMo->cd(5)->SetLogy();
    plot_CSV2_NuMo->Draw();

    canvas_com_NuMo->cd(6);
    canvas_com_NuMo->cd(6)->SetLogy();
    plot_CSV3_NuMo->Draw();
    CSV_leg_NuMo->Draw("SAME");

    canvas_com_NuMo->SaveAs("Canvas_com_NuMo.pdf");

    TCanvas *canvas_par_NuMo = new TCanvas("canvas_par_NuMo",   "Parameter Plots");
    canvas_par_NuMo->Divide(2,2);


    RooPlot *plot_NLL_FRatio_ttbbttjj_NuMo = FRatio_ttbbttjj_NuMo->frame();
    nll_NuMo->plotOn(plot_NLL_FRatio_ttbbttjj_NuMo, ShiftToZero());
    RooPlot *plot_NLL_k_NuMo = k_NuMo->frame();
    nll_NuMo->plotOn(plot_NLL_k_NuMo, ShiftToZero());
    RooPlot *plot_NLL_AlphaSys23PileUp_NuMo = AlphaSys23PileUp_NuMo->frame();
    nll_NuMo->plotOn(plot_NLL_AlphaSys23PileUp_NuMo, ShiftToZero());
    RooPlot *plot_NLL_AlphaSys23JES_NuMo = AlphaSys23JES_NuMo->frame();
    nll_NuMo->plotOn(plot_NLL_AlphaSys23JES_NuMo, ShiftToZero());

    canvas_par_NuMo->cd(1);
    plot_NLL_k_NuMo->GetXaxis()->SetRangeUser(vk_NuMo - ErrorRange*vk_error_NuMo, 
					      vk_NuMo + ErrorRange*vk_error_NuMo);
    plot_NLL_k_NuMo->SetMinimum(0.0);
    plot_NLL_k_NuMo->SetMaximum(5.0);
    plot_NLL_k_NuMo->Draw();

    canvas_par_NuMo->cd(2);
    plot_NLL_FRatio_ttbbttjj_NuMo->GetXaxis()->SetRangeUser(vFRatio_ttbbttjj_NuMo - ErrorRange*vFRatio_ttbbttjj_error_NuMo, 
							    vFRatio_ttbbttjj_NuMo + ErrorRange*vFRatio_ttbbttjj_error_NuMo);
    plot_NLL_FRatio_ttbbttjj_NuMo->SetMinimum(0.0);
    plot_NLL_FRatio_ttbbttjj_NuMo->SetMaximum(5.0);
    plot_NLL_FRatio_ttbbttjj_NuMo->Draw();

    canvas_par_NuMo->cd(3);
    plot_NLL_AlphaSys23PileUp_NuMo->GetXaxis()->SetRangeUser(vAlphaSys23PileUp_NuMo - ErrorRange*vAlphaSys23PileUp_error_NuMo, 
							     vAlphaSys23PileUp_NuMo + ErrorRange*vAlphaSys23PileUp_error_NuMo);
    plot_NLL_AlphaSys23PileUp_NuMo->SetMinimum(0.0);
    plot_NLL_AlphaSys23PileUp_NuMo->SetMaximum(5.0);
    plot_NLL_AlphaSys23PileUp_NuMo->Draw();

    canvas_par_NuMo->cd(4);
    plot_NLL_AlphaSys23JES_NuMo->GetXaxis()->SetRangeUser(vAlphaSys23JES_NuMo - ErrorRange*vAlphaSys23JES_error_NuMo, 
							     vAlphaSys23JES_NuMo + ErrorRange*vAlphaSys23JES_error_NuMo);
    plot_NLL_AlphaSys23JES_NuMo->SetMinimum(0.0);
    plot_NLL_AlphaSys23JES_NuMo->SetMaximum(5.0);
    plot_NLL_AlphaSys23JES_NuMo->Draw();

    canvas_par_NuMo->SaveAs("Canvas_par_NuMo.pdf");


  } // for(ch)
  
  fclose(fResult);
  
}


HistoFit LoadSample(TString FileName){

  
  TFile* fInput  = new TFile(dirnameIn + fl + "_" + FileName + ".root");
  if(!fInput->GetFile()){
    std::cerr << dirnameIn + fl +  FileName << " not Found!!!" << std::endl;
    std::exit(0);
  }  

  TString Cut = "2btag";   
  TString nch[3] = {"mujets","ejets","ljets"};
  TString nca[3] = {"Jet-2","Jet-3","Jet23"};
  
  HistoFit Output;

  for(int irca=0; irca<3;irca++){
    for(int irch=0; irch<2;irch++){
      if (irca<2){
	Output.hist1D[irca][irch] = (TH1D*)fInput->Get(Cut + "/" + nch[irch] + "/" + "hCSV_" + nca[irca] + "_" + nch[irch] + "_" + Cut)->Clone();
	// Avoid negative entries (from aMC@NLO MC)
	for(int ibin = 1; ibin<= Output.hist1D[irca][irch]->GetNbinsX();ibin++){
	  double binCont = Output.hist1D[irca][irch]->GetBinContent(ibin);
	  if (binCont < 0.0) Output.hist1D[irca][irch]->SetBinContent(ibin,0.0);
	}
      } // if(irca)
      else {
	Output.hist2D[irch] = (TH2D*)fInput->Get(Cut + "/" + nch[irch] + "/" + "h2DCSV_" + nca[irca] + "_" + nch[irch] + "_" + Cut)->Clone();
	// Avoid negative entries (from aMC@NLO MC)
	for(int ibinX = 1; ibinX<= Output.hist2D[irch]->GetNbinsX();ibinX++){
	  for(int ibinY = 1; ibinY<= Output.hist2D[irch]->GetNbinsX();ibinY++){
	    double binCont = Output.hist2D[irch]->GetBinContent(ibinX,ibinY);
	    if (binCont < 0.0) Output.hist2D[irch]->SetBinContent(ibinX,ibinY,0.0);
	  }
	}
      } // else
    } // for(irch)
 
    if (irca<2){
      Output.hist1D[irca][2] = (TH1D*)Output.hist1D[irca][0]->Clone();
      Output.hist1D[irca][2]->Add(Output.hist1D[irca][1]);
    }
    else{
      Output.hist2D[2] = (TH2D*)Output.hist2D[0]->Clone();
      Output.hist2D[2]->Add(Output.hist2D[1]);    
    }
  } // for(irca)

  // Number of Events
  for(unsigned int irch=0; irch<3; irch++) Output.events[irch] = Output.hist1D[0][irch]->Integral(0,1000);
  
  cout << "All the histograms from " << dirnameIn + fl +  FileName << " have been loaded successfully!!!" << endl;

  // ----------------------------------------------------------------------------------
  // ----------------------------------------------------------------------------------
  
  if(!FileName.Contains("Data")){
        
    for(unsigned int isys = 0; isys<14; isys++){
      
      TFile* fInputSys[3];
      fInputSys[Nom]  = new TFile(dirnameIn + fl + "_" + FileName + ".root");
      
      if(isys == PileUp || isys == JES){
	fInputSys[Up]    = new TFile(dirnameIn + fl + "_" + FileName + "_SYS_" + SystNam[isys] + "_Up.root");
	fInputSys[Down]  = new TFile(dirnameIn + fl + "_" + FileName + "_SYS_" + SystNam[isys] + "_Down.root");
      }
      else{
	fInputSys[Up]   = new TFile(dirnameIn + fl + "_" + FileName + ".root");
	fInputSys[Down] = new TFile(dirnameIn + fl + "_" + FileName + ".root");
      }
      
      if(!fInputSys[Nom]->GetFile() || !fInputSys[Up]->GetFile() || !fInputSys[Down]->GetFile()){
	std::cerr << SystNam[isys] << " systematic variation for " << dirnameIn + fl +  FileName << " not Found!!!" << std::endl;
	std::exit(0);
      }  
      
      for(unsigned int ivar = 0; ivar<3; ivar++){
	for(int irca=0; irca<3;irca++){
	  for(int irch=0; irch<2;irch++){
	    if (irca<2){
	      Output.hsyst1D[isys][ivar][irca][irch] = (TH1D*)fInputSys[ivar]->Get(Cut + "/" + nch[irch] + "/" + "hCSV_" + nca[irca] + "_" + nch[irch] + "_" + Cut)->Clone();
	      // Avoid negative entries (from aMC@NLO MC)
	      for(int ibin = 1; ibin<= Output.hsyst1D[isys][ivar][irca][irch]->GetNbinsX();ibin++){
		double binCont = Output.hsyst1D[isys][ivar][irca][irch]->GetBinContent(ibin);
		if (binCont < 0.0) Output.hsyst1D[isys][ivar][irca][irch]->SetBinContent(ibin,0.0);
	      }
	      // Normalize wrt the number of events in the nominal??? (to take into account the yields variations... Is it needed?)
	      Output.hsyst1D[isys][ivar][irca][irch]->Scale(1.0/Output.hsyst1D[isys][ivar][irca][irch]->Integral());
	    } // if(irca)
	    else {
	      Output.hsyst2D[isys][ivar][irch] = (TH2D*)fInputSys[ivar]->Get(Cut + "/" + nch[irch] + "/" + "h2DCSV_" + nca[irca] + "_" + nch[irch] + "_" + Cut)->Clone();
	      // Avoid negative entries (from aMC@NLO MC)
	      for(int ibinX = 1; ibinX<= Output.hsyst2D[isys][ivar][irch]->GetNbinsX();ibinX++){
		for(int ibinY = 1; ibinY<= Output.hsyst2D[isys][ivar][irch]->GetNbinsX();ibinY++){
		  double binCont = Output.hsyst2D[isys][ivar][irch]->GetBinContent(ibinX,ibinY);
		  if (binCont < 0.0) Output.hsyst2D[isys][ivar][irch]->SetBinContent(ibinX,ibinY,0.0);
		}
	      }
	      // Normalize wrt the number of events in the nominal??? (to take into account the yields variations... Is it needed?)
	      Output.hsyst2D[isys][ivar][irch]->Scale(1.0/Output.hsyst2D[isys][ivar][irch]->Integral());
	    } // else
	  } // for(irch)
	  
	  if (irca<2){
	    Output.hsyst1D[isys][ivar][irca][2] = (TH1D*)Output.hsyst1D[isys][ivar][irca][0]->Clone();
	    Output.hsyst1D[isys][ivar][irca][2]->Add(Output.hsyst1D[isys][ivar][irca][1]);
	  }
	  else{
	    Output.hsyst2D[isys][ivar][2] = (TH2D*)Output.hsyst2D[isys][ivar][0]->Clone();
	    Output.hsyst2D[isys][ivar][2]->Add(Output.hsyst2D[isys][ivar][1]);    
	  }
	} // for(irca)
      } // for(ivar)
    } // for(isys)
  } // if(!data)
  
  //fInput->Close();
  
  return Output;
}


RooPlot *PlotPDFModel(RooRealVar *var, RooHistPdf *Model, RooDataHist *DataHis, bool PlotData, TLegend *leg){
  using namespace RooFit;
  
  RooPlot *Plot = var->frame();

  RooArgSet* model_comps = Model->getComponents() ;
  model_comps->Print("v");
  cout << model_comps->contentsString () << endl; 

  cout << "Plotting " << Model->getTitle() << endl;

  TString ncm;
  if (Model->getTitle() == "TotModel23")        ncm = "_hispdf23";
  if (Model->getTitle() == "TotModel23_SysCons") ncm = "_PInterSyst23";

  if (PlotData) DataHis->plotOn (Plot, 
				 Name("Data"));
  else DataHis->plotOn (Plot, 
			Name("Data"), Invisible());
  Model->plotOn (Plot, 
  		 LineColor(kRed), 
  		 Name("FullModel"));
  Model->plotOn (Plot, 
  		 Components(SamNam[Bkgtt] + ncm + "," + SamNam[BkgOther] + ncm), 
		 LineColor(colors[BkgFull]), 
  		 Name("Bkg"));
  Model->plotOn (Plot, 
  		 Components(SamNam[ttjj] + ncm),
  		 LineColor(colors[ttjj]), 
  		 RooFit::Name("ttjj"));
  Model->plotOn (Plot,
  		 Components(SamNam[ttbb] + ncm),
  		 LineColor(colors[ttbb]),
  		 RooFit::Name("ttbb"));
  Model->plotOn (Plot,
  		 Components(SamNam[ttb] + ncm),
  		 LineColor(colors[ttb]),
  		 RooFit::Name("ttb"));
  Model->plotOn (Plot,
  		 Components(SamNam[ttccLF] + ncm),
  		 LineColor(colors[ttccLF]),
  		 RooFit::Name("ttccLF"));

  // Legend
  leg->Clear();
  leg->SetFillColor(0);
  leg->SetLineColor(0);
  leg->SetLineWidth(0.0);
  leg->SetTextFont(62);
  leg->SetTextSize(0.03);
  leg->SetNColumns(2);

  leg->AddEntry(Plot->findObject("Data"),"Data","p");
  leg->AddEntry(Plot->findObject("FullModel"),"Total Fit","l");
  leg->AddEntry(Plot->findObject("Bkg"),"Bkg","l");
  leg->AddEntry(Plot->findObject("ttjj"),"ttjj","l");
  leg->AddEntry(Plot->findObject("ttbb"),"ttbb","l");
  leg->AddEntry(Plot->findObject("ttb"),"ttbj","l");
  leg->AddEntry(Plot->findObject("ttccLF"),"ttcc+ttLF","l");

  return Plot;  
}


RooPlot *PlotPDF_NuMo(RooRealVar *var, RooHistPdf *Model, RooDataHist *DataHis, bool PlotData, TLegend *leg, RooRealVar *k, RooRealVar *R){
  using namespace RooFit;
  
  float vk = k->getVal();
  float vR = R->getVal();

  RooPlot *Plot = var->frame();

  RooArgSet* model_comps = Model->getComponents() ;
  model_comps->Print("v");
  cout << model_comps->contentsString () << endl; 

  cout << "-- Plotting: " << Model->getTitle() << endl;
  
  //Normalization(2340,RooAbsReal::NumEvent),
  //Normalization(1.,RooAbsReal::Relative),
  
  if (PlotData) DataHis->plotOn (Plot, 
				 Name("Data"));
  else DataHis->plotOn (Plot, 
			Name("Data"), Invisible());
  Model->plotOn (Plot, 
  		 LineColor(kRed), 
  		 Name("FullModel"));
  Model->plotOn (Plot, 
  		 Components("Bkgtt_PInterSyst23,BkgOther_PInterSyst23"), 
  		 LineColor(colors[BkgFull]), 
  		 Name("Bkg"));
  Model->plotOn (Plot, 
  		 Components("ttjj_PInterSyst23"),
  		 LineColor(colors[ttjj]), 
  		 RooFit::Name("ttjj"));
  // Model->plotOn (Plot,
  // 		 Components("ttbb_PInterSyst23"),
  // 		 LineColor(colors[ttbb]),
  // 		 RooFit::Name("ttbb"));
  
  // legend
  leg->Clear();
  leg->SetFillColor(0);
  leg->SetLineColor(0);
  leg->SetLineWidth(0.0);
  leg->SetTextFont(62);
  leg->SetTextSize(0.03);
  leg->SetNColumns(2);

  leg->AddEntry(Plot->findObject("Data"),"Data","p");
  leg->AddEntry(Plot->findObject("FullModel"),"Total Fit","l");
  leg->AddEntry(Plot->findObject("Bkg"),"Bkg","l");
  leg->AddEntry(Plot->findObject("ttjj"),"ttjj","l");
  //leg->AddEntry(Plot->findObject("ttbb"),"ttbb","l");
  // leg->AddEntry(Plot->findObject("ttb"),"ttbj","l");
  // leg->AddEntry(Plot->findObject("ttccLF"),"ttcc+ttLF","l");

  return Plot;  
}
