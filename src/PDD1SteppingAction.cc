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
#include "PDD1SteppingAction.hh"
#include "PDD1EventAction.hh"
#include "PDD1DetectorConstruction.hh"
#include "Analysis.hh"

// Geant4 Headers
#include "G4SystemOfUnits.hh"
#include "G4Step.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4Run.hh"
#include "G4LogicalVolume.hh"

PDD1SteppingAction::PDD1SteppingAction(PDD1EventAction* eventAction)
: G4UserSteppingAction(),
  fEventAction(eventAction)
{}

PDD1SteppingAction::~PDD1SteppingAction()
{}

void PDD1SteppingAction::UserSteppingAction(const G4Step* aStep)
{

	if (fScoringVolumeVector.empty()) {
		const PDD1DetectorConstruction* detectorConstruction
		= static_cast<const PDD1DetectorConstruction*>
		(G4RunManager::GetRunManager()->GetUserDetectorConstruction());
		fScoringVolumeVector = detectorConstruction->GetScoringVolumeVector();
	}

	// get volume of the current step
	G4LogicalVolume* currentVolume = aStep->GetPreStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume();

	G4bool IsScoringVolume = false;

	// check if we are in scoring volume
	for (unsigned i=0; i < fScoringVolumeVector.size(); i++) {
		if (currentVolume == fScoringVolumeVector[i]) IsScoringVolume = true;
	}

	if(!IsScoringVolume) return;

	G4int TrackID 							= aStep->GetTrack()->GetTrackID();
	G4double DE					 			= aStep->GetTotalEnergyDeposit();
	G4double z								= aStep->GetPreStepPoint()->GetPosition().z();

	fEventAction->AddEdep(DE);

	if(TrackID==1 && z >= 242*mm){
		if(fEventAction->GetK50()==0.){
			fEventAction->SetK50(aStep->GetTrack()->GetKineticEnergy());
		}
	}

	if(TrackID==1 && z >= 253*mm){
		if(fEventAction->GetK70()==0.){
			fEventAction->SetK70(aStep->GetTrack()->GetKineticEnergy());
		}
	}

	if(TrackID==1 && z >= 256*mm){
		if(fEventAction->GetK90()==0.){
			fEventAction->SetK90(aStep->GetTrack()->GetKineticEnergy());
		}
	}

}
