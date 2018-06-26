all: TreeReader.run AccTreeReader.run TableYields.run 

TreeReader.run: TopTools/SF_ID-ISO-Trigger/SFIDISOTrigger.o TopTools/ttbar_Categorization/ttbar_category.o TopTools/SF_Lumi/SFLumi.o TopTools/JEC_btag/JetCorrections.o TopTools/JEC_btag/JetResolutionObject.o TopTools/JEC_btag/JetResolution.o TopTools/JEC_btag/Jetbtag.o TreeReader.o 
	g++ -o TreeReader.run TreeReader.o TopTools/SF_ID-ISO-Trigger/SFIDISOTrigger.o TopTools/ttbar_Categorization/ttbar_category.o TopTools/SF_Lumi/SFLumi.o TopTools/JEC_btag/JetCorrections.o TopTools/JEC_btag/Jetbtag.o -DSTANDALONE TopTools/JEC_btag/JetResolutionObject.o TopTools/JEC_btag/JetResolution.o `root-config --libs` -lMinuit
TreeReader.o: TreeReader.C TreeReader.h VariablesDefinition.h TopTools/JEC_btag/JER/JER_806/JetResolution.h TopTools/JEC_btag/JER/JER_806/JetResolutionObject.h
	g++ -std=c++11 -static -I`root-config --incdir` -ITopTools/SF_ID-ISO-Trigger -ITopTools/ttbar_Categorization -ITopTools/SF_Lumi -DSTANDALONE -ITopTools/JEC_btag -c -g TreeReader.C

# -- TopTools
TopTools/SF_ID-ISO-Trigger/SFIDISOTrigger.o: TopTools/SF_ID-ISO-Trigger/SFIDISOTrigger.h TopTools/SF_ID-ISO-Trigger/SFIDISOTrigger.C
	g++ -std=c++11 -static -I`root-config --incdir` -c TopTools/SF_ID-ISO-Trigger/SFIDISOTrigger.C -o TopTools/SF_ID-ISO-Trigger/SFIDISOTrigger.o

TopTools/SF_Lumi/SFLumi.o: TopTools/SF_Lumi/SFLumi.h TopTools/SF_Lumi/SFLumi.C
	g++ -std=c++11 -static -I`root-config --incdir` -c TopTools/SF_Lumi/SFLumi.C -o TopTools/SF_Lumi/SFLumi.o

TopTools/ttbar_Categorization/ttbar_category.o: TopTools/ttbar_Categorization/ttbar_category.h TopTools/ttbar_Categorization/ttbar_category.C
	g++ -std=c++11 -static -I`root-config --incdir` -c TopTools/ttbar_Categorization/ttbar_category.C -o TopTools/ttbar_Categorization/ttbar_category.o

# -- JETS
# Jet corrections (JER Objects)
TopTools/JEC_btag/JetResolutionObject.o: TopTools/JEC_btag/JER/JER_806/JetResolutionObject.h TopTools/JEC_btag/JER/JER_806/JetResolutionObject.cc
	c++ -std=c++11 -static -I`root-config --incdir` -ITopTools/JEC_btag/JER/JER_806/ -c -DSTANDALONE TopTools/JEC_btag/JER/JER_806/JetResolutionObject.cc -o TopTools/JEC_btag/JetResolutionObject.o
# Jet corrections (JER)
TopTools/JEC_btag/JetResolution.o: TopTools/JEC_btag/JER/JER_806/JetResolution.h TopTools/JEC_btag/JER/JER_806/JetResolution.cc
	c++ -std=c++11 -static -I`root-config --incdir` -ITopTools/JEC_btag/JER/JER_806/ -c -DSTANDALONE TopTools/JEC_btag/JER/JER_806/JetResolution.cc -o TopTools/JEC_btag/JetResolution.o
# JES and JER
TopTools/JEC_btag/JetCorrections.o: TreeReader.h TopTools/JEC_btag/JER/JER_806/JetResolution.h TopTools/JEC_btag/JER/JER_806/JetResolutionObject.h TopTools/JEC_btag/JetCorrections.cc
	g++ -std=c++11 -static -I`root-config --incdir` -ITopTools/SF_ID-ISO-Trigger -ITopTools/ttbar_Categorization -ITopTools/ttbar_Categorization -ITopTools/SF_Lumi -c -DSTANDALONE TopTools/JEC_btag/JetCorrections.cc -o TopTools/JEC_btag/JetCorrections.o
# b-tag Weights
TopTools/JEC_btag/Jetbtag.o: TopTools/JEC_btag/Jetbtag.cc  TreeReader.h
	c++ -std=c++11 -static -I`root-config --incdir` -ITopTools/SF_ID-ISO-Trigger -ITopTools/ttbar_Categorization -ITopTools/SF_Lumi -c TopTools/JEC_btag/Jetbtag.cc -o TopTools/JEC_btag/Jetbtag.o

# Acceptances
AccTreeReader.run:  AccTreeReader.o TopTools/ttbar_Categorization/ttbar_category.o
	g++ -o AccTreeReader.run AccTreeReader.o `root-config --libs` TopTools/ttbar_Categorization/ttbar_category.o
AccTreeReader.o: AccTreeReader.C
	g++ -std=c++11 -static -I`root-config --incdir` -c -g AccTreeReader.C -ITopTools/ttbar_Categorization

TableYields.run: TableYields.o
	g++ -o TableYields.run TableYields.o `root-config --libs`
TableYields.o: TableYields.C
	g++ -std=c++11 -static -I`root-config --incdir` -c -g TableYields.C

clean:
	rm TreeReader.run TreeReader.o AccTreeReader.run AccTreeReader.o TableYields.o TableYields.run TopTools/SF_ID-ISO-Trigger/SFIDISOTrigger.o TopTools/ttbar_Categorization/ttbar_category.o TopTools/SF_Lumi/SFLumi.o TopTools/JEC_btag/Jetbtag.o TopTools/JEC_btag/JetCorrections.o TopTools/JEC_btag/JetResolution.o TopTools/JEC_btag/JetResolutionObject.o 

