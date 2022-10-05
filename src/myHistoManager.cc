#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"		// to ignore wanings on nameclash from G4SystemOfUnits "s" with ROOT def of "s"
#include "myHistoManager.hh"
#include "myDetectorConstruction.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "TH1D.h"
#include "TH2D.h"
#include "TFile.h"
#include "TTree.h"
#include "G4UIcommand.hh"
#include "TGaxis.h"
#include "TObject.h"
#include "TNamed.h"

#pragma GCC diagnostic pop

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

myHistoManager::myHistoManager(myDetectorConstruction* det)
:rootFile(0), detConst(det)
{ 
  	   
  // histograms
	for (G4int k=0; k< 2 * maxNbOfClusters * (maxNbOfCrystals + 1); k++) histo[k] = 0;
  // ntuple
  //~ ntupl = 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

myHistoManager::~myHistoManager()
{
 
    if ( rootFile ) delete rootFile;
   
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void myHistoManager::book(G4String fileName = "myDetector.root")
{ 

 
	// Creating a tree container to handle histograms and ntuples.
	// This tree is associated to an output file.
	NbOfClusters = detConst->NbOfClusters;
	for (G4int nbCrystal = 0; nbCrystal < maxNbOfCrystals; nbCrystal++) {
		NbOfCrystals[nbCrystal] = detConst->NbOfCrystals[nbCrystal];
	}
	for (G4int nbCluster = 0; nbCluster < NbOfClusters; nbCluster++) {
		ConstructBGO[nbCluster] = detConst->ConstructBGO[nbCluster];
	}
	G4cout	<< "start booking histos" << G4endl;
 	eMin = 0;
	eMax = 16000;
   	G4int bins = eMax;
   	G4double angleMin = 0, angleMax = 360.;
   	G4int angleBins = 720.;
   	G4int histoStart = 0;
	G4String histoTitles[NbOfHistos];

	for (G4int k = 0; k < NbOfClusters; k++) {
		for(G4int i = 0; i < 2 * (NbOfCrystals[k] + 1); i++) //two histos per crystal, two histos per cluster
		{
			G4int nArray = histoStart + i;
			if (i < NbOfCrystals[k]) histoTitles[nArray] = "E spectrum ClusterHPGe_" + detConst->nameCluster[k] + "_" + G4UIcommand::ConvertToString(i+1);
			else if (i == NbOfCrystals[k]) histoTitles[nArray] = "Sum of E spectrum ClusterHPGe" +detConst->nameCluster[k];
			else if (i < 2 * NbOfCrystals[k] + 1 && ConstructBGO[k] == true) histoTitles[nArray] = "E spectrum ClusterHPGe_" + detConst->nameCluster[k] + "_"  + G4UIcommand::ConvertToString(i-NbOfCrystals[k]) + " + veto";
			else if (i == 2 * NbOfCrystals[k] + 1 && ConstructBGO[k] == true) histoTitles[nArray] = "Sum of E spectrum in ClusterHPGe" + detConst->nameCluster[k] + " veto";
		}
		histoStart += NbOfCrystals[k] + 1 + ((NbOfCrystals[k] + 1) * ConstructBGO[k]);
	}
	//histoTitles[2*2*(NbOfCrystals + 1)] = "E spectrum in HPGe90";
	//histoTitles[2*2*(NbOfCrystals + 1)+1] = "E spectrum in HPGe90 + veto";
	G4cout << "saving Histos in " << fileName << G4endl;
	rootFile = new TFile(fileName,"RECREATE");
	if(!rootFile) 
	{
		G4cout	<< " myHistoManager::book :" 
				<< " problem creating the ROOT TFile "
				<< G4endl;
		return;
	}
	for(G4int i = 0; i < 2 * maxNbOfClusters * (maxNbOfCrystals + 1); i++) 
	{
		histo[i] = new TH1D(G4UIcommand::ConvertToString(i), histoTitles[i], bins, eMin, eMax);
		histoAngle[i] = new TH1D((G4String)("Angle"+G4UIcommand::ConvertToString(i)), histoTitles[i], angleBins, angleMin, angleMax);
		histoAngle2D[i] = new TH2D((G4String)("Angle2D"+G4UIcommand::ConvertToString(i)), histoTitles[i], angleBins, angleMin, angleMax, angleBins, -90, 90);
		if (!histo[i]) G4cout << "\n can't create histo "<< i << G4endl;
		if (!histoAngle[i]) G4cout << "\n can't create histo "<< i << G4endl;
		else
		{
			histo[i]->GetXaxis()->SetTitle("E_{#gamma} [keV]");
			histo[i]->GetYaxis()->SetTitle("counts");
			histoAngle[i]->GetXaxis()->SetTitle("Angle");
			histoAngle[i]->GetYaxis()->SetTitle("counts");
		}
	}

	//~ // create 1 ntuple in subdirectory "tuples"
//~ 
	//~ ntupl = new TTree("101", "Edep");
	//~ ntupl->Branch("eDepHPGe60", &eDepHPGe60, "eDepHPGe60/D");
	//~ ntupl->Branch("eDepHPGe100", &eDepHPGe100, "eDepHPGe100/D");
	//~ ntupl->Branch("eDepBGO100", &eDepBGO100, "eDepBGO100/D");
	//~ ntupl->Branch("eDepClover", &eDepClover, "eDepClover/D");
	//~ ntupl->Branch("eDepBGOClover", &eDepBGOClover, "eDepBGOClover/D");
 
 G4cout << "\n----> Histogram file is opened in " << fileName << G4endl;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void myHistoManager::load(G4String fileName = "myDetector.root")
{ 

    rootFile = new TFile(fileName, "UPDATE");       // Open an existing histogram file to write
    if(!rootFile) {
		G4cout	<< " myHistoManager::load : problem loading the ROOT TFile "<< fileName << G4endl;
		return;
	}
    else G4cout << "\n----> Histogram-File " << fileName << " reloaded \n" << G4endl;
  
	for(G4int i = 0 ; i < 2 * maxNbOfClusters * (maxNbOfCrystals + 1); i++)
	{
		histo[i] = (TH1D*)rootFile->Get(G4UIcommand::ConvertToString(i));
		if (!histo[i]) G4cout << "\n can't load histo "<< i << G4endl;
	}
		
	//~ ntupl = (TTree*)rootFile->Get("101");
	//~ if (!ntupl) G4cout << "\n can't load tree " << G4endl;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void myHistoManager::save()
{ 

  if (rootFile) {
    rootFile->Write();       // Writing the histograms to the file
    rootFile->Close();        // and closing the tree (and the file)
    G4cout << "\n----> Histogram Tree is saved \n" << G4endl;
  }

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void myHistoManager::FillHisto(G4int ih, G4double energy, G4double weight)
{
  if (ih >= NbOfHistos) {
    G4cout << "---> warning from HistoManager::FillHisto() : histo " << ih
           << " does not exist." << G4endl;
    return;
  }

 if  (histo[ih]) { 
	 //~ G4int xbin = histo[ih]->FindFixBin(energy/keV);
	 histo[ih]->Fill(energy/keV, weight);
	 //~ histo[ih]->SetBinError(xbin, 0.);
  }

}

void myHistoManager::FillHistoAngle(G4int ih, G4double angle, G4double weight)
{
  if (ih >= NbOfHistos) {
    G4cout << "---> warning from HistoManager::FillHisto() : histo " << ih
           << " does not exist." << G4endl;
    return;
  }

 if  (histoAngle[ih]) { 
	 //~ G4int xbin = histo[ih]->FindFixBin(energy/keV);
	 histoAngle[ih]->Fill(angle/deg, weight);
	 //~ histo[ih]->SetBinError(xbin, 0.);
  }

}

void myHistoManager::FillHistoAngle2D(G4int ih, G4double angle, G4double angle2, G4double weight)
{
  if (ih >= NbOfHistos) {
    G4cout << "---> warning from HistoManager::FillHisto() : histo " << ih
           << " does not exist." << G4endl;
    return;
  }

 if  (histoAngle2D[ih]) { 
	 //~ G4int xbin = histo[ih]->FindFixBin(energy/keV);
	 histoAngle2D[ih]->Fill(angle/deg, angle2/deg, weight);
	 //~ histo[ih]->SetBinError(xbin, 0.);
  }

}

G4double myHistoManager::GetPeakContentOfHisto(G4int histoNb, G4double energy, G4double fwhm)
{	
	G4int binCont = 0;
	if (histo[histoNb]) 
	{
		G4int nBins = histo[histoNb]->GetNbinsX();
		//~ G4cout << "\n total BinNumber " << nBins << G4endl;
		G4int bin = histo[histoNb]->FindFixBin(energy/keV);
		//~ G4cout << "\n BinNumber " << bin << " = energy " << energy << G4endl;
		for (G4int j = -1 * floor(fwhm + 0.5); j <= floor(fwhm + 0.5); j++) {
			if(bin + j > 0 && bin + j < nBins) binCont += histo[histoNb]->GetBinContent(bin + j);
		}
		return binCont;
	} else {
		G4cout << "---> warning from HistoManager::GetBinContentOfHisto() : histo " << histoNb
        << " does not exist." << G4endl;
		return 0;
	}
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void myHistoManager::Normalize(G4int histoNb, G4double scaleFac)
{
 	char buffer[100];

	if (histo[histoNb]) 
	{
		histo[histoNb]->Scale(scaleFac, "width");
		TAxis* yax = histo[histoNb]->GetYaxis();
		sprintf(buffer, "Count rate [/d /binwidth]");
		yax->SetTitle(buffer);
	} else 
	{
		G4cout	<< "---> warning from HistoManager::Normalize() : histo " << histoNb
				<< " does not exist. (fac=" << scaleFac << ")" << G4endl;
		return;
	}
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void myHistoManager::SetHistoTitle(G4int histoNb, G4String hTitle) {
	
	if (histo[histoNb]) 
	{
		G4cout << "Setting title of histo " << histoNb << " to " << hTitle << G4endl;
		histo[histoNb]->SetTitle(hTitle);
	} else 
	{
		G4cout	<< "---> warning from HistoManager::SetHistoTitle() : histo " << histoNb
				<< " does not exist." << G4endl;
		return;
	}	

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void myHistoManager::PrintStatistic()
{

	//~ if(histo[2 * NbOfClusters * (NbOfCrystals + 1)]) {
		//~ G4cout << "\n ----> print histograms statistic \n" << G4endl;
		//~ for(G4int i = 0; i < 2 * NbOfClusters * (NbOfCrystals + 1); i++) 
		//~ {
			//~ G4cout << histo[i]->GetTitle() << " : mean = " << G4BestUnit(histo[i]->GetMean(), "Energy") 
				//~ << " rms = " << G4BestUnit(histo[1]->GetRMS(),  "Energy") << G4endl;
		//~ }
	//~ }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4String myHistoManager::GetHistoTitle(G4int histoNb)
{
	if (histo[histoNb]) 
	{
		return histo[histoNb]->TNamed::GetTitle();
	} else 
	{
		G4cout	<< "---> warning from HistoManager::SetHistoTitle() : histo " << histoNb
				<< " does not exist." << G4endl;
		return "error histo not not found";
	}	

}
