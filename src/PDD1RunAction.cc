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

#include "PDD1RunAction.hh"
#include "PDD1Analysis.hh"
#include "PDD1DetectorConstruction.hh"
#include "PDD1Run.hh"

#include "G4RunManager.hh"
#include "G4Run.hh"
#include "G4AccumulableManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include <fstream>

PDD1RunAction::PDD1RunAction()
: G4UserRunAction(),
  fNx(0),
  fNy(0),
  fNz(0)
{
	// - Prepare data member for PDD1Run.
	//   vector represents a list of MultiFunctionalDetector names.
	fSDName.push_back(G4String("PhantomSD"));

	// add new units for dose
	//
	const G4double milligray = 1.e-3*gray;
	const G4double microgray = 1.e-6*gray;
	const G4double nanogray  = 1.e-9*gray;
	const G4double picogray  = 1.e-12*gray;

	new G4UnitDefinition("milligray", "milliGy" , "Dose", milligray);
	new G4UnitDefinition("microgray", "microGy" , "Dose", microgray);
	new G4UnitDefinition("nanogray" , "nanoGy"  , "Dose", nanogray);
	new G4UnitDefinition("picogray" , "picoGy"  , "Dose", picogray);

	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
	analysisManager->SetActivation(true);
	G4cout << "Using " << analysisManager->GetType() << G4endl;
	analysisManager->SetVerboseLevel(0);
	analysisManager->SetFileName("data/PDD1");

	// Create Histograms an n-Tuples
	CreateNTuples();
	CreateHistos();

}

// Destructor.
PDD1RunAction::~PDD1RunAction()
{
	fSDName.clear();
	delete G4AccumulableManager::Instance();
	delete G4AnalysisManager::Instance();
}

//== 
G4Run* PDD1RunAction::GenerateRun()
{
	// Generate new RUN object, which is specially
	// dedicated for MultiFunctionalDetector scheme.
	//  Detail description can be found in PDD1Run.hh/cc.
	return new PDD1Run(fSDName);
}

//==
void PDD1RunAction::BeginOfRunAction(const G4Run* /*aRun*/)
{

	// reset accumulables to their initial values
	G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
	accumulableManager->Reset();

	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();


	if(analysisManager->IsActive()){

		// Open an output file
		analysisManager->OpenFile();
	}

	// Instances for Total LET
	totalLetD =      new G4double[500];
	DtotalLetD =     new G4double[500];
	totalLetT =      new G4double[500];
	DtotalLetT =     new G4double[500];

}

//== 
void PDD1RunAction::EndOfRunAction(const G4Run* aRun)
{

	// Merge accumulables
	G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
	accumulableManager->Merge();

	G4int nofEvents = aRun->GetNumberOfEvent();
	if (nofEvents == 0) return;

	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();


	G4int nBins = analysisManager->GetH1(2)->get_bins();
	G4double DE, DX;

	for(G4int i=0; i<nBins; i++){
		DE = analysisManager->GetH1(0)->bin_Sw(i);
		DX = analysisManager->GetH1(1)->bin_Sw(i);
		if(DX>0.) analysisManager->GetH1(2)->set_bin_content(i, 1,DE/DX,0.0,0.0,0.0);
	}


	if(analysisManager->IsActive()){
		// Save histograms and ntuples
		analysisManager->Write();
		analysisManager->CloseFile();
	}

	if(!IsMaster()) return;

	//- PDD1Run object.
	PDD1Run* run = (PDD1Run*) aRun;
	//--- Dump all socred quantities involved in PDD1Run.
	run->DumpAllScorer();
	//---

	//
	//- water phantom (Detector) Information.
	//-- Number of segments in the water phantom.
	const PDD1DetectorConstruction* detector =
			(const PDD1DetectorConstruction*)
			(G4RunManager::GetRunManager()->GetUserDetectorConstruction());
	detector->GetNumberOfSegmentsInPhantom(fNx,fNy,fNz); //Fill fNx,y,z.

	//---------------------------------------------
	// Dump accumulated quantities for this RUN.
	//  (Display only central region of x-y plane)
	//---------------------------------------------
	G4THitsMap<G4double>* totalEdep  = run->GetHitsMap("PhantomSD/totalEDep");
	G4THitsMap<G4double>* protonEdep  = run->GetHitsMap("PhantomSD/protonEDep");


	G4cout << "============================================================="
			<< G4endl;
	G4cout << " Number of event processed : "<< aRun->GetNumberOfEvent() << G4endl;
	G4cout << "============================================================="<< G4endl;
	G4cout << std::setw( 8) << "#Z Cell#";
	G4cout << std::setw(16) << totalEdep->GetName();
	G4cout << std::setw(16) << protonEdep->GetName();
	G4cout << G4endl;

	G4int ix = fNx/2;
	G4int iy = fNy/2;
	G4int iz;
	//G4double totE, proE, proN,pasCF,CF,surfF,gCr0,gCr1,gCr2,gCr3;
	for ( iz = 0; iz < fNz; iz++){
		G4double* totalED = (*totalEdep)[CopyNo(ix,iy,iz)];
		G4double* protonED = (*protonEdep)[CopyNo(ix,iy,iz)];

		if ( !totalED ) totalED = new G4double(0.0);
		if ( !protonED ) protonED = new G4double(0.0);

		G4cout << std::setw( 6) << iz << "  "
				<< std::setw(12) << G4BestUnit(*totalED/nofEvents,"Energy")
				<< std::setw(12) << G4BestUnit(*protonED/nofEvents,"Energy")
				<< G4endl;
	}
	G4cout << "============================================="<<G4endl;

	std::ofstream  file("totED.out");
	for ( iz = 0; iz < fNz; iz++){
		for ( iy = 0; iy < fNy; iy++){
			for ( ix = 0; ix < fNx; ix++){
				G4double* totalED = (*totalEdep)[CopyNo(ix,iy,iz)];
				G4double* protonED = (*protonEdep)[CopyNo(ix,iy,iz)];

				if ( !totalED ) totalED = new G4double(0.0);
				if ( !protonED ) protonED = new G4double(0.0);

				file << ix << " "<<iy<<" "<<iz<<" "<< *totalED/MeV/nofEvents<<" "<< *protonED/MeV/nofEvents  << G4endl;

			}
		}
	}
	file.close();

}

void PDD1RunAction::CreateNTuples(){

	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
	analysisManager->SetNtupleMerging(true);

	// Creating ntuples
	analysisManager->SetFirstNtupleId(0);

	// id = 0
	analysisManager->CreateNtuple("SecondariesProduction", "Variables related to secondaries production");
	analysisManager->SetNtupleActivation(0,true);
	analysisManager->CreateNtupleDColumn(0,"KineticEnergyAtVertex");
	analysisManager->CreateNtupleSColumn(0,"CreatorProcessName");
	analysisManager->CreateNtupleSColumn(0,"CreatorModelName");
	analysisManager->CreateNtupleSColumn(0,"ParticleName");
	analysisManager->CreateNtupleDColumn(0,"TrackLength");
	analysisManager->CreateNtupleDColumn(0,"TotalEnergyDeposit");
	analysisManager->FinishNtuple();

}

void PDD1RunAction::CreateHistos(){

	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

	// Creating histos
	analysisManager->SetFirstHistoId(0);

	// id = 0
	analysisManager->CreateH1("eDep","Energy deposited per event", 500, 0, 500*mm,"mm");
	analysisManager->SetH1Activation(0,true);
	analysisManager->SetH1XAxisTitle(0,"Phantom depth [mm]");
	analysisManager->SetH1YAxisTitle(0,"Deposited Energy [keV]");

	// id = 1
	analysisManager->CreateH1("StepLength","Step length per event", 500, 0, 500*mm,"mm");
	analysisManager->SetH1Activation(1,true);
	analysisManager->SetH1XAxisTitle(1,"Phantom depth [mm]");
	analysisManager->SetH1YAxisTitle(1,"Step length [um]");

	// id = 2
	analysisManager->CreateH1("LET","Linear energy transfer", 500, 0, 500*mm,"mm");
	analysisManager->SetH1Activation(2,true);
	analysisManager->SetH1XAxisTitle(2,"Phantom depth [mm]");
	analysisManager->SetH1YAxisTitle(2,"LET [keV/um]");

	// id = 3
	analysisManager->CreateH1("Range","Range", 500, 0, 500*mm,"mm");
	analysisManager->SetH1Activation(3,true);
	analysisManager->SetH1XAxisTitle(3,"Range [mm]");
	analysisManager->SetH1YAxisTitle(3,"[Counts]");

	// id = 4
	analysisManager->CreateH1("KineticEnergyAt50","Kinetic energy at 50% of dose", 3000, 0, 300*MeV,"MeV");
	analysisManager->SetH1Activation(4,true);
	analysisManager->SetH1XAxisTitle(4,"Kinetic energy at 50% of dose [MeV]");
	analysisManager->SetH1YAxisTitle(4,"[Counts]");

	// id = 5
	analysisManager->CreateH1("KineticEnergyAt70","Kinetic energy at 70% of dose", 3000, 0, 300*MeV,"MeV");
	analysisManager->SetH1Activation(5,true);
	analysisManager->SetH1XAxisTitle(5,"Kinetic energy at 70% of dose [MeV]");
	analysisManager->SetH1YAxisTitle(5,"[Counts]");

	// id = 6
	analysisManager->CreateH1("KineticEnergyAt90","Kinetic energy at 90% of dose", 3000, 0, 300*MeV,"MeV");
	analysisManager->SetH1Activation(6,true);
	analysisManager->SetH1XAxisTitle(6,"Kinetic energy at 90% of dose [MeV]");
	analysisManager->SetH1YAxisTitle(6,"[Counts]");

	// id = 7
	analysisManager->CreateH1("KineticEnergyAtVertex","Kinetic energy at vertex", 3000, 0, 300*MeV,"MeV","none","linear");
	//analysisManager->CreateH1("KineticEnergyAtVertex","Kinetic energy at vertex", 12, 1E-9*MeV, 1E+2*MeV,"MeV","none","log");
	analysisManager->SetH1Activation(7,true);
	analysisManager->SetH1XAxisTitle(7,"Energy [MeV]");
	analysisManager->SetH1YAxisTitle(7,"Relative Number of particles");

}
