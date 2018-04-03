class Cat{
public:
  double Nfull[3][3];
  double Nvis [3][3];
  double N    [3][3];
  double Nnw  [3][3];
  double Acc  [3][3];
  double Eff  [3][3];
  double NormW[3];

  TString TableLine   [3][3];
  TString SysTableLine[3];
};

enum sysvar {Nom=0, Down, Up};
TString varname[3] = {"Nom","Down","Up"};

enum level  {GEN=0, RECO};
enum ech {mujets=0,ejets,lepjets};
TString chname[3] = {"mujets","ejets","lepjets"};

enum ettcat  {ttjj=0, ttbb, ttbj, ttcc, ttLF, tt};
TString catname[6] = {"ttjj","ttbb","ttbj","ttcc","ttLF","tt"};

TString tabname[2] = {"tab","systab"};

enum syssource{ScaleRnF=0, ScaleRuF, ScaleRdF, ISR, FSR, UE, hdamp, EDR, gCR, gCREDR, QCDCREDR};

TString ToString (double number, double precision = 1.){
  ostringstream ostemp;  
  ostemp << TMath::Nint(number*precision)/precision;
  return ostemp.str();
}

double Uncertainty (double vnom, double vdown, double vup, TString source = ""){

  double DownVar = std::abs(vnom - vdown)/vnom;
  double UpVar   = std::abs(vnom - vup)  /vnom;

  if(source.Contains("FSR")){
    cout << "Reducing the " << source << " variation! " << endl;
    float CorrSqr2[3] = {1.0,1.0,1.0};
    CorrSqr2[Up]   = 0.40; // 40% from 2 to sqrt(2)
    CorrSqr2[Down] = 0.60; // 60% from 1/2 to 1/sqrt(2)
    return 100.*std::max(CorrSqr2[Down]*DownVar,CorrSqr2[Up]*UpVar);
  }
  
  // return 100.*(DownVar+UpVar)*0.5; // Mean value
  // return 100.*std::min(DownVar,UpVar); // Min value
  return 100.*std::max(DownVar,UpVar); // Max value

}

TString TableEntry (Cat pr, int var, int ch, TString source = ""){

  return source                             + " & " + 
    varname[var]                            + " & " + 
    ToString(pr.Nfull[var][ch])             + " & " +
    ToString(pr.Nvis[var][ch])              + " & " +
    ToString((pr.Nvis[var][ch]*pr.NormW[var])) + " & " +
    ToString(pr.Nnw[var][ch])               + " & " +
    ToString(pr.N[var][ch])                 + " & " +
    ToString(pr.Acc[var][ch],100.)          + " & " +
    ToString(pr.Eff[var][ch],100.) + " \\\\ ";
  
}

TString SysTableEntry (Cat pr, int ch, TString source = "", TString ttcat = ""){

  float Un_Nfull = Uncertainty (pr.Nfull[Nom][ch], pr.Nfull[Down][ch], pr.Nfull[Up][ch],source);
  float Un_Nvis  = Uncertainty (pr.Nvis[Nom][ch], pr.Nvis[Down][ch], pr.Nvis[Up][ch],source);
  float Un_NvisW = Uncertainty ((pr.NormW[Nom]*pr.Nvis[Nom][ch]), (pr.NormW[Down]*pr.Nvis[Down][ch]), (pr.NormW[Up]*pr.Nvis[Up][ch]),source);

  float Un_Nnw = Uncertainty (pr.Nnw[Nom][ch], pr.Nnw[Down][ch], pr.Nnw[Up][ch],source);
  float Un_N   = Uncertainty (pr.N[Nom][ch], pr.N[Down][ch], pr.N[Up][ch],source);

  float Un_Acc = Uncertainty (pr.Acc[Nom][ch], pr.Acc[Down][ch], pr.Acc[Up][ch],source);
  float Un_Eff = Uncertainty (pr.Eff[Nom][ch], pr.Eff[Down][ch], pr.Eff[Up][ch],source);

  float Un_AccEff = Uncertainty (pr.Acc[Nom][ch]  * pr.Eff[Nom][ch], 
				 pr.Acc[Down][ch] * pr.Eff[Down][ch], 
				 pr.Acc[Up][ch]   * pr.Eff[Up][ch],source);  

  return source              + " & " + 
    ttcat                    + " & " +
    ToString(Un_Nfull,10.)   + " & " +
    ToString(Un_Nvis,10.)    + " & " +
    ToString(Un_NvisW,10.)   + " & " +
    ToString(Un_Nnw,10.)     + " & " +
    ToString(Un_N,10.)       + " & " +
    ToString(Un_Acc,100.)    + " & " +
    ToString(Un_Eff,100.)    + " & " +
    ToString(Un_AccEff,100.) + " & " ;

}

TString SysTableAccEffEntry (Cat prjj, Cat prbb, int ch, TString source){

  float Un_Accttbbttjj = Uncertainty ((prbb.Acc[Nom][ch] / prjj.Acc[Nom][ch]),
				      (prbb.Acc[Down][ch]/ prjj.Acc[Down][ch]),
				      (prbb.Acc[Up][ch]  / prjj.Acc[Up][ch]),source);

  float Un_Effttbbttjj = Uncertainty ((prbb.Eff[Nom][ch] / prjj.Eff[Nom][ch]),
				      (prbb.Eff[Down][ch]/ prjj.Eff[Down][ch]),
				      (prbb.Eff[Up][ch]  / prjj.Eff[Up][ch]),source);

  float Un_AccEffttbbttjj = Uncertainty ( ( (prbb.Acc[Nom][ch]  * prbb.Eff[Nom][ch])   / (prjj.Acc[Nom][ch]  * prjj.Eff[Nom][ch])  ),
					  ( (prbb.Acc[Down][ch] * prbb.Eff[Down][ch])  / (prjj.Acc[Down][ch] * prjj.Eff[Down][ch]) ),
					  ( (prbb.Acc[Up][ch]   * prbb.Eff[Up][ch])    / (prjj.Acc[Up][ch]   * prjj.Eff[Up][ch])   ),source );

  TString MRT = "\\multirow{2}{*}{ ";

  return  MRT + ToString(Un_Accttbbttjj,100.) + " } & " +
    MRT + ToString(Un_Effttbbttjj,100.)       + " } & " +
    MRT + ToString(Un_AccEffttbbttjj,100.)    + " } \\\\ \\cline{2-10} ";
  

}

std::map<TString, TString> EffAccCreator(TString systname, TString Variation1, TString Variation2, TString FileVersion, TString basename){
  
  std::map<TString, TString>  Tables;
  Cat ttcat[6];

  for(int icat=ttjj; icat<=tt; icat++){  

    TString namefile = FileVersion + "_" + basename + catname[icat];

    TFile *file[2][3] = {NULL, NULL, NULL,
			 NULL, NULL, NULL};
    // GEN
    file[GEN][Nom]  = TFile::Open("TopResults/" + FileVersion + "/hAcc-" + namefile + ".root");
    file[GEN][Down] = TFile::Open("TopResults/" + FileVersion + "/hAcc-" + namefile +  "_SYS_" + systname + Variation1 + ".root");
    file[GEN][Up]   = TFile::Open("TopResults/" + FileVersion + "/hAcc-" + namefile +  "_SYS_" + systname + Variation2 + ".root");

    // RECO
    file[RECO][Nom]  = TFile::Open("TopResults/" + FileVersion + "/hSF-" + namefile + ".root");
    file[RECO][Down] = TFile::Open("TopResults/" + FileVersion + "/hSF-" + namefile +  "_SYS_" + systname + Variation1 + ".root");
    file[RECO][Up]   = TFile::Open("TopResults/" + FileVersion + "/hSF-" + namefile +  "_SYS_" + systname + Variation2 + ".root");

    TH1D *RECOYi[3]     = {NULL,NULL,NULL};
    TH1D *RECOYiNoWe[3] = {NULL,NULL,NULL};
    TH2D *GENYiFull[3]  = {NULL,NULL,NULL};
    TH2D *GENYiVis[3]   = {NULL,NULL,NULL}; 

    TString dirname[3];
    dirname[Nom]  = "central";
    TString hyieldname[3];
    hyieldname[Nom]  = "";
    if(systname.Contains("hdamp") || systname.Contains("UE") || systname.Contains("SR") || systname.Contains("ScaleR")){
      dirname[Down] = systname + Variation1;
      dirname[Up]   = systname + Variation2;
      hyieldname[Down] = "_" + systname + Variation1;
      hyieldname[Up]   = "_" + systname + Variation2;
    }
    else {
      dirname[Down] = "central";
      dirname[Up]   = "central";
      hyieldname[Down] = "";
      hyieldname[Up]   = "";
    }

    for (unsigned int ivar=Nom; ivar<=Up; ivar++){

      cout << "Loading " << dirname[ivar] << " histograms..." << endl;

      RECOYi[ivar]     = (TH1D*)file[RECO][ivar]->Get(dirname[ivar]+"/Yields" + hyieldname[ivar])         ->Clone("RECO"   + dirname[ivar]);  
      RECOYiNoWe[ivar] = (TH1D*)file[RECO][ivar]->Get(dirname[ivar]+"/YieldsNoWeights" + hyieldname[ivar])->Clone("RECONW" + dirname[ivar]);  

      cout << "Reco" << endl;

      GENYiFull[ivar]  = (TH2D*)file[GEN][ivar]->Get("Yields_FullPh-Sp_" + catname[icat])->Clone("GENFull" + dirname[ivar]);  
      cout << "Acc1" << endl;
      GENYiVis [ivar]  = (TH2D*)file[GEN][ivar]->Get("Yields_VisPh-Sp_"  + catname[icat])->Clone("GENVis"  + dirname[ivar]);  
    
      cout << "Histograms " << dirname[ivar] << " loaded..." << endl;
    
    }

    cout << "Histograms loaded..." << endl;

    for (unsigned int ivar=Nom; ivar<=Up; ivar++){
    
      // Last bin has the normalization factor (NormW)
      ttcat[icat].NormW[ivar] =  RECOYi[ivar]->GetBinContent(RECOYi[ivar]->GetNbinsX());
      for (unsigned int ich=0; ich<=2; ich++){
	// -----------------------------
	// -- Full Ph-Sp
	// -----------------------------
	ttcat[icat].Nfull[ivar][ich] = GENYiFull[ivar]->GetBinContent(ich+1,1);
	// -----------------------------
	// -- Visible Ph-Sp
	// -----------------------------
	ttcat[icat].Nvis[ivar][ich] = GENYiVis[ivar]->GetBinContent(ich+1,1);
	// -----------------------------
	// -- RECO Ph-Sp (2 b-tag level is bin 7)      
	// -----------------------------
	ttcat[icat].N[ivar][ich]   = RECOYi[ivar]    ->GetBinContent(7+ich);
	ttcat[icat].Nnw[ivar][ich] = RECOYiNoWe[ivar]->GetBinContent(7+ich);

	// -----------------------------
	// ttjj and ttbb Acceptance
	// -----------------------------
	ttcat[icat].Acc[ivar][ich] = 100.*(ttcat[icat].Nvis[ivar][ich]/ttcat[icat].Nfull[ivar][ich]);
	// -----------------------------
	// ttjj and ttbb Efficiency
	// -----------------------------
	ttcat[icat].Eff[ivar][ich] = 100.*(ttcat[icat].N[ivar][ich]/(ttcat[icat].NormW[ivar]*ttcat[icat].Nvis[ivar][ich]));

      } // for(ich)
    } // for(ivar)
  
    for (unsigned int ivar=Nom; ivar<=Up; ivar++){
      for (unsigned int ich=0; ich<=2; ich++){	
	Tables[tabname[0]+catname[icat]+chname[ich]+varname[ivar]] = TableEntry (ttcat[icat], ivar, ich, systname) ;	
      } // for(ivar)
    } // for(ich)
  } // for(icat)
    
  // Table comparing Acc and Eff of ttbb w.r.t. the ttjj
  for(int icat=ttbb; icat<=tt; icat++){  
    for (unsigned int ich=0; ich<=2; ich++){    
      Tables[tabname[1]+catname[ttjj]+catname[icat]+chname[ich]] = SysTableEntry (ttcat[ttjj], ich, systname, catname[ttjj]) + SysTableAccEffEntry (ttcat[ttjj],ttcat[icat], ich, systname); 
      Tables[tabname[1]+catname[icat]+chname[ich]] = SysTableEntry (ttcat[icat], ich, systname, catname[icat]) + " &  &  \\\\ \\hline";
    } // for(ich)
  } // for(icat)
  
  return Tables;
}

void CreateTable (std::map<TString, TString>  tLine[6], int tch, int tcat, TString dirTeX){
  
  // -----------------------------
  // Table 1
  // -----------------------------
  FILE*   Yields_Sys     = fopen(dirTeX + "YieldsSys_" + catname[tcat] + chname[tch] + ".tex", "w");
  
  fprintf(Yields_Sys,"~{\\Tiny \\def \\arraystretch{2} \\tabcolsep=4pt \n");
  
  fprintf(Yields_Sys,"\\begin{tabular}{|l|c|c|c|c|c|c|c|c|}\\hline \n");
  fprintf(Yields_Sys,"\\multirow{2}{*}{Source} & \\multirow{2}{*}{Var} & \\multicolumn{3}{c|}{GEN} & \\multicolumn{2}{c|}{RECO} & \\multirow{2}{*}{\\Acc} & \\multirow{2}{*}{\\Eff} \\\\ \\cline{3-7} \n");
  fprintf(Yields_Sys,"& & Full & Vis & Vis Weight & 2btag & 2btag Weight & &  \\\\ \\hline\\hline \n");   

  fprintf(Yields_Sys,"\\multicolumn{9}{|c|}{Process: %s -- Channel: %s } \\\\ \\hline \n",
	  catname[tcat].Data(), chname[tch].Data());

  for(int isys=ScaleRnF; isys<=QCDCREDR; isys++){
    fprintf(Yields_Sys,"%s \n",
	    tLine[isys][tabname[0]+catname[tcat]+chname[tch]+varname[Nom]].Data());
    fprintf(Yields_Sys,"%s \n",
	    tLine[isys][tabname[0]+catname[tcat]+chname[tch]+varname[Down]].Data());
    fprintf(Yields_Sys,"%s \\hline \n",
	    tLine[isys][tabname[0]+catname[tcat]+chname[tch]+varname[Up]].Data());
  }
    
  fprintf(Yields_Sys,"\\end{tabular} \n } \n");

  fclose (Yields_Sys);
}


void CreateComTable (std::map<TString, TString>  tLine[6], int tch, int tcat, TString dirTeX){

  // -----------------------------
  // Table 2
  // -----------------------------
  FILE*   Comp_Sys = fopen(dirTeX + "ComparisonSys_ttjjVs" + catname[tcat] + "_" + chname[tch] + ".tex", "w");

  fprintf(Comp_Sys,"~{\\Tiny \\def \\arraystretch{2} \\tabcolsep=4pt \n");
  fprintf(Comp_Sys,"");
  fprintf(Comp_Sys,"\\begin{tabular}{|l|c|c|c|c|c|c|c|c|c|c|c|c|}\\hline \n");
  fprintf(Comp_Sys,"\\multirow{2}{*}{Source} & \\multirow{2}{*}{Cat} & \\multicolumn{3}{c|}{GEN} & \\multicolumn{2}{c|}{RECO} & \\multirow{2}{*}{\\Acc} & \\multirow{2}{*}{\\Eff} & \\multirow{2}{*}{\\Acc\\Eff} & \\multirow{2}{*}{\\Acc/\\Acc} & \\multirow{2}{*}{\\Eff/\\Eff} & \\multirow{2}{*}{\\Eff\\Acc/\\Eff\\Acc}  \\\\\\cline{3-7} \n");
  fprintf(Comp_Sys,"& & Full & Vis & VisW & 2btag & 2btagW & & & & & & \\\\\\hline\\hline \n"); 
  fprintf(Comp_Sys,"\\multicolumn{13}{|c|}{Channel: %s } \\\\ \\hline \n",
	  chname[tch].Data());

  for(int isys=ScaleRnF; isys<=QCDCREDR; isys++){

    fprintf(Comp_Sys,"%s \n",
  	    tLine[isys][tabname[1]+catname[0]+catname[tcat]+chname[tch]].Data() );    
    fprintf(Comp_Sys,"%s \n",
    	    tLine[isys][tabname[1]+catname[tcat]+chname[tch]].Data());    
    
  }   
  
  fprintf(Comp_Sys,"\\end{tabular} \n } \n");
 
  fclose (Comp_Sys);


}


void EffAccEstimation(TString FileVersion = "Full-v0", TString fbasename = "Tree_LepJets_EGTightSkim_v8-0-6_Spring16-80X_36814pb-1", TString ttbarname = "ttbar_PowhegPythia"){
//void EffAccEstimation(TString FileVersion = "JESCom-v0", TString fbasename = "Tree_LepJets_FallSkim_v8-0-6_Spring16-80X_36814pb-1", TString ttbarname = "ttbar_PowhegPythia"){
//void EffAccEstimation(TString FileVersion = "3btag-v0", TString fbasename = "Tree_LepJets_EGTightSkim_v8-0-6_Spring16-80X_36814pb-1", TString ttbarname = "ttbar_PowhegPythia"){

  
  std::map<TString, TString>  fTables[11]; // [systematics]
  
  fTables[hdamp] = EffAccCreator("hdamp",
				 "Down",
				 "Up",
				 FileVersion, 
				 fbasename + "_" + ttbarname);
  
  fTables[ISR] = EffAccCreator("ISR",
			       "Down",
			       "Up",
			       FileVersion, 
			       fbasename + "_" + ttbarname);
  
  fTables[FSR] = EffAccCreator("FSR",
			       "Down",
			       "Up",
			       FileVersion, 
			       fbasename + "_" + ttbarname);
  
  fTables[UE] = EffAccCreator("UE",
			      "Down",
			      "Up",
			      FileVersion, 
			      fbasename + "_" + ttbarname);

  fTables[EDR] = EffAccCreator("EDR",
			      "",
			      "",
			      FileVersion, 
			      fbasename + "_" + ttbarname);

  fTables[gCR] = EffAccCreator("gCR",
			       "",
			       "",
			       FileVersion, 
			       fbasename + "_" + ttbarname);
  
  fTables[gCREDR] = EffAccCreator("gCREDR",
				  "",
				  "",
				  FileVersion, 
				  fbasename + "_" + ttbarname);

  fTables[QCDCREDR] = EffAccCreator("QCDCREDR",
				    "",
				    "",
				    FileVersion, 
				    fbasename + "_" + ttbarname);

  ttbarname = "ttbar_LepJetsPowhegPythia";
  
  fTables[ScaleRnF] = EffAccCreator("ScaleRnF",
				    "Down",
				    "Up",
				    FileVersion, 
				    fbasename + "_" + ttbarname);
  
  fTables[ScaleRuF] = EffAccCreator("ScaleRuF",
				    "Nom",
				    "Up",
				    FileVersion, 
				    fbasename + "_" + ttbarname);
  
  fTables[ScaleRdF] = EffAccCreator("ScaleRdF",
				    "Up",
				    "Down",
				    FileVersion, 
				    fbasename + "_" + ttbarname);
  
  
  // LaTeX table
  TString dirTeXname;
  dirTeXname = "TopResults/TeX_Tables_hSF-" + fbasename + "_Yields/";
  // make a dir if it does not exist!!
  gSystem->mkdir(dirTeXname, kTRUE);


  // Tables
  for(int icat=ttjj; icat<=tt; icat++){  
    CreateTable (fTables, mujets,  icat, dirTeXname);
    CreateTable (fTables, ejets,   icat, dirTeXname);
    CreateTable (fTables, lepjets, icat, dirTeXname);
    if (icat !=ttjj){
      CreateComTable (fTables, mujets,  icat, dirTeXname);
      CreateComTable (fTables, ejets,   icat, dirTeXname);
      CreateComTable (fTables, lepjets, icat, dirTeXname);
    }
  }

}
