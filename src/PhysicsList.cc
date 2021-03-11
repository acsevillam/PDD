/*
 * PhysicsList
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

#include "PhysicsList.hh"
#include "G4SystemOfUnits.hh"
#include "G4EmParameters.hh"

#include "G4EmDNAPhysics.hh"
#include "G4EmDNAPhysics_option1.hh"
#include "G4EmDNAPhysics_option2.hh"
#include "G4EmDNAPhysics_option3.hh"
#include "G4EmDNAPhysics_option4.hh"
#include "G4EmDNAPhysics_option5.hh"
#include "G4EmDNAPhysics_option6.hh"
#include "G4EmDNAPhysics_option7.hh"

#include "G4EmStandardPhysics.hh"
#include "G4EmStandardPhysics_option3.hh"
#include "G4EmDNAPhysicsActivator.hh"
#include "G4DecayPhysics.hh"
#include "G4RadioactiveDecayPhysics.hh"
#include "G4EmExtraPhysics.hh"
#include "G4IonPhysics.hh"
#include "G4IonBinaryCascadePhysics.hh"
#include "G4StoppingPhysics.hh"
#include "G4HadronElasticPhysicsHP.hh"
#include "G4HadronPhysicsQGSP_BIC_HP.hh"
#include "G4NeutronTrackingCut.hh"

PhysicsList::PhysicsList()
: G4VModularPhysicsList()
{
	SetDefaultCutValue(0.1*micrometer);
	SetVerboseLevel(1);

	// FIRST METHOD TO ACTIVATE Geant4-DNA Physics,
	//  using a Geant4-DNA Physics constructor only
	//
	//  RegisterPhysics(new G4EmDNAPhysics());
	// or
	//  RegisterPhysics(new G4EmDNAPhysics_option1());
	// or
	//  RegisterPhysics(new G4EmDNAPhysics_option2());
	// or
	//  RegisterPhysics(new G4EmDNAPhysics_option3());
	// or
	//  RegisterPhysics(new G4EmDNAPhysics_option4());
	// or
	//  RegisterPhysics(new G4EmDNAPhysics_option5());
	// or
	//  RegisterPhysics(new G4EmDNAPhysics_option6());
	// or
	//  RegisterPhysics(new G4EmDNAPhysics_option7());

	// or SECOND METHOD TO ACTIVATE Geant4-DNA Physics
	// (this includes combination with Geant4 EM Physics)

	//this->RegisterPhysics(new G4EmStandardPhysics());
	// or
	this->RegisterPhysics(new G4EmStandardPhysics_option3());

	this->RegisterPhysics(new G4EmDNAPhysicsActivator());

	this->RegisterPhysics(new G4EmExtraPhysics());

	this->RegisterPhysics(new G4DecayPhysics());

	this->RegisterPhysics(new G4RadioactiveDecayPhysics());

	this->RegisterPhysics(new G4HadronElasticPhysicsHP());

	this->RegisterPhysics(new G4HadronPhysicsQGSP_BIC_HP());

	this->RegisterPhysics(new G4StoppingPhysics());

	// Ion Physics

	//this->RegisterPhysics(new G4IonPhysics());

	this->RegisterPhysics(new G4IonBinaryCascadePhysics());

	this->RegisterPhysics(new G4NeutronTrackingCut());


	G4ProductionCutsTable::GetProductionCutsTable()->SetEnergyRange(100*eV, 1*GeV);
	G4EmParameters* param = G4EmParameters::Instance();
	param->SetMinEnergy(100*eV);
	param->SetMaxEnergy(1*GeV);
}

PhysicsList::~PhysicsList()
{}
