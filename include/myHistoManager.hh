#ifndef HistoManager_h
#define HistoManager_h 1

#include "globals.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 class myDetectorConstruction;
 class TFile;
 class TTree;
 class TH1D;
 class TH2D;

  const G4int NbOfHistos = 100;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class myHistoManager
{
  public:
	
	myHistoManager(myDetectorConstruction*);
	~myHistoManager();
	
	void book(G4String fileName);
	void load(G4String fileName);
	void save();	
	void FillHisto(G4int id, G4double bin, G4double weight = 1.0);
	void FillHistoAngle(G4int id, G4double bin, G4double weight = 1.0);
	void FillHistoAngle2D(G4int id, G4double bin, G4double bin2, G4double weight = 1.0);
	G4double GetPeakContentOfHisto(G4int histoNb, G4double energy, G4double fwhm = 1.0);
	void Normalize(G4int id, G4double fac);    
	void PrintStatistic();
	void SetHistoTitle(G4int id, G4String title);
	G4String GetHistoTitle(G4int id);
		
  private:
	
	myDetectorConstruction* detConst;
	static const G4int maxNbOfClusters = 5;
	static const G4int maxNbOfCrystals = 7;
	G4int NbOfClusters;
	G4int NbOfCrystals[maxNbOfClusters];
	G4bool ConstructBGO[maxNbOfClusters];
	G4int NbOfDets;
	//G4String nameCluster[maxNbOfClusters];
	TFile*   rootFile;
	TH1D*    histo[NbOfHistos+1];            
	TH1D*    histoAngle[NbOfHistos+1];  
	TH2D*    histoAngle2D[NbOfHistos+1]; 
	//~ TTree*   ntupl;    
	
	//~ G4double eDepHPGe60, eDepHPGe100, eDepBGO100, eDepClover, eDepBGOClover;
	G4double eMin, eMax;
	
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
