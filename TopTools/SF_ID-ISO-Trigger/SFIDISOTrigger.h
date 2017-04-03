#include<vector> 
#include "TH1.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TFile.h"
#include "TLorentzVector.h"
#include<iostream>

void GetSFHistogram(TString fSFdir, TString fSFname, TH2F **hmuIDISOSF, TH2F **hmuTriggerSF, TH2F **heIDISOSF,  TH2F **heTriggerSF);  

void SFIDISOTrigger(std::vector<float> &result,
		    TLorentzVector Lep, int channel,
		    TH2F *hmuIDISOSF, TH2F *hmuTriggerSF,
		    TH2F *heIDISOSF, TH2F *heTriggerSF);

 
