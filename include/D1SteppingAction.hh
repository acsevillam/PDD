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

#ifndef D1SteppingAction_h
#define D1SteppingAction_h 1


// Geant4 Headers
#include "G4UserSteppingAction.hh"
#include "globals.hh"
#include "G4LogicalVolume.hh"

class D1TrackingAction;

using namespace std;

/// Stepping action class
///

class D1SteppingAction : public G4UserSteppingAction
{

public:
	D1SteppingAction(D1TrackingAction* trackingAction);
	virtual ~D1SteppingAction();

	// method from the base class
	virtual void UserSteppingAction(const G4Step*);

private:
	D1TrackingAction*  fTrackingAction;
    vector<G4LogicalVolume*> 	fScoringVolumeVector;

};

#endif // D1SteppingAction_h
