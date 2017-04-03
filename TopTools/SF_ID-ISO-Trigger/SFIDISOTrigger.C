#include "SFIDISOTrigger.h"

void GetSFHistogram(TString fSFdir, TString fSFname, TH2F **hmuIDISOSF, TH2F **hmuTriggerSF, TH2F **hmuTrackerSF, TH2F **heIDISOSF,  TH2F **heTriggerSF){

  // Lepton SFs: ID and ISO with stat. + syst. Errors
  TString MuFile = fSFdir + "MuonSF_" + fSFname + ".root";
  TString ElFile = fSFdir + "ElectronSF_" + fSFname + ".root";

  TFile *MuSF = TFile::Open(MuFile);
  TFile *ElSF = TFile::Open(ElFile);
  
  std::cout << "Reading SF files: " << std::endl;
  std::cout << MuFile << std::endl;
  std::cout << ElFile << std::endl;

  if(!MuSF || !ElSF){
    std::cerr << "ERROR [SF]: Could not open SF files!!!"  << std::endl;
    std::exit(0);
  }
  
  *hmuIDISOSF = (TH2F*) MuSF->Get("GlobalSF")->Clone("muIDISOSF");
  *hmuTriggerSF = (TH2F*) MuSF->Get("TriggerSF")->Clone("muTriggerSF");
  *hmuTrackerSF = (TH2F*) MuSF->Get("TrackerSF")->Clone("muTrackerSF");

  if(!hmuIDISOSF || !hmuTriggerSF || hmuTrackerSF){
    std::cerr << "ERROR [MuonSF]: Could not find " << MuFile << " for SF reweighting" << std::endl;
    std::exit(0);
  }
  
  *heIDISOSF = (TH2F*) ElSF->Get("eTriggerSF")->Clone("eIDISOSF");
  *heTriggerSF = (TH2F*) ElSF->Get("eTriggerSF")->Clone("eTriggerSF");
  if(!heIDISOSF || !heTriggerSF){
    std::cerr << "ERROR [ElectronSF]: Could not find " << ElFile <<  " for SF reweighting" << std::endl;
    std::exit(0);
  }

}

/***************************
  SF: ID, ISO and Trigger
***************************/      
void SFIDISOTrigger(std::vector<float> &result,
		    TLorentzVector Lep, int channel, int nVtx,
		    TH2F *hmuIDISOSF, TH2F *hmuTriggerSF, TH2F *hmuTrackerSF,
		    TH2F *heIDISOSF,  TH2F *heTriggerSF){

  /***************************
    SF: ID, ISO and Trigger
  ***************************/
  float SF_ID_ISO=1.0;
  float SF_ID_ISO_Error=0.0;
  float SF_Tr=1.0;
  float SF_Tr_Error=0.0;

  float SF_ID_ISO_Tr=1.0;


  /***************************
         Lepton: muon 
  ***************************/
  if(channel == 0){
    // Binning for Muon ID/ISO histo
    int Nbinmueta = hmuIDISOSF->GetNbinsX(); // X-axis (eta)
    int NbinmupT =  hmuIDISOSF->GetNbinsY();  // Y-axis (pT)
    
    std::vector<float> Vbinmueta; // muon eta values
    std::vector<float> VbinmupT;  // muon pT values

    // X-axis (eta)
    for(int nbx=1; nbx <= Nbinmueta; nbx++)  Vbinmueta.push_back( hmuIDISOSF->GetXaxis()->GetBinLowEdge(nbx) );
    Vbinmueta.push_back( hmuIDISOSF->GetXaxis()->GetBinLowEdge(Nbinmueta + 1) ); // Add upper edge of the last bin
    
    // Y-axis (pT)
    for(int nby=1; nby <= NbinmupT; nby++)  VbinmupT.push_back( hmuIDISOSF->GetYaxis()->GetBinLowEdge(nby) );
    //VbinmupT.push_back( hmuIDISOSF->GetYaxis()->GetBinLowEdge(NbinmupT + 1) ); // Add upper edge of the last bin
    VbinmupT.push_back(1000.0); // Add Max pT for the last bin
    
    // std::cout << "Number of X-axis bins (eta) = " << Nbinmueta << std::endl; 
    // for(int nbx=0; nbx < Vbinmueta.size(); nbx++) std::cout << "Range of X-axis bins (eta) = " << Vbinmueta[nbx] << std::endl; 
    // std::cout << "Number of Y-axis bins (pT)  = " << NbinmupT << std::endl; 
    // for(int nby=0; nby < VbinmupT.size(); nby++) std::cout << "Range of Y-axis bins (pT) = " << VbinmupT[nby] << std::endl; 

    // Binning for Muon Trigger histo
    int NbinTrmueta = hmuTriggerSF->GetNbinsX(); // X-axis (eta)
    int NbinTrmupT  = hmuTriggerSF->GetNbinsY(); // Y-axis (pT)
    
    std::vector<float> VbinTrmueta; // muon eta values
    std::vector<float> VbinTrmupT;  // muon pT values
    
    // X-axis (eta)
    for(int nbx=1; nbx <= NbinTrmueta; nbx++)  VbinTrmueta.push_back( hmuTriggerSF->GetXaxis()->GetBinLowEdge(nbx) );
    VbinTrmueta.push_back( hmuTriggerSF->GetXaxis()->GetBinLowEdge(NbinTrmueta + 1) ); // Add upper edge of the last bin
    
    // Y-axis (pT)
    for(int nby=1; nby <= NbinTrmupT; nby++)  VbinTrmupT.push_back( hmuTriggerSF->GetYaxis()->GetBinLowEdge(nby) );

    VbinTrmupT.push_back( hmuTriggerSF->GetYaxis()->GetBinLowEdge(NbinTrmupT + 1) ); // Add upper edge of the last bin

    // Binning for Muon Tracker SF
    int NbinTckmueta = hmuTrackerSF->GetNbinsX(); // X-axis (eta)
    int NbinTckmuvtx = hmuTrackerSF->GetNbinsY(); // Y-axis (Vtx)
    
    std::vector<float> VbinTckmueta; // muon eta values
    std::vector<float> VbinTckmuvtx; // muon vtx values
    
    // X-axis (eta)
    for(int nbx=1; nbx <= NbinTckmueta; nbx++)  VbinTckmueta.push_back( hmuTrackerSF->GetXaxis()->GetBinLowEdge(nbx) );
    VbinTckmueta.push_back( hmuTrackerSF->GetXaxis()->GetBinLowEdge(NbinTckmueta + 1) ); // Add upper edge of the last bin
    
    // Y-axis (Vtx)
    for(int nby=1; nby <= NbinTckmuvtx; nby++)  VbinTckmuvtx.push_back( hmuTrackerSF->GetYaxis()->GetBinLowEdge(nby) );
    //VbinmupT.push_back( hmuIDISOSF->GetYaxis()->GetBinLowEdge(NbinmupT + 1) ); // Add upper edge of the last bin
    VbinTckmuvtx.push_back(1000.0); // Add Max Vtx for the last bin
    
    //--------------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------------
    
    // -- ID/ISO
    for(int binmupT=0; binmupT<NbinmupT; binmupT++){
      if(Lep.Pt()>VbinmupT[binmupT] && Lep.Pt()<=VbinmupT[binmupT+1]){ 
	
	for(int binmueta=0; binmueta<Nbinmueta; binmueta++){
	  if(Lep.Eta()>Vbinmueta[binmueta] && Lep.Eta()<=Vbinmueta[binmueta+1]){ 
	    
	    SF_ID_ISO       = hmuIDISOSF->GetBinContent(binmueta+1,binmupT+1);
	    SF_ID_ISO_Error = hmuIDISOSF->GetBinError  (binmueta+1,binmupT+1);
	    
	    break;
	  }// if(Lep.Eta())
	}// for(binmueta)
	
      }// if(Lep.Pt())
    }// for(binmupT)
    
    // -- Trigger
    for(int binTrmupT=0; binTrmupT<NbinTrmupT; binTrmupT++){
      if(Lep.Pt()>VbinTrmupT[binTrmupT] && Lep.Pt()<=VbinTrmupT[binTrmupT+1]){ 
	
	for(int binTrmueta=0; binTrmueta<NbinTrmueta; binTrmueta++){
	  if(Lep.Eta()>VbinTrmueta[binTrmueta] && Lep.Eta()<=VbinTrmueta[binTrmueta+1]){ 
	    
	    SF_Tr       = hmuTriggerSF->GetBinContent(binTrmueta+1,binTrmupT+1);
	    SF_Tr_Error = hmuTriggerSF->GetBinError  (binTrmueta+1,binTrmupT+1);
	    
	    break;
	  }// if(Lep.Eta())
	}// for(binTrmueta)
	
      }// if(Lep.Pt())
    }// for(binTrmupT)

    // -- Tracker
    float SF_Tck = 1.0;
    for(int binTckmuvtx=0; binTckmuvtx<NbinTckmuvtx; binTckmuvtx++){
      if(nVtx>VbinTckmuvtx[binTckmuvtx] && nVtx<=VbinTckmuvtx[binTckmuvtx+1]){ 
	
	for(int binTckmueta=0; binTckmueta<NbinTckmueta; binTckmueta++){
	  if(Lep.Eta()>VbinTckmueta[binTckmueta] && Lep.Eta()<=VbinTckmueta[binTckmueta+1]){ 
	    
	    SF_Tck = hmuTrackerSF->GetBinContent(binTckmueta+1,binTckmuvtx+1);
	    
	    break;
	  }// if(Lep.Eta())
	}// for(binTckmueta)
	
      }// if(Lep.Pt())
    }// for(binTckmuvtx)

    SF_ID_ISO = SF_ID_ISO*SF_Tck;
      
  }// if(channel == muon)
  
  /***************************
       Lepton: electron
  ***************************/
  if(channel == 1){
    // Binning for Electron ID/ISO histo
    int Nbineeta = heIDISOSF->GetNbinsX(); // X-axis (eta)
    int NbinepT  = heIDISOSF->GetNbinsY(); // Y-axis (pT)
    
    std::vector<float> Vbineeta; // electron eta values
    std::vector<float> VbinepT;  // electron pT values
    
    // X-axis (eta)
    for(int nbx=1; nbx <= Nbineeta; nbx++)  Vbineeta.push_back( heIDISOSF->GetXaxis()->GetBinLowEdge(nbx) );
    Vbineeta.push_back( heIDISOSF->GetXaxis()->GetBinLowEdge(Nbineeta + 1) ); // Add upper edge of the last bin
    
    // Y-axis (pT)
    for(int nby=1; nby <= NbinepT; nby++)  VbinepT.push_back( heIDISOSF->GetYaxis()->GetBinLowEdge(nby) );
    // VbinepT.push_back( heIDISOSF->GetYaxis()->GetBinLowEdge(NbinepT + 1) ); // Add upper edge of the last bin
    VbinepT.push_back(1000.0); // Add Max pT for the last bin
    
    // Binning for Electron Trigger histo
    int NbinTreeta = heTriggerSF->GetNbinsX(); // X-axis (eta)
    int NbinTrepT  = heTriggerSF->GetNbinsY(); // Y-axis (pT)
    
    std::vector<float> VbinTreeta; // electron eta values
    std::vector<float> VbinTrepT;  // electron pT values
    
    // X-axis (eta)
    for(int nbx=1; nbx <= NbinTreeta; nbx++)  VbinTreeta.push_back( heTriggerSF->GetXaxis()->GetBinLowEdge(nbx) );
    VbinTreeta.push_back( heTriggerSF->GetXaxis()->GetBinLowEdge(NbinTreeta + 1) ); // Add upper edge of the last bin
    
    // Y-axis (pT)
    for(int nby=1; nby <= NbinTrepT; nby++)  VbinTrepT.push_back( heTriggerSF->GetYaxis()->GetBinLowEdge(nby) );
    // VbinTrepT.push_back( heTriggerSF->GetYaxis()->GetBinLowEdge(NbinTrepT + 1) ); // Add upper edge of the last bin
    VbinTrepT.push_back(1000.0); // Add Max pT for the last bin

    //--------------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------------
    
    // -- ID/ISO  
    for(int binepT=0; binepT<NbinepT; binepT++){
      if(Lep.Pt()>VbinepT[binepT] && Lep.Pt()<=VbinepT[binepT+1]){ 
	
	for(int bineeta=0;bineeta<Nbineeta;bineeta++){
	  if(Lep.Eta()>Vbineeta[bineeta] && Lep.Eta()<=Vbineeta[bineeta+1]){ 
	    
	    SF_ID_ISO       = heIDISOSF->GetBinContent(bineeta+1,binepT+1);
	    SF_ID_ISO_Error = heIDISOSF->GetBinError(bineeta+1,binepT+1);
	    
	    break;
	  }// if(Lep.Eta())
	}// for(bineEta)
	
      }// if(Lep.Pt())
    }// for(binePt)
    
    // -- Trigger
    for(int binTrepT=0; binTrepT<NbinTrepT; binTrepT++){
      if(Lep.Pt()>VbinTrepT[binTrepT] && Lep.Pt()<=VbinTrepT[binTrepT+1]){ 
	
	for(int binTreeta=0;binTreeta<NbinTreeta;binTreeta++){
	  if(Lep.Eta()>VbinTreeta[binTreeta] && Lep.Eta()<=VbinTreeta[binTreeta+1]){ 
	    
	    SF_Tr       = heTriggerSF->GetBinContent(binTreeta+1,binTrepT+1);
	    SF_Tr_Error = heTriggerSF->GetBinError(binTreeta+1,binTrepT+1);
	    
	    break;
	  }// if(Lep.Eta())
	}// for(binTreeta)
	
      }// if(Lep.Pt())
    }// for(binTrePt)
    
  }// if(channel == electron)
  
  /*******************************************
   Trigger,ID & ISO Scale Factors/bin(Pt,Eta)
  *******************************************/
  // Control Information
  //std::cout << "Channel = " << channel << '\n' << "Lep.eta = " << Lep.Eta() << " -- Lep.pT = " << Lep.Pt() << '\n' << "SF = " << SF_ID_ISO << std::endl;
  
  result.push_back(SF_ID_ISO*SF_Tr); //[0]  
  result.push_back((SF_ID_ISO + SF_ID_ISO_Error)*(SF_Tr + SF_Tr_Error)); //[1]  
  result.push_back((SF_ID_ISO - SF_ID_ISO_Error)*(SF_Tr - SF_Tr_Error)); //[2]  
  result.push_back(SF_ID_ISO);       //[3]  
  result.push_back(SF_ID_ISO_Error); //[4]  
  result.push_back(SF_Tr);           //[5]  
  result.push_back(SF_Tr_Error);     //[6]  
  
}
 



