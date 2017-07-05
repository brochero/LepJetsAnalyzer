#!/usr/bin/python
import sys, os

ttbarbase = "ttbar_LepJetsPowhegPythia"
ttbarcat = ""

FileHead = str(sys.argv[1]) 

SysName = ["JER","JES","PileUp","btagjes","btaglf","btaghf","btaghfsI","btaghfsII","btaglfsI","btaglfsII","btagcfI","btagcfII"]
SysVar = ["Up","Down"]


# With Syst Var
ttbar   = ["",
           "ttjj",
           "ttbb", "ttbj","ttcc", "ttLF", "tt"]
bkg     = ["ttbar_PowhegPythiaBkgtt",
           "ttbar_PowhegPythiaBkg"]
singlet = ["SingleTop", 
           "tbar_tchannel_Powheg","t_tchannel_Powheg",
           "tbarW_Powheg","tW_Powheg"]
diboson = ["VV",
           "WW_Pythia", "WZ_Pythia", "ZZ_Pythia"]

# Only Central
zjets    = ["ZJets_aMCatNLO",
            "ZJets_M10to50_aMCatNLO", "ZJets_M50_aMCatNLO"]
realdata = ["DataSingleLep", 
            "DataSingleEG", "DataSingleMu"]
qcd      = ["QCD",
            "QCD_EGEnr", "QCD_MuEnr",
            "QCD_EGEnr_*", 
            "QCD_MuEnr_*"]
ttx      = ["ttV_Madgraph",
            "ttW_Madgraph", "ttZ_Madgraph"]


def PrintAndCommand (str):
    print(str + "\n")
    os.system(str)

def MergeCombineFiles (BaseN,SysN):
    basename = str(BaseN + "_") 
    if SysN is "central":
        sysname = ""
    else:
        sysname = "_SYS_" + str(SysN)

    print "Creating files to perform the FIT...." 
    
    TTbarNam = [basename + ttbarbase + ttbarcat + s + sysname + ".root" for s in ttbar]

    BkgNam = [basename + s + sysname + ".root" for s in bkg]

    STNam = [basename + s + sysname + ".root" for s in singlet]

    VVNam = [basename + s + sysname + ".root" for s in diboson]

    DYNam = [basename + s + ".root" for s in zjets]

    DataNam = [basename + s + ".root" for s in realdata]
    
    QCDNam = [basename + s + ".root" for s in qcd]

    ttVNam = [basename + s + ".root" for s in ttx]    
    
    # ttbar
    if (os.path.isfile(TTbarNam[2]) and os.path.isfile(TTbarNam[3]) and
        os.path.isfile(TTbarNam[4]) and os.path.isfile(TTbarNam[5]) and
        os.path.isfile(TTbarNam[6])):
        if os.path.isfile(TTbarNam[1]):
            print(TTbarNam[1] + " already exists!")
        else:
            PrintAndCommand("hadd " + TTbarNam[1] + " " + TTbarNam[2] + " " + TTbarNam[3] + " " + TTbarNam[4] + " " + TTbarNam[5])
        if os.path.isfile(TTbarNam[0]):
            PrintAndCommand("rm -rf " + TTbarNam[0])
        PrintAndCommand("hadd " + TTbarNam[0] + " " + TTbarNam[1] + " " + TTbarNam[6])
    else:
        print ("ttbar file missed!")
                
    # Single Top
    if (os.path.isfile(STNam[1]) and os.path.isfile(STNam[2]) and
        os.path.isfile(STNam[3]) and os.path.isfile(STNam[4])):
        if os.path.isfile(STNam[0]):
            PrintAndCommand("rm -rf " + STNam[0])
        PrintAndCommand("hadd " + STNam[0] + " " + STNam[1] + " " + STNam[2] + " " + STNam[3] + " " + STNam[4])
    else:
        print ("Single Top file missed!")
            
    # Boson-Boson
    if os.path.isfile(VVNam[1]) and os.path.isfile(VVNam[2]) and os.path.isfile(VVNam[3]):
        if os.path.isfile(VVNam[0]):
            PrintAndCommand("rm -rf " + VVNam[0])
        PrintAndCommand("hadd " + VVNam[0] + " " + VVNam[1] + " " + VVNam[2] + " " + VVNam[3])
    else:
        print ("VV file missed!")

    # ttbar Background
    if (os.path.isfile(BkgNam[1]) and os.path.isfile(TTbarNam[6])):
        if os.path.isfile(BkgNam[0]):
            PrintAndCommand("rm -rf " + BkgNam[0])
        PrintAndCommand("hadd " + BkgNam[0] + " " + BkgNam[1] + " " + TTbarNam[6])
    else:
        print ("ttbar Bkg file missed!")

    if SysN is "central":
        # DY
        if os.path.isfile(DYNam[1]) and os.path.isfile(DYNam[2]):
            if os.path.isfile(DYNam[0]):
                PrintAndCommand("rm -rf " + DYNam[0])
            PrintAndCommand("hadd " + DYNam[0] + " " + DYNam[1] + " " + DYNam[2])
        else:
            print ("DY file missed!")

        # Data
        if os.path.isfile(DataNam[1]) and os.path.isfile(DataNam[2]):
            if os.path.isfile(DataNam[0]):
                PrintAndCommand("rm -rf " + DataNam[0])
            PrintAndCommand("hadd " + DataNam[0] + " " + DataNam[1] + " " + DataNam[2])
        else:
            print ("Data file missed!")

        # QCD
        print ("Check all the correct QCD files!!!")
        if os.path.isfile(QCDNam[1]):
            print(QCDNam[1] + " already exists!")
            PrintAndCommand("rm -rf " + QCDNam[1])
        PrintAndCommand("hadd " + QCDNam[1] + " " + QCDNam[3])

        if os.path.isfile(QCDNam[2]):
            print(QCDNam[2] + " already exists!")
            PrintAndCommand("rm -rf " + QCDNam[2])    
        PrintAndCommand("hadd " + QCDNam[2] + " " + QCDNam[4])

        if os.path.isfile(QCDNam[0]):
            PrintAndCommand("rm -rf " + QCDNam[0])
        PrintAndCommand("hadd " + QCDNam[0] + " " + QCDNam[1] + " " + QCDNam[2])

        # ttV
        if os.path.isfile(ttVNam[1]) and os.path.isfile(ttVNam[2]):
            if os.path.isfile(ttVNam[0]):
                PrintAndCommand("rm -rf " + ttVNam[0])
            PrintAndCommand("hadd " + ttVNam[0] + " " + ttVNam[1] + " " + ttVNam[2])
        else:
            print ("ttV file missed!")


PrintAndCommand("rm  -rf " + FileHead + "*_SYS.root")

MergeCombineFiles (FileHead,"central")
print "\n@@@@@@@@@@----@@@@@@@@@@\n@@@@@@@@@------@@@@@@@@@\n\n" 

for sysname in range(len(SysName)):
    for sysvar in range(len(SysVar)):
        MergeCombineFiles (FileHead, SysName[sysname] + SysVar[sysvar])
        print "\n@@@@@@@@@@----@@@@@@@@@@\n@@@@@@@@@------@@@@@@@@@\n\n"

# Add All Files
for sit in ttbar:
    namesample = FileHead + "_" + ttbarbase + ttbarcat + sit
    PrintAndCommand("hadd " + namesample + "_SYS.root " +  namesample + ".root " + namesample + "_SYS_*.root " )

for sit in bkg:
    namesample = FileHead + "_" + sit
    PrintAndCommand("hadd " + namesample + "_SYS.root " +  namesample + ".root " + namesample + "_SYS_*.root " )

for sit in singlet:
    namesample = FileHead + "_" + sit
    PrintAndCommand("hadd " + namesample + "_SYS.root " +  namesample + ".root " + namesample + "_SYS_*.root " )

for sit in diboson:
    namesample = FileHead + "_" + sit
    PrintAndCommand("hadd " + namesample + "_SYS.root " +  namesample + ".root " + namesample + "_SYS_*.root " )


