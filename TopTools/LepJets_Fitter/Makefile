all: LepJets_Fitter.run

LepJets_Fitter.run: LepJets_Fitter.o 
	g++ -o LepJets_Fitter.run LepJets_Fitter.o `root-config --libs`

LepJets_Fitter.o: LepJets_Fitter.C
	g++ -std=c++11 -static -I`root-config --incdir` -c -g LepJets_Fitter.C


