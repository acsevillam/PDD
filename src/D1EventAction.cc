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
/// \file runAndEvent/D1/src/D1EventAction.cc
/// \brief Implementation of the D1EventAction class
//
//
//
 
#include "D1EventAction.hh"

#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "G4RunManager.hh"
#include "G4Run.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
D1EventAction::D1EventAction(D1RunAction* runAction)
 : G4UserEventAction(),
   fRunAction(runAction),
   fRange(0.),
   fK50(0.),
   fK70(0.),
   fK90(0.)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
D1EventAction::~D1EventAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void D1EventAction::BeginOfEventAction(const G4Event*)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void D1EventAction::EndOfEventAction(const G4Event* anEvent)
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

}

//
