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

#ifndef PDD1TrackingAction_h
#define PDD1TrackingAction_h 1

// Geant4 Headers
#include "G4UserTrackingAction.hh"
#include "globals.hh"
#include "G4Track.hh"
#include "G4SystemOfUnits.hh"

#include "PDD1EventAction.hh"

using namespace std;

/// Tracking action class
///

class PDD1TrackingAction : public G4UserTrackingAction{

public :

	PDD1TrackingAction(PDD1EventAction* eventAction);
	virtual ~PDD1TrackingAction();

	// method from the base class
	virtual void PreUserTrackingAction (const G4Track* aTrack);
	virtual void PostUserTrackingAction (const G4Track* aTrack);
	PDD1EventAction* GetEventAction (){return fEventAction;};

    void AddTotalEdep(G4double Edep) { fTotalEdep += Edep; }


private:
	PDD1EventAction*  fEventAction;
	G4double     	fTotalEdep;

};

#endif // PDD1TrackingAction_h

