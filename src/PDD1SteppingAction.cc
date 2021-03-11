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

// Geant4 Headers

#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4VPhysicalVolume.hh"
#include "G4TrackingManager.hh"
#include "G4Run.hh"
#include "G4TouchableHandle.hh"
#include "G4EmCalculator.hh"

#include "PDD1SteppingAction.hh"
#include "PDD1Analysis.hh"
#include "PDD1DetectorConstruction.hh"
#include "PDD1EventAction.hh"
#include "PDD1RunAction.hh"
#include "PDD1TrackingAction.hh"
#include "LET.hh"

// D1 Headers

using namespace std;

PDD1SteppingAction::PDD1SteppingAction(PDD1TrackingAction* trackingAction):G4UserSteppingAction(),
		fTrackingAction(trackingAction)
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

	G4int nEvents = G4RunManager::GetRunManager()->GetCurrentRun()->GetNumberOfEventToBeProcessed() ;

	G4Track* Track 							= aStep->GetTrack();
	G4int TrackID 							= aStep->GetTrack()->GetTrackID();
	G4ParticleDefinition *particleDef 		= aStep->GetTrack()->GetDefinition();
	G4String ParticleName 					= aStep->GetTrack()->GetDefinition()->GetParticleName();
	G4double DE					 			= aStep->GetTotalEnergyDeposit();
	G4double DX			 					= aStep->GetStepLength();
	G4int Z									= aStep->GetTrack()->GetDefinition()->GetAtomicNumber();
	G4int A 								= aStep->GetTrack()->GetDefinition()->GetAtomicMass();
	G4int pdg 								= aStep->GetTrack()->GetDefinition()->GetPDGEncoding();
	G4double z								= aStep->GetPreStepPoint()->GetPosition().z();

	const G4VTouchable* touchable = aStep->GetPreStepPoint()->GetTouchable();
	G4int ix = touchable->GetReplicaNumber(1);
	G4int iy = touchable->GetReplicaNumber(2);
	G4int iz = touchable->GetReplicaNumber(0);


	PDD1EventAction* eventAction = fTrackingAction->GetEventAction();

	if(TrackID==1 && z >= 242*mm){
		if(eventAction->GetK50()==0.){
			eventAction->SetK50(aStep->GetTrack()->GetKineticEnergy());
		}
	}

	if(TrackID==1 && z >= 253*mm){
		if(eventAction->GetK70()==0.){
			eventAction->SetK70(aStep->GetTrack()->GetKineticEnergy());
		}
	}

	if(TrackID==1 && z >= 256*mm){
		if(eventAction->GetK90()==0.){
			eventAction->SetK90(aStep->GetTrack()->GetKineticEnergy());
		}
	}

	// Analysis manager
	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

	if ( !(Z==0 && A==1) ) // All but not neutrons
	{
		if( DE>0. && DX >0. )// calculate only energy deposit
		{
			if (pdg !=22 && pdg !=11) // not gamma and electrons
			{

				// Get the pre-step kinetic energy
				G4double eKinPre = aStep -> GetPreStepPoint() -> GetKineticEnergy();
				// Get the post-step kinetic energy
				G4double eKinPost = aStep -> GetPostStepPoint() -> GetKineticEnergy();
				// Get the step average kinetic energy
				G4double eKinMean = (eKinPre + eKinPost) * 0.5;

				// get the material
				G4Material * materialStep = aStep -> GetPreStepPoint() -> GetMaterial();

				// get the secondary paticles
				G4Step fstep = *Track -> GetStep();
				// store all the secondary partilce in current step
				const std::vector<const G4Track*> * secondary = fstep.GetSecondaryInCurrentStep();

				size_t SecondarySize = (*secondary).size();
				G4double DEEletrons = 0.;

				// get secondary electrons energy deposited
				if (SecondarySize) // calculate only secondary particles
				{
					for (size_t numsec = 0; numsec< SecondarySize ; numsec ++)
					{
						//Get the PDG code of every secondaty particles in current step
						G4int PDGSecondary=(*secondary)[numsec]->GetDefinition()->GetPDGEncoding();

						if(PDGSecondary == 11) // calculate only secondary electrons
						{
							// calculate the energy deposit of secondary electrons in current step
							DEEletrons += (*secondary)[numsec]->GetKineticEnergy();
						}
					}

				}

				// call the let filldatas function to calculate let
				LET* let = LET::GetInstance();
				let -> FillEnergySpectrum(TrackID, particleDef, eKinMean, materialStep, DE,DEEletrons,DX, ix, iy, iz);

				if(TrackID==1){
					analysisManager->FillH1(0,iz, (DE+DEEletrons)/keV/nEvents);
					analysisManager->FillH1(1,iz, DX/um/nEvents);
				}
			}
		}
	}


}

