#include "../../TreeReader.h"
#include "JER/JER_806/JetResolution.h"
#include "JER/JER_806/JetResolutionObject.h"
#include "TRandom3.h"

// Jet Energy Resolution
// Smearing: ALWAYS NOMINAL
// https://twiki.cern.ch/twiki/bin/view/CMS/JetResolution
void JERCorrection(JME::JetResolutionScaleFactor res_sf, JME::JetResolution resolution, float rho, std::vector<ComJet> *jets, int nSystVar){
  
  for (auto itjet = jets->begin(); itjet != jets->end(); itjet++ ){
    
    ComJet jet = (*itjet);

    float JetVar = 1.;
    float Res_JER, SF_JER;
    // Resolution
    Res_JER = resolution.getResolution({{JME::Binning::JetPt,  jet.Pt()},
	                                {JME::Binning::JetEta, jet.Eta()}, 
	                                {JME::Binning::Rho,    rho}
                                       });
    // Variation::NOMINAL = 0, Variation::UP = 1, Variation::DOWN = 2
    SF_JER = res_sf.getScaleFactor({{JME::Binning::JetEta, jet.Eta()}}, (Variation) nSystVar);

    // Correct Jets and MET ONLY for jets with pT>20
    if ( jet.Pt() < 20.) continue ;
    
    bool Jet_recogenMatch = false;
    
    if (jet.Gen_DR < 0.2 && 
	(jet.Pt() - jet.Gen_pT) < 3.*Res_JER*jet.Pt()) Jet_recogenMatch = true;

    if (Jet_recogenMatch){
      JetVar = std::max(0.0 , 
			(double)( jet.Pt() + (jet.Pt() - jet.Gen_pT)*(SF_JER-1) ) / jet.Pt() );      
    }// if(seljet.ptGen)
    
    else {
      TRandom3 *ran = new TRandom3(jet.Pt());
      float N_JER = ran->Gaus(0.0,Res_JER);
      JetVar = 1. + N_JER*sqrt(std::max(0.0,SF_JER*SF_JER-1.));
      delete ran;
    } // else

    // Scale original pT
    (*itjet) *= JetVar;

  } // for(ijets)
  
}


void JESCorrection(std::vector<ComJet> *jets){
    
  for (auto itjet = jets->begin(); itjet != jets->end(); itjet++ ){
    
    float JetVar = 1.;
    ComJet jet = (*itjet);
    // JES: Set of 25 variations saved in systJESUp, systJESDown 
    // Name structure saved in mJESName
    JetVar = jet.JES;       
    // Correct Jets and MET ONLY for jets with pT>20
    if ( jet.Pt() < 20.) continue ;
        
    // Scale original pT
    (*itjet) *= JetVar;
    
  } // for(ijets)
  
}

