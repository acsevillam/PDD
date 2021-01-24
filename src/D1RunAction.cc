//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
/// \file runAndEvent/D1/src/D1RunAction.cc
/// \brief Implementation of the D1RunAction class
//
//
// 
#include "D1RunAction.hh"
#include "D1Run.hh"

//-- In order to obtain detector information.
#include "G4RunManager.hh"
#include "G4Run.hh"
#include "G4AccumulableManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "D1Analysis.hh"
#include "D1DetectorConstruction.hh"

#include <fstream>

D1RunAction::D1RunAction()
: G4UserRunAction(),
	fNx(0),
	fNy(0),
	fNz(0),
	fRange(0.),
	fRange2(0.)
{
	// - Prepare data member for D1Run.
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

	G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
	// Register accumulable to the accumulable manager
	accumulableManager->RegisterAccumulable(fRange);
	accumulableManager->RegisterAccumulable(fRange2);

	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
	analysisManager->SetActivation(true);
	G4cout << "Using " << analysisManager->GetType() << G4endl;
	analysisManager->SetVerboseLevel(0);
	analysisManager->SetFileName("data/D1");

	// Create Histograms an n-Tuples
	CreateNTuples();
	CreateHistos();

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
// Destructor.
D1RunAction::~D1RunAction()
{
	fSDName.clear();
	delete G4AccumulableManager::Instance();
	delete G4AnalysisManager::Instance();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//== 
G4Run* D1RunAction::GenerateRun()
{
	// Generate new RUN object, which is specially
	// dedicated for MultiFunctionalDetector scheme.
	//  Detail description can be found in D1Run.hh/cc.
	return new D1Run(fSDName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//==
void D1RunAction::BeginOfRunAction(const G4Run* /*aRun*/)
{

	// reset accumulables to their initial values
	G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
	accumulableManager->Reset();

	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();


	if(analysisManager->IsActive()){

		// Open an output file
		analysisManager->OpenFile();
	}

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//== 
void D1RunAction::EndOfRunAction(const G4Run* aRun)
{

	// Merge accumulables
	G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
	accumulableManager->Merge();

	G4int nofEvents = aRun->GetNumberOfEvent();
	if (nofEvents == 0) return;

	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

	if(analysisManager->IsActive()){
		// Save histograms and ntuples
		analysisManager->Write();
		analysisManager->CloseFile();
	}

	// Total energy deposit in a run and its variance
	//
	G4double range=0., range2=0., rmsRange=0.;

	range=fRange.GetValue();
	range2=fRange2.GetValue();

	rmsRange = range2 - range*range/nofEvents;
	if (rmsRange > 0.) rmsRange = std::sqrt(rmsRange);

	if(!IsMaster()) return;

	//- D1Run object.
	D1Run* run = (D1Run*) aRun;
	//--- Dump all socred quantities involved in D1Run.
	run->DumpAllScorer();
	//---

	//
	//- water phantom (Detector) Information.
	//-- Number of segments in the water phantom.
	const D1DetectorConstruction* detector =
			(const D1DetectorConstruction*)
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
	G4cout << "============================================================="
			<< G4endl;
	G4cout << std::setw( 8) << "#Z Cell#";
	G4cout << std::setw(16) << totalEdep->GetName();
	G4cout << std::setw(16) << protonEdep->GetName();

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

	std::ofstream  file("totED.txt");
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

void D1RunAction::CreateNTuples(){

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

void D1RunAction::CreateHistos(){

	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

	// Creating histos
	analysisManager->SetFirstHistoId(0);

	// id = 0
	analysisManager->CreateH1("eDep","Energy deposited per event", 300, 0, 300*mm,"mm");
	analysisManager->SetH1Activation(0,true);
	analysisManager->SetH1XAxisTitle(0,"Phantom depth [mm]");
	analysisManager->SetH1YAxisTitle(0,"Deposited Energy [keV]");

	// id = 1
	analysisManager->CreateH1("StepLength","Step length per event", 300, 0, 300*mm,"mm");
	analysisManager->SetH1Activation(1,true);
	analysisManager->SetH1XAxisTitle(1,"Phantom depth [mm]");
	analysisManager->SetH1YAxisTitle(1,"Step length [um]");

	// id = 2
	analysisManager->CreateH1("LET","Linear energy transfer", 300, 0, 300*mm,"mm");
	analysisManager->SetH1Activation(2,true);
	analysisManager->SetH1XAxisTitle(2,"Phantom depth [mm]");
	analysisManager->SetH1YAxisTitle(2,"LET [keV/um]");

	// id = 3
	analysisManager->CreateH1("Range","Range", 300, 0, 300*mm,"mm");
	analysisManager->SetH1Activation(3,true);
	analysisManager->SetH1XAxisTitle(3,"Range [mm]");
	analysisManager->SetH1YAxisTitle(3,"[Counts]");

	// id = 4
	analysisManager->CreateH1("KineticEnergyAt50","Kinetic energy at 50% of dose", 400, 0, 200*MeV,"MeV");
	analysisManager->SetH1Activation(4,true);
	analysisManager->SetH1XAxisTitle(4,"Kinetic energy at 50% of dose [MeV]");
	analysisManager->SetH1YAxisTitle(4,"[Counts]");

	// id = 5
	analysisManager->CreateH1("KineticEnergyAt70","Kinetic energy at 70% of dose", 400, 0, 200*MeV,"MeV");
	analysisManager->SetH1Activation(5,true);
	analysisManager->SetH1XAxisTitle(5,"Kinetic energy at 70% of dose [MeV]");
	analysisManager->SetH1YAxisTitle(5,"[Counts]");

	// id = 6
	analysisManager->CreateH1("KineticEnergyAt90","Kinetic energy at 90% of dose", 400, 0, 200*MeV,"MeV");
	analysisManager->SetH1Activation(6,true);
	analysisManager->SetH1XAxisTitle(6,"Kinetic energy at 90% of dose [MeV]");
	analysisManager->SetH1YAxisTitle(6,"[Counts]");

}
