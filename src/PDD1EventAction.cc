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
 
#include "PDD1EventAction.hh"

#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "G4RunManager.hh"
#include "G4Run.hh"
#include "PDD1Analysis.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

PDD1EventAction::PDD1EventAction(PDD1RunAction* runAction)
 : G4UserEventAction(),
   fRunAction(runAction),
   fRange(0.),
   fK50(0.),
   fK70(0.),
   fK90(0.)
{}

PDD1EventAction::~PDD1EventAction()
{}

void PDD1EventAction::BeginOfEventAction(const G4Event*)
{
	fK50=0.;
	fK70=0.;
	fK90=0.;
	fRange=0;


}

void PDD1EventAction::EndOfEventAction(const G4Event* anEvent)
{
	G4int nEvents = G4RunManager::GetRunManager()->GetCurrentRun()->GetNumberOfEventToBeProcessed() ;
	if(nEvents>10){
		G4int 	fraction 	= G4int(nEvents/100) ;
		if(anEvent->GetEventID()%fraction == 0)
			G4cout<<"("<<(anEvent->GetEventID()/(nEvents*1.0)*100)<<" %)"<<G4endl ;

	}else {
		G4int 	fraction 	= G4int(nEvents/1) ;
		if(anEvent->GetEventID()%fraction == 0)
			G4cout<<"("<<(anEvent->GetEventID()/(nEvents*1.0)*100)<<" %)"<<G4endl ;
	}

	// Analysis manager
	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

	if(fK50>0.) analysisManager->FillH1(4,fK50);

	if(fK70>0.) analysisManager->FillH1(5,fK70);

	if(fK90>0.) analysisManager->FillH1(6,fK90);


	G4double KineticEnergyAtVertex = anEvent->GetPrimaryVertex()->GetPrimary()->GetKineticEnergy();
	analysisManager->FillH1(3, fRange);
	analysisManager->FillH1(7, KineticEnergyAtVertex);


}
