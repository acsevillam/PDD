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
/// \file runAndEvent/D1/include/D1EventAction.hh
/// \brief Definition of the D1EventAction class
//
//
//

#ifndef D1EventAction_h
#define D1EventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"

class G4Event;
class D1RunAction;

class D1EventAction : public G4UserEventAction
{
public:
	D1EventAction(D1RunAction* runAction);
	~D1EventAction();

public:
	virtual void BeginOfEventAction(const G4Event*);
	virtual void EndOfEventAction(const G4Event*);

	void SetRange(G4double range) { fRange = range; }
	void SetK50(G4double kineticEnergy) { fK50 = kineticEnergy; }
	double GetK50() { return fK50; }
	void SetK70(G4double kineticEnergy) { fK70 = kineticEnergy; }
	double GetK70() { return fK70; }
	void SetK90(G4double kineticEnergy) { fK90 = kineticEnergy; }
	double GetK90() { return fK90; }

	D1RunAction* GetRunAction (){return fRunAction;};

private:
	D1RunAction* 			fRunAction;
	G4double     			fRange;
	G4double     			fK50;
	G4double     			fK70;
	G4double     			fK90;

};

//

#endif


