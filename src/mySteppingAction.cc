//additional Class for information-extraction per step

#include "mySteppingAction.hh"
#include "myDetectorConstruction.hh"
#include "myEventAction.hh"

#include "G4SteppingManager.hh"
#include "G4ios.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleTypes.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4UserLimits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

mySteppingAction::mySteppingAction(myDetectorConstruction* detConst, myEventAction* evt)
:detector(detConst), 
 eventAct(evt)
{ 
	//NbOfDets = detector->NbOfClusters;
	secondaryStepLimit = new G4UserLimits(100.*m);
	otherStepLimit = new G4UserLimits(200.*m);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void mySteppingAction::UserSteppingAction(const G4Step* aStep)
{
	// get volume of the current step
	G4VPhysicalVolume* volume = aStep->GetPreStepPoint()->GetTouchableHandle()->GetVolume();
	
	// collect energy
	//~ G4double edep = aStep->GetTotalEnergyDeposit();

	G4ParticleDefinition* PartDef = aStep->GetTrack()->GetDefinition();
	G4Track* track = aStep->GetTrack();
	//~ G4int stepNbr = track->GetCurrentStepNumber();
	//~ G4int secondaries = aStep->GetNumberOfSecondariesInCurrentStep();
	
	//if (volume->GetLogicalVolume()->GetMaterial() == detector->Ge) G4cerr << "detector hit: " << volume->GetName() << G4endl;		 
	//if (volume->GetLogicalVolume()->GetMaterial() == detector->Al) G4cerr << "housing hit: " << volume->GetName() << G4endl;		 
	if ((PartDef->GetPDGCharge() != 0.) && (track->GetCurrentStepNumber() > 9999)){
		track->SetTrackStatus(fStopAndKill);
		eventAct->KillCounter();
		G4cout << "\n ----> Track killed because it reached 10000 steps\n" << G4endl;
		G4cout << "\n ----> Particle currently in \n" << volume->GetLogicalVolume()->GetName() << G4endl;
	}

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

