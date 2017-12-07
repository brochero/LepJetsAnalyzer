#!/usr/bin/env python
import os, time, socket, sys

OutputLocation = "/cms/scratch/brochero/LepJetsAnalyzer/TopResults/"
InputLocation  = "/xrootd/store/user/brochero/"
RunFileName    = "SubmitCondorGEN"
InputDB        = str(sys.argv[1])
CondorArg      = "'$(filehead)$(filesample) $(outputref)'"

# Systematic Variations
SysThe = {"ScaleRnF Up","ScaleRnF Down","ScaleRuF Nom","ScaleRuF Up","ScaleRdF Up","ScaleRdF Down"}
SysDedicated = {"UEUp","UEDown","ISRUp","ISRDown","FSRUp","FSRDown"}


RunThe = False
RunDedicated = False
if(len(sys.argv)>2):
    insys = sys.argv[2]
    if (insys == "scale"):
        RunThe = True
        print "Scale Variations variations will be processed....."
        RunFileName += "Scale"
    elif (insys == "dedicated"):
        RunDedicated = True
        RunFileName += "Dedicated"
    else:
        sys.exit(0)

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
    print>>fout, """executable = CondorGENJob.sh
universe   = vanilla

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

    if(RunThe):
        for isam in SamNam:
            for ithe in SysThe:
                scale = " -s " + ithe
                print>>fout, "filesample=" + isam
                print>>fout, """arguments  = " '$(filehead)$(filesample)' '$(outputref)""" + scale + """ '" """
                print>>fout, "queue 1"
    elif(RunDedicated):
        for isam in SamNam:
            for ided in SysDedicated:
                print>>fout, "filesample=" + isam + "_SYS_"+ ided
                print>>fout, """arguments  = " '$(filehead)$(filesample)' '$(outputref) '" """
                print>>fout, "queue 1"
    else:
        for isam in SamNam:
            print>>fout, "filesample=" + isam
            print>>fout, """
arguments  = "'$(filehead)$(filesample)' '$(outputref)' "
queue 1
"""

    fout = None
    
os.system("condor_submit " + RunFileName)
