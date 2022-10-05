// zusätzliche UserRunAction Klasse

#include "myRunAction.hh"
//~ #include "myRun.hh"
#include "myHistoManager.hh"
#include "myPrimaryGeneratorAction.hh"
#include "myDetectorConstruction.hh"

#include "G4RegionStore.hh"
#include "G4Region.hh"
#include "G4ProductionCuts.hh"
#include "G4ios.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4Event.hh"
#include "G4UIcommand.hh"
#include "G4Run.hh"
#include "G4GeneralParticleSource.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

myRunAction::myRunAction(myHistoManager* histo, myPrimaryGeneratorAction* genAction, G4String _rootFileName, myDetectorConstruction* det)
:histoMan(histo),
 primGenAction(genAction),
 rootFileName(_rootFileName)
{
	NbOfClusters = det->maxNbOfClusters;
	NbOfCrystals = det->maxNbOfCrystals;  
}

myRunAction::~myRunAction()
{
	//~ fSDName.clear();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void myRunAction::BeginOfRunAction(const G4Run* aRun)
{
	G4int totalNbOfEvents = aRun->GetNumberOfEventToBeProcessed();
	G4int runID = aRun->GetRunID();
	G4cout << "Position Quelle: " << primGenAction->generalParticleSource->GetParticlePosition() << G4endl;
	G4cout << totalNbOfEvents << " Events to process in run " << runID <<  G4endl;
	//inform the runManager to save random number seed
	//~ G4RunManager::GetRunManager()->SetRandomNumberStore(true);

	//initialize cumulative quantities
	photoCounts = TracksKilledPerRun = 0.;
	//histograms
	if(runID == 0) histoMan->book(rootFileName);
	else histoMan->load(rootFileName);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//~ void myRunAction::fillPerEvent(G4double EHPGe60)
//~ {
  //~ //accumulate statistic
  //~ //
  //~ sumEHPGe60 += EHPGe60;  sum2EHPGe60 += EHPGe60*EHPGe60;
 //~ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void myRunAction::EndOfRunAction(const G4Run* aRun)
{
	G4double NbOfEvents = aRun->GetNumberOfEvent();
	if (NbOfEvents == 0) return;

	
	//G4double peakEnergy = primGenAction->GetEnergy();
	
	G4cout << "\n" << TracksKilledPerRun << " track(s) killed after 10000 steps in run " << aRun->GetRunID() << G4endl;
	G4cout << "------------------------------------------------------------" << G4endl;
		
	
	//save histograms

	//histoMan->PrintStatistic();
	histoMan->save();   
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......



