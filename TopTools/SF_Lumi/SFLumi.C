#include<vector>
#include<iostream>
#include "TString.h"


/***************************
    SF: Normalization
***************************/
      
float SFLumi(TString filename,
	     float Lumi,
	     float NGenEvents){
  
  /************************
    Normalization Weights
  *************************/

  float NormWeight = 0.0;
  // NormWeight = Lumi*(1.0/N_Gen_events)*(Xsec)*(Br)

  if     (filename.Contains("QCD_MuEnr_15to20"))     NormWeight = Lumi * (1.0/NGenEvents) * (1273190000.0) * (0.003); // [pb] (cross section) * (Filter Eff)
  else if(filename.Contains("QCD_MuEnr_20to30"))     NormWeight = Lumi * (1.0/NGenEvents) * (558528000.0) * (0.0053); // [pb] (cross section) * (Filter Eff)
  else if(filename.Contains("QCD_MuEnr_30to50"))     NormWeight = Lumi * (1.0/NGenEvents) * (139803000.0) * (0.0118); // [pb] (cross section) * (Filter Eff)
  else if(filename.Contains("QCD_MuEnr_50to80"))     NormWeight = Lumi * (1.0/NGenEvents) * (19222500.0) * (0.02276); // [pb] (cross section) * (Filter Eff)
  else if(filename.Contains("QCD_MuEnr_80to120"))    NormWeight = Lumi * (1.0/NGenEvents) * (2758420.0) * (0.03844);  // [pb] (cross section) * (Filter Eff)
  else if(filename.Contains("QCD_MuEnr_120to170"))   NormWeight = Lumi * (1.0/NGenEvents) * (469797.0) * (0.05362);   // [pb] (cross section) * (Filter Eff)
  else if(filename.Contains("QCD_MuEnr_170to300"))   NormWeight = Lumi * (1.0/NGenEvents) * (117989.0) * (0.07335);   // [pb] (cross section) * (Filter Eff)
  else if(filename.Contains("QCD_MuEnr_300to470"))   NormWeight = Lumi * (1.0/NGenEvents) * (7820.25) * (0.10196);    // [pb] (cross section) * (Filter Eff)
  else if(filename.Contains("QCD_MuEnr_470to600"))   NormWeight = Lumi * (1.0/NGenEvents) * (645.528) * (0.12242);    // [pb] (cross section) * (Filter Eff)
  else if(filename.Contains("QCD_MuEnr_600to800"))   NormWeight = Lumi * (1.0/NGenEvents) * (187.09) * (0.13412);     // [pb] (cross section) * (Filter Eff)
  else if(filename.Contains("QCD_MuEnr_800to1000"))  NormWeight = Lumi * (1.0/NGenEvents) * (32.3486) * (0.14552);    // [pb] (cross section) * (Filter Eff)
  else if(filename.Contains("QCD_MuEnr_1000toInf"))  NormWeight = Lumi * (1.0/NGenEvents) * (10.4305) * (0.15544);    // [pb] (cross section) * (Filter Eff)

  else if(filename.Contains("QCD_EGEnr_15to20"))     NormWeight = Lumi * (1.0/NGenEvents) * (1279000000.0) * (0.0018); // [pb] (cross section) * (Filter Eff)
  else if(filename.Contains("QCD_EGEnr_20to30"))     NormWeight = Lumi * (1.0/NGenEvents) * (557600000.0) * (0.0096);  // [pb] (cross section) * (Filter Eff)
  else if(filename.Contains("QCD_EGEnr_30to50"))     NormWeight = Lumi * (1.0/NGenEvents) * (136000000.0) * (0.073);   // [pb] (cross section) * (Filter Eff)
  else if(filename.Contains("QCD_EGEnr_50to80"))     NormWeight = Lumi * (1.0/NGenEvents) * (19800000.0) * (0.146);    // [pb] (cross section) * (Filter Eff)
  else if(filename.Contains("QCD_EGEnr_80to120"))    NormWeight = Lumi * (1.0/NGenEvents) * (2800000.0) * (0.125);     // [pb] (cross section) * (Filter Eff)
  else if(filename.Contains("QCD_EGEnr_120to170"))   NormWeight = Lumi * (1.0/NGenEvents) * (477000.0) * (0.132);      // [pb] (cross section) * (Filter Eff)
  else if(filename.Contains("QCD_EGEnr_170to300"))   NormWeight = Lumi * (1.0/NGenEvents) * (114000.0) * (0.165);      // [pb] (cross section) * (Filter Eff)
  else if(filename.Contains("QCD_EGEnr_300toInf"))   NormWeight = Lumi * (1.0/NGenEvents) * (9000.0) * (0.15);         // [pb] (cross section) * (Filter Eff)

  // Cross Section from https://twiki.cern.ch/twiki/bin/view/CMS/TTbarHbbRun2ReferenceAnalysis#Samples
  else if(filename.Contains("ttHbb"))          NormWeight = Lumi * (1.0/NGenEvents) * (0.577*0.5058); // [pb]

  else if(filename.Contains("ZJets_M50"))      NormWeight = Lumi * (1.0/NGenEvents) * (6025.2);   // [pb]
  else if(filename.Contains("ZJets_M10to50"))  NormWeight = Lumi * (1.0/NGenEvents) * (18610.0);  // [pb]

  else if(filename.Contains("WJets"))          NormWeight = Lumi * (1.0/NGenEvents) * (61526.7);  // [pb]

  // 5f Inclusive
  else if(filename.Contains("tW_Powheg"))      NormWeight = Lumi * (1.0/NGenEvents) * (35.6);     // [pb]
  else if(filename.Contains("tbarW_Powheg"))   NormWeight = Lumi * (1.0/NGenEvents) * (35.6);     // [pb]
  // 4f Lepton Decay
  // else if(filename.Contains("t_tchannel"))     NormWeight = Lumi * (1.0/NGenEvents) * (44.33);    // [pb]
  // else if(filename.Contains("tbar_tchannel"))  NormWeight = Lumi * (1.0/NGenEvents) * (26.38);    // [pb]
  // 4f Inclusive
  else if(filename.Contains("t_tchannel_Powheg"))     NormWeight = Lumi * (1.0/NGenEvents) * (136.02);    // [pb]
  else if(filename.Contains("tbar_tchannel_Powheg"))  NormWeight = Lumi * (1.0/NGenEvents) * (80.95);    // [pb]
  // Inclusive
  else if(filename.Contains("WW_Pythia"))             NormWeight = Lumi * (1.0/NGenEvents) * (118.7);    // [pb]
  else if(filename.Contains("WZ_Pythia"))             NormWeight = Lumi * (1.0/NGenEvents) * (47.13);    // [pb]
  else if(filename.Contains("ZZ_Pythia"))             NormWeight = Lumi * (1.0/NGenEvents) * (16.52);    // [pb]

  else if(filename.Contains("ttW_Madgraph"))             NormWeight = Lumi * (1.0/NGenEvents) * (0.61);    // [pb]
  else if(filename.Contains("ttZ_Madgraph"))             NormWeight = Lumi * (1.0/NGenEvents) * (0.78);    // [pb]
  
  else if(filename.Contains("ttbar_LepJetsPowhegPythia"))  NormWeight = Lumi * (1.0/NGenEvents) * (363.1); // [pb] Xsec*Br(lep+Jets) = (831.76) * 2[(0.1086*3) * (0.67)] = 363.12 (356.4-> mtop = 173)
  else if(filename.Contains("ttbb_aMCatNLO"))              NormWeight = Lumi * (1.0/NGenEvents) * (13.93*1.3531);  // [pb] Xsec*Br(tt+bb) - From Higgs Group. 1.35 from norma. with PowhegPythia
  else if(filename.Contains("ttbb_Sherpa"))                NormWeight = Lumi * (1.0/NGenEvents) * (13.93);  // [pb] Xsec from POWHEG 0.993 from norma. with PowhegPythia
  // else if(filename.Contains("ttbb_aMCatNLO"))              NormWeight = Lumi * (1.0/NGenEvents) * (25.63);  // [pb] Xsec from POWHEG
  // else if(filename.Contains("ttbb_Sherpa"))                NormWeight = Lumi * (1.0/NGenEvents) * (25.63);  // [pb] Xsec from https://cms-gen-dev.cern.ch/xsdb/?columns=40501248&searchQuery=process_name%3Dttbb_4FS_OpenLoops_13TeV-sherpa 
  else if(filename.Contains("ttbar_PowhegPythia"))         NormWeight = Lumi * (1.0/NGenEvents) * (831.76); // [pb] Br = (leptonic) * Hadronic = (0.1086*3) * (0.67)
  else if(filename.Contains("ttbar_aMCatNLO"))             NormWeight = Lumi * (1.0/NGenEvents) * (831.76); // * (0.1086*3.0*3.0); // Br correction
  else if(filename.Contains("ttbar_Madgraph"))             NormWeight = Lumi * (1.0/NGenEvents) * (831.76);  
  else if(filename.Contains("ttbar_PowhegHerwig"))         NormWeight = Lumi * (1.0/NGenEvents) * (831.76); // [pb] Br = (leptonic) * Hadronic = (0.1086*3) * (0.67)

   
  else if (filename.Contains("DataSingleMu") || filename.Contains("DataSingleEG"))               NormWeight = 1.0;

  else{
    std::cout << "Cross section for " << filename << " not found. Include it in plugins/SFLumi.cc" << std::endl;
    std::exit (EXIT_FAILURE);  
  }

  return NormWeight;
}

