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
#include "PDD1NestedPhantomParameterisation.hh"

// Geant4 Headers
#include "G4VPhysicalVolume.hh"
#include "G4VTouchable.hh"
#include "G4ThreeVector.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"

PDD1NestedPhantomParameterisation
::PDD1NestedPhantomParameterisation(const G4ThreeVector& voxelSize,
		G4int nz,
		std::vector<G4Material*>& mat):
		G4VNestedParameterisation(),
		fdX(voxelSize.x()),fdY(voxelSize.y()),fdZ(voxelSize.z()),
		fNz(nz),fMat(mat)
{
	// Position of voxels.
	// x and y positions are already defined in DetectorConstruction
	// by using replicated volume. Here only we need to define is z positions
	// of voxles.
	fpZ.clear();
	G4double zp;
	for ( G4int iz = 0; iz < fNz; iz++){
		zp = (-fNz+1+2*iz)*fdZ;
		fpZ.push_back(zp);
	}

}

PDD1NestedPhantomParameterisation::~PDD1NestedPhantomParameterisation(){
	fpZ.clear();
}

// Material assignment to geometry.
//
G4Material* PDD1NestedPhantomParameterisation
::ComputeMaterial(G4VPhysicalVolume* /*currentVol*/, const G4int copyNo, 
		const G4VTouchable* parentTouch)
{
	if(parentTouch==0) return fMat[0]; // protection for initialization and
	// vis at idle state
	// Copy number of voxels.
	// Copy number of X and Y are obtained from replication number.
	// Copy nymber of Z is the copy number of current voxel.
	G4int ix = parentTouch->GetReplicaNumber(0);
	G4int iy = parentTouch->GetReplicaNumber(1);
	G4int iz = copyNo;
	// For demonstration purpose,a couple of materials are chosen alternately.
	G4Material* mat=0;
	if ( ix%2 == 0 && iy%2 == 0 && iz%2 == 0 ) mat = fMat[0];
	else mat = fMat[1];

	return mat;
}

//  Number of Materials
//  Material scanner is required for preparing physics tables and so on before 
//  stating simulation, so that G4 has to know number of materials.
G4int PDD1NestedPhantomParameterisation::GetNumberOfMaterials() const{
	return fMat.size();
}

// GetMaterial
//  This is needed for material scanner and realizing geometry.
//
G4Material* PDD1NestedPhantomParameterisation::GetMaterial(G4int i) const{
	return fMat[i];
}

// Transformation of voxels.
//
void PDD1NestedPhantomParameterisation
::ComputeTransformation(const G4int copyNo, G4VPhysicalVolume* physVol) const{
	G4ThreeVector position(0.,0.,fpZ[copyNo]);
	physVol->SetTranslation(position);
}

// Dimensions are always same in this PDD1 example.
//
void PDD1NestedPhantomParameterisation
::ComputeDimensions(G4Box& box, const G4int, const G4VPhysicalVolume* ) const{
	box.SetXHalfLength(fdX);
	box.SetYHalfLength(fdY);
	box.SetZHalfLength(fdZ);
}
