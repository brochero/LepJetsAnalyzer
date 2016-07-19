#include "TLorentzVector.h"
#include "TMinuit.h"

// Jet Class
class ComJet: public TLorentzVector{
 public:
  float CSV, CvsL, CvsB;
  int Flavour, pTIndex, Mom;
};

float JetEResolution         (float energy);
float METPhiResolution       (float met);
float METResolution          (float met);
float TwoObjectMassResolution(TLorentzVector &j1, float releres1, TLorentzVector &j2, float releres2);
float twojetmassresolution   (TLorentzVector &j1, TLorentzVector &j2);// relative mass resolution
float SolvettbarLepJets      (float &nupz, float &metscale, float &blscale, float &bjscale, float &j1scale, float &j2scale);
void   FindHadronicTop        (TLorentzVector &lepton, std::vector<ComJet> &jets, TLorentzVector met, bool usebtaginfo, std::vector<int> &bestindices, float &bestchi2, TLorentzVector &nusol, TLorentzVector &blrefit, TLorentzVector &bjrefit, TLorentzVector &j1refit, TLorentzVector &j2refit);

void fcnfull(int &npar, float *gin, float &f, float *par, int iflag);
void fcn(int &npar, float *gin, float &f, float *par, int iflag);
