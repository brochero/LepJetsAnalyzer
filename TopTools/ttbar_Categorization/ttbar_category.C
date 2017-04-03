/***************************************************************************************************************************************
   ttbar Categorization from:
   https://twiki.cern.ch/twiki/pub/CMSPublic/GenHFHadronMatcher/GenTtbarCategorizer.cc


     The classification scheme returns an ID per event, and works as follows:
     
     All jets in the following need to be in the acceptance as given by the config parameters |eta|, pt.
     
     First, b jets from top are identified, i.e. jets containing a b hadron from t->b decay
     They are encoded in the ID as numberOfBjetsFromTop*100, i.e.
     0xx: no b jets from top in acceptance
     1xx: 1 b jet from top in acceptance
     2xx: both b jets from top in acceptance
     
     Then, b jets from W are identified, i.e. jets containing a b hadron from W->b decay
     They are encoded in the ID as numberOfBjetsFromW*1000, i.e.
     0xxx: no b jets from W in acceptance
     1xxx: 1 b jet from W in acceptance
     2xxx: 2 b jets from W in acceptance
     
     Then, c jets from W are identified, i.e. jets containing a c hadron from W->c decay, but no b hadrons
     They are encoded in the ID as numberOfCjetsFromW*10000, i.e.
     0xxxx: no c jets from W in acceptance
     1xxxx: 1 c jet from W in acceptance
     2xxxx: 2 c jets from W in acceptance
     
     From the remaining jets, the ID is formed based on the additional b jets (IDs x5x) and c jets (IDs x4x) in the following order:
     x55: at least 2 additional b jets with two of them having >= 2 b hadrons
     x54: at least 2 additional b jets with one of them having >= 2 b hadrons, the other having =1 b hadron
     x53: at least 2 additional b jets with all having =1 b hadron
     x52: exactly 1 additional b jet having >=2 b hadrons
     x51: exactly 1 additional b jet having =1 b hadron
     x45: at least 2 additional c jets with two of them having >= 2 c hadrons
     x44: at least 2 additional c jets with one of them having >= 2 c hadrons, the other having =1 c hadron
     x43: at least 2 additional c jets with all having =1 c hadron
     x42: exactly 1 additional c jet having >=2 c hadrons
     x41: exactly 1 additional c jet having =1 c hadron
     x00: No additional b or c jet, i.e. only light flavour jets

***************************************************************************************************************************************/
#include "ttbar_category.h"

bool ttbar_category(TString ttbarCatName, int ttbarGenCatId, int NGenJets){

  bool tt_Evt   = false;
  bool ttb_Evt  = false;
  bool tt2b_Evt = false;
  bool ttLF_Evt = false;
  bool ttbb_Evt = false;
  bool ttcc_Evt = false;

  int h, tu, t, u, Wb, Wc;

  h  = ttbarGenCatId / 100;
  tu = ttbarGenCatId % 100;
  t  = tu / 10;
  u  = tu % 10;
    
  if (tu == 0){
    //(Inside Acceptance (pT>25,|eta|<2.4))
    // if      ((h == 2 || h==102) && NGenJets>5) ttLF_Evt = true;
    // else if ((h == 1 || h==101) && NGenJets>4) ttLF_Evt = true;
    // else if ((h == 0 || h==100) && NGenJets>3) ttLF_Evt = true;
    // else tt_Evt   = true;
    tt_Evt   = true;
  }
  else if (tu == 55 || tu == 54 || tu == 53) ttbb_Evt = true;
  else if (tu == 52)                         tt2b_Evt = true;
  else if (tu == 51)                         ttb_Evt  = true;
  else if (tu == 45 || tu == 44 || tu == 43) ttcc_Evt = true;
  // ttcj included in ttLF
  else if (tu == 42 || tu == 41)             tt_Evt  = true;
  
  if (ttbarCatName == "tt")   return tt_Evt;
  else if (ttbarCatName == "ttb")  return ttb_Evt;
  else if (ttbarCatName == "tt2b") return tt2b_Evt;
  else if (ttbarCatName == "ttcc") return ttcc_Evt;
  else if (ttbarCatName == "ttbb") return ttbb_Evt;    
  else return false;
}

TString ttbar_Wjjcategory(int ttbarGenCatId){

  /* W Jets studie
     First, b jets from top are identified, i.e. jets containing a b hadron from t->b decay
     They are encoded in the ID as numberOfBjetsFromTop*100, i.e.
     0xx: no b jets from top in acceptance
     1xx: 1 b jet from top in acceptance
     2xx: both b jets from top in acceptance
     
     Then, b jets from W are identified, i.e. jets containing a b hadron from W->b decay
     They are encoded in the ID as numberOfBjetsFromW*1000, i.e.
     0xxx: no b jets from W in acceptance
     1xxx: 1 b jet from W in acceptance
     2xxx: 2 b jets from W in acceptance
     
     Then, c jets from W are identified, i.e. jets containing a c hadron from W->c decay, but no b hadrons
     They are encoded in the ID as numberOfCjetsFromW*10000, i.e.
     0xxxx: no c jets from W in acceptance
     1xxxx: 1 c jet from W in acceptance
     2xxxx: 2 c jets from W in acceptance
  */

  TString nWjj;

  bool bbjj_Evt = false;
  bool bbcj_Evt = false;
  bool bjj_Evt  = false;
  bool bcj_Evt  = false;
  bool jj_Evt   = false;
  bool cj_Evt   = false;

  int tb, tu, t, u, Wb, Wc;

  Wc = ttbarGenCatId / 10000;
 
  Wb = ttbarGenCatId / 1000;
  Wb = Wb % 10;

  tb = ttbarGenCatId / 100;
  tb = tb % 100;

  tu = ttbarGenCatId % 100;
  t  = tu / 10;
  u  = tu % 10;

  if(tb == 2 && Wc == 1) nWjj = "bb_c";
  if(tb == 2 && Wc == 0) nWjj = "bb_LF";
  if(tb == 1 && Wc == 1) nWjj = "b_c";
  if(tb == 1 && Wc == 0) nWjj = "b_LF";
  if(tb == 0 && Wc == 0) nWjj = "_LF";
  if(tb == 0 && Wc == 0) nWjj = "_c";

  // std::cout << "-Cat = " << ttbarGenCatId << std::endl; 
  // std::cout << " Wc = " << Wc << std::endl; 
  // std::cout << " Wb = " << Wb << std::endl; 
  // std::cout << " h = " << h << std::endl; 
  // std::cout << " tu = " << tu << std::endl; 
  // std::cout << " t = " << t << std::endl; 
  // std::cout << " u = " << u << std::endl; 
  // std::cout << " nWjj = " << nWjj << std::endl; 

  return nWjj;


}
