# Full Description to run the ttbb/ttjj analysis

1. Produce histograms:
   
   a- Edit the relevant information in the files containing the samples to be processed (CATTuple version, header and output name):
   
   `emacs Inputs.info`
   
   `emacs InputsSys.info`
   
   `emacs InputsScale.info`
   
   `emacs InputsDedicated.info`

   b- Histograms are produced running in **Condor**. To run central values:
   
   `python SubmitSamples.py --input Inputs/Inputs.info`

   c(1)- To run systematic variations affecting the shape of the profiles:

   `python SubmitSamples.py --input Inputs/InputsSys.info --option sys`

   c(2)- To run the 25 JES variations affecting the shape of the profiles:

   `python SubmitSamples.py --input Inputs/InputsSys.info --option jes`

   d- To run scale variations (ME):
   
   `python SubmitSamples.py --inpit Inputs/InputsScale.info --option scale`

   e- To run over **ttbar INCLUSIVE** samples with theoretical variations (Scale at PS, UE).
   
   `python SubmitSamples.py --input Inputs/InputsDedicated.info --option dedicated`

   f- To run over **ttbar INCLUSIVE** samples with color reconection (gluon and QCD CR).
   
   `python SubmitSamples.py --input Inputs/InputsCR.info`


2. Setup all the outputs. All the samples **(central + syst(shape))** must be ready in TopResults directory.

   a- COPY all files (histograms) to lxplus:

   `scp -r kisti:/cms/ldap_home/brochero/brochero_WorkArea/LepJetsAnalyzer/TopResults/hSF-?_Tree_LepJets_?_v8-0-6_Spring16-80X_36814pb-1* /afs/cern.ch/user/b/brochero/brochero_WorkArea/LepJetsAnalyzer/TopResults/`

   b- Create `ttbar_PowhegPythiaBkg` with the different theoretical uncertainties: Just copy the central value of the histo to use with syst. name.
   
   `root -l -b -q 'ProduceTheUnc_ttbarBkg.C("FileVersion-v?")'`

   It produces `hSF-?_Tree_LepJets_?_v8-0-6_Spring16-80X_36814pb-1_ttbar_PowhegPythiaBkg_SYS_Theory.root`, including ISR, FSR, UE and ScaleRdF.

   c- VERY IMPORTANT: As the theoretical uncertainties are in the inclusive samples (different tunning), it is necessary to estimate the variations BIN by BIN to propagate them to the LepJets sample.
   
   `root -l -b -q 'InclToLepJetsTheUnc.C("FileVersion-v?")'`
      
   It produces a file for each ttbar category with the correct sys variation in the LepJets sample. Name is similar to previous step: `hSF-?_Tree_LepJets_Summer_v8-0-6_Spring16-80X_36814pb-1_ttbar_LepJetsPowhegPythiaCAT_SYS_Theory.root`

   NOTE: Take into account that b and c steps produce the output ONLY for the UNROLL histogram.

   d- Merge Theoretical uncertainties:

   `PrepareCombineFiles.py hSF-?_Tree_LepJets_Summer_v8-0-6_Spring16-80X_36814pb-1`

3. To produce all plots:

   a- edit Plots.h to include the head of the name file (for example `TString fl  = "hSF-MCJER-v0_Tree_LepJets_Final_v8-0-6_Spring16-80X_36814pb-1";`)

   b- create a root file with all systematic uncertainties to be included. `hSF-MCJER-v0_Tree_LepJets_Final_v8-0-6_Spring16-80X_36814pb-1_MCAllSyst.root`
   
   c- then run
   
   `source List_RunAllPlots.sh`

4. Produce Tables and DataCards
   
   a- Only yields tables

   `./TableYields.run -i hSF-?_Tree_LepJets_Summer_v8-0-6_Spring16-80X_36814pb-1 -cut 2btag`

   b- Yields tables and DataCards
   
   `./TableYields.run -i hSF-MCJER-v0_Tree_LepJets_Final_v8-0-6_Spring16-80X_36814pb-1 -cut 2btag -combine DataCard -his hKinAddCSVUnroll`
   
   It produces 2 sets of DataCards: the standard and the normalized (only for test).

5. Generation Level: Acceptances, efficiencies and theoretical uncertainties

   a- 

6. Do fit with COMBINE: Check README in Combine repository.

7. Additional plots: After have the full set of root files (including Combine outputs), several plots can be produced:
   
   a- Pre/Post fit plots:

   `root -l -b -q 'PostFitCombine.C("pre/post","FitResults_DataCard_?CombineRef?_hSF-?FileVersion?_Tree_LepJets_?ProductionVersion?_Spring16-80X_36814pb-1_2btag")'`

   b- Check of syst. variation in the regions of the Unrolled plot:

   `root -l -b -q 'UnRollTheSystVar.C("FileVersion","SystVariation")'`

8. Acceptances and efficiencies at GENERATOR level. 
   
   a- Start with the central values for all the available MC samples
   
   `python SubmitGENSamples.py Inputs/InputsGenCentral.info`
   
   b- Run Scale variations at ME level for the exclusive LepJets sample
   
   'python SubmitGENSamples.py Inputs/InputsScale.info scale'

   c- Run the theoretical uncertainties using the dedicated **ttbar INCLUSIVE** samples  (UE, ISR and FSR)

   'python SubmitGENSamples.py Inputs/InputsDedicated.info dedicated'
   
   d- Run the color reconection **ttbar INCLUSIVE** samples (gluon and QCD CR).
   
   `python SubmitGENSamples.py Inputs/InputsCR.info`
