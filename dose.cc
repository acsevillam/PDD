/*
 * PDD 1.0
 * Copyright (c) 2020
 * Universidad Nacional de Colombia
 * Servicio Geológico Colombiano
 * All Right Reserved.
 *
 * Developed by Andrés Camilo Sevilla Moreno
 *
 * Use and copying of these libraries and preparation of derivative works
 * based upon these libraries are permitted. Any copy of these libraries
 * must include this copyright notice.
 *
 * Bogotá, Colombia.
 *
 */

// PPD1 Headers
#include "PDD1DetectorConstruction.hh"
#include "PDD1ActionInitialization.hh"
#include "DetectorMatrix.hh"

// Geant4 Headers
#include "G4RunManagerFactory.hh"
#include "G4UImanager.hh"
#include "QBBC.hh"
#include "PhysicsList.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "Randomize.hh"
#include "G4ScoringManager.hh"

namespace {
void PrintUsage() {
	G4cerr << " Usage: " << G4endl;
	G4cerr << " ./dose [-m macro ] "
			<< " [-vm vis_macro ]"
			<< " [-ui user interface {'on','off'}]"
			<< " [-n numberOfEvent ]"
			<< "\n or\n ./dose [macro.mac]"
			<< G4endl;
}
}

int main(int argc,char** argv)
{
	// Detect interactive mode (if no arguments) and define UI session
	//
	G4UIExecutive* ui = 0;

	// Evaluate arguments
	if ( argc > 9 ) {
		PrintUsage();
		return 1;
	}

	G4String macro("");
	G4String vis_macro("");
	G4String onOffUI("");
	G4int numberOfEvent(0);

	if (argc == 1) {
		ui = new G4UIExecutive(argc, argv);
		onOffUI="on";
	}else{
		for ( G4int i=1; i<argc; i=i+2 )
		{
			if ( G4String(argv[i]) == "-m" ) macro = argv[i+1];
			else if (G4String(argv[i]) == "-ui" ){
				onOffUI=argv[i+1];
				if(onOffUI=="on"){
					ui = new G4UIExecutive(argc, argv);
				}
			}
			else if ( G4String(argv[i]) == "-vm" ) {
				if(G4String(argv[i]) == "-vm") vis_macro = argv[i+1];
				onOffUI="on";
				if(!ui) ui = new G4UIExecutive(argc, argv);
			}
			else if ( G4String(argv[i]) == "-n" ) numberOfEvent = G4UIcommand::ConvertToInt(argv[i+1]);
			else{
				PrintUsage();
				return 1;
			}
		}
	}

	if(macro == "") macro = "mac/init.mac";
	if(onOffUI == "") onOffUI = "off";
	if(vis_macro == "") vis_macro = "mac/vis1.mac";

	// Choose the Random engine
	//
	CLHEP::RanluxEngine defaultEngine( 1234567, 4 );
	G4Random::setTheEngine( &defaultEngine );
	G4int seed = time( NULL );
	G4Random::setTheSeed( seed );

	// Construct the default run manager
	//
	auto* runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::Default);

	// Activate UI-command base scorer
	G4ScoringManager * scoringManager = G4ScoringManager::GetScoringManager();
	scoringManager->SetVerboseLevel(1);

	// Set mandatory initialization classes
	//
	// Detector construction
	runManager->SetUserInitialization(new PDD1DetectorConstruction());

	// Physics list
	//G4VModularPhysicsList* physicsList = new QBBC;
	G4VModularPhysicsList* physicsList = new PhysicsList();
	physicsList->SetVerboseLevel(1);
	runManager->SetUserInitialization(physicsList);

	// User action initialization
	runManager->SetUserInitialization(new PDD1ActionInitialization());

	// Initialize visualization
	//
	G4VisManager* visManager = new G4VisExecutive;
	// G4VisExecutive can take a verbosity argument - see /vis/verbose guidance.
	// G4VisManager* visManager = new G4VisExecutive("Quiet");
	visManager->Initialize();

	// Get the pointer to the User Interface manager
	G4UImanager* UImanager = G4UImanager::GetUIpointer();



	// Process macro or start UI session
	//
	if ( ! ui ) {
		// batch mode
		G4String command = "/control/execute ";
		UImanager->ApplyCommand(command+macro);
		if(numberOfEvent>=0) runManager->BeamOn(numberOfEvent);
	}
	else {
		// interactive mode
		G4String command = "/control/execute ";
		UImanager->ApplyCommand(command+macro);
		UImanager->ApplyCommand(command+vis_macro);
		ui->SessionStart();
		delete ui;
	}

	if (DetectorMatrix* matrix = DetectorMatrix::GetInstance())
	{
		matrix -> StoreEDepAscii();
		matrix -> StoreLetAscii();
		matrix -> StoreFluenceAscii();
	}

	// Job termination
	// Free the store: user actions, physics_list and detector_description are
	// owned and deleted by the run manager, so they should not be deleted
	// in the main() program !

	delete visManager;
	delete runManager;
}
