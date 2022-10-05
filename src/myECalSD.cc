/// \brief Implementation of the myECal Sensitive Detector class

#include "myECalSD.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

myECalSD::myECalSD(const G4String& name, const G4String& hitsCollectionName, G4int nofCells)
: G4MultiFunctionalDetector(name),
  fHitsCollection(0),
  fNofCells(nofCells)
{
	collectionName.insert(hitsCollectionName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

myECalSD::~myECalSD() 
{ 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void myECalSD::Initialize(G4HCofThisEvent* hce)
{
	// Create hits collection
	fHitsCollection = new myECalHitsCollection(SensitiveDetectorName, collectionName[0]); 
	
	// Add this collection in hce
	G4int hcID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
	hce->AddHitsCollection( hcID, fHitsCollection ); 
	
	// Create hit for this event
	// fNofCells for cells + one more for total sums 
	for (G4int i=0; i<fNofCells+1; i++ ) 
	{
		fHitsCollection->insert(new myECalHit());
	}	
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool myECalSD::ProcessHits(G4Step* step, G4TouchableHistory*)
{  
	// energy deposit
	G4double edep = step->GetTotalEnergyDeposit();
	
	// step length
	G4double stepLength = 0.;
	//~ if ( step->GetTrack()->GetDefinition()->GetPDGCharge() != 0. ) {
	stepLength = step->GetStepLength();
	//~ }
	
	if ( edep==0. && stepLength == 0. ) return false;      

	G4TouchableHistory* touchable = (G4TouchableHistory*)(step->GetPreStepPoint()->GetTouchable());
	
	// Get calorimeter cell id 
	G4int layerNumber = touchable->GetCopyNumber(0);
	//~ G4cout << "For Volume " << touchable->GetVolume()->GetName() << " the replica number in this step:" << layerNumber << G4endl;
	
	// Get hit accounting data for this cell
	myECalHit* hit = (*fHitsCollection)[layerNumber];
	if ( ! hit ) 
	{
		G4ExceptionDescription msg;
		msg << "Cannot access hit for copy number " << layerNumber; 
		G4Exception("myECalSD::ProcessHits()", "MyCode0004", FatalException, msg);
	}         
	
	// Get hit for total accounting
	myECalHit* hitTotal = (*fHitsCollection)[fHitsCollection->entries()-1];
	if ( ! hitTotal ) 
	{
		G4ExceptionDescription msg;
		msg << "Cannot access hitTotal"; 
		G4Exception("myECalSD::ProcessHits()", "MyCode0004", FatalException, msg);
	}         
	
	// Add values
	hit->Add(edep, stepLength);
	hitTotal->Add(edep, stepLength); 
 					  
	return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void myECalSD::EndOfEvent(G4HCofThisEvent*)
{
  //~ if (event_id%printModulo == 0) { 
     //~ G4int nofHits = fHitsCollection->entries();
     //~ G4cout << "\n-------->Hits Collection: in this event there are " << nofHits 
            //~ << " hits in the myECal: " << G4endl;
     //~ for ( G4int i=0; i<nofHits; i++ ) (*fHitsCollection)[i]->Print();
     //~ (*fHitsCollection)[0]->Print();
  //~ }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
