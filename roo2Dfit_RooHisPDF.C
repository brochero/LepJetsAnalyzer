#include "roo2Dfit_RooHisPDF.h"
#include "RooStats/ProfileLikelihoodCalculator.h"

#include "RooStats/LikelihoodInterval.h"


void roo2Dfit_RooHisPDF(TString nModel = "RttbCon"){

  //setTDRStyle();
  gSystem->Load("libRooFit") ;
  using namespace RooFit;
  using namespace RooStats;

  HistoFit InFile[18];
  float ratio[15];

  // Data
  InFile[data] = LoadSample("DataSingleLep");

  // MC: Signal
  InFile[ttbb]   = LoadSample("ttbar_LepJetsPowhegPythiattbb");
  InFile[ttb]    = LoadSample("ttbar_LepJetsPowhegPythiattbj");
  InFile[ttcc]   = LoadSample("ttbar_LepJetsPowhegPythiattcc");
  InFile[ttLF]   = LoadSample("ttbar_LepJetsPowhegPythiattLF"); // Includes ttc
  InFile[ttccLF] = LoadSample("ttbar_LepJetsPowhegPythiattccLF");

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
  // -- Cross Sections from MC 
  RooRealVar *Vis_Xsecttjj   = new RooRealVar("Vis_Xsecttjj",  "ttjj cross section Vis Ph-Sp",   26.40,  10.0,   50.0);// mu
  RooRealVar *Vis_Xsecttbb   = new RooRealVar("Vis_Xsecttbb",  "ttbb cross section Vis Ph-Sp",    0.375,  0.001,  1.5);// mu
  RooRealVar *Full_Xsecttjj  = new RooRealVar("Full_Xsecttjj", "ttjj cross section Full Ph-Sp", 290.2,  150.0,  350.0);
  RooRealVar *Full_Xsecttbb  = new RooRealVar("Full_Xsecttbb", "ttbb cross section Full Ph-Sp",   3.9,    2.0,    5.0);
  // Ratios Xsecttbb/Xsecttjj    
  RooFormulaVar *Vis_Xsecttbb_Xsecttjj  = new RooFormulaVar("Vis_Xsecttbb_Xsecttjj",  "Xsecttbb/Xsecttjj Vis-PhSp",  
							    "Vis_Xsecttbb/Vis_Xsecttjj",   RooArgList(*Vis_Xsecttbb,*Vis_Xsecttjj));
  RooFormulaVar *Full_Xsecttbb_Xsecttjj = new RooFormulaVar("Full_Xsecttbb_Xsecttjj", "Xsecttbb/Xsecttjj Full-PhSp", 
							    "Full_Xsecttbb/Full_Xsecttjj", RooArgList(*Full_Xsecttbb,*Full_Xsecttjj));
  RooRealVar *Vis_C_Xsecttbb_Xsecttjj   = new RooRealVar("Vis_C_Xsecttbb_Xsecttjj",  "Xsecttbb/Xsecttjj Vis-PhSp",  0.375/26.40,  0.001,  0.5); // 0.0142
  RooRealVar *Full_C_Xsecttbb_Xsecttjj  = new RooRealVar("Full_C_Xsecttbb_Xsecttjj", "Xsecttbb/Xsecttjj Full-PhSp", 3.900/290.2,  0.001,  0.5); // 0.0134

  
  // -- Efficiency
  vEffttjj[muJets] = 0.05;  vEffttjj[eJets] = 0.05;  vEffttjj[LepJets] = 0.0561;
  vEffttbb[muJets] = 0.22;  vEffttbb[eJets] = 0.22;  vEffttbb[LepJets] = 0.2216;
  // -- Acceptance
  vAccttjj[muJets] = 0.25*0.34;  vAccttjj[eJets] = 0.25*0.34;  vAccttjj[LepJets] = 0.2413*0.34;
  vAccttbb[muJets] = 0.29*0.34;  vAccttbb[eJets] = 0.29*0.34;  vAccttbb[LepJets] = 0.2914*0.34;
  // -- Luminosity 
  vLumi = 36814.; // Full 2016 Dataset

  for (unsigned int ch=2; ch<3; ch++){
    // Efficiencies ttjj
    RooRealVar *Effttjj_nom = new RooRealVar("Effttjj_nom", "ttjj Nom. Efficiency", vEffttjj[ch]);
    // Efficiencies ttbb
    RooRealVar *Effttbb_nom = new RooRealVar("Effttbb_nom", "ttbb Nom. Efficiency", vEffttbb[ch]);
    
    // Aceptancies ttjj
    RooRealVar *Accttjj_nom = new RooRealVar("Accttjj_nom",  "ttjj Nom. Acceptancy", vAccttjj[ch]);
    // Aceptancies ttbb
    RooRealVar *Accttbb_nom = new RooRealVar("Accttbb_nom",  "ttbb Nom. Acceptancy", vAccttbb[ch]);
    
    // Luminosity
    RooRealVar *Lumi_nom  = new RooRealVar("Lumi_nom",  "Luminosity", vLumi);
    
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


    // Visible and Full Ph-Sp
    WS_StMo->import(*Lumi_nom);
    WS_StMo->import(*Accttjj_nom);
    WS_StMo->import(*Accttbb_nom);
    WS_StMo->import(*Effttjj_nom);
    WS_StMo->import(*Effttbb_nom);
    WS_StMo->import(*Vis_Xsecttbb_Xsecttjj);
    WS_StMo->import(*Full_Xsecttbb_Xsecttjj);
    WS_StMo->import(*Vis_C_Xsecttbb_Xsecttjj);
    WS_StMo->import(*Full_C_Xsecttbb_Xsecttjj);

    WS_StMo->factory("expr::Effttbbttjj('Effttbb_nom/Effttjj_nom',Effttbb_nom,Effttjj_nom)");
    WS_StMo->factory("expr::Accttbbttjj('Accttbb_nom/Accttjj_nom',Accttbb_nom,Accttjj_nom)");

    // Visible Ph-Sp
    WS_StMo->factory("prod::Vis_XsecRatiottbbttjj(Effttbbttjj,Vis_Xsecttbb_Xsecttjj)");
    WS_StMo->factory("prod::Vis_C_XsecRatiottbbttjj(Effttbbttjj,Vis_C_Xsecttbb_Xsecttjj)");
    WS_StMo->factory("expr::Vis_Nttjj('Vis_Xsecttjj*Lumi_nom', Vis_Xsecttjj,Lumi_nom)");
    WS_StMo->factory("prod::Vis_k(Vis_Xsecttjj,0.03788)"); // 0.03788 from 1/26.40
    // Model: Visible Ph-Sp -> Cross Sections
    WS_StMo->factory("SUM::Vis_ttjj_hispdf23(Vis_XsecRatiottbbttjj*ttbb_hispdf23,prod(Vis_XsecRatiottbbttjj,CRatio_ttbjttbb)*ttb_hispdf23, ttccLF_hispdf23)");
    WS_StMo->factory("SUM::Vis_TotModel23(prod(Vis_Nttjj,Effttjj_nom)*Vis_ttjj_hispdf23, prod(Vis_k,n_Bkgtt_var)*Bkgtt_hispdf23, n_BkgOther_var*BkgOther_hispdf23)");
    // Model: Visible Ph-Sp -> Cross Section and Ratio
    WS_StMo->factory("SUM::Vis_C_ttjj_hispdf23(Vis_C_XsecRatiottbbttjj*ttbb_hispdf23,prod(Vis_C_XsecRatiottbbttjj,CRatio_ttbjttbb)*ttb_hispdf23, ttccLF_hispdf23)");
    WS_StMo->factory("SUM::Vis_C_TotModel23(prod(Vis_Nttjj,Effttjj_nom)*Vis_C_ttjj_hispdf23, prod(Vis_k,n_Bkgtt_var)*Bkgtt_hispdf23, n_BkgOther_var*BkgOther_hispdf23)");


    // Full Ph-Sp
    WS_StMo->factory("prod::Full_XsecRatiottbbttjj(Effttbbttjj,Accttbbttjj,Full_Xsecttbb_Xsecttjj)");
    WS_StMo->factory("prod::Full_C_XsecRatiottbbttjj(Effttbbttjj,Accttbbttjj,Full_C_Xsecttbb_Xsecttjj)");
    WS_StMo->factory("expr::Full_Nttjj('Full_Xsecttjj*Lumi_nom', Full_Xsecttjj,Lumi_nom)");
    WS_StMo->factory("prod::Full_k(Full_Xsecttjj,0.00345)"); // 0.00345 from 1/290.2
    // Model: Visible Ph-Sp -> Cross Section
    WS_StMo->factory("SUM::Full_ttjj_hispdf23(Full_XsecRatiottbbttjj*ttbb_hispdf23,prod(Full_XsecRatiottbbttjj,CRatio_ttbjttbb)*ttb_hispdf23, ttccLF_hispdf23)");
    WS_StMo->factory("SUM::Full_TotModel23(prod(Full_Nttjj,Effttjj_nom,Accttjj_nom)*Full_ttjj_hispdf23, prod(Full_k,n_Bkgtt_var)*Bkgtt_hispdf23, n_BkgOther_var*BkgOther_hispdf23)");
    // Model: Visible Ph-Sp -> Cross Section and Ratio
    WS_StMo->factory("SUM::Full_C_ttjj_hispdf23(Full_C_XsecRatiottbbttjj*ttbb_hispdf23,prod(Full_C_XsecRatiottbbttjj,CRatio_ttbjttbb)*ttb_hispdf23, ttccLF_hispdf23)");
    WS_StMo->factory("SUM::Full_C_TotModel23(prod(Full_Nttjj,Effttjj_nom,Accttjj_nom)*Full_C_ttjj_hispdf23, prod(Full_k,n_Bkgtt_var)*Bkgtt_hispdf23, n_BkgOther_var*BkgOther_hispdf23)");


    // ----------------------------------------------------------------------------------
    // ----------------------    Systematic Uncertainties   -----------------------------
    // ----------------------------------------------------------------------------------

    RooDataHist *DataHisSys[18][3][14][3]; // [sample][case][syst][var]
    RooHistPdf  *HisPdfSys[18][3][14][3];  // [sample][case][syst][var]
    RooArgList  *arg_PdfVar[18][3][3];     // [sample][case][var]

    RooRealVar *AlphaSys[14][3]; // [syst][case]   
    RooArgList *arg_AlphaSys[3]; // [case]

    PiecewiseInterpolation *PInterSyst[18][3]; // [sample][case]
    
    for(int is=0;is<18;is++) {
      for(int ic=0;ic<3;ic++) {
	for(int iv=0;iv<3;iv++) arg_PdfVar[is][ic][iv]= new RooArgList();
      }
    }    

    for(int iv=0;iv<3;iv++) arg_AlphaSys[iv]= new RooArgList();

    // Set of Nuisance parameters    
    for(int isys=0; isys<14; isys++){      
      AlphaSys[isys][0] = new RooRealVar("AlphaSys2"  + SystNam[isys], "Alpha Syst " + SystNam[isys], 0.0, -5.0, 5.0);
      AlphaSys[isys][1] = new RooRealVar("AlphaSys3"  + SystNam[isys], "Alpha Syst " + SystNam[isys], 0.0, -5.0, 5.0);
      AlphaSys[isys][2] = new RooRealVar("AlphaSys23" + SystNam[isys], "Alpha Syst " + SystNam[isys], 0.0, -5.0, 5.0);

      // if(isys == PileUp || isys == JES
      // 	 || isys == LES || isys == LepSF || isys == JER){
	arg_AlphaSys[0]->add(*AlphaSys[isys][0]);
	arg_AlphaSys[1]->add(*AlphaSys[isys][1]);
	arg_AlphaSys[2]->add(*AlphaSys[isys][2]);
    //   }
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
	  // if(isys == PileUp || isys == JES
	  //    || isys == LES || isys == LepSF || isys == JER){
	    arg_PdfVar[isam][0][ivar]->add(*HisPdfSys[isam][0][isys][ivar]);
	    arg_PdfVar[isam][1][ivar]->add(*HisPdfSys[isam][1][isys][ivar]);
	    arg_PdfVar[isam][2][ivar]->add(*HisPdfSys[isam][2][isys][ivar]);	    
	    //	  }
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

    // Visible and Full Ph-Sp
    WS_NuMo->import(*Lumi_nom);
    WS_NuMo->import(*Accttjj_nom);
    WS_NuMo->import(*Accttbb_nom);
    WS_NuMo->import(*Effttjj_nom);
    WS_NuMo->import(*Effttbb_nom);
    WS_NuMo->import(*Vis_Xsecttbb_Xsecttjj);
    WS_NuMo->import(*Full_Xsecttbb_Xsecttjj);
    WS_NuMo->import(*Vis_C_Xsecttbb_Xsecttjj);
    WS_NuMo->import(*Full_C_Xsecttbb_Xsecttjj);


    // Nuisance Parameters for Theoretical Uncertainties 
    RooRealVar *EttbbAlphaThSys[3], *EttjjAlphaThSys[3]; // [thsyst]
    for(int isys=0; isys<3; isys++){
      cout << "EttjjAlphaThSys" << SystThNam[isys] << endl;
      EttjjAlphaThSys[isys] = new RooRealVar("EttjjAlphaThSys" + SystThNam[isys], "Ettjj_Alpha Syst " + SystThNam[isys], -5.0, 5.0);    
      EttbbAlphaThSys[isys] = new RooRealVar("EttbbAlphaThSys" + SystThNam[isys], "Ettbb_Alpha Syst " + SystThNam[isys], -5.0, 5.0);

      WS_NuMo->import(*EttjjAlphaThSys[isys]);
      WS_NuMo->import(*EttbbAlphaThSys[isys]);

    }

    // -- Luminosity
    // kappaLumi is 1.062 -> Uncertainty in the Luminosity 6.2% 
    WS_NuMo->factory("expr::betaLumi('pow(kappaLumi,AlphaThSysLumi)',kappaLumi[1.062],AlphaThSysLumi[-5.,5.])");
    WS_NuMo->factory("prod::Lumi(Lumi_nom,betaLumi)");
    // -- Scale
    //ttjjkappaScale is 1.010 -> Scale contribution to the error in Eff   
    WS_NuMo->factory("expr::EttjjbetaScale('pow(EttjjkappaScale,EttjjAlphaThSysScale)',EttjjkappaScale[1.010],EttjjAlphaThSysScale)");
    WS_NuMo->factory("expr::EttbbbetaScale('pow(EttbbkappaScale,EttbbAlphaThSysScale)',EttbbkappaScale[1.010],EttbbAlphaThSysScale)");
    // -- PS
    //ttjjkappaPS is 1.010 -> PS contribution to the error in Eff   
    WS_NuMo->factory("expr::EttjjbetaPS('pow(EttjjkappaPS,EttjjAlphaThSysPS)',EttjjkappaPS[1.010],EttjjAlphaThSysPS)");
    WS_NuMo->factory("expr::EttbbbetaPS('pow(EttbbkappaPS,EttbbAlphaThSysPS)',EttbbkappaPS[1.010],EttbbAlphaThSysPS)");
    // -- PDF
    //kappaPDF is 1.010 -> PDF contribution to the error in Eff   
    WS_NuMo->factory("expr::EttjjbetaPDF('pow(EttjjkappaPDF,EttjjAlphaThSysPDF)',EttjjkappaPDF[1.010],EttjjAlphaThSysPDF)");
    WS_NuMo->factory("expr::EttbbbetaPDF('pow(EttbbkappaPDF,EttbbAlphaThSysPDF)',EttbbkappaPDF[1.010],EttbbAlphaThSysPDF)");


    // -- Efficiency with Nuisance Parameters
    WS_NuMo->factory("prod::Effttbb(Effttbb_nom,EttbbbetaScale,EttbbbetaPS,EttbbbetaPDF)");
    WS_NuMo->factory("prod::Effttjj(Effttjj_nom,EttjjbetaScale,EttjjbetaPS,EttjjbetaPDF)");
    // -- Efficiencies and Acceptance rations 
    WS_NuMo->factory("expr::Effttbbttjj('Effttbb_nom/Effttjj_nom',Effttbb_nom,Effttjj_nom)");
    WS_NuMo->factory("expr::Accttbbttjj('Accttbb_nom/Accttjj_nom',Accttbb_nom,Accttjj_nom)");

    // Visible Ph-Sp
    WS_NuMo->factory("prod::Vis_XsecRatiottbbttjj(Effttbbttjj,Vis_Xsecttbb_Xsecttjj)");
    WS_NuMo->factory("prod::Vis_C_XsecRatiottbbttjj(Effttbbttjj,Vis_C_Xsecttbb_Xsecttjj)");
    WS_NuMo->factory("expr::Vis_Nttjj('Vis_Xsecttjj*Lumi_nom', Vis_Xsecttjj,Lumi_nom)");
    WS_NuMo->factory("prod::Vis_k(Vis_Xsecttjj,0.03788)"); // 0.03788 from 1/26.40
    // Model: Visible Ph-Sp -> Cross Sections
    WS_NuMo->factory("ASUM::Vis_ttjj_PInterSyst23(Vis_XsecRatiottbbttjj*ttbb_PInterSyst23,prod(Vis_XsecRatiottbbttjj,CRatio_ttbjttbb)*ttb_PInterSyst23, ttccLF_PInterSyst23)");
    WS_NuMo->factory("ASUM::Vis_TotModel23_sys(prod(Vis_Nttjj,Effttjj_nom)*Vis_ttjj_PInterSyst23, prod(Vis_k,n_Bkgtt_var)*Bkgtt_PInterSyst23, n_BkgOther_var*BkgOther_PInterSyst23)");
    // Model: Visible Ph-Sp -> Cross Section and Ratio
    WS_NuMo->factory("ASUM::Vis_C_ttjj_PInterSyst23(Vis_C_XsecRatiottbbttjj*ttbb_PInterSyst23,prod(Vis_C_XsecRatiottbbttjj,CRatio_ttbjttbb)*ttb_PInterSyst23, ttccLF_PInterSyst23)");
    WS_NuMo->factory("ASUM::Vis_C_TotModel23_sys(prod(Vis_Nttjj,Effttjj_nom)*Vis_C_ttjj_PInterSyst23, prod(Vis_k,n_Bkgtt_var)*Bkgtt_PInterSyst23, n_BkgOther_var*BkgOther_PInterSyst23)");


    // Full Ph-Sp
    WS_NuMo->factory("prod::Full_XsecRatiottbbttjj(Effttbbttjj,Accttbbttjj,Full_Xsecttbb_Xsecttjj)");
    WS_NuMo->factory("prod::Full_C_XsecRatiottbbttjj(Effttbbttjj,Accttbbttjj,Full_C_Xsecttbb_Xsecttjj)");
    WS_NuMo->factory("expr::Full_Nttjj('Full_Xsecttjj*Lumi_nom', Full_Xsecttjj,Lumi_nom)");
    WS_NuMo->factory("prod::Full_k(Full_Xsecttjj,0.00345)"); // 0.00345 from 1/290.2
    // Model: Full Ph-Sp -> Cross Section
    WS_NuMo->factory("ASUM::Full_ttjj_PInterSyst23(Full_XsecRatiottbbttjj*ttbb_PInterSyst23,prod(Full_XsecRatiottbbttjj,CRatio_ttbjttbb)*ttb_PInterSyst23, ttccLF_PInterSyst23)");
    WS_NuMo->factory("ASUM::Full_TotModel23_sys(prod(Full_Nttjj,Effttjj_nom,Accttjj_nom)*Full_ttjj_PInterSyst23, prod(Full_k,n_Bkgtt_var)*Bkgtt_PInterSyst23, n_BkgOther_var*BkgOther_PInterSyst23)");
    // Model: Full Ph-Sp -> Cross Section and Ratio
    WS_NuMo->factory("ASUM::Full_C_ttjj_PInterSyst23(Full_C_XsecRatiottbbttjj*ttbb_PInterSyst23,prod(Full_C_XsecRatiottbbttjj,CRatio_ttbjttbb)*ttb_PInterSyst23, ttccLF_PInterSyst23)");
    WS_NuMo->factory("ASUM::Full_C_TotModel23_sys(prod(Full_Nttjj,Effttjj_nom,Accttjj_nom)*Full_C_ttjj_PInterSyst23, prod(Full_k,n_Bkgtt_var)*Bkgtt_PInterSyst23, n_BkgOther_var*BkgOther_PInterSyst23)");



    // Constrain the models
    TString ModelSysName[3] = {"TotModel2_SysCons",
			       "TotModel3_SysCons",
			       "TotModel23_SysCons"};
 
    TString sTotModel2_sys  = "PROD::" + ModelSysName[0] + "(TotModel2_sys"; 
    TString sTotModel3_sys  = "PROD::" + ModelSysName[1] + "(TotModel3_sys"; 
    // TString sTotModel23_sys = "PROD::" + ModelSysName[2] + "(TotModel23_sys"; 
    // TString sTotModel23_sys = "PROD::" + ModelSysName[2] + "(Vis_TotModel23_sys"; 
    // TString sTotModel23_sys = "PROD::" + ModelSysName[2] + "(Vis_C_TotModel23_sys"; 
    // TString sTotModel23_sys = "PROD::" + ModelSysName[2] + "(Full_TotModel23_sys"; 
    TString sTotModel23_sys = "PROD::" + ModelSysName[2] + "(Full_C_TotModel23_sys"; 
    
    for(int isys=0; isys<14; isys++){
      // if(isys == PileUp || isys == JES
      // 	 || isys == LES || isys == LepSF || isys == JER){
	sTotModel2_sys  += ",Gaussian(AlphaSys2"  + SystNam[isys] + ",0,1)"; 
	sTotModel3_sys  += ",Gaussian(AlphaSys3"  + SystNam[isys] + ",0,1)"; 
	sTotModel23_sys += ",Gaussian(AlphaSys23" + SystNam[isys] + ",0,1)"; 
	// }
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


    // WS_NuMo->Print();

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
    RooAbsReal *nll_StMo = WS_StMo->pdf("Full_C_TotModel23")->createNLL(*DataHis[data][2]);
    // Create MINUIT interface object
    RooMinimizer m_StMo(*nll_StMo);
    // Call MIGRAD to minimize the likelihood
    m_StMo.migrad();
    // Run HESSE to calculate errors from d2L/dp2
    m_StMo.hesse();
    // Save the current fit result
    RooFitResult* r_StMo = m_StMo.save() ;

    RooAbsReal *nll_NuMo = WS_NuMo->pdf("TotModel23_SysCons")->createNLL(*DataHis[data][2]);
    // Create MINUIT interface object
    RooMinimizer m_NuMo(*nll_NuMo);
    // Call MIGRAD to minimize the likelihood
    m_NuMo.migrad();
    // Run HESSE to calculate errors from d2L/dp2
    m_NuMo.hesse();
    // More precise estimation of the parameter uncertainties
    // MINOS uses a likelihood scan
    // m_NuMo.minos();
    
    // Save the current fit result
    RooFitResult* r_NuMo = m_NuMo.save() ;

    // To fix parameter "par"
    // par.setConstant(kTRUE) ;

    // Save the current fit result
    // RooFitResult* r = m.save() ;

    // Make contour plot of mx vs sx at 1,2,3 sigma
    // RooPlot* frame = m.contour(frac,sigma_g2,1,2,3) ;

    // All the Nuisance values after fit
    float val_AlphaSys[14];
    RooFitResult *r_NuMo_sys[14];
    RooFitResult *r_NuMo_zero, *r_NuMo_fixed;

    cout << "Nuisance Parameters.........................." << endl; 
    for(int isys=0; isys<14; isys++){
      val_AlphaSys[isys] = WS_NuMo->var("AlphaSys23" + SystNam[isys])->getVal();
      cout << "Parameter [" << SystNam[isys] << "] = " << val_AlphaSys[isys] << endl;
    } 
    
    // Contribution of each source
    for(int isys=0; isys<14; isys++){ 
      
      cout << "\n" <<endl;
      cout << "Setting new values........................." << endl;
      cout << "\n" <<endl;
      
      for(int jsys=0; jsys<14; jsys++){
    	TString NuiParName = "AlphaSys23" + SystNam[jsys];
    	// WS_NuMo->var(NuiParName)->setVal(0.0);
    	WS_NuMo->var(NuiParName)->setVal(val_AlphaSys[jsys]);
    	WS_NuMo->var(NuiParName)->setConstant(kFALSE);    
      }
      TString NuiParName = "AlphaSys23" + SystNam[isys];
      //WS_NuMo->var(NuiParName)->setVal();
      WS_NuMo->var(NuiParName)->setConstant(kTRUE);    
      
      m_NuMo.migrad();
      m_NuMo.hesse();
      
    r_NuMo_sys[isys] = m_NuMo.save() ;
      
    }

    // Statistical Uncertainty (All parameters Fixed)
    for(int isys=0; isys<14; isys++){
      TString NuiParName = "AlphaSys23" + SystNam[isys];
      WS_NuMo->var(NuiParName)->setVal(val_AlphaSys[isys]);
      WS_NuMo->var(NuiParName)->setConstant(kTRUE);    
    }

    m_NuMo.migrad();
    m_NuMo.hesse();
    
    r_NuMo_fixed = m_NuMo.save() ;

    for(int isys=0; isys<14; isys++){
      TString NuiParName = "AlphaSys23" + SystNam[isys];
      WS_NuMo->var(NuiParName)->setVal(0.0);
      WS_NuMo->var(NuiParName)->setConstant(kTRUE);          
    }
    
    m_NuMo.migrad();
    m_NuMo.hesse();

    r_NuMo_zero = m_NuMo.save() ;



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

    RooRealVar *k_StMo = WS_StMo->var("Full_Xsecttjj");
    float vk_StMo =  k_StMo->getVal();
    float vk_error_StMo =  k_StMo->getError();

    RooRealVar *FRatio_ttbbttjj_StMo = WS_StMo->var("Full_C_Xsecttbb_Xsecttjj");
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
    plot_NLL_FRatio_ttbbttjj_StMo->SetMaximum(5.0);
    plot_NLL_FRatio_ttbbttjj_StMo->Draw();

    canvas_par_StMo->cd(2);
    plot_NLL_k_StMo->GetXaxis()->SetRangeUser(vk_StMo - ErrorRange*vk_error_StMo, 
					      vk_StMo + ErrorRange*vk_error_StMo);
    plot_NLL_k_StMo->SetMinimum(0.0);
    plot_NLL_k_StMo->SetMaximum(5.0);
    plot_NLL_k_StMo->Draw();

    canvas_par_StMo->SaveAs("Canvas_par_StMo.pdf");


    RooRealVar *k_NuMo  = WS_NuMo->var("Full_Xsecttjj");
    float vk_NuMo       = k_NuMo->getVal();
    float vk_error_NuMo = k_NuMo->getError();

    RooRealVar *FRatio_ttbbttjj_NuMo  = WS_NuMo->var("Full_Xsecttbb");
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
    RooPlot *plot_CSV2_NuMo = PlotPDF_NuMo(CSV2, WS_NuMo, DataHis[data][2], true, CSV_leg_NuMo);
    RooPlot *plot_CSV3_NuMo = PlotPDF_NuMo(CSV3, WS_NuMo, DataHis[data][2], true, CSV_leg_NuMo);
      
    TH1 *hModel[3];
    hModel[0] = WS_NuMo->pdf(ModelSysName[0])->createHistogram("CSV2");
    hModel[1] = WS_NuMo->pdf(ModelSysName[1])->createHistogram("CSV3");
    hModel[2] = WS_NuMo->pdf(ModelSysName[2])->createHistogram("CSV2,CSV3");
       
    // TH1 *hModel2D_PileUp     = WS_NuMo->pdf("TotModel2_SysCons")->createHistogram("CSV2,AlphaSys2PileUp", 20, 100);

    TCanvas *canvas_sys_NuMo = new TCanvas("canvas_sys_NuMo",   "Model Plots");
    canvas_sys_NuMo->Divide(2,2);
    TH1 *hModel2D_sys  = WS_NuMo->function("ttccLF_PInterSyst2")->createHistogram("CSV2,AlphaSys2btagcfI", 20, 50);
    TH1 *hModel_sys_Nom = WS_NuMo->pdf("ttccLF_hisPdf2PileUpNom")->createHistogram("CSV2");
    TH1 *hModel_sys_Up  = WS_NuMo->pdf("ttccLF_hisPdf2btagcfIUp")->createHistogram("CSV2");
    TH1 *hModel_sys_Down = WS_NuMo->pdf("ttccLF_hisPdf2btagcfIDown")->createHistogram("CSV2");
    canvas_sys_NuMo->cd(1);
    //canvas_sys_NuMo->cd(1)->SetTheta(120);
    canvas_sys_NuMo->cd(1)->SetPhi(-60);
    hModel2D_sys->Draw("LEGO2");
    canvas_sys_NuMo->cd(2);
    hModel_sys_Nom->SetTitle("Nominal");
    hModel_sys_Nom->SetLineColor(1);
    hModel_sys_Nom->Draw();
    canvas_sys_NuMo->cd(3);
    hModel_sys_Up->SetTitle("Up Variation");
    hModel_sys_Up->SetLineColor(2);
    hModel_sys_Up->Draw();
    canvas_sys_NuMo->cd(4);
    hModel_sys_Down->SetTitle("Down Variation");
    hModel_sys_Down->SetLineColor(4);
    hModel_sys_Down->Draw();

    canvas_sys_NuMo->SaveAs("Canvas_sys_NuMo.pdf");


    TCanvas *canvas_com_NuMo = new TCanvas("canvas_com_NuMo",   "Model Plots");
    canvas_com_NuMo->Divide(2,2);
    
    canvas_com_NuMo->cd(1);
    plot_CSV2_NuMo->Draw();

    canvas_com_NuMo->cd(2);
    plot_CSV3_NuMo->Draw();
    CSV_leg_NuMo->Draw("SAME");

    canvas_com_NuMo->cd(3);
    canvas_com_NuMo->cd(3)->SetLogy();
    plot_CSV2_NuMo->Draw();

    canvas_com_NuMo->cd(4);
    canvas_com_NuMo->cd(4)->SetLogy();
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

    double DeltaSigma = 0.5; //one sigma variation. for two sigma, it should be 2.0

    canvas_par_NuMo->cd(1);
    plot_NLL_k_NuMo->GetXaxis()->SetRangeUser(vk_NuMo - ErrorRange*vk_error_NuMo, 
    					      vk_NuMo + ErrorRange*vk_error_NuMo);
    plot_NLL_k_NuMo->SetMinimum(0.0);
    plot_NLL_k_NuMo->SetMaximum(2);
    plot_NLL_k_NuMo->Draw();
    TLine *line_1 = new TLine(vk_NuMo - ErrorRange*vk_error_NuMo, DeltaSigma, vk_NuMo + ErrorRange*vk_error_NuMo, DeltaSigma);
    line_1->SetLineColor(kRed);
    line_1->Draw("SAME");


    canvas_par_NuMo->cd(2);
    plot_NLL_FRatio_ttbbttjj_NuMo->GetXaxis()->SetRangeUser(vFRatio_ttbbttjj_NuMo - ErrorRange*vFRatio_ttbbttjj_error_NuMo, 
							    vFRatio_ttbbttjj_NuMo + ErrorRange*vFRatio_ttbbttjj_error_NuMo);
    plot_NLL_FRatio_ttbbttjj_NuMo->SetMinimum(0.0);
    plot_NLL_FRatio_ttbbttjj_NuMo->SetMaximum(2.0);
    plot_NLL_FRatio_ttbbttjj_NuMo->Draw();
    TLine *line_2 = new TLine(vFRatio_ttbbttjj_NuMo - ErrorRange*vFRatio_ttbbttjj_error_NuMo, DeltaSigma, vFRatio_ttbbttjj_NuMo + ErrorRange*vFRatio_ttbbttjj_error_NuMo, DeltaSigma);
    line_2->SetLineColor(kRed);
    line_2->Draw("SAME");

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


    //TEST
    ProfileLikelihoodCalculator* PL = new ProfileLikelihoodCalculator(*DataHis[data][2],*WS_StMo->pdf("Full_C_TotModel23"), RooArgSet(*WS_StMo->var("Full_Xsecttjj")));
    PL->SetConfidenceLevel(0.68);
    LikelihoodInterval* interval = PL->GetInterval();
    double intervalValue = WS_StMo->var("Full_Xsecttjj")->getVal();
    double intervalError = WS_StMo->var("Full_Xsecttjj")->getError();
    double intervalLow = interval->LowerLimit(*WS_StMo->var("Full_Xsecttjj"));
    double intervalUp = interval->UpperLimit(*WS_StMo->var("Full_Xsecttjj"));

    cout << intervalValue << " +/- " << intervalError << " --- Up = " << intervalUp  << " Down = " << intervalLow << endl;

    // Save the current fit result
    cout << "Standard Fit (No Nuisance Parameters)" << endl;
    r_StMo->Print();

    cout << "Fit including Nuisance Parameters" << endl;
    r_NuMo->Print();
    RooRealVar *Xsecttjj_fitresult_nom          = (RooRealVar*) r_NuMo->floatParsFinal().find("Full_Xsecttjj"); 
    RooRealVar *XsecRatiottbbttjj_fitresult_nom = (RooRealVar*) r_NuMo->floatParsFinal().find("Full_C_Xsecttbb_Xsecttjj"); 
    float r_Full_Xsecttjj_nom[3];
    float r_Full_XsecRatiottbbttjj_nom[3];
    r_Full_Xsecttjj_nom[0] = Xsecttjj_fitresult_nom->getVal();
    r_Full_Xsecttjj_nom[1] = Xsecttjj_fitresult_nom->getAsymErrorHi();
    r_Full_Xsecttjj_nom[2] = Xsecttjj_fitresult_nom->getAsymErrorHi();
    
    r_Full_XsecRatiottbbttjj_nom[0] = XsecRatiottbbttjj_fitresult_nom->getVal();
    r_Full_XsecRatiottbbttjj_nom[1] = XsecRatiottbbttjj_fitresult_nom->getAsymErrorHi();
    r_Full_XsecRatiottbbttjj_nom[2] = XsecRatiottbbttjj_fitresult_nom->getAsymErrorHi();

    float per_Xsecttjj_nom[3];
    per_Xsecttjj_nom[0] = 100.;
    per_Xsecttjj_nom[1] = 100.*r_Full_Xsecttjj_nom[1]/r_Full_Xsecttjj_nom[0];
    per_Xsecttjj_nom[2] = 100.*r_Full_Xsecttjj_nom[2]/r_Full_Xsecttjj_nom[0];
    cout << "kk" <<endl;
    float per_XsecRatiottbbttjj_nom[3];
    per_XsecRatiottbbttjj_nom[0] = 100.;
    per_XsecRatiottbbttjj_nom[1] = 100.*r_Full_XsecRatiottbbttjj_nom[1]/r_Full_XsecRatiottbbttjj_nom[0];
    per_XsecRatiottbbttjj_nom[2] = 100.*r_Full_XsecRatiottbbttjj_nom[2]/r_Full_XsecRatiottbbttjj_nom[0];
    cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
    cout << " Central: " << endl;
    cout << "Ratio-> " << r_Full_XsecRatiottbbttjj_nom[0] << " + " << r_Full_XsecRatiottbbttjj_nom[1] << " - " << r_Full_XsecRatiottbbttjj_nom[2] << endl;
    cout << "Xsecttjj-> " << r_Full_Xsecttjj_nom[0] << " + " << r_Full_Xsecttjj_nom[1] << " - " << r_Full_Xsecttjj_nom[2] << endl;
    

    cout << "Nuisance Fit: Fixed values (after fit)" << endl;
    r_NuMo_fixed->Print();
    RooRealVar *Xsecttjj_fitresult_fixed          = (RooRealVar*) r_NuMo_fixed->floatParsFinal().find("Full_Xsecttjj"); 
    RooRealVar *XsecRatiottbbttjj_fitresult_fixed = (RooRealVar*) r_NuMo_fixed->floatParsFinal().find("Full_C_Xsecttbb_Xsecttjj"); 
    float r_Full_Xsecttjj_stat[3];
    float r_Full_XsecRatiottbbttjj_stat[3];
    r_Full_Xsecttjj_stat[0] = Xsecttjj_fitresult_fixed->getVal();
    r_Full_Xsecttjj_stat[1] = Xsecttjj_fitresult_fixed->getAsymErrorHi();
    r_Full_Xsecttjj_stat[2] = Xsecttjj_fitresult_fixed->getAsymErrorHi();
    
    r_Full_XsecRatiottbbttjj_stat[0] = XsecRatiottbbttjj_fitresult_fixed->getVal();
    r_Full_XsecRatiottbbttjj_stat[1] = XsecRatiottbbttjj_fitresult_fixed->getAsymErrorHi();
    r_Full_XsecRatiottbbttjj_stat[2] = XsecRatiottbbttjj_fitresult_fixed->getAsymErrorHi();
    cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
    cout << " Stat: Fixed to Values after fit" << endl;
    cout << "Ratio-> " << r_Full_XsecRatiottbbttjj_stat[0] << " + " << r_Full_XsecRatiottbbttjj_stat[1] << " - " << r_Full_XsecRatiottbbttjj_stat[2] << endl;
    cout << "Xsecttjj-> " << r_Full_Xsecttjj_stat[0] << " + " << r_Full_Xsecttjj_stat[1] << " - " << r_Full_Xsecttjj_stat[2] << endl;

    float per_Xsecttjj_stat[3];
    per_Xsecttjj_stat[0] = 100.;
    per_Xsecttjj_stat[1] = 100.*r_Full_Xsecttjj_stat[1]/r_Full_Xsecttjj_stat[0];
    per_Xsecttjj_stat[2] = 100.*r_Full_Xsecttjj_stat[2]/r_Full_Xsecttjj_stat[0];
    float per_XsecRatiottbbttjj_stat[3];
    per_XsecRatiottbbttjj_stat[0] = 100.;
    per_XsecRatiottbbttjj_stat[1] = 100.*r_Full_XsecRatiottbbttjj_stat[1]/r_Full_XsecRatiottbbttjj_stat[0];
    per_XsecRatiottbbttjj_stat[2] = 100.*r_Full_XsecRatiottbbttjj_stat[2]/r_Full_XsecRatiottbbttjj_stat[0];
    

    cout << "Nuisance Fit: Fixed values (ZERO)" << endl;
    r_NuMo_zero->Print();

    float r_Full_Xsecttjj[14][3];
    float r_Full_XsecRatiottbbttjj[14][3];
    float per_Xsecttjj[14][3];
    float per_XsecRatiottbbttjj[14][3];
    float Nui_Sys_Xsecttjj[14][3];
    float Nui_Sys_XsecRatiottbbttjj[14][3];
    
    FILE*   Yields_ttjj      = fopen("Table_ttjj.tex", "w");
    FILE*   Yields_R         = fopen("Table_R.tex", "w");
    FILE*   Yields_ttjj_Unc  = fopen("Table_ttjj_Unc.tex", "w");
    FILE*   Yields_R_Unc     = fopen("Table_R_Unc.tex", "w");

    fprintf(Yields_ttjj,"Nomimal  & %.2f & %.3f(%.1f\\\%) & %.3f(%.1f\\\%)  \\\\\\hline \n", 
	    r_Full_Xsecttjj_nom[0],r_Full_Xsecttjj_nom[1],per_Xsecttjj_nom[1],r_Full_Xsecttjj_nom[2],per_Xsecttjj_nom[2]);
    fprintf(Yields_ttjj,"For Stat & %.2f & %.3f(%.1f\\\%) & %.3f(%.1f\\\%)  \\\\\\hline \n", 
	    r_Full_Xsecttjj_stat[0],r_Full_Xsecttjj_stat[1],per_Xsecttjj_stat[1],r_Full_Xsecttjj_stat[2],per_Xsecttjj_stat[2]);

    fprintf(Yields_R,"Nomimal  & %.4f & %.5f(%.1f\\\%) & %.5f(%.1f\\\%)  \\\\\\hline \n", 
	    r_Full_XsecRatiottbbttjj_nom[0],r_Full_XsecRatiottbbttjj_nom[1],per_XsecRatiottbbttjj_nom[1],r_Full_XsecRatiottbbttjj_nom[2],per_XsecRatiottbbttjj_nom[2]);
    fprintf(Yields_R,"For Stat & %.4f & %.5f(%.1f\\\%) & %.5f(%.1f\\\%)  \\\\\\hline \n", 
	    r_Full_XsecRatiottbbttjj_stat[0],r_Full_XsecRatiottbbttjj_stat[1],per_Xsecttjj_stat[1],r_Full_XsecRatiottbbttjj_stat[2],per_Xsecttjj_stat[2]);

    float Tot_Sys_Xsecttjj[3], Tot_Sys_XsecRatiottbbttjj[3];
    Tot_Sys_Xsecttjj[1] = sqrt(fabs( ((r_Full_Xsecttjj_nom[0]  - r_Full_Xsecttjj_nom[1] )*(r_Full_Xsecttjj_nom[0]  - r_Full_Xsecttjj_nom[1])) - 
				     ((r_Full_Xsecttjj_stat[0] - r_Full_Xsecttjj_stat[1])*(r_Full_Xsecttjj_stat[0] - r_Full_Xsecttjj_stat[1]))
				     )
			       );
    Tot_Sys_Xsecttjj[2] = sqrt(fabs( ((r_Full_Xsecttjj_nom[0]  - r_Full_Xsecttjj_nom[2] )*(r_Full_Xsecttjj_nom[0]  - r_Full_Xsecttjj_nom[2])) - 
				     ((r_Full_Xsecttjj_stat[0] - r_Full_Xsecttjj_stat[2])*(r_Full_Xsecttjj_stat[0] - r_Full_Xsecttjj_stat[2]))
				     )
			       );
    

    Tot_Sys_XsecRatiottbbttjj[1] = sqrt(fabs( (r_Full_XsecRatiottbbttjj_nom[0]  - r_Full_XsecRatiottbbttjj_nom[1] )*(r_Full_XsecRatiottbbttjj_nom[0]  - r_Full_XsecRatiottbbttjj_nom[1]) - 
					      (r_Full_XsecRatiottbbttjj_stat[0] - r_Full_XsecRatiottbbttjj_stat[1])*(r_Full_XsecRatiottbbttjj_stat[0] - r_Full_XsecRatiottbbttjj_stat[1])));
    Tot_Sys_XsecRatiottbbttjj[2] = sqrt(fabs( (r_Full_XsecRatiottbbttjj_nom[0]  - r_Full_XsecRatiottbbttjj_nom[2] )*(r_Full_XsecRatiottbbttjj_nom[0]  - r_Full_XsecRatiottbbttjj_nom[2]) - 
					      (r_Full_XsecRatiottbbttjj_stat[0] - r_Full_XsecRatiottbbttjj_stat[2])*(r_Full_XsecRatiottbbttjj_stat[0] - r_Full_XsecRatiottbbttjj_stat[2])));

    fprintf(Yields_ttjj_Unc,"Total Syst. & %.2f & %.2f \\\\\\hline \n",
	    Tot_Sys_Xsecttjj[1], Tot_Sys_Xsecttjj[2]);
    fprintf(Yields_R_Unc,"Total Syst. & %.5f & %.5f \\\\\\hline \n",
	    Tot_Sys_XsecRatiottbbttjj[1],Tot_Sys_XsecRatiottbbttjj[2]);

    fprintf(Yields_ttjj_Unc,"Total Stat. & %.2f & %.2f \\\\\\hline \n",
	    r_Full_Xsecttjj_stat[1], r_Full_Xsecttjj_stat[2]);
    fprintf(Yields_R_Unc,"Total Stat. & %.5f & %.5f \\\\\\hline \n",
	    r_Full_XsecRatiottbbttjj_stat[1],r_Full_XsecRatiottbbttjj_stat[2]);
    
    
    for(int isys=0; isys<14; isys++){
      cout << "Nuisance Fit: Contribution from " << SystNam[isys] << endl;
      //r_NuMo_sys[isys]->Print() ;
      
      RooRealVar *Xsecttjj_fitresult = (RooRealVar*) r_NuMo_sys[isys]->floatParsFinal().find("Full_Xsecttjj"); 
      RooRealVar *XsecRatiottbbttjj_fitresult = (RooRealVar*) r_NuMo_sys[isys]->floatParsFinal().find("Full_C_Xsecttbb_Xsecttjj"); 
      
      r_Full_Xsecttjj[isys][0] = Xsecttjj_fitresult->getVal();
      r_Full_Xsecttjj[isys][1] = Xsecttjj_fitresult->getAsymErrorHi();
      r_Full_Xsecttjj[isys][2] = Xsecttjj_fitresult->getAsymErrorHi();
      
      r_Full_XsecRatiottbbttjj[isys][0] = XsecRatiottbbttjj_fitresult->getVal();
      r_Full_XsecRatiottbbttjj[isys][1] = XsecRatiottbbttjj_fitresult->getAsymErrorHi();
      r_Full_XsecRatiottbbttjj[isys][2] = XsecRatiottbbttjj_fitresult->getAsymErrorHi();

      per_Xsecttjj[isys][0] = 100.;
      per_Xsecttjj[isys][1] = 100.*r_Full_Xsecttjj[isys][1]/r_Full_Xsecttjj[isys][0];
      per_Xsecttjj[isys][2] = 100.*r_Full_Xsecttjj[isys][2]/r_Full_Xsecttjj[isys][0];
      per_XsecRatiottbbttjj[isys][0] = 100.;
      per_XsecRatiottbbttjj[isys][1] = 100.*r_Full_XsecRatiottbbttjj[isys][1]/r_Full_XsecRatiottbbttjj[isys][0];
      per_XsecRatiottbbttjj[isys][2] = 100.*r_Full_XsecRatiottbbttjj[isys][2]/r_Full_XsecRatiottbbttjj[isys][0];
      

      fprintf(Yields_ttjj,"%s & %.2f & %.3f(%.1f\\\%) & %.3f (%.1f\\\%) \\\\ \n",
	      SystNam[isys].Data(),r_Full_Xsecttjj[isys][0],r_Full_Xsecttjj[isys][1],per_Xsecttjj[isys][1] ,r_Full_Xsecttjj[isys][2],per_Xsecttjj[isys][2]);
      fprintf(Yields_R,"%s & %.4f & %.5f(%.1f\\\%) & %.5f (%.1f\\\%) \\\\ \n",
	      SystNam[isys].Data(),r_Full_XsecRatiottbbttjj[isys][0],r_Full_XsecRatiottbbttjj[isys][1],per_XsecRatiottbbttjj[isys][1] ,r_Full_XsecRatiottbbttjj[isys][2],per_XsecRatiottbbttjj[isys][2]);
     

      Nui_Sys_Xsecttjj[isys][1] = sqrt(fabs( (r_Full_Xsecttjj_nom[0]  - r_Full_Xsecttjj_nom[1] )*(r_Full_Xsecttjj_nom[0]  - r_Full_Xsecttjj_nom[1]) - 
					     (r_Full_Xsecttjj[isys][0] - r_Full_Xsecttjj[isys][1])*(r_Full_Xsecttjj[isys][0] - r_Full_Xsecttjj[isys][1])));
      Nui_Sys_Xsecttjj[isys][2] = sqrt(fabs( (r_Full_Xsecttjj_nom[0]  - r_Full_Xsecttjj_nom[2] )*(r_Full_Xsecttjj_nom[2]  - r_Full_Xsecttjj_nom[2]) - 
					     (r_Full_Xsecttjj[isys][0] - r_Full_Xsecttjj[isys][2])*(r_Full_Xsecttjj[isys][2] - r_Full_Xsecttjj[isys][2])));
      
      Nui_Sys_XsecRatiottbbttjj[isys][1] = sqrt(fabs( (r_Full_XsecRatiottbbttjj_nom[0]  - r_Full_XsecRatiottbbttjj_nom[1] )*(r_Full_XsecRatiottbbttjj_nom[0]  - r_Full_XsecRatiottbbttjj_nom[1]) - 
						      (r_Full_XsecRatiottbbttjj[isys][0] - r_Full_XsecRatiottbbttjj[isys][1])*(r_Full_XsecRatiottbbttjj[isys][0] - r_Full_XsecRatiottbbttjj[isys][1])));
      Nui_Sys_XsecRatiottbbttjj[isys][2] = sqrt(fabs( (r_Full_XsecRatiottbbttjj_nom[0]  - r_Full_XsecRatiottbbttjj_nom[2] )*(r_Full_XsecRatiottbbttjj_nom[0]  - r_Full_XsecRatiottbbttjj_nom[2]) - 
						      (r_Full_XsecRatiottbbttjj[isys][0] - r_Full_XsecRatiottbbttjj[isys][2])*(r_Full_XsecRatiottbbttjj[isys][0] - r_Full_XsecRatiottbbttjj[isys][2])));
      
      
      fprintf(Yields_ttjj_Unc,"%s & %.2f & %.2f \\\\ \n",
	      SystNam[isys].Data(),Nui_Sys_Xsecttjj[isys][1],Nui_Sys_Xsecttjj[isys][2]);

      fprintf(Yields_R_Unc,"%s & %.5f & %.5f \\\\ \n",
	      SystNam[isys].Data(),Nui_Sys_XsecRatiottbbttjj[isys][1],Nui_Sys_XsecRatiottbbttjj[isys][2]);
      
    }// for(isys)

    fprintf(Yields_ttjj,"\\hline \n");
    fprintf(Yields_R,"\\hline \n");

    fclose(Yields_ttjj);
    fclose(Yields_R);
    fclose(Yields_R_Unc);
    fclose(Yields_ttjj_Unc);

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
  // TString nca[3] = {"hCSV_Jet-2","hCSV_Jet-3","h2DCSV_Jet23"};
  TString nca[3] = {"hKinAdd1CSV","hKinAdd2CSV","h2DKinAddCSV"};
  
  HistoFit Output;

  for(int irca=0; irca<3;irca++){
    for(int irch=0; irch<2;irch++){
      if (irca<2){
	Output.hist1D[irca][irch] = (TH1D*)fInput->Get(Cut + "/" + nch[irch] + "/" + nca[irca] + "_" + nch[irch] + "_" + Cut)->Clone();
	// Avoid negative entries (from aMC@NLO MC)
	for(int ibin = 1; ibin<= Output.hist1D[irca][irch]->GetNbinsX();ibin++){
	  double binCont = Output.hist1D[irca][irch]->GetBinContent(ibin);
	  if (binCont < 0.0) Output.hist1D[irca][irch]->SetBinContent(ibin,0.0);
	}
      } // if(irca)
      else {
	Output.hist2D[irch] = (TH2D*)fInput->Get(Cut + "/" + nch[irch] + "/" + nca[irca] + "_" + nch[irch] + "_" + Cut)->Clone();
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
      
      fInputSys[Up]    = new TFile(dirnameIn + fl + "_" + FileName + "_SYS_" + SystNam[isys] + "_Up.root");
      fInputSys[Down]  = new TFile(dirnameIn + fl + "_" + FileName + "_SYS_" + SystNam[isys] + "_Down.root");
      // fInputSys[Up]   = new TFile(dirnameIn + fl + "_" + FileName + ".root");
      // fInputSys[Down] = new TFile(dirnameIn + fl + "_" + FileName + ".root");
      
      if(!fInputSys[Nom]->GetFile() || !fInputSys[Up]->GetFile() || !fInputSys[Down]->GetFile()){
	std::cerr << SystNam[isys] << " systematic variation for " << dirnameIn + fl +  FileName << " not Found!!!" << std::endl;
	std::exit(0);
      }  
      
      for(unsigned int ivar = 0; ivar<3; ivar++){
	for(int irca=0; irca<3;irca++){
	  for(int irch=0; irch<2;irch++){
	    if (irca<2){
	      Output.hsyst1D[isys][ivar][irca][irch] = (TH1D*)fInputSys[ivar]->Get(Cut + "/" + nch[irch] + "/" +  nca[irca] + "_" + nch[irch] + "_" + Cut)->Clone();
	      // Avoid negative entries (from aMC@NLO MC)
	      for(int ibin = 1; ibin<= Output.hsyst1D[isys][ivar][irca][irch]->GetNbinsX();ibin++){
		double binCont = Output.hsyst1D[isys][ivar][irca][irch]->GetBinContent(ibin);
		if (binCont < 0.0) Output.hsyst1D[isys][ivar][irca][irch]->SetBinContent(ibin,0.0);
	      }
	      // Normalize wrt the number of events in the nominal??? (to take into account the yields variations... Is it needed?)
	      Output.hsyst1D[isys][ivar][irca][irch]->Scale(1.0/Output.hsyst1D[isys][ivar][irca][irch]->Integral());
	    } // if(irca)
	    else {
	      Output.hsyst2D[isys][ivar][irch] = (TH2D*)fInputSys[ivar]->Get(Cut + "/" + nch[irch] + "/" + nca[irca] + "_" + nch[irch] + "_" + Cut)->Clone();
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
	    // Normalize wrt the number of events in the nominal??? (to take into account the yields variations... Is it needed?)
	    Output.hsyst1D[isys][ivar][irca][2]->Scale(1.0/Output.hsyst1D[isys][ivar][irca][2]->Integral());
	  }
	  else{
	    Output.hsyst2D[isys][ivar][2] = (TH2D*)Output.hsyst2D[isys][ivar][0]->Clone();
	    Output.hsyst2D[isys][ivar][2]->Add(Output.hsyst2D[isys][ivar][1]);    
	    // Normalize wrt the number of events in the nominal??? (to take into account the yields variations... Is it needed?)
	    Output.hsyst2D[isys][ivar][2]->Scale(1.0/Output.hsyst2D[isys][ivar][2]->Integral());
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
  // model_comps->Print("v");
  // cout << model_comps->contentsString () << endl; 

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


RooPlot *PlotPDF_NuMo(RooRealVar *var, RooWorkspace *WS, RooDataHist *DataHis, bool PlotData, TLegend *leg){
  using namespace RooFit;

  // Normalization
  float nttjj = WS->var("n_ttjj_var")->getVal()*WS->var("k")->getVal();
  float FRatio = WS->var("FRatio_ttbbttjj")->getVal();
  float CRatio = WS->var("CRatio_ttbjttbb")->getVal();
  
  float nttbb   = FRatio*nttjj; 
  float nttbj   = FRatio*CRatio*nttjj; 
  float nttccLF = (1.-FRatio-FRatio*CRatio)*nttjj; 
  
  float nbin = 20.;

  RooPlot *Plot = var->frame();
  RooArgSet* model_comps = WS->function("TotModel23_SysCons")->getComponents() ;
  // model_comps->Print("v");
  // cout << model_comps->contentsString () << endl; 

  if (PlotData) DataHis->plotOn (Plot, 
				 Name("Data"));
  else DataHis->plotOn (Plot, 
			Name("Data"), Invisible());

  WS->function("TotModel23_SysCons")->plotOn (Plot, 
					      LineColor(kRed), 
					      Name("FullModel"));
  WS->function("TotModel23_SysCons")->plotOn (Plot, 
					      Components("Bkgtt_PInterSyst23,BkgOther_PInterSyst23"), 
					      LineColor(colors[BkgFull]), 
					      Name("Bkg"));
  WS->function("TotModel23_SysCons")->plotOn (Plot, 
					      Components("Full_C_ttjj_PInterSyst23"),
					      LineColor(colors[ttjj]), 
					      RooFit::Name("ttjj"));
  WS->function("ttbb_PInterSyst23")->plotOn (Plot,
					     LineColor(colors[ttbb]),
					     Normalization(nttbb/(nbin)),
					     RooFit::Name("ttbb"));
  WS->function("ttb_PInterSyst23")->plotOn (Plot,
					    LineColor(colors[ttb]),
					    RooFit::Name("ttb"),
					    Normalization(nttbj/(nbin)));
  WS->function("ttccLF_PInterSyst23")->plotOn (Plot,
  					       LineColor(colors[ttccLF]),
  					       Normalization(nttccLF/(nbin)),
  					       RooFit::Name("ttccLF"));
  
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
  leg->AddEntry(Plot->findObject("ttbb"),"ttbb","l");
  leg->AddEntry(Plot->findObject("ttb"),"ttbj","l");
  leg->AddEntry(Plot->findObject("ttccLF"),"ttcc+ttLF","l");
  
  return Plot;  
}

