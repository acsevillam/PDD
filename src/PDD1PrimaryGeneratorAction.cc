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

// PDD1 Headers
#include "PDD1PrimaryGeneratorAction.hh"

// Geant4 Headers
#include "G4SystemOfUnits.hh"
#include "G4ParticleTable.hh"

PDD1PrimaryGeneratorAction::PDD1PrimaryGeneratorAction(): G4VUserPrimaryGeneratorAction(),
fParticleSource(0)
{
	G4cout<<"01 - Primary Generator action have started !!!"<<G4endl;
	fParticleSource = new G4GeneralParticleSource();

}

PDD1PrimaryGeneratorAction::~PDD1PrimaryGeneratorAction()
{
	delete fParticleSource;
}

void PDD1PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{

	fParticleSource->GeneratePrimaryVertex(anEvent);
}

