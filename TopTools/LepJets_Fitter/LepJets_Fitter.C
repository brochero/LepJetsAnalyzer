#include "LepJets_Fitter.h"

TMinuit *tm=0;

TLorentzVector tmplep, tmpnu, tmpbl, tmpbj, tmpj1, tmpj2;
float blres, bjres, j1res, j2res, metres;

// relative jet energy resolution
float JetEResolution(float energy){ 
  return TMath::Sqrt(TMath::Power(0.05*energy,2.0) + TMath::Power(1.5*sqrt(energy),2.0))/energy;
}

// met phi resolution as a function of reconstructed met from Delphes
float METPhiResolution(float met){
  return 0.05539 - 0.5183*exp(-0.01507*met);
}

float METResolution(float met){
  return 15.0;
}

float TwoObjectMassResolution(TLorentzVector &j1, float releres1, TLorentzVector &j2, float releres2){
  // crude, but OK
  float massnominal = (j1+j2).M();
  TLorentzVector j1smeared = (1.0+releres1)*j1;
  TLorentzVector j2smeared = (1.0+releres2)*j2;
  
  float deltamass1up = (j1smeared+j2).M()-massnominal;
  float deltamass2up = (j1+j2smeared).M()-massnominal;
  return TMath::Hypot(deltamass1up, deltamass2up)/massnominal;
}

// relative mass resolution
float TwoJetMassResolution(TLorentzVector &j1, TLorentzVector &j2){
  float releres1 = JetEResolution(j1.E());
  float releres2 = JetEResolution(j2.E());
  
  return TwoObjectMassResolution(j1, releres1, j2, releres2);
}


void fcn(int &npar, float *gin, float &f, float *par, int iflag){
  //calculate chisquare 
  tmpnu.SetPz(par[0]);
  float massres = TwoObjectMassResolution(tmplep, 0.0, tmpnu, 15.0/tmpnu.Pt())*80.4;
  f = TMath::Power(((tmpnu+tmplep).M()-80.4)/massres, 2.0);
}

// full solution
// par[0] - neutrino Pz
// par[1] - missing Et scale factor 
// par[2] - b-jet energy scale factor (leptonic side)
// par[3] - b-jet energy scale factor (hadronic side)
// par[4] - jet energy scale factor for jet 1
// par[5] - jet energy scale factor for jet 2
void fcnfull(int &npar, float *gin, float &f, float *par, int iflag){
  
  TLorentzVector nuscaled = tmpnu*par[1];
  nuscaled.SetPz(par[0]);
  TLorentzVector blscaled = tmpbl*par[2];
  TLorentzVector bjscaled = tmpbj*par[3];
  TLorentzVector j1scaled = tmpj1*par[4];
  TLorentzVector j2scaled = tmpj2*par[5];
  
  //calculate chisquare 
  f = TMath::Power(((nuscaled+tmplep).M()-80.4)/2.085, 2.0)
    + TMath::Power(((blscaled + nuscaled+tmplep).M()-172.0)/1.5, 2.0)
    + TMath::Power(((j1scaled+j2scaled).M()-80.4)/2.085, 2.0)
    + TMath::Power(((bjscaled + j1scaled+j2scaled).M()-172.0)/1.5, 2.0)
    + TMath::Power((par[1]-1.0)/metres, 2.0)
    + TMath::Power((par[2]-1.0)/blres,  2.0)
    + TMath::Power((par[3]-1.0)/bjres,  2.0)
    + TMath::Power((par[4]-1.0)/j1res,  2.0)
    + TMath::Power((par[5]-1.0)/j2res,  2.0);
}

void InitMinuit(){
  tm= new TMinuit(6);
  tm->SetFCN(fcnfull);
  float arglist[10];
  int ierflg = 0;
  
  arglist[0] = 1;
  gMinuit->mnexcm("SET ERR", arglist, 1, ierflg);
}


float SolvettbarLepJets(float &nupz, float &metscale, float &blscale, float &bjscale, float &j1scale, float &j2scale){
  
  if (tm==0) InitMinuit();
  
  // Set starting values and step sizes for parameters
  static float vstart[6] = {0.0, 1.0, 1.0, 1.0, 1.0, 1.0};
  static float step  [6] = {0.1, 0.1, 0.1, 0.1, 0.1, 0.1};
  int ierflg;

  tm->mnparm(0, "nupz",    vstart[0], step[0], 0,0, ierflg);
  tm->mnparm(1, "metsf",   vstart[1], step[1], 0,0, ierflg);
  tm->mnparm(2, "bjetlsf", vstart[2], step[2], 0,0, ierflg);
  tm->mnparm(3, "bjethsf", vstart[3], step[3], 0,0, ierflg);
  tm->mnparm(4, "wjet1",   vstart[4], step[4], 0,0, ierflg);
  tm->mnparm(5, "wjet2",   vstart[5], step[5], 0,0, ierflg);
  
  // Now ready for minimization step
  float arglist[10];
  arglist[0] = 500;
  arglist[1] = 1.;
  tm->mnexcm("MIGRAD", arglist ,2,ierflg);
  
  // Print results
  float amin,edm,errdef;
  int nvpar,nparx,icstat;
  float err;

  tm->mnstat(amin,edm,errdef,nvpar,nparx,icstat);

  tm->GetParameter(0, nupz,     err);
  tm->GetParameter(1, metscale, err);
  tm->GetParameter(2, blscale,  err);
  tm->GetParameter(3, bjscale,  err);
  tm->GetParameter(4, j1scale,  err);
  tm->GetParameter(5, j2scale,  err);
  
  return amin;
}


void FindHadronicTop(TLorentzVector &lepton, std::vector<ComJet> &jets, TLorentzVector &met, bool usebtaginfo, std::vector<int> &bestindices, float &bestchi2, TLorentzVector &nusol, TLorentzVector &blrefit, TLorentzVector &bjrefit, TLorentzVector &j1refit, TLorentzVector &j2refit){

  int njets = jets.size();
  
  TLorentzVector trialW, trialb, trialtop, trialWjet1, trialWjet2, trialblepton, trialtoplepton, trialwlepton;

  // re-sort  
  int bestidx1=-1, bestidx2=-1, bestidx3=-1, bestidx4=-1;
  
  bestchi2=1.0e6;
  float chi2;
  
  bestindices[0]=-1;
  bestindices[1]=-1;
  bestindices[2]=-1;
  bestindices[3]=-1;
  
  nusol.SetPtEtaPhiM(met.E(), 0.0, met.Phi(), 0.0);
  metres = METResolution(nusol.Pt())/nusol.Pt();
  float wlmassrelres = TwoObjectMassResolution(lepton, 0.0, nusol, 15.0/nusol.Pt());
  
  // not using b-tagging information
  if (!usebtaginfo){
    // at least there should be 4 hadronic jets
    if (njets>=4){
      for (int i1=0; i1<njets; i1++){
	for (int i2=0; i2<njets-1 ; i2++){
	  for (int i3=i2+1; i3<njets; i3++){
	    for (int i4=0; i4<njets; i4++){
	      
	      if (i2 != i1 && i3 != i1 && i4!=i1 && i4 != i2 && i4 != i3){
		
		trialb         = jets[i1];
		trialWjet1     = jets[i2];
		trialWjet2     = jets[i3];
		trialW         = trialWjet1 + trialWjet2;
		trialtop       = trialb + trialW;
		trialblepton   = jets[i4];
		trialtoplepton = trialwlepton + trialblepton;
		
		// set global variables - ugly!
		tmplep = lepton;
		tmpnu  = nusol;
		tmpbl  = trialblepton;
		tmpbj  = trialb;
		tmpj1  = trialWjet1;
		tmpj2  = trialWjet2;
		
		blres = JetEResolution(tmpbl.E());
		bjres = JetEResolution(tmpbj.E());
		j1res = JetEResolution(tmpj1.E());
		j2res = JetEResolution(tmpj2.E());
		
		float nupz, metscale, blscale, bjscale, j1scale, j2scale;
		
		// dynamic resolutions
		chi2 = SolvettbarLepJets(nupz, metscale, blscale, bjscale, j1scale, j2scale);
		
		if(chi2 < bestchi2){
		  bestchi2 = chi2;
		  nusol = tmpnu*metscale;
		  nusol.SetPz(nupz);
		  blrefit = tmpbl*blscale;
		  bjrefit = tmpbj*bjscale;
		  j1refit = tmpj1*j1scale;
		  j2refit = tmpj2*j2scale;
		  bestidx1 = i1;
		  bestidx2 = i2;
		  bestidx3 = i3;
		  bestidx4 = i4;
		}
	      }
	    }
	  }
	}
      }
      
      bestindices[0]=bestidx1; // b for hadronic side
      bestindices[1]=bestidx2; // W jet
      bestindices[2]=bestidx3; // W jet
      bestindices[3]=bestidx4; // b for leptonic side
    }
  }
  
  // use b-tagging information
  else {
    // at least there should be 4 hadronic jets
    if (njets>=4){
      int nbjets=0;
      for (int i1=0; i1<njets; i1++){
	if (jets[i1]->BTag==1) nbjets++;
      }
      
      for (int i1=0; i1<njets; i1++){
	for (int i2=0; i2<njets-1 ; i2++){
	  for (int i3=i2+1; i3<njets; i3++){
	    for (int i4=0; i4<njets; i4++){
	      if (i2 != i1 && i3 != i1 && i4!=i1 && i4 != i2 && i4 != i3
		  && ((lepton + jets[i4]).M() < 170.0)
		  && ( nbjets==0 
		       ||  (nbjets==1 && (jets[i2]->BTag==0 && jets[i3]->BTag==0))
		       ||  (nbjets==2 && (jets[i2]->BTag==0 && jets[i3]->BTag==0))
		       ||  (nbjets==3 && (jets[i2]->BTag==0 || jets[i3]->BTag==0))
		       ||  (nbjets>3  && (jets[i2]->BTag==1 || jets[i3]->BTag==1))
		       )
		  ){
		
		trialb            = jets[i1];
		trialWjet1        = jets[i2];
		trialWjet2        = jets[i3];
		trialW            = trialWjet1 + trialWjet2;
		trialtop          = trialb + trialW;
		trialblepton      = jets[i4];
		trialtoplepton    = trialwlepton + trialblepton;
		
		float bjetreleres = JetEResolution(trialb.E());
		
		// set global variables - ugly!
		tmplep = lepton;
		tmpnu  = nusol;
		tmpbl  = trialblepton;
		tmpbj  = trialb;
		tmpj1  = trialWjet1;
		tmpj2  = trialWjet2;
		
		blres  = jetEresolution(tmpbl.E());
		bjres  = jetEresolution(tmpbj.E());
		j1res  = jetEresolution(tmpj1.E());
		j2res  = jetEresolution(tmpj2.E());
		

		float nupz, metscale, blscale, bjscale, j1scale, j2scale;
		
		// dynamic resolutions
		chi2 = SolvettbarLepJets(nupz, metscale, blscale, bjscale, j1scale, j2scale);
		
		if (chi2 < bestchi2){
		  bestchi2 = chi2;
		  nusol = tmpnu*metscale;
		  nusol.SetPz(nupz);
		  blrefit = tmpbl*blscale;
		  bjrefit = tmpbj*bjscale;
		  j1refit = tmpj1*j1scale;
		  j2refit = tmpj2*j2scale;
		  bestidx1 = i1;
		  bestidx2 = i2;
		  bestidx3 = i3;
		  bestidx4 = i4;		  
		}
	      }
	    }
	  }
	}
      }
      bestindices[0]=bestidx1; // b for hadronic side
      bestindices[1]=bestidx2; // W jet
      bestindices[2]=bestidx3; // W jet
      bestindices[3]=bestidx4; // b for leptonic side
    }
  }
  
}
