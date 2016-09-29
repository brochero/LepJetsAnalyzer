#!/bin/sh
file="Tree_LepJets_KFCSVOrder01_v8-0-1_Spring16-80X_15920pb-1"

#######################################
# $2 should be '-o OutputName... etc' #
#######################################

./TreeReader.run -i ${file}_DataSingleEG $1  & disown
./TreeReader.run -i ${file}_DataSingleMu $1  & disown

./TreeReader.run -i ${file}_ttbar_PowhegPythiattbb $1  & disown
./TreeReader.run -i ${file}_ttbar_PowhegPythiattbj $1  & disown 
./TreeReader.run -i ${file}_ttbar_PowhegPythiattcc $1 
./TreeReader.run -i ${file}_ttbar_PowhegPythiattLF $1  & disown
./TreeReader.run -i ${file}_ttbar_PowhegPythiatt   $1  & disown
./TreeReader.run -i ${file}_ttbar_PowhegPythiattjj $1  & disown

./TreeReader.run -i ${file}_ttbar_PowhegPythiattbbAlphaS $1  & disown
./TreeReader.run -i ${file}_ttbar_PowhegPythiattbjAlphaS $1  & disown 
./TreeReader.run -i ${file}_ttbar_PowhegPythiattccAlphaS $1 
./TreeReader.run -i ${file}_ttbar_PowhegPythiattLFAlphaS $1  & disown
./TreeReader.run -i ${file}_ttbar_PowhegPythiattAlphaS   $1  & disown
./TreeReader.run -i ${file}_ttbar_PowhegPythiattjjAlphaS $1  & disown

./TreeReader.run -i ${file}_ttbar_LepJetsPowhegPythiattbb $1 & disown
./TreeReader.run -i ${file}_ttbar_LepJetsPowhegPythiattbj $1 & disown
./TreeReader.run -i ${file}_ttbar_LepJetsPowhegPythiattcc $1 
./TreeReader.run -i ${file}_ttbar_LepJetsPowhegPythiattLF $1 & disown
./TreeReader.run -i ${file}_ttbar_LepJetsPowhegPythiatt   $1 & disown
./TreeReader.run -i ${file}_ttbar_LepJetsPowhegPythiattjj $1 & disown

./TreeReader.run -i ${file}_ttbar_LepJetsPowhegPythiattbbAlphaS $1 & disown
./TreeReader.run -i ${file}_ttbar_LepJetsPowhegPythiattbjAlphaS $1 & disown
./TreeReader.run -i ${file}_ttbar_LepJetsPowhegPythiattccAlphaS $1 
./TreeReader.run -i ${file}_ttbar_LepJetsPowhegPythiattLFAlphaS $1 & disown
./TreeReader.run -i ${file}_ttbar_LepJetsPowhegPythiattAlphaS   $1 & disown
./TreeReader.run -i ${file}_ttbar_LepJetsPowhegPythiattjjAlphaS $1 & disown

./TreeReader.run -i ${file}_ttbb_aMCatNLOPythia $1  & disown

./TreeReader.run -i ${file}_ttbar_MadgraphPythia $1  & disown
./TreeReader.run -i ${file}_ttbar_aMCatNLOPythia $1  


./TreeReader.run -i ${file}_ttbar_PowhegPythiaBkg $1 

./TreeReader.run -i ${file}_tW_Powheg            $1  & disown
./TreeReader.run -i ${file}_tbarW_Powheg         $1  & disown
./TreeReader.run -i ${file}_tbar_tchannel_Powheg $1  & disown
./TreeReader.run -i ${file}_t_tchannel_Powheg    $1

./TreeReader.run -i ${file}_ttW_Madgraph $1  & disown 
./TreeReader.run -i ${file}_ttZ_Madgraph $1  & disown
./TreeReader.run -i ${file}_ttHbb_PowhegPythia $1

./TreeReader.run -i ${file}_WW_Pythia $1 & disown 
./TreeReader.run -i ${file}_WZ_Pythia $1 & disown 
./TreeReader.run -i ${file}_ZZ_Pythia $1  

./TreeReader.run -i ${file}_WJets_aMCatNLO         $1 & disown
#./TreeReader.run -i ${file}_WJets_Madgraph         $1 & disown
./TreeReader.run -i ${file}_ZJets_M50_aMCatNLO     $1 & disown
./TreeReader.run -i ${file}_ZJets_M10to50_aMCatNLO $1 

./TreeReader.run -i ${file}_QCD_MuEnr_20to30    $1
./TreeReader.run -i ${file}_QCD_MuEnr_30to50    $1
./TreeReader.run -i ${file}_QCD_MuEnr_50to80    $1
./TreeReader.run -i ${file}_QCD_MuEnr_80to120   $1
./TreeReader.run -i ${file}_QCD_MuEnr_120to170  $1
./TreeReader.run -i ${file}_QCD_MuEnr_170to300  $1
./TreeReader.run -i ${file}_QCD_MuEnr_300to470  $1
./TreeReader.run -i ${file}_QCD_MuEnr_470to600  $1
./TreeReader.run -i ${file}_QCD_MuEnr_800to1000 $1
./TreeReader.run -i ${file}_QCD_MuEnr_1000toInf $1

./TreeReader.run -i ${file}_QCD_EGEnr_15to20      $1
./TreeReader.run -i ${file}_QCD_EGEnr_20to30      $1
./TreeReader.run -i ${file}_QCD_EGEnr_30to50      $1
./TreeReader.run -i ${file}_QCD_EGEnr_50to80      $1
./TreeReader.run -i ${file}_QCD_EGEnr_80to120     $1
./TreeReader.run -i ${file}_QCD_EGEnr_120to170    $1
./TreeReader.run -i ${file}_QCD_EGEnr_170to300    $1
./TreeReader.run -i ${file}_QCD_EGEnr_300toInf    $1




