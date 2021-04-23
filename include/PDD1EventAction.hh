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

#ifndef PDD1EventAction_h
#define PDD1EventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"

class PDD1RunAction;

/// Event action class
///

class PDD1EventAction : public G4UserEventAction
{
public:
	PDD1EventAction(PDD1RunAction* runAction);
	virtual ~PDD1EventAction();

	virtual void BeginOfEventAction(const G4Event* event);
	virtual void EndOfEventAction(const G4Event* event);

	void AddEdep(G4double edep) { fEdep += edep; }

	void SetRange(G4double range) { fRange = range; }

	void SetK50(G4double kineticEnergy) { fK50 = kineticEnergy; }
	double GetK50() { return fK50; }

	void SetK70(G4double kineticEnergy) { fK70 = kineticEnergy; }
	double GetK70() { return fK70; }

	void SetK90(G4double kineticEnergy) { fK90 = kineticEnergy; }
	double GetK90() { return fK90; }

private:
	PDD1RunAction* 	fRunAction;
	G4double     	fEdep;
	G4double     	fRange;
	G4double     	fK50;
	G4double     	fK70;
	G4double     	fK90;
	G4int 			hitsCollectionID;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif


