#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "THStack.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TStyle.h"
#include "TSystem.h"
#include "TString.h"
#include "TDirectory.h"
#include "TROOT.h"
#include "TObject.h"
#include "TStopwatch.h"
#include "TLegend.h"
#include <vector>
#include <string>
#include <fstream>
#include <vector>
#include <sys/stat.h>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <set>
#include "tdrstyle.C"


typedef struct HistoFit{
  TH1D     *hist1D[2][3];
  TH2D     *hist2D[3];
  double    events[3];
} HistoFit;


TString dirnameIn= "TopResults/";
TString fl  = "hSF-pTj30_FitBin20-v1_Tree_LepJets_KFCSVOrder01NoSkim_v8-0-1_Spring16-80X_15920pb-1";  

enum FileSample{data, 
		ttbb, ttb, ttcc, ttLF, ttccLF, Bkgtt, ttjj, 
		WJets, ZJets, SingleTop, VV, QCD, BkgOther, BkgFull};

HistoFit LoadSample(TString FileName);
