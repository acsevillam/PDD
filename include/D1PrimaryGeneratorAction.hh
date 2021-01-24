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

#ifndef D1PrimaryGeneratorAction_h
#define D1PrimaryGeneratorAction_h 1

// Geant4 Headers
#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4GeneralParticleSource.hh"
#include "globals.hh"


class G4Event;

class D1PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
	D1PrimaryGeneratorAction();
	virtual ~D1PrimaryGeneratorAction();

	// method from the base class
	virtual void GeneratePrimaries(G4Event*);

	// method to access particle gun
	inline const G4GeneralParticleSource* GetParticleSource() const {return fParticleSource; } ;

private:
	G4GeneralParticleSource*  fParticleSource; // pointer a to G4 gun class

};


#endif // D1PrimaryGeneratorAction_h
