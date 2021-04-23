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

#ifndef Materials_hh
#define Materials_hh 1

// Geant4 Headers
#include "G4Material.hh"

using namespace std ;

typedef map <G4String,G4Material*> MaterialsCollection ;

class Materials {

public:

	/// Static method returning an instance of Control.
	static Materials* GetInstance() ;
	/// Static method killing the instance.
	static void Kill() ;
	G4Material* GetMaterial(const G4String materialName);
	G4Material* GetMaterial(const G4String materialName, G4double concentration);
	G4Material* otherMaterials(const G4String materialName);
	G4Material* otherMaterials(const G4String materialName, G4double concentration);

private:

	// Constructor
	Materials();

	//Destructor
	virtual ~Materials();

	// Singleton material instance.
	static Materials* instance;

	/// Collection of materials.
	MaterialsCollection		fMaterialsCollection ;


};

#endif // Materials_hh
