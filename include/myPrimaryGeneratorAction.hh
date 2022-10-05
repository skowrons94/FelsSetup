//obligatorischer PrimaryGeneratorAction Header
 
#ifndef myPrimaryGeneratorAction_h
#define myPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4GenericMessenger.hh"
#include "G4ThreeVector.hh"
#include "Randomize.hh"

class myDetectorConstruction;
class G4GenericMessenger;
class G4GeneralParticleSource;
class G4ParticleGun;
class G4Event;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
class myPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    myPrimaryGeneratorAction(myDetectorConstruction*, G4double);    
   ~myPrimaryGeneratorAction();

 public:
	G4double Theta, Phi;
    void GeneratePrimaries(G4Event*);
    void SetEnergy(G4double);
    void Cascade(G4int, G4Event*);
    G4ThreeVector ParticleMomentumDirection(G4double);
    G4double CalculateParticleEnergy(G4double);
	G4double GetEnergy() 		{return particleEnergy; } ;
	G4ThreeVector GetNewUz() 	{return newUz; };
    G4double GetAlphaMax()   	{return alphaMax; };
    G4double modifiedEnergy;
    G4GeneralParticleSource* generalParticleSource;
    G4ParticleGun* particleGun;

  private:
  //methods
	void DefineCommands();
  //members
    myDetectorConstruction* myDetectorConst;
    G4GenericMessenger* fMessenger;    
    G4ThreeVector newUz;
    G4bool source, doppler, cascade;
    G4int printModulo;
    G4double Qvalue;
    G4double alphaMax;
    G4double cosAlpha, sinAlpha;
	G4double psi;
	G4double particleEnergy;
	G4String particleName;
	CLHEP::HepRandomEngine* myRandomEngine;

     

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

