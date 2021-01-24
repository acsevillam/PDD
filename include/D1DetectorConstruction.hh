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
/// \file runAndEvent/D1/include/D1DetectorConstruction.hh
/// \brief Definition of the D1DetectorConstruction class
//
//
//

#ifndef D1DetectorConstruction_h
#define D1DetectorConstruction_h 1

#include "globals.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4MultiFunctionalDetector.hh"

class G4Box;
class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;
class LET;

using namespace std;

class D1DetectorConstruction : public G4VUserDetectorConstruction
{
public:
	// constructor and destructor.
	D1DetectorConstruction();
	virtual ~D1DetectorConstruction();

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
	G4ThreeVector fPhantomSize;   // Size of Water Phantom
	G4int         fNx,fNy,fNz;    // Number of segmentation of water phantom.
	G4double        			fConcentration;
	G4LogicalVolume* fLVPhantomSens;
	G4Region*           		fpRegion;
	vector<G4LogicalVolume*>  	fScoringVolumeVector;
	LET*           let;

};
#endif
