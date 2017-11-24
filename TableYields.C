#include<string>
#include<iostream>
#include<sstream>
#include<stdio.h>
#include<stdlib.h>
#include<cmath>
#include<set>
#include<vector>
#include <sys/stat.h>

// Root
#include "TString.h"
#include "TRegexp.h"
#include "TDirectory.h"
#include "TROOT.h"
#include "TObject.h"
#include "TCanvas.h"
#include "TStopwatch.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TProfile.h"
#include "TFile.h"
#include "TChain.h"
#include "TLorentzVector.h"
#include "TSystem.h"
#include "TTree.h"
#include "TError.h"

struct Yields{
  std::vector<float>   Evt;
  std::vector<float>   Error;
  TString              Name;
  TString              SamComName;
  TString              InDataCard;
  std::vector<float>   RatioComb[2];
};

Yields loadhistoYields(TString SelCut, TString TName, TString HeadFile, TString SampleFile, TString AddToDC = "");
void   EntryPrinter   (FILE *file, Yields Sample);
void   AddhistoYields (Yields *s1, Yields *s2);
void   CreateDataCard (FILE *file, std::vector<Yields> Samples, TString ljchannel, TString HeadFile,  TString HistoName, bool DCNorm = false);
void   RemoveNegativeBins(TH1D *histo);

void display_usage()
{
  std::cout << "\033[1;37musage:\033[1;m skimfile cutindex [options]" << std::endl;
  std::cout << "" << std::endl;
  std::cout << "Options:" << std::endl;
  std::cout << "    -i inputfile  Input file without .root" << std::endl;
  std::cout << "    -o name of the output directory. Default: Yields" << std::endl;
  std::cout << "    -d Input file directory. Default directory: TopResults" << std::endl;
  std::cout << "    -combine datacardname Create datacard for COMBINE." << std::endl;
  std::cout << "    -his Histo name to use in COMBINE." << std::endl;
  std::cout << "    -h displays this help message and exits " << std::endl;
  std::cout << "" << std::endl;
}



int main (int argc, char *argv[]){


  const char * _output   = "Yields";
  const char * _outputDC;
  const char * _input    = 0;
  const char * _dir      = "TopResults/";
  const char * _cut      = 0;
  const char * _histoDC = "hKinAddCSVUnroll";  
  
  bool _createDC = false;

  // Arguments used
  //Parsing input options
  if(argc == 1){
    display_usage();
    return -1;
  }
  else{
    //Argumet 1 must be a valid input fileName
    for (int i = 1; i < argc; i++){
      if( strcmp(argv[i],"-i") == 0 ){
	_input = argv[i+1];
	i++;
      }
      if( strcmp(argv[i],"-d") == 0 ){
	_dir = argv[i+1];
	i++;
      }
      if( strcmp(argv[i],"-o") == 0 ){
	_output= argv[i+1];
	i++;
      }
      if( strcmp(argv[i],"-combine") == 0 ){
	_createDC= true;
	_outputDC= argv[i+1];
	i++;
      }
      if( strcmp(argv[i],"-his") == 0 ){
	_histoDC= argv[i+1];
	i++;
      }
      if( strcmp(argv[i],"-cut") == 0 ){
	_cut= argv[i+1];
	i++;
      }
      if( strcmp(argv[i],"-h") == 0 ||
	  strcmp(argv[i],"--help") == 0 ){
	display_usage();
	return 0;
      }
    }
  }//else
  if( _input ==0 ){
    std::cerr << "\033[1;31mskimfile ERROR:\033[1;m The '-i' option is mandatory!"
	      << std::endl;
    display_usage();
    return -1;
  }
  
  // reassigning
  TString fname(_input);
  TString outfiledir(_output);
  TString outDataCard(_outputDC);
  TString fdir(_dir);
  TString cutname(_cut);
  TString hisComname(_histoDC);

  ///////////////////////////////////////
  // Please, IGNORE. Temporal solution //
  ///////////////////////////////////////
  TCanvas *mydummycanvas=new TCanvas();// 
  ///////////////////////////////////////

  std::vector<Yields> Samples;

  // Yields ttbar_1;
  // ttbar_1 = loadhistoYields(cutname, "ttbar Herwig", fdir + fname, "ttbar_PowhegHerwig");
  // Samples.push_back(ttbar_1);
  // Yields ttbar_2;
  // ttbar_2 = loadhistoYields(cutname, "ttbar Evt", fdir + fname, "ttbar_PowhegPythiaEvt");
  // Samples.push_back(ttbar_2);
  // Yields ttbar;
  // ttbar = loadhistoYields(cutname, "\\ttbar", fdir + fname, "ttbar_LepJetsPowhegPythia");
  // Samples.push_back(ttbar);
    
  Yields ttbb;
  ttbb = loadhistoYields(cutname, "\\ttbar\\bbbar", fdir + fname, "ttbar_LepJetsPowhegPythiattbb", "DCSysThe");
  Samples.push_back(ttbb);
  Yields ttb;
  ttb = loadhistoYields(cutname, "\\ttbar\\qb", fdir + fname, "ttbar_LepJetsPowhegPythiattbj", "DCSysThe");
  Samples.push_back(ttb);
  Yields ttcc;
  ttcc = loadhistoYields(cutname, "\\ttbar\\ccbar", fdir + fname, "ttbar_LepJetsPowhegPythiattcc", "DCSysThe");
  Samples.push_back(ttcc);
  Yields ttLF;
  ttLF = loadhistoYields(cutname, "\\ttbar LF", fdir + fname, "ttbar_LepJetsPowhegPythiattLF", "DCSysThe");
  Samples.push_back(ttLF);
  Yields ttjj;
  ttjj = loadhistoYields(cutname, "\\ttbar\\jj", fdir + fname, "ttbar_LepJetsPowhegPythiattjj");
  Samples.push_back(ttjj);

  Yields ttOther;
  ttOther = loadhistoYields(cutname, "\\ttbar\\ Other", fdir + fname, "ttbar_PowhegPythiaBkgtt", "DCSysThe");
  Samples.push_back(ttOther);

  Yields tW;
  tW = loadhistoYields(cutname, "Single Top", fdir + fname, "SingleTop", "DCSys");
  Samples.push_back(tW);

  Yields WJets;
  WJets = loadhistoYields(cutname, "W+Jets", fdir + fname, "WJets_aMCatNLO", "DC");
  Samples.push_back(WJets);

  Yields ZJets;
  ZJets = loadhistoYields(cutname, "Z+Jets", fdir + fname, "ZJets_aMCatNLO", "DC");
  Samples.push_back(ZJets);

  Yields VV;
  VV = loadhistoYields(cutname, "VV", fdir + fname, "VV", "DCSys");
  Samples.push_back(VV);

  Yields QCD;
  QCD = loadhistoYields(cutname, "QCD", fdir + fname, "QCD", "DC");
  Samples.push_back(QCD);

  Yields ttH;
  ttH = loadhistoYields(cutname, "\\ttbar H", fdir + fname, "ttHbb_PowhegPythia", "DC");
  Samples.push_back(ttH);

  Yields ttV;
  ttV = loadhistoYields(cutname, "\\ttbar V", fdir + fname, "ttV_Madgraph", "DC");
  Samples.push_back(ttV);

  Yields BkgMC;
  BkgMC = loadhistoYields(cutname, "Total Bkg MC", fdir + fname, "ttbar_PowhegPythiaBkgtt");
  AddhistoYields(&BkgMC,&tW);
  AddhistoYields(&BkgMC,&WJets);
  AddhistoYields(&BkgMC,&ZJets);
  AddhistoYields(&BkgMC,&VV);
  AddhistoYields(&BkgMC,&QCD);
  AddhistoYields(&BkgMC,&ttH);
  AddhistoYields(&BkgMC,&ttV);

  Samples.push_back(BkgMC);

  Yields TotalMC;
  TotalMC = loadhistoYields(cutname, "Total MC", fdir + fname, "ttbar_LepJetsPowhegPythiattjj");
  AddhistoYields(&TotalMC,&ttOther);
  AddhistoYields(&TotalMC,&tW);
  AddhistoYields(&TotalMC,&WJets);
  AddhistoYields(&TotalMC,&ZJets);
  AddhistoYields(&TotalMC,&VV);
  AddhistoYields(&TotalMC,&QCD);
  AddhistoYields(&TotalMC,&ttH);
  AddhistoYields(&TotalMC,&ttV);
 
  Samples.push_back(TotalMC);

  Yields Data;
  Data = loadhistoYields(cutname, "Data", fdir + fname, "DataSingleLep");
  Samples.push_back(Data);

  std::cout << "All samples loaded!" << std::endl;
  
  // LaTeX table
  TString dirTeXname;
  dirTeXname = "TopResults/TeX_Tables_" + fname + "_" + outfiledir + "/";
  // make a dir if it does not exist!!
  gSystem->mkdir(dirTeXname, kTRUE);

  // Yields
  TString YieldsTeXfile  = dirTeXname + "Yields_" + cutname + ".tex";
  FILE*   Yields         = fopen(YieldsTeXfile, "w");

  fprintf(Yields,"~{\\Tiny \\def \\arraystretch{2} \\tabcolsep=4pt \n");
  fprintf(Yields,"\\begin{tabular}{|l|r@{~$\\pm$~}l|r@{~$\\pm$~}l|r@{~$\\pm$~}l|}\\hline\n");
  fprintf(Yields,"Source & \\multicolumn{2}{c|}{${\\mu}$+Jets} & \\multicolumn{2}{c|}{${\\e}$+Jets} & \\multicolumn{2}{c|}{${\\ell}$+Jets} \\\\\\hline\\hline\n");
  
  for(int ns = 0; ns < Samples.size(); ns++)  EntryPrinter(Yields, Samples.at(ns));

  fprintf(Yields,"Data/MC & %.2f & -- & %.2f & -- & %.2f & --   \\\\\\hline\n", 
	  Data.Evt[0]/TotalMC.Evt[0],
	  Data.Evt[1]/TotalMC.Evt[1],
	  Data.Evt[2]/TotalMC.Evt[2]);
  
  fprintf(Yields,"\\end{tabular}\n }");

  fclose(Yields);

  std::cout << "Table with yields has been done...................." << std::endl;
  std::cout <<  YieldsTeXfile << " has been created. " << std::endl;

  std::cout << "\nFor the Combine Model:\n" << std::endl;

  std::cout << "Par_MCRatios=\"--PO RMC_ttbbttjj=" <<  ttbb.Evt[2]/ttjj.Evt[2] ;
  std::cout << " --PO RMC_ttbjttjj=" <<  ttb.Evt[2]/ttjj.Evt[2] ;
  std::cout << " --PO RMC_ttccLFttjj=" <<  (ttcc.Evt[2]+ttLF.Evt[2])/ttjj.Evt[2] ;
  std::cout << " --PO RMC_ttbjttbb=" <<  ttb.Evt[2]/ttbb.Evt[2] << "\"" << std::endl ;


  //--------------------------------------------
  // DataCard
  //--------------------------------------------

  if(_createDC){

    TString dirCombinename;
    dirCombinename = "TopResults/Combine_" + fname + "_" + outfiledir + "/";
    // make a dir if it does not exist!!
    gSystem->mkdir(dirCombinename, kTRUE);

    TString CombineDCfile_mu  = dirCombinename +  outDataCard + "_mujets_" + fname + "_" + cutname + ".txt";
    FILE*   DataCard_mu       = fopen(CombineDCfile_mu, "w");
  
    CreateDataCard(DataCard_mu, Samples, "mujets", fdir + fname, hisComname, false);
    fclose(DataCard_mu);

    TString CombineDCfile_e  = dirCombinename + outDataCard + "_ejets_" + fname + "_" + cutname + ".txt";
    FILE*   DataCard_e       = fopen(CombineDCfile_e, "w");
  
    CreateDataCard(DataCard_e, Samples, "ejets", fdir + fname, hisComname, false);
    fclose(DataCard_e);

    TString CombineDCfileNorm_mu  = dirCombinename +  outDataCard + "_Norm_mujets_" + fname + "_" + cutname + ".txt";
    FILE*   DataCardNorm_mu       = fopen(CombineDCfileNorm_mu, "w");
  
    CreateDataCard(DataCardNorm_mu, Samples, "mujets", fdir + fname, hisComname, true);
    fclose(DataCardNorm_mu);

    TString CombineDCfileNorm_e  = dirCombinename + outDataCard + "_Norm_ejets_" + fname + "_" + cutname + ".txt";
    FILE*   DataCardNorm_e       = fopen(CombineDCfileNorm_e, "w");
  
    CreateDataCard(DataCardNorm_e, Samples, "ejets", fdir + fname, hisComname, true);
    fclose(DataCardNorm_e);

  }


  return 0;  

}// end Code

void AddhistoYields(Yields *s1, Yields *s2){

  for (unsigned int i = 0; i < 3; i++){ 
    s1->Evt[i] = s1->Evt[i] + s2->Evt[i];
    float Error2 = s1->Error[i]*s1->Error[i] + s2->Error[i]*s2->Error[i];
    s1->Error[i] = sqrt(Error2);
  }

}

Yields loadhistoYields(TString SelCut, TString TName, TString HeadFile, TString SampleFile, TString AddToDC){

  int iSelCut;
  if      (SelCut=="lepton")    iSelCut = 0;
  else if (SelCut=="6Jets")     iSelCut = 1;
  else if (SelCut=="2btag")     iSelCut = 2;
  else if (SelCut=="3btag")     iSelCut = 3;
  else if (SelCut=="4Jets")     iSelCut = 4;
  else if (SelCut=="4Jets2btag")iSelCut = 5;
  else if (SelCut=="Only2btag") iSelCut = 6;
  else{
    std::cerr << "Invalid cut name!!!" << std::endl;
    std::exit(0); 
  }

  TString ChNameTable[3];
  ChNameTable[0] = "mu+Jets ";
  ChNameTable[1] = "e+Jets  ";
  ChNameTable[2] = "Lep+Jets";
  TString ChName[3];
  ChName[0] = "mujets";
  ChName[1] = "ejets";
  ChName[2] = "lepjets";

  // All Systematics to be included 
  std::vector<TString> SysName;
  SysName.push_back("JER");
  // SysName.push_back("JES");
  SysName.push_back("JESAbsoluteStat");
  SysName.push_back("JESAbsoluteScale");
  SysName.push_back("JESAbsoluteMPFBias");
  SysName.push_back("JESFragmentation");
  SysName.push_back("JESSinglePionECAL");
  SysName.push_back("JESSinglePionHCAL");
  SysName.push_back("JESFlavorQCD");
  SysName.push_back("JESTimePtEta");
  SysName.push_back("JESRelativeJEREC1");
  SysName.push_back("JESRelativeJEREC2");
  SysName.push_back("JESRelativeJERHF");
  SysName.push_back("JESRelativePtBB");
  SysName.push_back("JESRelativePtEC1");
  SysName.push_back("JESRelativePtEC2");
  SysName.push_back("JESRelativePtHF");
  SysName.push_back("JESRelativeBal");
  SysName.push_back("JESRelativeFSR");
  SysName.push_back("JESRelativeStatFSR");
  SysName.push_back("JESRelativeStatEC");
  SysName.push_back("JESRelativeStatHF");
  SysName.push_back("JESPileUpDataMC");
  SysName.push_back("JESPileUpPtRef");
  SysName.push_back("JESPileUpPtBB");
  SysName.push_back("JESPileUpPtEC1");
  SysName.push_back("JESPileUpPtHF");
  SysName.push_back("PileUp");
  //SysName.push_back("btagjes");
  SysName.push_back("btaglf");
  SysName.push_back("btaghf");
  SysName.push_back("btaghfsI");
  SysName.push_back("btaghfsII");
  SysName.push_back("btaglfsI");
  SysName.push_back("btaglfsII");
  SysName.push_back("btagcfI");
  SysName.push_back("btagcfII");
  SysName.push_back("ScaleRdF");
  SysName.push_back("ISR");
  SysName.push_back("FSR");
  SysName.push_back("UE");

  TString SysNameVar[2] = {"Up","Down"};

  TString namefile = HeadFile + "_" + SampleFile;

  TFile *file=NULL; // new TFile(namefile);
  if(AddToDC.Contains("DCSys")){
    std::cout << "Loading " << namefile + "_SYS.root" << " sample..." << std::endl;
    file = TFile::Open(namefile + "_SYS.root");
  }
  else{
    std::cout << "Loading " << namefile + ".root" << " sample..." << std::endl;
    file = TFile::Open(namefile + ".root");
  }

  if(!file){
    std::cerr << "ERROR: Could not open " <<  namefile  << " files!!!"  << std::endl;
    std::exit(0);
  }

  if(iSelCut > 6 || iSelCut < 0) {
    std::cerr << "ERROR: Incorrect selection cut name!!! "  << std::endl;
    std::exit(0);
  }

  float aYields[7][3];
  float aError [7][3];

  Yields output;

  // Histograms
  TH1F *YieldsSample;
  YieldsSample = (TH1F*)file->Get("central/Yields");
  // YieldsSample = (TH1F*)file->Get("Yields");

  int nbin = 1;
  for(int nc = 0; nc < 7; nc++){
    for(int nch = 0; nch < 3; nch++){

      aYields[nc][nch] = YieldsSample->GetBinContent(nbin);
      aError [nc][nch] = YieldsSample->GetBinError  (nbin);

      nbin++;      
    }
  }  
  // Table LaTeX Name
  output.Name = TName;
  // Name to use in DC
  output.SamComName = SampleFile;
  // Save in dataCard? Include Shape Syst?
  output.InDataCard = AddToDC;

  std::cout << output.Name << std::endl;

  // Print Terminal Yields
  for(int nch = 0; nch < 3; nch++){
    output.Evt  .push_back(aYields[iSelCut][nch]);
    output.Error.push_back(aError [iSelCut][nch]);

    std::cout << ChNameTable[nch] << ": " << output.Evt[nch] << " +/- "  << output.Error[nch] << std::endl;
  }

  // Histogram For Combine (Normalized to the number of events of the nominal)
  if(AddToDC.Contains("DCSys")){
    TString HistoCombName = "hKinAddCSVUnroll"; // To Fix
    // --- Write histograms
    TString outfile;
    outfile = namefile + "_SYSNorm.root";
    TFile *target  = new TFile(outfile,"RECREATE" );  
    target->cd();

    for(int nch = 0; nch < 2; nch++){
      TString hpath = SelCut + "/" + ChName[nch] + "/" + HistoCombName + "_" + ChName[nch] + "_" + SelCut;

      TH1D *hiCentral = (TH1D*) file->Get("central/" + hpath);

      RemoveNegativeBins(hiCentral);

      target->mkdir("central/" + SelCut + "/" + ChName[nch]);
      target->cd   ("central/" + SelCut + "/" + ChName[nch]);
      hiCentral->Write();
      target->cd();

      int maxSys = SysName.size();
      if (AddToDC == "DCSys") maxSys = SysName.size() - 4; // Remove theoretical uncertainties  

      // FAKE Uncertainty      
      // for (int nsys = 0; nsys < maxSys; nsys++){
      // 	for (int nsysvar = 0; nsysvar < 2; nsysvar++){
      // 	  // Fake Shape Uncertainty
      // 	  TString HistoNameC = hiCentral->GetName();
      // 	  TH1D *hiSyst   = (TH1D *)hiCentral->Clone( (TString)hiCentral->GetName() + "_" + SysName.at(nsys) + SysNameVar[nsysvar]); 
	  
      // 	  // 5% of YIELD Variations
      // 	  if (SysNameVar[nsysvar]=="Up") {
      // 	    //hiSyst ->Scale(1.05);
      // 	    output.RatioComb[nch].push_back(1.05);
      // 	  }
      // 	  if (SysNameVar[nsysvar]=="Down"){
      // 	    //hiSyst ->Scale(0.95);
      // 	    output.RatioComb[nch].push_back(0.95);
      // 	  }	  

      
      // 	  target->mkdir(SysName.at(nsys) + SysNameVar[nsysvar] + "/" + SelCut + "/" + ChName[nch]);
      // 	  target->cd   (SysName.at(nsys) + SysNameVar[nsysvar] + "/" + SelCut + "/" + ChName[nch]);
      // 	  hiSyst->Write();
      // 	  target->cd();
      // 	}
      // }

      for (int nsys = 0; nsys < maxSys; nsys++){
      	for (int nsysvar = 0; nsysvar < 2; nsysvar++){
	  
      	  TH1D *hiSyst = (TH1D*) file->Get(SysName.at(nsys) + SysNameVar[nsysvar] + "/" + hpath + "_" + SysName.at(nsys) + SysNameVar[nsysvar]);
      	  RemoveNegativeBins(hiSyst);
      	  double nEvt = hiSyst->Integral();
      	  float Ratio = nEvt/output.Evt.at(nch);
      	  hiSyst->Scale(1.0/Ratio);
      	  output.RatioComb[nch].push_back(Ratio);
      	  // output.RatioComb[nch].push_back(1.0/Ratio);
      	  target->mkdir(SysName.at(nsys) + SysNameVar[nsysvar] + "/" + SelCut + "/" + ChName[nch]);
      	  target->cd   (SysName.at(nsys) + SysNameVar[nsysvar] + "/" + SelCut + "/" + ChName[nch]);
      	  hiSyst->Write();
      	  target->cd();
	  
      	}// for(nsysvar)
      } // for(nsys)
    }// for(nch)
    
    target->Close();
    
  } // if(DCSys)
  
  std::cout << "All Yields from " << namefile << ".root have been loaded successfully!!!"  << std::endl;
  return output;
}


void  EntryPrinter(FILE *file, Yields Sample){
  fprintf(file,"%s & %.1f & %.1f & %.1f & %.1f & %.1f & %.1f   \\\\\\hline\n", 
   	  (Sample.Name).Data(), 
	  Sample.Evt[0], Sample.Error[0],
	  Sample.Evt[1], Sample.Error[1],
	  Sample.Evt[2], Sample.Error[2]);
}

void CreateDataCard (FILE *file, std::vector<Yields> Samples, TString ljchannel, TString HeadFile,  TString HistoName, bool DCNorm){

  // All Systematics to be included 
  std::vector<TString> SysName;
  SysName.push_back("JER");
  //SysName.push_back("JES");
  SysName.push_back("JESAbsoluteStat");
  SysName.push_back("JESAbsoluteScale");
  SysName.push_back("JESAbsoluteMPFBias");
  SysName.push_back("JESFragmentation");
  SysName.push_back("JESSinglePionECAL");
  SysName.push_back("JESSinglePionHCAL");
  SysName.push_back("JESFlavorQCD");
  SysName.push_back("JESTimePtEta");
  SysName.push_back("JESRelativeJEREC1");
  SysName.push_back("JESRelativeJEREC2");
  SysName.push_back("JESRelativeJERHF");
  SysName.push_back("JESRelativePtBB");
  SysName.push_back("JESRelativePtEC1");
  SysName.push_back("JESRelativePtEC2");
  SysName.push_back("JESRelativePtHF");
  SysName.push_back("JESRelativeBal");
  SysName.push_back("JESRelativeFSR");
  SysName.push_back("JESRelativeStatFSR");
  SysName.push_back("JESRelativeStatEC");
  SysName.push_back("JESRelativeStatHF");
  SysName.push_back("JESPileUpDataMC");
  SysName.push_back("JESPileUpPtRef");
  SysName.push_back("JESPileUpPtBB");
  SysName.push_back("JESPileUpPtEC1");
  SysName.push_back("JESPileUpPtHF");
  SysName.push_back("PileUp");
  //SysName.push_back("btagjes");
  SysName.push_back("btaglf");
  SysName.push_back("btaghf");
  SysName.push_back("btaghfsI");
  SysName.push_back("btaghfsII");
  SysName.push_back("btaglfsI");
  SysName.push_back("btaglfsII");
  SysName.push_back("btagcfI");
  SysName.push_back("btagcfII");
  SysName.push_back("ScaleRdF");
  SysName.push_back("ISR");
  SysName.push_back("FSR");
  SysName.push_back("UE");

  int ich;
  if (ljchannel == "mujets") ich = 0;
  else if (ljchannel == "ejets") ich = 1;
  else {std::cerr << "Wrong channel name!!!" << std::endl;std::exit(0);}

  HeadFile = "/afs/cern.ch/user/b/brochero/brochero_WorkArea/LepJetsAnalyzer/" + HeadFile;    
  int nDCSam = 0;
  for(int ns = 0; ns < Samples.size(); ns++){
    if(Samples.at(ns).InDataCard.Contains("DC")) nDCSam ++;
  }

  // -- HEAD
  fprintf(file,"## -- DataCard for ttbb->Lep+Jets \n"); 
  fprintf(file,"----------------------------------------------------------\n"); 
  fprintf(file,"imax \t 1 # Number of channels\n"); 
  fprintf(file,"jmax \t %i # Number of contribution - 1 \n", (nDCSam-1)); 
  if(DCNorm) fprintf(file,"kmax \t %i # Number of Nuisance Parameters \n", 2*SysName.size()); 
  else fprintf(file,"kmax \t %i # Number of Nuisance Parameters \n", SysName.size()); 
  fprintf(file,"----------------------------------------------------------\n"); 
  fprintf(file,"----------------------------------------------------------\n"); 
  fprintf(file,"# $CHANNEL means the bin \n"); 
  fprintf(file,"# $PROCESS means the name process \n"); 
  fprintf(file,"\n# To create each entry: \n"); 
  fprintf(file,"# *shapes*  _process_  _channel_   _file_    _histogram-name_      _histogram-name-for-systematics_ \n"); 

  // -- Data Entry
  for(int ns = 0; ns < Samples.size(); ns++){
    Yields samEntry = Samples.at(ns);
    if(samEntry.InDataCard.Contains("DCSys")){
      TString namefile = HeadFile + "_" + samEntry.SamComName;
      if(DCNorm)  namefile += "_SYSNorm.root" ;
      else  namefile += "_SYS.root" ;
      fprintf(file,"shapes %s \t * %s central/2btag/$CHANNEL/%s_$CHANNEL_2btag $SYSTEMATIC/2btag/$CHANNEL/%s_$CHANNEL_2btag_$SYSTEMATIC \n", 
	      (samEntry.SamComName).Data(), 
	      namefile.Data(),
	      HistoName.Data(),
	      HistoName.Data());      
    }
    else if(samEntry.InDataCard == "DC"){
      TString namefile = HeadFile + "_" + samEntry.SamComName + ".root" ;
      fprintf(file,"shapes %s \t * %s central/2btag/$CHANNEL/%s_$CHANNEL_2btag \n", 
	      (samEntry.SamComName).Data(), 
	      namefile.Data(),
	      HistoName.Data());      
    }
  } // for(ns)
  
  for(int ns = 0; ns < Samples.size(); ns++){
    Yields samEntry = Samples.at(ns);
    if(samEntry.SamComName.Contains("Data")){ 
      TString namefile = HeadFile + "_" + samEntry.SamComName + ".root"; 
	fprintf(file,"shapes data_obs \t * %s central/2btag/$CHANNEL/%s_$CHANNEL_2btag \n", 
		namefile.Data(),
		HistoName.Data());      
      fprintf(file,"----------------------------------------------------------\n");
      fprintf(file,"bin \t %s \n", ljchannel.Data());
      fprintf(file,"observation \t %.1f  \n",
	      samEntry.Evt[ich]);
    } // if(Data)
  } // for(ns)
  
  fprintf(file,"----------------------------------------------------------\n");
  
  fprintf(file,"bin ");
  for(int ns = 0; ns < nDCSam; ns++) fprintf(file,"\t %s ", ljchannel.Data());
  fprintf(file," \n");
  
  // -- MC Names
  fprintf(file,"process ");
 
  for(int ns = 0; ns < Samples.size(); ns++){
    Yields samEntry = Samples.at(ns);
    if((samEntry.InDataCard).Contains("DC")){
      fprintf(file,"\t %s ",  (samEntry.SamComName).Data());      
    } // if(InDataCard)
  } // for(ns)
  fprintf(file,"\n");
  
  // -- MC Number
  fprintf(file,"process");
  for(int ns = 0; ns < nDCSam; ns++){
    fprintf(file,"\t %i ", (ns - 4));      
  } // for(ns)
  fprintf(file,"\n");
  
  // -- MC Rates
  fprintf(file,"rate");
  for(int ns = 0; ns < Samples.size(); ns++){
    Yields samEntry = Samples.at(ns);
    if((samEntry.InDataCard).Contains("DC")){
      fprintf(file,"\t %.3f ", samEntry.Evt[ich]);      
    } // if(InDataCard)
  } // for(ns)
  fprintf(file,"\n");
  
  fprintf(file,"----------------------------------------------------------\n");

  // Systematics Uncertainties
  for (int nsys = 0; nsys < SysName.size(); nsys++){
    if (DCNorm) fprintf(file,"%s \t shape ", SysName.at(nsys).Data());
    else fprintf(file,"%s \t shapeN2 ", SysName.at(nsys).Data());
    for(int ns = 0; ns < Samples.size(); ns++){
      Yields samEntry = Samples.at(ns);
      if(samEntry.InDataCard == "DCSysThe"){ 
	if( SysName.at(nsys)=="FSR") fprintf(file,"\t 0.78 ");
	else fprintf(file,"\t 1.0 ");
      }
      else if(samEntry.InDataCard == "DCSys"){ 
	if(nsys<(SysName.size()-4)){
	  fprintf(file,"\t 1.0 ");      
	}
	else fprintf(file,"\t - ");
      }
      else if(samEntry.InDataCard == "DC") fprintf(file,"\t - ");      
    } // for(ns)
    fprintf(file,"\n");
  } // for(nsys)

  if(DCNorm){
    // Systematics Uncertainties YIELDS
    for (int nsys = 0; nsys < SysName.size(); nsys++){
      fprintf(file,"%s_Rate \t lnN ", SysName.at(nsys).Data());
      for(int ns = 0; ns < Samples.size(); ns++){
	Yields samEntry = Samples.at(ns);
	if(samEntry.InDataCard == "DCSysThe"){ 
	  int sysidup   = 2*nsys;
	  int sysiddown = sysidup + 1;
	  float FSRfactor = 0.78;
	  if( SysName.at(nsys)=="FSR") fprintf(file,"\t %.3f/%.3f ",FSRfactor*samEntry.RatioComb[ich].at(sysiddown),FSRfactor*samEntry.RatioComb[ich].at(sysidup));
	  else fprintf(file,"\t %.3f/%.3f ",samEntry.RatioComb[ich].at(sysiddown),samEntry.RatioComb[ich].at(sysidup));
	}
	else if(samEntry.InDataCard == "DCSys") {
	  if(nsys<(SysName.size()-4)){
	    int sysidup   = 2*nsys;
	    int sysiddown = sysidup + 1;
	    fprintf(file,"\t %.3f/%.3f ",samEntry.RatioComb[ich].at(sysiddown),samEntry.RatioComb[ich].at(sysidup));
	  }
	  else fprintf(file,"\t - ");
	}
	else if(samEntry.InDataCard == "DC") fprintf(file,"\t - ");      
      } // for(ns)
      fprintf(file,"\n");
    } // for(nsys)
  } // if(DCNorm)
  
  // Luminosity
  fprintf(file,"# Lumin \t lnN ");
  for(int ns = 0; ns < Samples.size(); ns++){ 
    if(Samples.at(ns).InDataCard.Contains("DC")) fprintf(file,"\t 1.026 ");      
  } 
 fprintf(file,"\n");
  
  fprintf(file,"----------------------------------------------------------\n");

  fprintf(file,"theory group = ScaleRdF ISR FSR UE \n");
  fprintf(file,"btag_Shape group = btaghf btaghfsI btaghfsII btaglfsI btaglfsII btagcfII btagcfI btaglf \n");
  fprintf(file,"Jet_Shape group = JER \n");
  fprintf(file,"JetScale_Shape group = JESAbsoluteStat JESAbsoluteScale JESAbsoluteMPFBias JESFragmentation JESSinglePionECAL JESSinglePionHCAL JESFlavorQCD JESTimePtEta JESRelativeJEREC1 JESRelativeJEREC2 JESRelativeJERHF JESRelativePtBB JESRelativePtEC1 JESRelativePtEC2 JESRelativePtHF JESRelativeBal JESRelativeFSR JESRelativeStatFSR JESRelativeStatEC JESRelativeStatHF JESPileUpDataMC JESPileUpPtRef JESPileUpPtBB JESPileUpPtEC1 JESPileUpPtHF \n");

  fprintf(file,"Other_Shape group = PileUp \n");
  if(DCNorm){
    fprintf(file,"Jet_Rate  group = JER_Rate \n");
    fprintf(file,"JetScale_Rate  group = JESAbsoluteStat_Rate JESAbsoluteScale_Rate JESAbsoluteMPFBias_Rate JESFragmentation_Rate JESSinglePionECAL_Rate JESSinglePionHCAL_Rate JESFlavorQCD_Rate JESTimePtEta_Rate JESRelativeJEREC1_Rate JESRelativeJEREC2_Rate JESRelativeJERHF_Rate JESRelativePtBB_Rate JESRelativePtEC1_Rate JESRelativePtEC2_Rate JESRelativePtHF_Rate JESRelativeBal_Rate JESRelativeFSR_Rate JESRelativeStatFSR_Rate JESRelativeStatEC_Rate JESRelativeStatHF_Rate JESPileUpDataMC_Rate JESPileUpPtRef_Rate JESPileUpPtBB_Rate JESPileUpPtEC1_Rate JESPileUpPtHF_Rate \n");
    fprintf(file,"btag_Rate  group = btaghf_Rate btaghfsI_Rate btaghfsII_Rate btaglfsI_Rate btaglfsII_Rate btagcfII_Rate btagcfI_Rate btaglf_Rate \n");
    fprintf(file,"Other_Rate  group = PileUp_Rate \n");
  }
}

void RemoveNegativeBins(TH1D *histo){
  for(int ibin=1; ibin<=histo->GetNbinsX(); ibin ++){
    float bcont = histo->GetBinContent(ibin);
    if (bcont < 0.0) {
      std::cout << "Warning!!! Negative bin entry! Setting it to 0.0" << std::endl;
      histo->SetBinContent(ibin,0.0);
    }
  }
}
