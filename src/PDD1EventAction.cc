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

// PDD Headers
#include "PDD1EventAction.hh"
#include "PDD1RunAction.hh"
#include "DetectorHit.hh"
#include "DetectorMatrix.hh"
#include "Analysis.hh"

// Geant4 Headers
#include "G4Run.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4SDManager.hh"
#include "G4SystemOfUnits.hh"

PDD1EventAction::PDD1EventAction(PDD1RunAction* runAction)
: G4UserEventAction(),
  fRunAction(runAction),
  fEdep(0.),
  fRange(0.),
  fK30(0.),
  fK50(0.),
  fK70(0.),
  fK90(0.)
{
	hitsCollectionID = -1;
}

PDD1EventAction::~PDD1EventAction()
{}

void PDD1EventAction::BeginOfEventAction(const G4Event*)
{    
	fEdep = 0.;
	fK30=0.;
	fK50=0.;
	fK70=0.;
	fK90=0.;
	fRange=0;


	G4SDManager* pSDManager = G4SDManager::GetSDMpointer();
	if(hitsCollectionID == -1)
		hitsCollectionID = pSDManager -> GetCollectionID("PhantomHitsCollection");

}

void PDD1EventAction::EndOfEventAction(const G4Event* event)
{
	G4int nEvents = G4RunManager::GetRunManager()->GetCurrentRun()->GetNumberOfEventToBeProcessed() ;

	if(nEvents>10){
		G4int 	fraction 	= G4int(nEvents/100) ;
		if(event->GetEventID()%fraction == 0)
			G4cout<<"("<<(event->GetEventID()/(nEvents*1.0)*100)<<" %)"<<G4endl ;

	}else {
		G4int 	fraction 	= G4int(nEvents/1) ;
		if(event->GetEventID()%fraction == 0)
			G4cout<<"("<<(event->GetEventID()/(nEvents*1.0)*100)<<" %)"<<G4endl ;
	}

	// accumulate statistics in run action
	fRunAction->AddEdep(fEdep);

	G4double KineticEnergyAtVertex = event->GetPrimaryVertex()->GetPrimary()->GetKineticEnergy();

	// Analysis manager
	G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

	if(analysisManager->GetActivation()){

		analysisManager->FillH1(0, fRange);

		if(fK30>0.) analysisManager->FillH1(1,fK30);

		if(fK50>0.) analysisManager->FillH1(2,fK50);

		if(fK70>0.) analysisManager->FillH1(3,fK70);

		if(fK90>0.) analysisManager->FillH1(4,fK90);

		analysisManager->FillH1(5, KineticEnergyAtVertex);
	}

	if(hitsCollectionID < 0)
		return;
	G4HCofThisEvent* HCE = event -> GetHCofThisEvent();

	// Clear voxels hit list
	DetectorMatrix* matrix = DetectorMatrix::GetInstance();
	if (matrix) matrix -> ClearHitTrack();

	if(HCE)
	{
		PDD1HitsCollection* CHC = (PDD1HitsCollection*)(HCE -> GetHC(hitsCollectionID));
		if(CHC)
		{
			if(matrix)
			{
				// Fill the matrix with the information: voxel and associated energy deposit
				// in the detector at the end of the event

				size_t HitCount = CHC -> entries();
				for (size_t h=0; h<HitCount; h++)
				{
					G4int i = ((*CHC)[h]) -> GetIx();
					G4int j = ((*CHC)[h]) -> GetIy();
					G4int k = ((*CHC)[h]) -> GetIz();
					G4double eDep = ((*CHC)[h]) -> GetEdep();
					G4double secondariesEDep = ((*CHC)[h]) -> GetSecondariesEdep();
					G4double dx = ((*CHC)[h]) ->GetDX();
					G4double kinEMean = ((*CHC)[h]) ->GetKinEMean();
					G4int trackID = ((*CHC)[h]) -> GetTrackID();
					G4ParticleDefinition* particleDef = ((*CHC)[h]) ->GetParticleDef();
					G4Material* mat = ((*CHC)[h]) ->GetMat();
					G4double vol = ((*CHC)[h]) ->GetVol();

					matrix ->FillEdep(i, j, k, eDep+secondariesEDep, trackID, particleDef);

					G4int Z = particleDef-> GetAtomicNumber();
					G4int A = particleDef-> GetAtomicMass();
					G4int PDGCode = particleDef->GetPDGEncoding();

					if ( !(Z==0 && A==1) ) // All but not neutrons
					{
						// calculate only energy deposit
						if( (eDep>0. || secondariesEDep>0.) && dx >0. )
						{
							if (PDGCode !=22 && PDGCode !=11) // not gamma and electrons
							{
								matrix ->FillLet(i, j, k, eDep+secondariesEDep, dx, kinEMean, trackID, particleDef, mat);
							}
						}
					}

					matrix ->FillFluence(i, j, k, dx, vol, trackID, particleDef);

				}
			}
		}
	}

}
