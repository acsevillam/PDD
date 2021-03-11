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

#ifndef PDD1SteppingAction_h
#define PDD1SteppingAction_h 1


// Geant4 Headers
#include "G4UserSteppingAction.hh"
#include "globals.hh"
#include "G4LogicalVolume.hh"

class PDD1TrackingAction;

using namespace std;

/// Stepping action class
///

class PDD1SteppingAction : public G4UserSteppingAction
{

public:
	PDD1SteppingAction(PDD1TrackingAction* trackingAction);
	virtual ~PDD1SteppingAction();

	// method from the base class
	virtual void UserSteppingAction(const G4Step*);

private:
	PDD1TrackingAction*  		fTrackingAction;
    vector<G4LogicalVolume*> 	fScoringVolumeVector;

};

#endif // PDD1SteppingAction_h
