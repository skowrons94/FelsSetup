// additional UserEventAction header
 
#ifndef myEventAction_h
#define myEventAction_h 1

#include "myECalHit.hh"

#include "G4UserEventAction.hh"
#include "G4GenericMessenger.hh"

class G4Event;
class myRunAction;
class G4GenericMessenger;
class myHistoManager;
class myDetectorConstruction;
class myPrimaryGeneratorAction;



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class myEventAction : public G4UserEventAction
{
  public:
	myEventAction(myRunAction*, myHistoManager*, myDetectorConstruction*, myPrimaryGeneratorAction*);
	virtual ~myEventAction();
	
	void BeginOfEventAction(const G4Event*);
	void EndOfEventAction(const G4Event*);
	void KillCounter() {trackKillsPerEvt++;};
	
	void SetPrintModulo(G4int val) {printModulo = val;};
	
  private:
  //methods
    myECalHitsCollection* GetHitsCollection(G4int hcID, const G4Event* event) const;
	void DefineCommands();
  //members
	myRunAction*  runAct;
	myHistoManager* histoMan;
	myDetectorConstruction* detConst;
	myPrimaryGeneratorAction* primGen;
	static const G4int maxDets = 20;
	static const G4int maxNbOfClusters = 5;
	static const G4int maxNbOfCrystals = 7;
	G4double BGOThreshold;
	G4int     printModulo;
	G4int  fClusterHPGeHCID[maxDets], fClusterBGOHCID[maxDets];
	G4int fHPGe90HCID, fHPGe90BGOHCID;
	G4int NbOfCrystals[maxNbOfClusters]; // number of crystals per cluster
	G4int NbOfClusters; //number of cluster detectors
	G4String nameCluster[maxDets];
	G4GenericMessenger* fMessenger;
	G4bool ConstructBGO[maxNbOfClusters];
	G4double energyClusterHPGe[maxDets][maxDets];
	G4double energyHPGe90, energyHPGe90BGO;
	G4double energyRes[maxDets][maxDets];
	G4double energyResHPGe90;
	G4double trackKillsPerEvt, energyBGO[maxDets], sumEnergyClusterHPGe[maxDets];
	G4double angle, angle2, energy, dEnergy;
  
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

    
