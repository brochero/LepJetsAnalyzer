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
};

Yields loadhistoYields(int SelCut, TString TName, TString HeadFile, TString SampleFile, TString AddToDC = "");
void   EntryPrinter   (FILE *file, Yields Sample);
void   AddhistoYields (Yields *s1, Yields *s2);
void   CreateDataCard (FILE *file, std::vector<Yields> Samples, TString ljchannel, TString HeadFile,  TString HistoName);

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
  const char * _histoDC = "hKinAdd12CSV";  
  
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

  int cut;
  if      (cutname=="lepton")    cut = 0;
  else if (cutname=="6Jets")     cut = 1;
  else if (cutname=="2btag")     cut = 2;
  else if (cutname=="3btag")     cut = 3;
  else if (cutname=="4Jets")     cut = 4;
  else if (cutname=="4Jets2btag")cut = 5;
  else if (cutname=="Only2btag") cut = 6;
  else{
    std::cerr << "Invalid cut name!!!" << std::endl;
    std::exit(0); 
  }

  std::vector<Yields> Samples;

  // Yields ttbar_1;
  // ttbar_1 = loadhistoYields(cut, "ttbar Herwig", fdir + fname, "ttbar_PowhegHerwig");
  // Samples.push_back(ttbar_1);
  // Yields ttbar_2;
  // ttbar_2 = loadhistoYields(cut, "ttbar Evt", fdir + fname, "ttbar_PowhegPythiaEvt");
  // Samples.push_back(ttbar_2);
  // Yields ttbar;
  // ttbar = loadhistoYields(cut, "\\ttbar", fdir + fname, "ttbar_LepJetsPowhegPythia");
  // Samples.push_back(ttbar);
    
  Yields ttbb;
  ttbb = loadhistoYields(cut, "\\ttbar\\bbbar", fdir + fname, "ttbar_LepJetsPowhegPythiattbb", "DCSys");
  Samples.push_back(ttbb);
  Yields ttb;
  ttb = loadhistoYields(cut, "\\ttbar\\qb", fdir + fname, "ttbar_LepJetsPowhegPythiattbj", "DCSys");
  Samples.push_back(ttb);
  Yields ttcc;
  ttcc = loadhistoYields(cut, "\\ttbar\\ccbar", fdir + fname, "ttbar_LepJetsPowhegPythiattcc", "DCSys");
  Samples.push_back(ttcc);
  Yields ttLF;
  ttLF = loadhistoYields(cut, "\\ttbar LF", fdir + fname, "ttbar_LepJetsPowhegPythiattLF", "DCSys");
  Samples.push_back(ttLF);
  Yields ttjj;
  ttjj = loadhistoYields(cut, "\\ttbar\\jj", fdir + fname, "ttbar_LepJetsPowhegPythiattjj");
  Samples.push_back(ttjj);

  Yields ttOther;
  ttOther = loadhistoYields(cut, "\\ttbar\\ Other", fdir + fname, "ttbar_PowhegPythiaBkgtt", "DCSys");
  Samples.push_back(ttOther);

  Yields tW;
  tW = loadhistoYields(cut, "Single Top", fdir + fname, "SingleTop", "DCSys");
  Samples.push_back(tW);

  Yields WJets;
  WJets = loadhistoYields(cut, "W+Jets", fdir + fname, "WJets_aMCatNLO", "DC");
  Samples.push_back(WJets);

  Yields ZJets;
  ZJets = loadhistoYields(cut, "Z+Jets", fdir + fname, "ZJets_aMCatNLO", "DC");
  Samples.push_back(ZJets);

  Yields VV;
  VV = loadhistoYields(cut, "VV", fdir + fname, "VV", "DCSys");
  Samples.push_back(VV);

  Yields QCD;
  QCD = loadhistoYields(cut, "QCD", fdir + fname, "QCD", "DC");
  Samples.push_back(QCD);

  Yields ttH;
  ttH = loadhistoYields(cut, "\\ttbar H", fdir + fname, "ttHbb_PowhegPythia", "DC");
  Samples.push_back(ttH);

  Yields ttV;
  ttV = loadhistoYields(cut, "\\ttbar V", fdir + fname, "ttV_Madgraph", "DC");
  Samples.push_back(ttV);

  Yields BkgMC;
  BkgMC = loadhistoYields(cut, "Total Bkg MC", fdir + fname, "ttbar_PowhegPythiaBkgtt");
  AddhistoYields(&BkgMC,&tW);
  AddhistoYields(&BkgMC,&WJets);
  AddhistoYields(&BkgMC,&ZJets);
  AddhistoYields(&BkgMC,&VV);
  AddhistoYields(&BkgMC,&QCD);
  AddhistoYields(&BkgMC,&ttH);
  AddhistoYields(&BkgMC,&ttV);

  Samples.push_back(BkgMC);

  Yields TotalMC;
  TotalMC = loadhistoYields(cut, "Total MC", fdir + fname, "ttbar_LepJetsPowhegPythiattjj");
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
  Data = loadhistoYields(cut, "Data", fdir + fname, "DataSingleLep");
  Samples.push_back(Data);
  
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
  
    CreateDataCard(DataCard_mu, Samples, "mujets", fdir + fname, hisComname);
    fclose(DataCard_mu);

    TString CombineDCfile_e  = dirCombinename + outDataCard + "_ejets_" + fname + "_" + cutname + ".txt";
    FILE*   DataCard_e       = fopen(CombineDCfile_e, "w");
  
    CreateDataCard(DataCard_e, Samples, "ejets", fdir + fname, hisComname);
    fclose(DataCard_e);

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

Yields loadhistoYields(int SelCut, TString TName, TString HeadFile, TString SampleFile, TString AddToDC){

  TString namefile = HeadFile + "_" + SampleFile;

  std::cout << "Loading " << namefile + ".root" << " sample..." << std::endl;

  TFile *file=NULL; // new TFile(namefile);
  file = TFile::Open(namefile + ".root");

  if(!file){
    std::cerr << "ERROR: Could not open " <<  namefile  << " files!!!"  << std::endl;
    std::exit(0);
  }

  if(SelCut > 6 || SelCut < 0) {
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

  output.Name = TName;
  output.SamComName = SampleFile;
  output.InDataCard = AddToDC;

  std::cout << output.Name << std::endl;

  TString chname[3];
  chname[0] = "mu+Jets ";
  chname[1] = "e+Jets  ";
  chname[2] = "Lep+Jets";

  for(int nch = 0; nch < 3; nch++){
    output.Evt  .push_back(aYields[SelCut][nch]);
    output.Error.push_back(aError [SelCut][nch]);

    std::cout << chname[nch] << ": " << output.Evt[nch] << " +/- "  << output.Error[nch] << std::endl;
  }

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

void CreateDataCard (FILE *file, std::vector<Yields> Samples, TString ljchannel, TString HeadFile,  TString HistoName){

  int ich;
  if (ljchannel == "mujets") ich = 0;
  else if (ljchannel == "ejets") ich = 1;
  else {std::cerr << "Wrong channel name creating DATACARD" << std::endl;std::exit(0);}

  std::vector<TString> SysName;
  SysName.push_back("JER");
  SysName.push_back("JES");
  SysName.push_back("PileUp");
  SysName.push_back("btagjes");
  SysName.push_back("btaglf");
  SysName.push_back("btaghf");
  SysName.push_back("btaghfsI");
  SysName.push_back("btaghfsII");
  SysName.push_back("btaglfsI");
  SysName.push_back("btaglfsII");
  SysName.push_back("btagcfI");
  SysName.push_back("btagcfII");

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
  fprintf(file,"kmax \t %i # Number of Nuisance Parameters \n", SysName.size()); 
  fprintf(file,"----------------------------------------------------------\n"); 
  fprintf(file,"----------------------------------------------------------\n"); 
  fprintf(file,"# $CHANNEL means the bin \n"); 
  fprintf(file,"# $PROCESS means the name process \n"); 
  fprintf(file,"\n# To create each entry: \n"); 
  fprintf(file,"# *shapes*  _process_  _channel_   _file_    _histogram-name_      _histogram-name-for-systematics_ \n"); 

  // -- Data Entry
  for(int ns = 0; ns < Samples.size(); ns++){
    Yields samEntry = Samples.at(ns);
    if(samEntry.InDataCard == "DCSys"){
      TString namefile = HeadFile + "_" + samEntry.SamComName + "_SYS.root" ;
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
      if (HistoName.Contains("hKinAdd12CSV") ) fprintf(file,"observation \t %.1f  \n",
						       (2.0*samEntry.Evt[ich]));
      else fprintf(file,"observation \t %.1f  \n",
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
      if (HistoName.Contains("hKinAdd12CSV") ) fprintf(file,"\t %.1f ", (2.0*samEntry.Evt[ich]));      
      else fprintf(file,"\t %.1f ", samEntry.Evt[ich]);      
    } // if(InDataCard)
  } // for(ns)
  fprintf(file,"\n");
  
  fprintf(file,"----------------------------------------------------------\n");

  // Systematics Uncertainties
  for (int nsys = 0; nsys < SysName.size(); nsys++){
    fprintf(file,"%s \t shapeN2 ", SysName.at(nsys).Data());
    for(int ns = 0; ns < Samples.size(); ns++){
      Yields samEntry = Samples.at(ns);
      if(samEntry.InDataCard == "DCSys") fprintf(file,"\t 1 ");      
      else if(samEntry.InDataCard == "DC") fprintf(file,"\t - ");      
    } // for(ns)
    fprintf(file,"\n");
  } // for(nsys)
  fprintf(file,"----------------------------------------------------------\n");

}
