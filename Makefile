all: TreeReader.run AccTreeReader.run TableYields.run 

TreeReader.run: TopTools/SF_ID-ISO-Trigger/SFIDISOTrigger.o TopTools/ttbar_Categorization/ttbar_category.o TopTools/SF_Lumi/SFLumi.o TopTools/SF_btag/BTagCalibrationStandalone.o TreeReader.o TopTools/LepJets_Fitter/LepJets_Fitter.o 
	g++ -o TreeReader.run TreeReader.o TopTools/SF_ID-ISO-Trigger/SFIDISOTrigger.o TopTools/ttbar_Categorization/ttbar_category.o TopTools/SF_btag/BTagCalibrationStandalone.o TopTools/SF_Lumi/SFLumi.o TopTools/LepJets_Fitter/LepJets_Fitter.o `root-config --libs`

TreeReader.o: TreeReader.C
	g++ -std=c++11 -static -I`root-config --incdir` -ITopTools/SF_ID-ISO-Trigger -ITopTools/ttbar_Categorization -ITopTools/SF_Lumi -ITopTools/LepJets_Fitter -c -g TreeReader.C

TopTools/SF_ID-ISO-Trigger/SFIDISOTrigger.o: TopTools/SF_ID-ISO-Trigger/SFIDISOTrigger.C
	g++ -std=c++11 -static -I`root-config --incdir` -c TopTools/SF_ID-ISO-Trigger/SFIDISOTrigger.C -o TopTools/SF_ID-ISO-Trigger/SFIDISOTrigger.o

TopTools/LepJets_Fitter/LepJets_Fitter.o: TopTools/LepJets_Fitter/LepJets_Fitter.C
	g++ -std=c++11 -static -I`root-config --incdir` -c TopTools/LepJets_Fitter/LepJets_Fitter.C -o TopTools/LepJets_Fitter/LepJets_Fitter.o

TopTools/ttbar_Categorization/ttbar_category.o: TopTools/ttbar_Categorization/ttbar_category.C
	g++ -std=c++11 -static -I`root-config --incdir` -c TopTools/ttbar_Categorization/ttbar_category.C -o TopTools/ttbar_Categorization/ttbar_category.o

TopTools/SF_Lumi/SFLumi.o: TopTools/SF_Lumi/SFLumi.C
	g++ -std=c++11 -static -I`root-config --incdir` -c TopTools/SF_Lumi/SFLumi.C -o TopTools/SF_Lumi/SFLumi.o

TopTools/SF_btag/BTagCalibrationStandalone.o: TopTools/SF_btag/BTagCalibrationStandalone.C
	g++ -std=c++11 -static -I`root-config --incdir` -c TopTools/SF_btag/BTagCalibrationStandalone.C -o TopTools/SF_btag/BTagCalibrationStandalone.o

AccTreeReader.run:  AccTreeReader.o TopTools/ttbar_Categorization/ttbar_category.o
	g++ -o AccTreeReader.run AccTreeReader.o `root-config --libs` TopTools/ttbar_Categorization/ttbar_category.o

AccTreeReader.o: AccTreeReader.C
	g++ -std=c++11 -static -I`root-config --incdir` -c -g AccTreeReader.C -ITopTools/ttbar_Categorization

TableYields.run: TableYields.o
	g++ -o TableYields.run TableYields.o `root-config --libs`

TableYields.o: TableYields.C
	g++ -std=c++11 -static -I`root-config --incdir` -c -g TableYields.C

clean:
	rm TreeReader.run TreeReader.o AccTreeReader.run AccTreeReader.o TopTools/SF_ID-ISO-Trigger/SFIDISOTrigger.o TopTools/ttbar_Categorization/ttbar_category.o TopTools/SF_Lumi/SFLumi.o TableYields.o TableYields.run TopTools/SF_btag/BTagCalibrationStandalone.o TopTools/LepJets_Fitter/LepJets_Fitter.o

