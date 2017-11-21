#!/usr/bin/env python
import os, time, socket, sys, argparse

parser = argparse.ArgumentParser(description="ttbb/ttjj Analysis Histograms (RECO and GEN level)")
parser.add_argument('--input', type=str,
                    help='File .info')
parser.add_argument('--option', type=str, default="", choices=["","sys","jes","scale","dedicated"],
                    help='select option to run')
parser.add_argument('--gen', action="store_true",
                    help='Run generation level histos')
args = parser.parse_args()


OutputLocation = "/cms/scratch/brochero/LepJetsAnalyzer/TopResults/"
InputLocation  = "/xrootd/store/user/brochero/"
RunFileName    = "SubmitCondor"
if args.gen:
    RunFileName += "Gen"
InputDB        = args.input
CondorArg      = "'$(filehead)$(filesample) $(outputref)'"

# Systematic Variations
SysCom = {"PileUp","JER","btagjes","btaglf","btaghf","btaghfsI","btaghfsII","btaglfsI","btaglfsII","btagcfI","btagcfII"}
SysJES = {"AbsoluteStat","AbsoluteScale","AbsoluteMPFBias","Fragmentation","SinglePionECAL","SinglePionHCAL","FlavorQCD","TimePtEta","RelativeJEREC1", "RelativeJEREC2", "RelativeJERHF","RelativePtBB","RelativePtEC1","RelativePtEC2","RelativePtHF","RelativeBal","RelativeFSR","RelativeStatFSR","RelativeStatEC","RelativeStatHF","PileUpDataMC","PileUpPtRef","PileUpPtBB","PileUpPtEC1","PileUpPtHF"}
SysVar = {"Up", "Down"}
SysThe = {"ScaleRnF Up","ScaleRnF Down","ScaleRuF Nom","ScaleRuF Up","ScaleRdF Up","ScaleRdF Down"}
SysDedicated = {"UE","ISR","FSR"}

RunSys = False
RunJES = False
RunThe = False
RunDed = False

insys = args.option
if (insys == "sys"):
    RunSys = True
    print "Systematic variations will be processed....."
    RunFileName += "Syst"
if (insys == "jes"):
    RunJES = True
    print "JES variations will be processed....."
    RunFileName += "JES"
elif (insys == "scale"):
    RunThe = True
    print "Scale variations will be processed....."
    RunFileName += "Scale"
elif (insys == "dedicated"):
    RunDed = True
    print "Scale Variations in DEDICATED samples will be processed....."
    RunFileName += "Dedicated"
    
print "Reading database for Lepton+Jets in " + InputDB

fr = open(InputDB,'r')

CatVersion = ""
FileHeader = ""
OutNameRef = ""
SamNam = []

NLines = 0
for line in fr:
    if line and "#" not in line:
        if line.split():
            for word in line.split():
                if NLines is 0:
                    CatVersion = word
                elif NLines is 1:
                    FileHeader = word
                elif NLines is 2:
                    OutNameRef = word
                else:
                    sample = word
                    fullpath = InputLocation + CatVersion + "/" + FileHeader + "_" + sample
                    if os.path.isfile(fullpath + ".root"):
                        print "Sample to be processed: " + str(fullpath)
                        SamNam.append(sample)
                    else:
                        print "File " + str(fullpath) + " not found!"
            NLines += 1
fr.close()

if len(SamNam) is not 0:
    
    LogCondorDir = OutputLocation + "CondorLog_" + OutNameRef
    if (os.path.isdir(LogCondorDir)):
        print "Directory already created."
    else:
        print "Creating " + LogCondorDir + " directory and changing rights"
        os.system("mkdir " + LogCondorDir)
        os.chmod(LogCondorDir,0777)

    fout = open(RunFileName, "w")
    print>>fout, "filehead =" + FileHeader + "_"
    print>>fout, "outputref=" + OutNameRef 
    print>>fout, "filesample="
    print>>fout, "sysvar=Central"
    print>>fout, "################################"
    if args.gen:
        print>>fout, "executable = CondorGENJob.sh"
    else:
        print>>fout, "executable = CondorJob.sh"

    print>>fout, """universe   = vanilla

initial_dir= """ + LogCondorDir
    print>>fout, """
getenv                  = True
should_transfer_files   = YES
when_to_transfer_output = ON_EXIT
log                     = job_$(outputref)_$(filesample)_$(sysvar).log
output                  = job_$(outputref)_$(filesample)_$(sysvar).info
error                   = job_$(outputref)_$(filesample)_$(sysvar).err
requirements            = OpSysMajorVer == 6
"""

    if(RunSys):
        for isam in SamNam:
            for isys in SysCom:
                for ivar in SysVar:
                    systematic = " -s " + isys + " " + ivar
                    print>>fout, "filesample=" + isam
                    print>>fout, """arguments  = " '$(filehead)$(filesample)' '$(outputref)""" + systematic + """ '" """
                    print>>fout, "queue 1"
    if(RunJES):
        for isam in SamNam:
            for ijes in SysJES:
                for ivar in SysVar:
                    systematic = " -s JES" + ijes + " " + ivar
                    print>>fout, "filesample=" + isam
                    print>>fout, """arguments  = " '$(filehead)$(filesample)' '$(outputref)""" + systematic + """ '" """
                    print>>fout, "queue 1"
    elif(RunThe):
        for isam in SamNam:
            for ithe in SysThe:
                scale = " -s " + ithe
                print>>fout, "filesample=" + isam
                print>>fout, """arguments  = " '$(filehead)$(filesample)' '$(outputref)""" + scale + """ '" """
                print>>fout, "queue 1"

    elif(RunDed):
        for isam in SamNam:
            for ided in SysDedicated:
                for ivar in SysVar:
                    dedicate = " -s " + ided + " " + ivar
                    print>>fout, "filesample=" + isam + "_SYS_" + ided+ivar
                    print>>fout, """arguments  = " '$(filehead)$(filesample)' '$(outputref)""" + dedicate + """ '" """
                    print>>fout, "queue 1"
                
    # Without Systematic Variations
    else:
        for isam in SamNam:
            print>>fout, "filesample=" + isam
            print>>fout, """
arguments  = "'$(filehead)$(filesample)' '$(outputref)' "
queue 1
"""

    fout = None
    
os.system("condor_submit " + RunFileName)
