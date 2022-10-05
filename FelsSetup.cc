/*
 * 1802setupFels.cc
 *
 *  Created on: Feb 05, 2018
 *      Author: wagner10
 */
// main Geant4 c++-File

#include "myDetectorConstruction.hh"
#include "myPrimaryGeneratorAction.hh"
#include "myRunAction.hh"
#include "myEventAction.hh"
#include "mySteppingAction.hh"
#include "myHistoManager.hh"
#include "TROOT.h"

#include "Randomize.hh"

#include "G4PhysListFactory.hh"
#include "FTFP_BERT.hh"
#include "ShieldingLEND.hh"

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIcommand.hh"
#include "G4ios.hh"
#include "G4SystemOfUnits.hh"

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif


#ifdef G4UI_USE
#include "G4UIExecutive.hh"
#endif

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int main(int argc,char** argv)  // o start efficiency sim: ./setupFelsHe3ag [mac/run1.mac] [opt. -e energy/MeV] [opt. -v verboselevel >=0] [opt. -r output filename] [opt. -j jobnumber]
{
  gROOT->GetInterpreter();
	G4String macro;
	G4bool useMacro = false;
	//~ G4String nPrecison = "LEND";
	G4int verbose = 0;
	G4double energy = 9.8419*MeV;
	G4String rootfilename = "test.root";
	//~ G4ThreeVector offsetVec = G4ThreeVector(0,0,0);
	G4int job = 0;
	//~ const G4int crystals = 3;
	//~ const G4int clusters = 2;
	G4double dCluster1 = 0.*mm, dCluster2 = 0.*mm, tiltCluster2 = 0.*mm;
	
	if (argc>1) 
	{
		macro = argv[1];
		useMacro  = true;
		for (G4int i=1; i<argc;  i++) 
		{
			// set verbose level
			if(strstr(argv[i], "-v")) verbose = G4UIcommand::ConvertToInt(argv[i+1]);
			// set netron precision level
			//~ if(strstr(argv[i], "-n")) nPrecison = argv[i+1];
			// set primary energy
			if(strstr(argv[i], "-e")) energy = G4UIcommand::ConvertToDouble(argv[i+1]) * keV;
			// set root outputfile
			if(strstr(argv[i], "-r")) rootfilename = argv[i+1];
			if(strstr(argv[i], "-d1")) dCluster1 = G4UIcommand::ConvertToDouble(argv[i+1]) * mm;
			if(strstr(argv[i], "-t1")) dCluster2 = G4UIcommand::ConvertToDouble(argv[i+1]) * deg;
			if(strstr(argv[i], "-t2")) tiltCluster2 = G4UIcommand::ConvertToDouble(argv[i+1]) * mm;
			// set job number
			if(strstr(argv[i], "-j")) job = G4UIcommand::ConvertToInt(argv[i+1]);
			//help
			if(strstr(argv[i], "-h") || strstr(argv[i], "-help") || strstr(argv[i], "?")) {
				G4cout << "to start efficiency sim: ./setupFelsHe3ag [mac/run1.mac] [opt. -e energy/keV]";
                G4cout << "[opt. -v verboselevel >=0] [opt. -r rootfilename] [opt. -j jobnr]" << G4endl;
                return 0;
			}
		}
	}
 	//dCluster2 = 30.*deg;
 	//tiltCluster2 = 30.*deg;
 	G4cout << "radius of target holder = " << (47./2.*mm-dCluster1)/mm << " mm" << G4endl;
 	G4cout << "shift EB17&EB18 = " << tiltCluster2/mm << " mm" << G4endl;
 	G4cout << "tilt target chamber = " << dCluster2/deg << " deg" <<  G4endl;
	G4cout << "Energy = " << energy << G4endl;
 	G4cout << rootfilename << G4endl;
 	G4cout << "Job number:" << job << G4endl;
	// Run manager
	//
	G4RunManager * runManager = new G4RunManager;
		
	//choose the Random engine
	CLHEP::HepRandom::setTheEngine(new CLHEP::Ranlux64Engine);
	CLHEP::HepRandomEngine* myRandomEngine = CLHEP::HepRandom::getTheEngine();
	// get random engine
	long seed;

	if (job<1)
	{
		
		// automatic (time-based) random seeds for each run
	    G4cout << "*** AUTOSEED ON ***" << G4endl;
		time_t systime = std::time(NULL);
		seed = (long) systime;
	} else 
	{
		// Job based seeding
	    G4cout << "*** JOBSEED ON ***" << G4endl;
	    G4cout << "---> Seed reproduced by: jobnr" << G4endl;
		seed = job;
	}
	
	myRandomEngine->setSeed(seed, 1);
	myRandomEngine->showStatus();	

	// User Initialization classes (mandatory)
	//
	myDetectorConstruction* detector = new myDetectorConstruction(dCluster1, dCluster2, tiltCluster2);
	runManager->SetUserInitialization(detector);

	G4int physListVerbose = verbose;
	G4String physicsListName = "Shielding_EMZ";
	G4PhysListFactory* factory = new G4PhysListFactory();
	G4VUserPhysicsList* physlist = factory->GetReferencePhysList(physicsListName);
	//physlist->SetDefaultCutValue(100.*um);
	//physlist->SetCuts();
	physlist->SetVerboseLevel(physListVerbose);					
	runManager->SetUserInitialization(physlist);
	
	// set an HistoManager
	//
	myHistoManager*  histo = new myHistoManager(detector);
	//~ runManager->SetUserInitialization(new FTFP_BERT); //just for testing
	// User Action classes
	//
	myPrimaryGeneratorAction* gen_action = new myPrimaryGeneratorAction(detector, energy);
	runManager->SetUserAction(gen_action);
	
	myRunAction* run_action = new myRunAction(histo, gen_action, rootfilename, detector);
	runManager->SetUserAction(run_action);
	 
	myEventAction* event_action = new myEventAction(run_action, histo, detector, gen_action);
	runManager->SetUserAction(event_action);
	
	//G4cerr << "test main\n";
	mySteppingAction* stepping_action = new mySteppingAction(detector, event_action);
	runManager->SetUserAction(stepping_action);

	// Initialize G4 kernel
	//
	runManager->Initialize();
	
	#ifdef G4VIS_USE
	G4VisManager* visManager = new G4VisExecutive;
	visManager->Initialize();
	#endif
	
	// Get the pointer to the User Interface manager
	//
	G4UImanager * UImanager = G4UImanager::GetUIpointer();
	UImanager->ApplyCommand("/control/execute mac/run_init.mac");
	//UImanager->ApplyCommand("/gps/pos/centre 100 100 100 mm ");
	
	if (useMacro)   // batch mode
    {
		G4String command = "/control/execute ";
		UImanager->ApplyCommand(command+macro);

    }
	else           // interactive mode : define UI session
    {
		#ifdef G4UI_USE
			G4UIExecutive * ui = new G4UIExecutive(argc,argv);
			#ifdef G4VIS_USE
				UImanager->ApplyCommand("/control/execute mac/vis.mac");
			#endif
			ui->SessionStart();
			delete ui;
		#endif
		
		#ifdef G4VIS_USE
			delete visManager;
		#endif
	}

  // Free the store: user actions, physics_list and detector_description are
  //                 owned and deleted by the run manager, so they should not
  //                 be deleted in the main() program !

  delete runManager;
  delete histo;
  delete myRandomEngine;
  return EXIT_SUCCESS;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


