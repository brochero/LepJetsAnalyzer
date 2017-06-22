#!/bin/bash

if [ _$CMS_PATH == _ ]; then
  export CMS_PATH=/cvmfs/cms.cern.ch
  source $CMS_PATH/cmsset_default.sh
fi

input=$1
output=$2

cd /cms/scratch/brochero/CATTuples_2017/CMSSW_8_0_20/src/
scram build ProjectRename
eval `scram runtime -sh`
cd /cms/home/brochero/brochero_WorkArea/LepJetsAnalyzer/
echo "To run with CONDOR eos should be used!"
echo ./TreeReader.run -eos -i $input -o $output
./TreeReader.run -eos -i $input -o $output
