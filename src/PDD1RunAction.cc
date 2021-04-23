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

// PDD1 Headers
#include "PDD1RunAction.hh"
#include "PDD1PrimaryGeneratorAction.hh"
#include "PDD1DetectorConstruction.hh"
#include "DetectorSD.hh"
#include "Analysis.hh"

// Geant4 Headers
#include "G4RunManager.hh"
#include "G4Run.hh"
#include "G4AccumulableManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4SDManager.hh"

using namespace std;

PDD1RunAction::PDD1RunAction()
: G4UserRunAction(),
  fEdep(0.),
  fEdep2(0.)
{ 

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

	// Register accumulable to the accumulable manager
	G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
	accumulableManager->RegisterAccumulable(fEdep);
	accumulableManager->RegisterAccumulable(fEdep2);

	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
	analysisManager->SetActivation(true);

}

PDD1RunAction::~PDD1RunAction()
{
	delete G4AccumulableManager::Instance();
	delete G4AnalysisManager::Instance();
}

void PDD1RunAction::BeginOfRunAction(const G4Run*)
{ 
	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
	if(analysisManager->GetActivation()){
		G4cout << "Using " << analysisManager->GetType() << G4endl;
		analysisManager->SetVerboseLevel(0);
		analysisManager->SetFileName("data/PDD1");

		// Create Histograms an n-Tuples
		CreateNTuples();
		CreateHistos();

		analysisManager->OpenFile();
	}
	// inform the runManager to save random number seed
	G4RunManager::GetRunManager()->SetRandomNumberStore(false);

	// reset accumulables to their initial values
	G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
	accumulableManager->Reset();

}

void PDD1RunAction::EndOfRunAction(const G4Run* aRun)
{
	G4int nofEvents = aRun->GetNumberOfEvent();
	if (nofEvents == 0) return;

	// Save histograms and ntuples
	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
	if(analysisManager->GetActivation()){
		analysisManager->Write();
		analysisManager->CloseFile();
	}

	// Merge accumulables
	G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
	accumulableManager->Merge();

	// Compute dose = total energy deposit in a run and its variance
	//
	G4double edep  = fEdep.GetValue();
	G4double edep2 = fEdep2.GetValue();

	G4double rms = edep2 - edep*edep/nofEvents;
	if (rms > 0.) rms = std::sqrt(rms); else rms = 0.;

	const PDD1DetectorConstruction* detectorConstruction
	= static_cast<const PDD1DetectorConstruction*>
	(G4RunManager::GetRunManager()->GetUserDetectorConstruction());


	G4double mass = 0.;
	vector<G4LogicalVolume*> scoringVolumeVector = detectorConstruction->GetScoringVolumeVector();

	for (unsigned i=0; i < scoringVolumeVector.size(); i++) {
		mass+=scoringVolumeVector[i]->GetMass();
	}

	G4double dose = edep/mass;
	G4double rmsDose = rms/mass;

	if(!IsMaster()) return;

	G4cout<<"Total dose: "<<G4BestUnit(dose,"Dose")<<"\t"<<"rms: "<<G4BestUnit(rmsDose,"Dose")<<G4endl;

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
	analysisManager->CreateH1("Range","Range", 500, 0, 500*mm,"mm");
	analysisManager->SetH1Activation(0,true);
	analysisManager->SetH1XAxisTitle(0,"Range [mm]");
	analysisManager->SetH1YAxisTitle(0,"[Counts]");

	// id = 1
	analysisManager->CreateH1("KineticEnergyAt50","Kinetic energy at 50% of dose", 3000, 0, 300*MeV,"MeV");
	analysisManager->SetH1Activation(1,true);
	analysisManager->SetH1XAxisTitle(1,"Kinetic energy at 50% of dose [MeV]");
	analysisManager->SetH1YAxisTitle(1,"[Counts]");

	// id = 2
	analysisManager->CreateH1("KineticEnergyAt70","Kinetic energy at 70% of dose", 3000, 0, 300*MeV,"MeV");
	analysisManager->SetH1Activation(2,true);
	analysisManager->SetH1XAxisTitle(2,"Kinetic energy at 70% of dose [MeV]");
	analysisManager->SetH1YAxisTitle(2,"[Counts]");

	// id = 3
	analysisManager->CreateH1("KineticEnergyAt90","Kinetic energy at 90% of dose", 3000, 0, 300*MeV,"MeV");
	analysisManager->SetH1Activation(3,true);
	analysisManager->SetH1XAxisTitle(3,"Kinetic energy at 90% of dose [MeV]");
	analysisManager->SetH1YAxisTitle(3,"[Counts]");

	// id = 4
	analysisManager->CreateH1("KineticEnergyAtVertex","Kinetic energy at vertex", 3000, 0, 300*MeV,"MeV","none","linear");
	//analysisManager->CreateH1("KineticEnergyAtVertex","Kinetic energy at vertex", 12, 1E-9*MeV, 1E+2*MeV,"MeV","none","log");
	analysisManager->SetH1Activation(4,true);
	analysisManager->SetH1XAxisTitle(4,"Energy [MeV]");
	analysisManager->SetH1YAxisTitle(4,"Relative Number of particles");

}
