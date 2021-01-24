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
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

// D1 Headers
#include "D1TrackingAction.hh"
#include "D1Analysis.hh"

using namespace std;

D1TrackingAction::D1TrackingAction(D1EventAction* eventAction)
:G4UserTrackingAction(),
 fEventAction(eventAction),
 fTotalEdep(0.)
{}

D1TrackingAction::~D1TrackingAction()
{}

void D1TrackingAction::PreUserTrackingAction (const G4Track* /*aTrack*/)
{
	fTotalEdep=0.;
}

void D1TrackingAction::PostUserTrackingAction (const G4Track* aTrack){

	// Analysis manager

	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

	G4String CreatorProcessName;
	G4String CreatorModelName ;

	if(aTrack -> GetCreatorProcess ()!=0){
		CreatorProcessName = aTrack -> GetCreatorProcess() -> GetProcessName() ;
		CreatorModelName = aTrack ->GetCreatorModelName();
		if(CreatorModelName=="Undefined"){
			CreatorModelName="u_"+CreatorProcessName;
		}
	}else{
		CreatorProcessName = "gun" ;
		CreatorModelName = "gun";
	}

	G4double KineticEnergyAtVertex 		= aTrack->GetVertexKineticEnergy();
	G4String ParticleName 				= aTrack->GetParticleDefinition()-> GetParticleName();
	G4double TrackLength		 		= aTrack->GetTrackLength();

	if(analysisManager->GetNtupleActivation(0)==true){
		analysisManager->FillNtupleDColumn(0,0,KineticEnergyAtVertex);
		analysisManager->FillNtupleSColumn(0,1,CreatorProcessName);
		analysisManager->FillNtupleSColumn(0,2,CreatorModelName);
		analysisManager->FillNtupleSColumn(0,3,ParticleName);
		analysisManager->FillNtupleDColumn(0,4,TrackLength);
		analysisManager->FillNtupleDColumn(0,5,fTotalEdep);
		analysisManager->AddNtupleRow(0);
	}

	if(aTrack->GetTrackID()==1){
		fEventAction->SetRange(aTrack->GetPosition().z());
	}

}
