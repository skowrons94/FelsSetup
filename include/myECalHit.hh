/// \brief Definition of the myECalHit class

#ifndef myECalHit_h
#define myECalHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "tls.hh"

/// Germanium detector myECal hit class
///
/// It defines data members to store the the energy and track lengths
/// of charged particles in a selected volume:
/// - fEdep, fTrackLength

class myECalHit : public G4VHit
{
  public:
    myECalHit();
    myECalHit(const myECalHit&);
    virtual ~myECalHit();

    // operators
    const myECalHit& operator=(const myECalHit&);
    G4int operator==(const myECalHit&) const;

    inline void* operator new(size_t);
    inline void  operator delete(void*);

    // methods from base class
    virtual void Draw() {}
    virtual void Print();

    // methods to handle data
   void Add(G4double de, G4double dl);

    // get methods
    G4double GetEdep() const;
    G4double GetTrackLength() const;
      
  private:
    G4double fEdep;        ///< Energy deposit in the sensitive volume
    G4double fTrackLength; ///< Track length in the  sensitive volume
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

typedef G4THitsCollection<myECalHit> myECalHitsCollection;

extern G4ThreadLocal G4Allocator<myECalHit>* myECalHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void* myECalHit::operator new(size_t)
{
  if(!myECalHitAllocator)
    myECalHitAllocator = new G4Allocator<myECalHit>;
  void *hit;
  hit = (void *) myECalHitAllocator->MallocSingle();
  return hit;
}

inline void myECalHit::operator delete(void *hit)
{
  if(!myECalHitAllocator)
    myECalHitAllocator = new G4Allocator<myECalHit>;
  myECalHitAllocator->FreeSingle((myECalHit*) hit);
}

inline void myECalHit::Add(G4double de, G4double dl) {
  fEdep += de; 
  fTrackLength += dl;
}

inline G4double myECalHit::GetEdep() const { 
  return fEdep; 
}

inline G4double myECalHit::GetTrackLength() const { 
  return fTrackLength; 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
