/*
 * Neutron Capture 1.0
 * Copyright (c) 2020
 * Universidad Nacional de Colombia
 * Servicio Geológico Colombiano
 * All Right Reserved.
 *
 * Developed by Andrea Giraldo Torres
 *              Andrés Camilo Sevilla Moreno
 *
 * Use and copying of these libraries and preparation of derivative works
 * based upon these libraries are permitted. Any copy of these libraries
 * must include this copyright notice.
 *
 * Bogotá, Colombia.
 *
 */

// Geant4 Headers
#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#else
#include "G4RunManager.hh"
#endif
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "Randomize.hh"
#include "PhysicsList.hh"
#include "G4PhysListFactory.hh"
#include "G4VModularPhysicsList.hh"
#include "G4ParallelWorldPhysics.hh"
#include "QGSP_BIC_HP.hh"
#include "G4ScoringManager.hh"
#include "G4SystemOfUnits.hh"
#include "LET.hh"

// D1 Headers
#include "D1DetectorConstruction.hh"
#include "include/D1ActionInitialization.hh"

namespace {
void PrintUsage() {
	G4cerr << " Usage: " << G4endl;
	G4cerr << " ./neutron_capture [-m macro ] "
			<< " [-v visualization {'on','off'}]"
			<< " [-vm vis_macro ]"
			<< " [-n numberOfEvent ]"
			<< "\n or\n ./neutron_capture [macro.mac]"
			<< G4endl;
}
}


int main(int argc,char** argv)
{
	// Detect interactive mode (if no arguments) and define UI session
	//
	G4UIExecutive* ui = 0;

	// Evaluate arguments
	//
	if ( argc > 9 ) {
		PrintUsage();
		return 1;
	}

	G4String macro("");
	G4String vis_macro("");
	G4String onOffVisualization("");
	G4int numberOfEvent(0);

	if (argc == 1) {
		ui = new G4UIExecutive(argc, argv);
		onOffVisualization="on";
	}else{
		for ( G4int i=1; i<argc; i=i+2 )
		{
			if ( G4String(argv[i]) == "-m" ) macro = argv[i+1];
			else if (G4String(argv[i]) == "-v" ) {
				onOffVisualization=argv[i+1];
				if(onOffVisualization=="on"){
					ui = new G4UIExecutive(argc, argv);
				}
			}
			else if ( G4String(argv[i]) == "-vm" ) {
				if(!ui) ui = new G4UIExecutive(argc, argv);
				if(G4String(argv[i]) == "-vm") vis_macro = argv[i+1];
				onOffVisualization="on";
			}
			else if ( G4String(argv[i]) == "-n" ) numberOfEvent = G4UIcommand::ConvertToInt(argv[i+1]);
			else{
				PrintUsage();
				return 1;
			}
		}
	}

	if(macro == "") macro = "mac/init.mac";
	if(vis_macro == "") vis_macro = "mac/vis1.mac";
	if(onOffVisualization == "") onOffVisualization = "off";

	// Choose the Random engine
	//
	CLHEP::RanluxEngine defaultEngine( 1234567, 4 );
	G4Random::setTheEngine( &defaultEngine );
	G4int seed = time( NULL );
	G4Random::setTheSeed( seed );


	// Construct the default run manager
	//
#ifdef G4MULTITHREADED
	G4MTRunManager* runManager = new G4MTRunManager;
	runManager->SetNumberOfThreads(G4Threading::G4GetNumberOfCores());
#else
	G4RunManager* runManager = new G4RunManager;
#endif

	// Activate UI-command base scorer
	G4ScoringManager * scoringManager = G4ScoringManager::GetScoringManager();
	scoringManager->SetVerboseLevel(1);

	// Set mandatory initialization classes
	//
	// Detector construction
	runManager->SetUserInitialization(new D1DetectorConstruction());

	// Physics list
	//runManager->SetUserInitialization(new QGSP_BIC_HP());
	runManager->SetUserInitialization(new PhysicsList());

	// User action initialization
	//runManager->SetUserInitialization(new D1ActionInitialization());
	runManager->SetUserInitialization(new D1ActionInitialization());

	// Initialize visualization
	G4VisManager* visManager = new G4VisExecutive;
	visManager->Initialize();

	// Get the pointer to the User Interface manager
	G4UImanager* UImanager = G4UImanager::GetUIpointer();

	// Setting execute command
	G4String command = "/control/execute ";

	// Process macro or start UI session
	UImanager->ApplyCommand(command+macro);

	if (onOffVisualization=="on"){
		// interactive mode
		UImanager->ApplyCommand(command+vis_macro);
		ui->SessionStart();
		delete ui;
	}else{
		if ( numberOfEvent >= 0 ) runManager->BeamOn(numberOfEvent);
	}


     if (LET *let = LET::GetInstance())

        if(let -> doCalculation)
        {
            let -> LetOutput(); 	// Calculate let
            let -> StoreLetAscii(); // Store it
        }


	// Job termination
	// Free the store: user actions, physics_list and detector_description are
	// owned and deleted by the run manager, so they should not be deleted
	// in the main() program !

	delete visManager;
	delete runManager;
}
