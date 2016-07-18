#include "TLorentzVector.h"
#include "TMinuit.h"

// Jet Class
class ComJet: public TLorentzVector{
 public:
  float CSV, CvsL, CvsB;
  int Flavour, pTIndex, Mom;
};

double jetEresolution (double energy);
double metphiresolution (double met);
double metresolution (double met);
double twoobjectmassresolution(TLorentzVector &j1, double releres1, TLorentzVector &j2, double releres2);
double twojetmassresolution   (TLorentzVector &j1, TLorentzVector &j2);// relative mass resolution
double solvettbarljets (double &nupz, double &metscale, double &blscale, double &bjscale, double &j1scale, double &j2scale);
void findhadronictop (TLorentzVector &lepton, std::vector<ComJet> &jets, TLorentzVector met, bool usebtaginfo, std::vector<int> &bestindices, float &bestchi2, TLorentzVector &nusol, TLorentzVector &blrefit, TLorentzVector &bjrefit, TLorentzVector &j1refit, TLorentzVector &j2refit);
