// additional UserRunAction header

#ifndef myRunAction_h
#define myRunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"
#include <vector>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//~ class G4Run;
class myHistoManager;
class myPrimaryGeneratorAction;
class myDetectorConstruction;

class myRunAction : public G4UserRunAction
{
  public:
    myRunAction(myHistoManager*, myPrimaryGeneratorAction*, G4String, myDetectorConstruction*);
    virtual ~myRunAction(); 
	//~ virtual G4Run* GenerateRun();
    void BeginOfRunAction(const G4Run*);
    void EndOfRunAction(const G4Run*);
    void KillCounter() {TracksKilledPerRun++;};
    const static int maxDets = 10;
    
  private:
	myHistoManager* histoMan;
	myPrimaryGeneratorAction* primGenAction;
	G4String rootFileName;
	G4int NbOfClusters;
	G4int NbOfCrystals;
	G4double photoCounts, TracksKilledPerRun;
 	// - vector of MultiFunctionalDetecor names.
	//~ std::vector<G4String> fSDName;  
   
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif





