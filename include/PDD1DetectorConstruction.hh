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

// Geant4 Headers
#include "globals.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4MultiFunctionalDetector.hh"

class G4Box;
class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;
class LET;
class DetectorMatrix;
class DetectorSD;

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

	// Phantom material
	G4bool SetPhantomMaterial(G4String material);
	G4bool SetPhantomMaterial(G4String material, G4double concentration);
    inline G4Material* GetPhantomMaterial(){return fPhantomMaterial;}

    // Phantom size
    void SetPhantomSize(G4double sizeX, G4double sizeY, G4double sizeZ);
    void GetPhantomSize(G4int& sizeX, G4int& sizeY, G4int& sizeZ)const{ sizeX=fPhantomSize.x(); sizeY=fPhantomSize.y(); sizeZ = fPhantomSize.z(); }

    // Phantom position
    inline void SetPhantomPosition(G4ThreeVector aPhantomPosition){fPhantomPosition=aPhantomPosition;}

    // Detector material
    G4bool SetDetectorMaterial(G4String material);
    G4bool SetDetectorMaterial(G4String material, G4double concentration);
    inline G4Material* GetDetectorMaterial(){return fDetectorMaterial;}

	// Concentration
    void SetConcentration (G4double aConcentration);
    inline G4double GetConcentration () { return fConcentration;};

    // Detector size
    void SetDetectorSize(G4double sizeX, G4double sizeY, G4double sizeZ);
    void GetDetectorSize(G4int& sizeX, G4int& sizeY, G4int& sizeZ)const{ sizeX=fDetectorSize.x(); sizeY=fDetectorSize.y(); sizeZ = fDetectorSize.z(); }

	// Number of segments of detector
	void SetDetectorSegmentation(G4int nX, G4int nY, G4int nZ){ fNX=nX; fNY=nY; fNZ=nZ; }
	void GetDetectorSegmentation(G4int& nX, G4int& nY, G4int& nZ)const{ nX=fNX; nY = fNY; nZ = fNZ; }

    // Detector position to phantom
    inline void SetDetectorToPhantomPosition(G4ThreeVector aDetectorToPhantomPosition){fDetectorToPhantomPosition=aDetectorToPhantomPosition;}

    // Scoring volume vector
    inline vector<G4LogicalVolume*> GetScoringVolumeVector() const { return fScoringVolumeVector; }

    // Get index of voxel
	inline G4int Index(G4int i, G4int j, G4int k) { return (i * fNY + j) * fNZ + k; }


private:
	// Data members


    /// Phantom

    // Phantom LV
    G4LogicalVolume*			fPhantomLogicalVolume;

    // Phantom PV
    G4VPhysicalVolume*			fPhantomPhysicalVolume;

    // Phantom material
    G4Material* 				fPhantomMaterial;

    // Phantom size
    G4ThreeVector 				fPhantomSize;

    // Phantom position
    G4ThreeVector				fPhantomPosition;

    /// Detector

    // Detector LV
    G4LogicalVolume*			fDetectorLogicalVolume;

    // Detector PV
    G4VPhysicalVolume*			fDetectorPhysicalVolume;

    // Detector material
    G4Material* 				fDetectorMaterial;
	G4double        			fConcentration;
    G4double 					fVolumeOfVoxel, fMassOfVoxel;

    // Detector size
    G4ThreeVector 				fDetectorSize;

    // Detector segmentation
    G4int         				fNX,fNY,fNZ;    // Number of segmentation of water phantom.

    // Detector to phantom position
    G4ThreeVector				fDetectorToPhantomPosition;

    // Voxel LV
    G4LogicalVolume*			fVoxelLogicalVolume;

    // Region
	G4Region*           		fpRegion;

	// Detector SD
	DetectorSD* 				fDetectorSD;

	// Detector Matrix
	DetectorMatrix*			    matrix;

	// LET
	LET*           				let;

	// Scoring volume vector
    vector<G4LogicalVolume*>  	fScoringVolumeVector;

};
#endif
