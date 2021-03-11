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

#ifndef PDD1DetectorConstruction_h
#define PDD1DetectorConstruction_h 1

#include "globals.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4MultiFunctionalDetector.hh"

class G4Box;
class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;
class LET;

using namespace std;

class PDD1DetectorConstruction : public G4VUserDetectorConstruction
{
public:
	// constructor and destructor.
	PDD1DetectorConstruction();
	virtual ~PDD1DetectorConstruction();

public:
	// virtual method from G4VUserDetectorConstruction.
	virtual G4VPhysicalVolume* Construct();
	virtual void ConstructSDandField();

public:
	// Get/Set Access methods for data members
	// Size of Whater Phantom
	void SetPhantomSize(G4ThreeVector size) { fPhantomSize=size; }
	const G4ThreeVector& GetPhantomSize() const { return fPhantomSize; }
	// Number of segments of water phantom
	void SetNumberOfSegmentsInPhantom(G4int nx, G4int ny, G4int nz)
	{ fNx=nx; fNy=ny; fNz=nz; }
	void GetNumberOfSegmentsInPhantom(G4int& nx, G4int& ny, G4int& nz)
	const{ nx=fNx; ny = fNy; nz = fNz; }
	inline G4int Index(G4int i, G4int j, G4int k) { return (i * fNy + j) * fNz + k; }

	// Set concentration
    void SetConcentration (G4double aConcentration);
    // Get concentration
    inline G4double GetConcentration () { return fConcentration;};

    inline vector<G4LogicalVolume*> GetScoringVolumeVector() const { return fScoringVolumeVector; }

    inline G4int GetNumberOfVoxelAlongX(){return fNx;}

    inline G4int GetNumberOfVoxelAlongY(){return fNy;}

    inline G4int GetNumberOfVoxelAlongZ(){return fNz;}


private:
	// Data members
	G4ThreeVector 				fPhantomSize;   // Size of Water Phantom
	G4int         				fNx,fNy,fNz;    // Number of segmentation of water phantom.
	G4double        			fConcentration;
	G4LogicalVolume* 			fLVPhantomSens;
	G4Region*           		fpRegion;
	vector<G4LogicalVolume*>  	fScoringVolumeVector;
	LET*           				let;

};
#endif
