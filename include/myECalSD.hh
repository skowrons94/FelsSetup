/// \brief Implementation of the myECal Sensitive Detector class

#ifndef myECalSD_h
#define myECalSD_h 1

#include "G4MultiFunctionalDetector.hh"

#include "myECalHit.hh"

#include <vector>

class G4Step;
class G4HCofThisEvent;

/// myECal sensitive detector class inherited from G4MultiFunctionalDetector
///
/// In Initialize(), it creates one hit for accounting the total quantities in all steps.
///
/// The values are accounted in hits in ProcessHits() function which is called
/// by Geant4 kernel at each step.

class myECalSD : public G4MultiFunctionalDetector
{
  public:
    myECalSD(const G4String& name, const G4String& hitsCollectionName, const G4int nofCells);
    virtual ~myECalSD();
  
    // methods from base class
    virtual void   Initialize(G4HCofThisEvent* hitCollection);
    virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history);
    virtual void   EndOfEvent(G4HCofThisEvent* hitCollection);

  private:
    myECalHitsCollection* fHitsCollection;
    G4int     fNofCells;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

