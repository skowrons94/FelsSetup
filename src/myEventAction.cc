// additional UserEventAction Class
 
#include "myEventAction.hh"
#include "myRunAction.hh"
#include "myHistoManager.hh"
#include "myECalHit.hh"
#include "myDetectorConstruction.hh"
#include "myPrimaryGeneratorAction.hh"

#include "G4GenericMessenger.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4SDManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "G4ios.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "Randomize.hh" 


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
myEventAction::myEventAction(myRunAction* run, myHistoManager* histo, myDetectorConstruction* det, myPrimaryGeneratorAction* prim)
:runAct(run), 
 histoMan(histo),
 detConst(det), 
 primGen(prim),
 BGOThreshold(10*keV), 
 printModulo(100)

{
    for (G4int k = 0; k < maxNbOfClusters; k++) { 
		fClusterHPGeHCID[k] = -1;
		fClusterBGOHCID[k] = -1;
		nameCluster[k] = detConst->nameCluster[k];
	}
 	//Messenger commands
	DefineCommands();
	G4cout << "--> Event Action initialized" << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
myEventAction::~myEventAction()
{
	delete fMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
void myEventAction::BeginOfEventAction(const G4Event* evt)
{
	NbOfClusters = detConst->NbOfClusters;

	G4int event_id = evt->GetEventID();
	if (event_id%printModulo == 0) { 
		G4cout << "\n---> Begin of event: " << event_id << G4endl;
		// CLHEP::HepRandom::showEngineStatus();
	}
	
	// initialisation per event
	for(G4int k = 0; k < maxDets; k++) {
		for(G4int i = 0; i < maxDets; i++) {
			energyClusterHPGe[k][i] = 0;
			energyBGO[k] = 0;
		}
	}
	//~ energyHPGe90 = 0;
	//~ energyHPGe90BGO = 0;
	trackKillsPerEvt = 0;
	
	G4SDManager * SDman = G4SDManager::GetSDMpointer();
	for(G4int k = 0; k < NbOfClusters; k++) {
		ConstructBGO[k] = detConst->ConstructBGO[k];
		NbOfCrystals[k] = detConst->NbOfCrystals[k];	
		if(fClusterHPGeHCID[k]<0)
		{
			fClusterHPGeHCID[k] = SDman->GetCollectionID("TripleClusterHPGe_HitsCollection"+nameCluster[k]);
			G4cerr << "TripleClusterHPGe_HitsCollection"+nameCluster[k] << G4endl;
		}
		if(fClusterBGOHCID[k]<0 && ConstructBGO[k] == true)
		{
			fClusterBGOHCID[k] = SDman->GetCollectionID("TripleClusterBGO_HitsCollection"+nameCluster[k]);
		}			
	}
	angle = primGen->Theta;
	angle2 = primGen->Phi;
	energy = primGen->GetEnergy();
	dEnergy = 100*keV;
	//G4cout << angle/deg << " " << " " << energy/keV << G4endl;
 }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
void myEventAction::EndOfEventAction(const G4Event* evt)
{
	G4int event_id = evt->GetEventID();
	//~ if (event_id%printModulo == 0) G4cout	<< "---> End of event: " << event_id << G4endl;	
	
	myECalHitsCollection* clusterHPGeHC[maxNbOfClusters];
	G4int clusterHPGeEntries[maxNbOfClusters];
	
	myECalHitsCollection* clusterBGOHC[maxNbOfClusters];
	G4int clusterBGOEntries[maxNbOfClusters];
	myECalHit* clusterBGOHit[maxNbOfClusters];

	for(G4int k = 0; k < NbOfClusters; k++) {
		clusterHPGeHC[k] = GetHitsCollection(fClusterHPGeHCID[k], evt);
		clusterHPGeEntries[k] = clusterHPGeHC[k]->entries();		
		//G4cerr << "Entries: " << clusterHPGeEntries[k] << G4endl;
		if (ConstructBGO[k] == true) {
			clusterBGOHC[k] = GetHitsCollection(fClusterBGOHCID[k], evt);
			clusterBGOEntries[k] = clusterBGOHC[k]->entries();
			// Get hit with total value
			clusterBGOHit[k] = (*clusterBGOHC[k])[clusterBGOEntries[k]-1];
			energyBGO[k] = clusterBGOHit[k]->GetEdep();
			//G4cout << energyBGO[k] << G4endl;
		}
	}

 	//three parameters for energy resolution: sqrt(a+bx+cx^2)
 	//calculated by Max
	G4double energyResParA[maxNbOfClusters][maxNbOfCrystals]
	{
		{1.95, 0, 0, 0, 0, 0, 0}, //Can60 not checked
		{1.95, 1.52, 1.533, 0, 0, 0, 0}, //MB1 by Max
		{1.344, 0.721, 1.155, 0, 0, 0, 0}, //MB2 by Max
		{1.95, 1.52, 1.533, 1.95, 1.52, 1.533, 1.95}, //18 not checked
		{1.95, 1.52, 1.533, 1.95, 1.52, 1.533, 1.95} //17 not checked		
	};
	G4double energyResParB[maxNbOfClusters][maxNbOfCrystals]
	{
		{-8e-06, 0, 0, 0, 0, 0, 0}, //Can60 not checked
		{-8e-06, 0.00031, 0.00042, 0., 0., 0., 0.}, //MB1 by Max
		{0.00042, 0.00023, 0.00013, 0, 0, 0, 0}, //MB2 by Max
		{-8e-06, 0.00031, 0.00042, 0.00042, 0.00023, 0.00013, 0.00031}, //18 not checked
		{-8e-06, 0.00031, 0.00042, 0.00042, 0.00023, 0.00013, 0.00031} //17 not checked		
	};
	G4double energyResParC[maxNbOfClusters][maxNbOfCrystals]
	{
		{3.1e-07, 0, 0, 0, 0, 0, 0}, //Can60 not checked
		{3.1e-07, 1.0e-07, 3.e-08, 0., 0., 0., 0.}, //MB1 by Max
		{-4e-08, 2.1e-07, 1.8e-07, 0, 0, 0, 0}, //MB2 by Max
		{3.1e-07, 1.0e-07, 3.e-08, -4e-08, 2.1e-07, 1.8e-07, 3.1e-07}, //18 not checked		
		{3.1e-07, 1.0e-07, 3.e-08, -4e-08, 2.1e-07, 1.8e-07, 3.1e-07} //17 not checked
	};
	
	myECalHit* clusterHPGeHit[maxNbOfClusters];
	G4int histoStart = 0;
	// Get hit for single SD and sum
	for(G4int k = 0; k < NbOfClusters; k++) {
		G4double sumEnergy = 0; //sum up energy of crystals separately, because the sum of the SD would get a different energy resolution otherwise
		G4double sumMiddleCrystals = 0;
		for(G4int i = 0; i < clusterHPGeEntries[k]; i++)
		{
			clusterHPGeHit[k] = (*clusterHPGeHC[k])[i];
			energyClusterHPGe[k][i] = clusterHPGeHit[k]->GetEdep();
			if (i == clusterHPGeEntries[k]-1) 
				energyClusterHPGe[k][i] = sumEnergy; //energy in addback is true sum of single crystals with faked energy resolution
			else if(energyClusterHPGe[k][i] > 10 * keV)	 {
				//	fake energy resolution
				energyClusterHPGe[k][i] = CLHEP::RandGauss::shoot( energyClusterHPGe[k][i] / keV, std::sqrt(energyResParA[k][i] + energyResParB[k][i]*energyClusterHPGe[k][i]/keV + energyResParC[k][i]*std::pow(energyClusterHPGe[k][i]/keV,2)) )* keV;
				sumEnergy += energyClusterHPGe[k][i];
				//~ if (i == 2 || i == 5 || i == 6)
					//~ sumMiddleCrystals += energyClusterHPGe[k][i];
			}
			//print per event (modulo n)
			if (event_id%printModulo == 0) 
			{
				G4cout	<< "   ClusterHPGe " << i+1 <<": total energy: " << std::setw(7)	<< G4BestUnit(energyClusterHPGe[k][i],"Energy") << G4endl;

			}
			//fill histograms
			//if (k == 3 || k == 4) G4cout << k << " " <<  i << " " << (i+1)%6 << " " << (i-1)%6 << " " << energyClusterHPGe[k][i] << " " << G4endl;
			if (energyClusterHPGe[k][i] > 0.) 
			{
				histoMan->FillHisto(histoStart + i, energyClusterHPGe[k][i]);
				//G4cerr << "histo: " << k * 2 * (NbOfCrystals + 1 ) + i << G4endl;
				if(energyBGO[k] <= BGOThreshold && ConstructBGO[k] == true) histoMan->FillHisto(histoStart + i + NbOfCrystals[k] + 1, energyClusterHPGe[k][i]);
				if (k == 3 && energyBGO[k] <= BGOThreshold && i == 0 && energyClusterHPGe[k][5] == 0. && energyClusterHPGe[k][1] == 0. && energyClusterHPGe[k][6] == 0.) {
					histoMan->FillHisto(42+i, energyClusterHPGe[k][i]);
					//G4cout << " hit and nothing around" << G4endl;
				}
				if (k == 3 && energyBGO[k] <= BGOThreshold && (i > 0 && i < 6) && energyClusterHPGe[k][(i+1)%6] == 0. && energyClusterHPGe[k][(i-1)%6] == 0. && energyClusterHPGe[k][6] == 0.) {
					histoMan->FillHisto(42+i, energyClusterHPGe[k][i]);
					//G4cout << " hit and nothing around" << G4endl;
				}
				if (k == 3 && energyBGO[k] <= BGOThreshold && i == 6 && energyClusterHPGe[k][0] == 0. && energyClusterHPGe[k][1] == 0. && energyClusterHPGe[k][2] == 0.
					&& energyClusterHPGe[k][3] == 0. && energyClusterHPGe[k][4] == 0. && energyClusterHPGe[k][5] == 0.) {
					histoMan->FillHisto(42+i, energyClusterHPGe[k][i]);
					//G4cout << " hit and nothing around" << G4endl;
				}								
				if (k == 4 && i == 0 && energyClusterHPGe[k][5] == 0. && energyClusterHPGe[k][1] == 0. && energyClusterHPGe[k][6] == 0.) {
					histoMan->FillHisto(52+i, energyClusterHPGe[k][i]);
					//G4cout << " hit and nothing around" << G4endl;
				}
				if (k == 4 && (i > 0 && i < 6) && energyClusterHPGe[k][(i+1)%6] == 0. && energyClusterHPGe[k][(i-1)%6] == 0. && energyClusterHPGe[k][6] == 0.) {
					histoMan->FillHisto(52+i, energyClusterHPGe[k][i]);
					//G4cout << " hit and nothing around" << G4endl;
				}
				if (k == 4 && i == 6 && energyClusterHPGe[k][0] == 0. && energyClusterHPGe[k][1] == 0. && energyClusterHPGe[k][2] == 0.
					&& energyClusterHPGe[k][3] == 0. && energyClusterHPGe[k][4] == 0. && energyClusterHPGe[k][5] == 0.) {
					histoMan->FillHisto(52+i, energyClusterHPGe[k][i]);
					//G4cout << " hit and nothing around" << G4endl;
				}
			}
			//G4cout << angle/deg << " " << energy << " " << energyClusterHPGe[k][i] << " " << (energyClusterHPGe[k][i] > energy-dEnergy) << " " << (energyClusterHPGe[k][i] < energy+dEnergy) << G4endl;
			if ((energyClusterHPGe[k][i] > energy-dEnergy) && (energyClusterHPGe[k][i] < energy+dEnergy)) {
				histoMan->FillHistoAngle(histoStart + i, angle);
				histoMan->FillHistoAngle2D(histoStart + i, angle, angle2);
			}
			//if (energyClusterHPGe[k][i] > 0 || energyBGO[k] > 0) G4cout << k << " " << i << " " << energyClusterHPGe[k][i]/keV << " " << energyBGO[k]/keV << G4endl;
		}
		histoStart += NbOfCrystals[k] + 1 + ((NbOfCrystals[k] + 1) * ConstructBGO[k]);
		//~ histoMan->FillHisto(98, sumMiddleCrystals);
	}
	//if (event_id%printModulo == 0) G4cout	<< "   BGO: total energy: " << std::setw(7)	<< G4BestUnit(energyBGO,"Energy") << G4endl;
	
	if (trackKillsPerEvt > 0) {
		runAct->KillCounter();
		G4cout << trackKillsPerEvt << " track(s) killed after 9999 steps in event " << event_id << G4endl;
	}

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

myECalHitsCollection* myEventAction::GetHitsCollection(G4int hcID, const G4Event* event) const
{
	myECalHitsCollection* hitsCollection = static_cast<myECalHitsCollection*>(event->GetHCofThisEvent()->GetHC(hcID));
	
	if ( ! hitsCollection )
	{
		G4ExceptionDescription msg;
		msg << "Cannot access hitsCollection ID " << hcID; 
		G4Exception("myEventAction::GetHitsCollection()",
		  "MyCode0003", FatalException, msg);
	}         
	return hitsCollection;
}    

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void myEventAction::DefineCommands()
{
//  Define /myDetector/event/ command directory using generic messenger class
	fMessenger = new G4GenericMessenger(this, "/myDetector/event/", "EventAction control");

//  print modulo command
	auto& printCmd = fMessenger->DeclareProperty("printModulo", printModulo, "Print event modulo n.");
	printCmd.SetParameterName("EventNb",false);
	printCmd.SetRange("EventNb>0");
	printCmd.SetDefaultValue("100.");

}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
