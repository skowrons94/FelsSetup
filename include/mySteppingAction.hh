//additional Header for information-extraction per step


#ifndef mySteppingAction_h
#define mySteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "globals.hh"
#include "G4UserLimits.hh"

class myDetectorConstruction;
class myEventAction;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class mySteppingAction : public G4UserSteppingAction
{
  public:
    mySteppingAction(myDetectorConstruction*, myEventAction*);
    virtual ~mySteppingAction(){};

    void UserSteppingAction(const G4Step*);
    
  private:
	myDetectorConstruction* detector;
	myEventAction*          eventAct;  
	
	G4int NbOfDets;
	G4UserLimits* secondaryStepLimit;
	G4UserLimits* otherStepLimit;


};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
