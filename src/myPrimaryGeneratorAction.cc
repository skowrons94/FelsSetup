//mandatory PrimaryGeneratorAction Class

#include "myPrimaryGeneratorAction.hh"
#include "myDetectorConstruction.hh"

#include "G4GenericMessenger.hh"
#include "G4Event.hh"
#include "G4GeneralParticleSource.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleGun.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "Randomize.hh"
#include "G4UImanager.hh"

#include <ctime>
#include <cmath>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

myPrimaryGeneratorAction::myPrimaryGeneratorAction(myDetectorConstruction* myDC, G4double _particleEnergy = 2.750*MeV)
:myDetectorConst(myDC),
 particleEnergy(_particleEnergy)
{
	source = true; //true: particle created with G4RadioactiveDecay, false: particles with adjustable angular dependency and energy
	doppler = false;
	cascade = true;
	G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
	G4ParticleDefinition* particle = particleTable->FindParticle("gamma");		//"gamma" "proton" "mu-" "e-"
	//G4ParticleDefinition* particle = particleTable->FindParticle("geantino");		//"gamma" "proton" "mu-" "e-"
	particleName = particle->GetParticleName();
	
	if(source == true) {
		generalParticleSource = new G4GeneralParticleSource();
		G4cout << "PARTICLE SOURCE: " << generalParticleSource->GetParticlePosition().getX() << " " << generalParticleSource->GetParticlePosition().getY() << " " << generalParticleSource->GetParticlePosition().getZ() << G4endl;
	}
	if (source == false) {
		particleGun = new G4ParticleGun();
		particleGun->SetParticleDefinition(particle);
		particleGun->SetParticlePosition(G4ThreeVector(0.,0.,0.));
	}
		
	//Messenger commands
	DefineCommands();

	//inform the runManager to save random number seed
	//~ G4RunManager::GetRunManager()->SetRandomNumberStore(true);
    // get random engine and set seed from system time
	myRandomEngine = CLHEP::HepRandom::getTheEngine();

}

myPrimaryGeneratorAction::~myPrimaryGeneratorAction()
{
	delete particleGun;
	delete fMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ThreeVector myPrimaryGeneratorAction::ParticleMomentumDirection(G4double a2 = 0)
{
	//phi -> theta and theta -> phi, because z-axis is not upwards
	G4bool foundAngle = false;
	//angular distribution
	while (!foundAngle) {
		Theta = myRandomEngine->flat() * twopi;
		G4double rnd = myRandomEngine->flat();
		//G4double angdist = (1. + 5./7. * 1./2. * (3.*pow(cos(Theta),2) - 1) - 12./7. * 1./8. * (35.*pow(cos(Theta),4) - 30.*pow(cos(Theta),2) + 3))/1.875; //c12(a,g) E2
		G4double angdist = 1. + a2 * 1./2. * (3.*pow(cos(Theta),2) - 1); //he3(a,g)
		if (rnd <= angdist)
			foundAngle = true;
	//G4cout << foundAngle << " " << Theta << " " << rnd << " " << angdist << G4endl;
	}	
	Phi = myRandomEngine->flat() * pi - pi/2.;
	cosAlpha = std::cos(Theta);	
	G4ThreeVector v;//(std::sin(Theta)*std::cos(Phi), std::sin(Theta)*std::sin(Phi), std::cos(Theta));
	v.setRThetaPhi(1.,Theta,Phi);
	return v;
}

G4double myPrimaryGeneratorAction::CalculateParticleEnergy(G4double particleE)
{
	G4double EtargetThickness = 12.*keV; //not yet implemented
	Qvalue =  1.58713*MeV;
	G4double mBeam = 4.00260325413; //in u
	G4double mTarget = 3.01602932265; //in u
	G4double mBound = 7.01692872; //mass of compound nucleus in u
	G4double Ebeam = (particleEnergy - Qvalue) * (mTarget + mBeam) / mTarget; //beam energy in laboratory system
	G4double modifiedParticleEnergy;
	if (Ebeam > 0) {
		G4double impulseBeam = sqrt(2 * mBeam * 931.5 * Ebeam);
		G4double vBeam = impulseBeam / (mBound * 931.5*MeV); //velocity of beam in units of c
		G4double Edoppler = vBeam * particleEnergy * cosAlpha;
		G4double Erecoil = pow(particleE, 2.) / (2. * mBound * 931.5*MeV);
		modifiedParticleEnergy = particleE - Erecoil + Edoppler; //recoil energy, target thickness for gas target, doppler shift
		//G4cout << particleE << " " << vBeam << " " << Edoppler << " " << particleEnergy << " " << cosAlpha << " " << Erecoil << G4endl;
	} 
	else {
		modifiedParticleEnergy = particleE;
	}
	return modifiedParticleEnergy;
}

void myPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{ 
	G4int event_id = anEvent->GetEventID();	
	if (event_id == 0) 	G4cout << "Generating primary particles..." << G4endl;
	
	G4double levelE;
	if (source == true) {
		generalParticleSource->GeneratePrimaryVertex(anEvent);
	}
	if (cascade == false && source == false && doppler == false) {
		particleGun->SetParticleEnergy(particleEnergy);
		particleGun->SetParticleMomentumDirection(ParticleMomentumDirection(0.));
		particleGun->GeneratePrimaryVertex(anEvent);		
	}
	if (cascade == true && source == false && doppler == false) {
		Cascade(4, anEvent);
	}
	if (source == false && doppler == true) { //doppler shift for simulations with beam
		particleGun->SetParticleEnergy(CalculateParticleEnergy(particleEnergy));
		particleGun->SetParticleMomentumDirection(ParticleMomentumDirection(0.));
		particleGun->GeneratePrimaryVertex(anEvent);		
	}

}

void myPrimaryGeneratorAction::Cascade(G4int cascadeOption, G4Event* anEvent) 
{
	//different cascade options>
	//0: 3He(a,g) full energy gamma or two gammas with excitation of 2321keV resonance, angular distribution implemented
	//1: 22Ne(a,g) 11319keV resonance
	//2: 14N(a,g) 4860keV
	//3: 14N(a,g) 5605keV
	//4: 14N(a,g) 5603keV
	//5: 14N(a,g) 5298keV
	//6: 12C(a,g) cascade with 7.1169MeV resonance
	//7: 12C(a,g) cascade with 6.9171MeV resonance
	
	//other option: use source = true and cascade_sbash_start.sh, isotope and excitation energy can be adapted in the mac file
	G4double fullE;
	G4double rand = myRandomEngine->flat();
	G4double rand2 = myRandomEngine->flat();
	G4double rand3 = myRandomEngine->flat();
	G4double rand4 = myRandomEngine->flat();
	G4double rand5 = myRandomEngine->flat();
	G4double rand6 = myRandomEngine->flat();

	//3He(a,g) 2750keV cascade
	if (cascadeOption == 0) {
		if (anEvent->GetEventID() == 0) G4cout << "Cascade option 0: 3He(a,g) 2750keV cascade chosen" << G4endl;
		fullE = particleEnergy;
		G4double levelE = 2321.*keV;
		if (rand < 0.69) {
			particleGun->SetParticleMomentumDirection(ParticleMomentumDirection(0.12));
			//particleGun->SetParticleEnergy(CalculateParticleEnergy(fullE));
			particleGun->SetParticleEnergy(fullE);
			particleGun->GeneratePrimaryVertex(anEvent);
		}
		else if (fullE > levelE){
			particleGun->SetParticleMomentumDirection(ParticleMomentumDirection(0.1));
			//particleGun->SetParticleEnergy(CalculateParticleEnergy(levelE));
			particleGun->SetParticleEnergy(levelE);
			particleGun->GeneratePrimaryVertex(anEvent);
				
			particleGun->SetParticleMomentumDirection(ParticleMomentumDirection(0.));
			particleGun->SetParticleEnergy(fullE-levelE);
			particleGun->GeneratePrimaryVertex(anEvent);
		}
	}	
		
	//22Ne(a,g) 11319keV
	if (cascadeOption == 1) {
		if (anEvent->GetEventID() == 0) G4cout << "Cascade option 1: 22Ne(a,g) 11319keV cascade chosen" << G4endl;
		fullE = 11319.*keV;
		if (rand < 0.54) {
			particleGun->SetParticleEnergy(fullE - 1809.*keV);
			particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
			particleGun->GeneratePrimaryVertex(anEvent);
			
			particleGun->SetParticleEnergy(1809.*keV);
			particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
			particleGun->GeneratePrimaryVertex(anEvent);
		}
		else {
			particleGun->SetParticleEnergy(fullE - 7062.*keV);
			particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
			particleGun->GeneratePrimaryVertex(anEvent);
			
			if (rand2 < 0.34) {
				particleGun->SetParticleEnergy(7062.*keV - 3589.*keV);
				particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
				particleGun->GeneratePrimaryVertex(anEvent);
				
				particleGun->SetParticleEnergy(3589.*keV - 1809.*keV);
				particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
				particleGun->GeneratePrimaryVertex(anEvent);
				
				particleGun->SetParticleEnergy(1809.*keV);
				particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
				particleGun->GeneratePrimaryVertex(anEvent);
			}
			else if (rand2 < 0.34 + 0.1) {
				particleGun->SetParticleEnergy(7062.*keV - 2939.*keV);
				particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
				particleGun->GeneratePrimaryVertex(anEvent);
				
				if (rand3 < 0.91) {
					particleGun->SetParticleEnergy(2939.*keV - 1809.*keV);
					particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
					particleGun->GeneratePrimaryVertex(anEvent);
					
					particleGun->SetParticleEnergy(1809.*keV);
					particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
					particleGun->GeneratePrimaryVertex(anEvent);
				}
				else {
					particleGun->SetParticleEnergy(2939.*keV);
					particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
					particleGun->GeneratePrimaryVertex(anEvent);
				}
			}
			else if (rand2 < 0.34 + 0.1 + 0.2) {
				particleGun->SetParticleEnergy(7062.*keV - 1809.*keV);
				particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
				particleGun->GeneratePrimaryVertex(anEvent);
				
				particleGun->SetParticleEnergy(1809.*keV);
				particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
				particleGun->GeneratePrimaryVertex(anEvent);
			}
			else {
				particleGun->SetParticleEnergy(7062.*keV);
				particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
				particleGun->GeneratePrimaryVertex(anEvent);
			}
			
		}
	}
	//14N(a,g) 4860keV
	if (cascadeOption == 2) {
		if (anEvent->GetEventID() == 0) G4cout << "Cascade option 2: 14N(a,g) 4860keV cascade chosen" << G4endl;
		fullE = 4860.*keV;
		if (rand < 0.65) {
			particleGun->SetParticleEnergy(fullE - 1042.*keV);
			particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
			particleGun->GeneratePrimaryVertex(anEvent);
			
			particleGun->SetParticleEnergy(1042.*keV);
			particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
			particleGun->GeneratePrimaryVertex(anEvent);
		}
		else if (rand < 0.65 + 0.08) {
			particleGun->SetParticleEnergy(fullE - 1081.*keV);
			particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
			particleGun->GeneratePrimaryVertex(anEvent);
			
			particleGun->SetParticleEnergy(1081.*keV);
			particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
			particleGun->GeneratePrimaryVertex(anEvent);
		}
		else if (rand < 0.65 + 0.08 + 0.23) {
			particleGun->SetParticleEnergy(fullE - 3062.*keV);
			particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
			particleGun->GeneratePrimaryVertex(anEvent);
			
			if (rand2 < 0.23) {
				particleGun->SetParticleEnergy(3062.*keV);
				particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
				particleGun->GeneratePrimaryVertex(anEvent);
			}
			else {
				particleGun->SetParticleEnergy(3062.*keV - 937.*keV);
				particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
				particleGun->GeneratePrimaryVertex(anEvent);
				
				particleGun->SetParticleEnergy(937.*keV);
				particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
				particleGun->GeneratePrimaryVertex(anEvent);	
			}
		}
		else {
			particleGun->SetParticleEnergy(fullE - 3134.*keV);
			particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
			particleGun->GeneratePrimaryVertex(anEvent);
			
			if (rand2 < 0.39) {
				particleGun->SetParticleEnergy(3134.*keV);
				particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
				particleGun->GeneratePrimaryVertex(anEvent);
			}
			else if (rand2 < 0.39+0.34) {
				particleGun->SetParticleEnergy(3134.*keV - 1042.*keV);
				particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
				particleGun->GeneratePrimaryVertex(anEvent);
				
				particleGun->SetParticleEnergy(1042.*keV);
				particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
				particleGun->GeneratePrimaryVertex(anEvent);
			}
			else {
				particleGun->SetParticleEnergy(3134.*keV - 1081.*keV);
				particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
				particleGun->GeneratePrimaryVertex(anEvent);

				particleGun->SetParticleEnergy(1081.*keV);
				particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
				particleGun->GeneratePrimaryVertex(anEvent);
			}
			
		}
	}
	
	//14N(a,g) 5605keV
	if (cascadeOption == 3) {
		if (anEvent->GetEventID() == 0) G4cout << "Cascade option 3: 14N(a,g) 5605keV cascade chosen" << G4endl;
		fullE = 5605.*keV;
		if (rand < 0.042) {
			particleGun->SetParticleEnergy(fullE - 1042.*keV);
			particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
			particleGun->GeneratePrimaryVertex(anEvent);
			particleGun->SetParticleEnergy(1042.*keV);
			particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
			particleGun->GeneratePrimaryVertex(anEvent);	
		}
		else if (rand < 0.042 + 0.543) {
			particleGun->SetParticleEnergy(fullE - 1081.*keV);
			particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
			particleGun->GeneratePrimaryVertex(anEvent);
			particleGun->SetParticleEnergy(1081.*keV);
			particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
			particleGun->GeneratePrimaryVertex(anEvent);	
		}
		else if (rand < 0.042 + 0.543 + 0.026) {
			particleGun->SetParticleEnergy(fullE - 3062.*keV);
			particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
			particleGun->GeneratePrimaryVertex(anEvent);			
			
			if (rand2 < 0.232) {
				particleGun->SetParticleEnergy(3062.*keV);
				particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
				particleGun->GeneratePrimaryVertex(anEvent);
			}
			else {
				particleGun->SetParticleEnergy(3062.*keV - 937.*keV);
				particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
				particleGun->GeneratePrimaryVertex(anEvent);
				particleGun->SetParticleEnergy(937.*keV);
				particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
				particleGun->GeneratePrimaryVertex(anEvent);								
			}
		}
		else if (rand < 0.042 + 0.543 + 0.026 + 0.322) {
			particleGun->SetParticleEnergy(fullE - 3134.*keV);
			particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
			particleGun->GeneratePrimaryVertex(anEvent);
			if (rand2 < 0.39) {
				particleGun->SetParticleEnergy(3134.*keV);
				particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
				particleGun->GeneratePrimaryVertex(anEvent);
			}
			else if (rand2 < 0.39 + 0.34) {
				particleGun->SetParticleEnergy(3134.*keV - 1042.*keV);
				particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
				particleGun->GeneratePrimaryVertex(anEvent);
				particleGun->SetParticleEnergy(1042.*keV);
				particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
				particleGun->GeneratePrimaryVertex(anEvent);
			}
			else if (rand2 < 0.39 + 0.34 + 0.25) {
				particleGun->SetParticleEnergy(3134.*keV - 1081.*keV);
				particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
				particleGun->GeneratePrimaryVertex(anEvent);
				particleGun->SetParticleEnergy(1081.*keV);
				particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
				particleGun->GeneratePrimaryVertex(anEvent);
			}
			else {
				particleGun->SetParticleEnergy(3134.*keV - 1701.*keV);
				particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
				particleGun->GeneratePrimaryVertex(anEvent);
				if (rand3 < 0.298) {
					particleGun->SetParticleEnergy(1701.*keV);
					particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
					particleGun->GeneratePrimaryVertex(anEvent);
				}
				else {
					particleGun->SetParticleEnergy(1701.*keV - 1042.*keV);
					particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
					particleGun->GeneratePrimaryVertex(anEvent);
					particleGun->SetParticleEnergy(1042.*keV);
					particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
					particleGun->GeneratePrimaryVertex(anEvent);
				}
			}
		}
		else {
			particleGun->SetParticleEnergy(fullE);
			particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
			particleGun->GeneratePrimaryVertex(anEvent);
		}	
	}
	
	//14N(a,g) 5603keV
	if (cascadeOption == 4) {
		if (anEvent->GetEventID() == 0) G4cout << "Cascade option 4: 14N(a,g) 5603keV cascade chosen" << G4endl;
		fullE = 5603.*keV;
		if (rand < 0.167) {
			particleGun->SetParticleEnergy(fullE);
			particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
			particleGun->GeneratePrimaryVertex(anEvent);
		}
		else if (rand < 0.167 + 0.038) {
			particleGun->SetParticleEnergy(fullE - 1042.*keV);
			particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
			particleGun->GeneratePrimaryVertex(anEvent);
			particleGun->SetParticleEnergy(1042.*keV);
			particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
			particleGun->GeneratePrimaryVertex(anEvent);
		}
		else {
			particleGun->SetParticleEnergy(fullE - 3062.*keV);
			particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
			particleGun->GeneratePrimaryVertex(anEvent);
			if (rand2 < 0.232) {
				particleGun->SetParticleEnergy(3062.*keV);
				particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
				particleGun->GeneratePrimaryVertex(anEvent);
			}
			else {
				particleGun->SetParticleEnergy(3062.*keV - 937.*keV);
				particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
				particleGun->GeneratePrimaryVertex(anEvent);
				particleGun->SetParticleEnergy(937.*keV);
				particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
				particleGun->GeneratePrimaryVertex(anEvent);
			}
		}		
	}
	
	//14N(a,g) 5298keV
	if (cascadeOption == 5) {
		if (anEvent->GetEventID() == 0) G4cout << "Cascade option 5: 14N(a,g) 5298keV cascade chosen" << G4endl;
		fullE = 5298.*keV;
		if (rand < 0.09) {
			particleGun->SetParticleEnergy(fullE - 937.*keV);
			particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
			particleGun->GeneratePrimaryVertex(anEvent);
			particleGun->SetParticleEnergy(937.*keV);
			particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
			particleGun->GeneratePrimaryVertex(anEvent);
		}
		else if (rand < 0.09 + 0.07) {
			particleGun->SetParticleEnergy(fullE - 1121.*keV);
			particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
			particleGun->GeneratePrimaryVertex(anEvent);
			particleGun->SetParticleEnergy(1121.*keV - 937.*keV);
			particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
			particleGun->GeneratePrimaryVertex(anEvent);			
			particleGun->SetParticleEnergy(937.*keV);
			particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
			particleGun->GeneratePrimaryVertex(anEvent);
		}
		else if (rand < 0.09 + 0.07 + 0.78) {
			particleGun->SetParticleEnergy(fullE - 2523.*keV);
			particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
			particleGun->GeneratePrimaryVertex(anEvent);
			//verzweigung insgesamt 100.3%, jeder zweig um 0.1% verringert
			if (rand2 < 0.748) {
				particleGun->SetParticleEnergy(2523.*keV);
				particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
				particleGun->GeneratePrimaryVertex(anEvent);	
			}
			else if (rand2 < 0.748 + 0.214) {
				particleGun->SetParticleEnergy(2523.*keV - 937.*keV);
				particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
				particleGun->GeneratePrimaryVertex(anEvent);
				particleGun->SetParticleEnergy(937.*keV);
				particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
				particleGun->GeneratePrimaryVertex(anEvent);
			}
			else {
				particleGun->SetParticleEnergy(2523.*keV - 1701.*keV);
				particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
				particleGun->GeneratePrimaryVertex(anEvent);
				if (rand3 < 0.298) {
					particleGun->SetParticleEnergy(1701.*keV);
					particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
					particleGun->GeneratePrimaryVertex(anEvent);
				}
				else {
					particleGun->SetParticleEnergy(1701.*keV - 1042.*keV);
					particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
					particleGun->GeneratePrimaryVertex(anEvent);
					particleGun->SetParticleEnergy(1042.*keV);
					particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
					particleGun->GeneratePrimaryVertex(anEvent);
				}
			}
		}
		else if (rand < 0.09 + 0.07 + 0.78 + 0.05) {
			particleGun->SetParticleEnergy(fullE - 3358.*keV);
			particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
			particleGun->GeneratePrimaryVertex(anEvent);
			if (rand2 < 0.45) {
				particleGun->SetParticleEnergy(3358.*keV);
				particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
				particleGun->GeneratePrimaryVertex(anEvent);
			}
			else if (rand2 < 0.45 + 0.09) {
				particleGun->SetParticleEnergy(3358.*keV - 937.*keV);
				particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
				particleGun->GeneratePrimaryVertex(anEvent);
				particleGun->SetParticleEnergy(937.*keV);
				particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
				particleGun->GeneratePrimaryVertex(anEvent);
			}
			else if (rand2 < 0.45 + 0.09 + 0.4) {
				particleGun->SetParticleEnergy(3358.*keV - 1701.*keV);
				particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
				particleGun->GeneratePrimaryVertex(anEvent);
				if (rand3 < 0.298) {
					particleGun->SetParticleEnergy(1701.*keV);
					particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
					particleGun->GeneratePrimaryVertex(anEvent);
				}
				else {
					particleGun->SetParticleEnergy(1701.*keV - 1042.*keV);
					particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
					particleGun->GeneratePrimaryVertex(anEvent);
					particleGun->SetParticleEnergy(1042.*keV);
					particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
					particleGun->GeneratePrimaryVertex(anEvent);
				}
			}
			else {
				particleGun->SetParticleEnergy(3358.*keV - 2523.*keV);
				particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
				particleGun->GeneratePrimaryVertex(anEvent);
				//verzweigung insgesamt 100.3%, jeder zweig um 0.1% verringert
				if (rand3 < 0.748) {
					particleGun->SetParticleEnergy(2523.*keV);
					particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
					particleGun->GeneratePrimaryVertex(anEvent);	
				}
				else if (rand3 < 0.748 + 0.214) {
					particleGun->SetParticleEnergy(2523.*keV - 937.*keV);
					particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
					particleGun->GeneratePrimaryVertex(anEvent);
					particleGun->SetParticleEnergy(937.*keV);
					particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
					particleGun->GeneratePrimaryVertex(anEvent);
				}
				else {
					particleGun->SetParticleEnergy(2523.*keV - 1701.*keV);
					particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
					particleGun->GeneratePrimaryVertex(anEvent);
					if (rand4 < 0.298) {
						particleGun->SetParticleEnergy(1701.*keV);
						particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
						particleGun->GeneratePrimaryVertex(anEvent);
					}
					else {
						particleGun->SetParticleEnergy(1701.*keV - 1042.*keV);
						particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
						particleGun->GeneratePrimaryVertex(anEvent);
						particleGun->SetParticleEnergy(1042.*keV);
						particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
						particleGun->GeneratePrimaryVertex(anEvent);
					}
				}
			}	
		}
		else {
			particleGun->SetParticleEnergy(fullE - 4652.*keV);
			particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
			particleGun->GeneratePrimaryVertex(anEvent);
			if (rand2 < 0.17) {
				particleGun->SetParticleEnergy(4625.*keV - 937.*keV);
				particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
				particleGun->GeneratePrimaryVertex(anEvent);
				particleGun->SetParticleEnergy(937.*keV);
				particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
				particleGun->GeneratePrimaryVertex(anEvent);
			}
			else {
				particleGun->SetParticleEnergy(4625.*keV - 1121.*keV);
				particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
				particleGun->GeneratePrimaryVertex(anEvent);
				particleGun->SetParticleEnergy(1121.*keV - 937.*keV);
				particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
				particleGun->GeneratePrimaryVertex(anEvent);
				particleGun->SetParticleEnergy(937.*keV);
				particleGun->SetParticleMomentumDirection(ParticleMomentumDirection());
				particleGun->GeneratePrimaryVertex(anEvent);
			}
		}									
	}
		
	//12C(a,g) cascade with 7.1169MeV resonance
	if (cascadeOption == 6) {
		if (anEvent->GetEventID() == 0) G4cout << "Cascade option 6: 12C(a,g) 7116.9keV cascade chosen" << G4endl;
		G4double levelE = 7.1169*MeV;
		particleGun->SetParticleMomentumDirection(ParticleMomentumDirection(0));
		particleGun->SetParticleEnergy(particleEnergy-levelE);
		//create vertex
		particleGun->GeneratePrimaryVertex(anEvent);
		
		//second gamma
		particleGun->SetParticleMomentumDirection(ParticleMomentumDirection(0));
		particleGun->SetParticleEnergy(levelE);
		//create vertex
		particleGun->GeneratePrimaryVertex(anEvent);
	}
	//12C(a,g) cascade with 6.9171MeV resonance	
	if (cascadeOption == 7) {
		if (anEvent->GetEventID() == 0) G4cout << "Cascade option 7: 12C(a,g) 6917.1keV cascade chosen" << G4endl;
		G4double levelE = 6.9171*MeV;
		particleGun->SetParticleMomentumDirection(ParticleMomentumDirection(0));
		particleGun->SetParticleEnergy(particleEnergy-levelE);
		//create vertex
		particleGun->GeneratePrimaryVertex(anEvent);
		
		//second gamma
		particleGun->SetParticleMomentumDirection(ParticleMomentumDirection(0));
		particleGun->SetParticleEnergy(levelE);
		//create vertex
		particleGun->GeneratePrimaryVertex(anEvent);		
	}		
}
void myPrimaryGeneratorAction::SetEnergy(G4double energy) {
	particleEnergy = energy;
	//particleGun->SetParticleEnergy(particleEnergy);
	G4cout << "\n New particle-energy "<< G4BestUnit(energy, "Energy") << " is saved!" << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void myPrimaryGeneratorAction::DefineCommands()
{
//  Define /myDetector/event/ command directory using generic messenger class
	fMessenger = new G4GenericMessenger(this, "/myDetector/gun/", "PrimaryGenerator control");

//  print modulo command
	auto& printCmd = fMessenger->DeclareProperty("printModulo", printModulo, "Print primary modulo n.");
	printCmd.SetParameterName("PrimaryNb",false);
	printCmd.SetRange("PrimaryNb>0");
	printCmd.SetDefaultValue("100.");
	
//  primary energy command
	auto& energyCmd = fMessenger->DeclareMethodWithUnit("energy", "GeV", &myPrimaryGeneratorAction::SetEnergy, "Sets energy of the incident particle.");
	energyCmd.SetParameterName("energy",false);
	energyCmd.SetRange("energy>=0.0");
	energyCmd.SetDefaultValue("30.");
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
