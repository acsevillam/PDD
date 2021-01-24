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
/// \file runAndEvent/D1/src/D1DetectorConstruction.cc
/// \brief Implementation of the D1DetectorConstruction class
//
//
//

#include "D1DetectorConstruction.hh"

#include "G4PSEnergyDeposit3D.hh"
#include "G4PSNofStep3D.hh"
#include "G4PSCellFlux3D.hh"
#include "G4PSPassageCellFlux3D.hh"
#include "G4PSFlatSurfaceFlux3D.hh"
#include "G4PSFlatSurfaceCurrent3D.hh"

#include "G4SDParticleWithEnergyFilter.hh"
#include "G4SDParticleFilter.hh"
#include "G4SDChargedFilter.hh"

#include "G4NistManager.hh"
#include "G4Material.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SDManager.hh"

#include "G4PVParameterised.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4SystemOfUnits.hh"    
#include "G4ios.hh"

#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "D1NestedPhantomParameterisation.hh"

#include "LET.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
D1DetectorConstruction::D1DetectorConstruction()
: G4VUserDetectorConstruction() ,
  fLVPhantomSens(0),
  fpRegion(0)
{

	const G4double ug = 1.e-6*g;
	const G4double ppm = ug/g;
	new G4UnitDefinition("microgram", "ug" , "Mass", ug);
	new G4UnitDefinition("ppm", "ppm","Concentration", ppm);
	new G4UnitDefinition("miligram/gram", "mg/g","Concentration", mg/g);
	new G4UnitDefinition("microgram/gram", "ug/g","Concentration", ug/g);

	// Default size of water phantom,and segmentation.
	fPhantomSize.setX(50.*um);
	fPhantomSize.setY(50.*um);
	fPhantomSize.setZ(50.*um);
	fNx = 5;
	fNy = 10;
	fNz = 20;

	fConcentration=0*mg/g;

	//fD1DetectorMessenger = new D1DetectorMessenger2(this) ;

    if ( (let = LET::GetInstance(this)) )
    {
        LET::GetInstance() -> Initialize();
    }

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
D1DetectorConstruction::~D1DetectorConstruction()
{;}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4VPhysicalVolume* D1DetectorConstruction::Construct()
{
	//=====================
	// Material Definitions
	//=====================
	//
	//-------- NIST Materials ----------------------------------------------------
	//  Material Information imported from NIST database.
	//
	G4NistManager* NISTman = G4NistManager::Instance();
	G4Material* vacuum  = NISTman->FindOrBuildMaterial("G4_Galactic");
	G4Material* air  = NISTman->FindOrBuildMaterial("G4_AIR");
	G4Material* water  = NISTman->FindOrBuildMaterial("G4_WATER");

	//
	// Print all the materials defined.
	G4cout << G4endl << "The materials defined are : " << G4endl << G4endl;
	G4cout << *(G4Material::GetMaterialTable()) << G4endl;

	//============================================================================
	//      Definitions of Solids, Logical Volumes, Physical Volumes
	//============================================================================

	//-------------
	// World Volume
	//-------------

	G4ThreeVector worldSize = G4ThreeVector(200*cm, 200*cm, 200*cm);

	G4Box * solidWorld
	= new G4Box("world", worldSize.x()/2., worldSize.y()/2., worldSize.z()/2.);
	G4LogicalVolume * logicWorld
	= new G4LogicalVolume(solidWorld, vacuum, "World", 0, 0, 0);

	//
	//  Must place the World Physical volume unrotated at (0,0,0).
	G4VPhysicalVolume * physiWorld
	= new G4PVPlacement(0,               // no rotation
			G4ThreeVector(), // at (0,0,0)
			logicWorld,      // its logical volume
			"World",         // its name
			0,               // its mother  volume
			false,           // no boolean operations
			0);              // copy number

	//---------------
	// Water Phantom
	//---------------

	//................................
	// Mother Volume of Water Phantom
	//................................

	//--  Default size of water phantom is defined at constructor.
	G4ThreeVector phantomSize = fPhantomSize;

	G4Box * solidPhantom
	= new G4Box("phantom",
			phantomSize.x()/2., phantomSize.y()/2., phantomSize.z()/2.);
	G4LogicalVolume * logicPhantom
	= new G4LogicalVolume(solidPhantom, water, "Phantom", 0, 0, 0);

	G4RotationMatrix* rot = new G4RotationMatrix();
	//rot->rotateY(30.*deg);
	G4ThreeVector positionPhantom = G4ThreeVector(0,0,phantomSize.z()/2.);
	//G4VPhysicalVolume * physiPhantom =
	new G4PVPlacement(rot,             // no rotation
			positionPhantom, // at (x,y,z)
			logicPhantom,    // its logical volume
			"Phantom",       // its name
			logicWorld,      // its mother  volume
			false,           // no boolean operations
			0);              // copy number

	//..............................................
	// Phantom segmentation using Parameterisation
	//..............................................
	//
	G4cout << "<-- D1DetectorConstruction::Construct-------" <<G4endl;
	G4cout << "  Water Phantom Size " << fPhantomSize/mm       << G4endl;
	G4cout << "  Segmentation  ("<< fNx<<","<<fNy<<","<<fNz<<")"<< G4endl;
	G4cout << "<---------------------------------------------"<< G4endl;
	// Number of segmentation.
	// - Default number of segmentation is defined at constructor.
	G4int nxCells = fNx;
	G4int nyCells = fNy;
	G4int nzCells = fNz;

	G4ThreeVector sensSize;
	sensSize.setX(phantomSize.x()/(G4double)nxCells);
	sensSize.setY(phantomSize.y()/(G4double)nyCells);
	sensSize.setZ(phantomSize.z()/(G4double)nzCells);
	// i.e Voxel size will be 2.0 x 2.0 x 2.0 mm3 cube by default.
	//

	// Replication of Water Phantom Volume.
	// Y Slice
	G4String yRepName("RepY");
	G4VSolid* solYRep =
			new G4Box(yRepName,phantomSize.x()/2.,sensSize.y()/2.,phantomSize.z()/2.);
	G4LogicalVolume* logYRep =
			new G4LogicalVolume(solYRep,water,yRepName);
	//G4PVReplica* yReplica =
	new G4PVReplica(yRepName,logYRep,logicPhantom,kYAxis,fNy,sensSize.y());
	// X Slice
	G4String xRepName("RepX");
	G4VSolid* solXRep =
			new G4Box(xRepName,sensSize.x()/2.,sensSize.y()/2.,phantomSize.z()/2.);
	G4LogicalVolume* logXRep =
			new G4LogicalVolume(solXRep,water,xRepName);
	//G4PVReplica* xReplica =
	new G4PVReplica(xRepName,logXRep,logYRep,kXAxis,fNx,sensSize.x());

	//
	//..................................
	// Voxel solid and logical volumes
	//..................................
	// Z Slice
	G4String zVoxName("phantomSens");
	G4VSolid* solVoxel =
			new G4Box(zVoxName,sensSize.x()/2.,sensSize.y()/2.,sensSize.z()/2.);
	fLVPhantomSens = new G4LogicalVolume(solVoxel,water,zVoxName);
	//
	//
	std::vector<G4Material*> phantomMat(2,water);
	//
	// Parameterisation for transformation of voxels.
	//  (voxel size is fixed in this example.
	//  e.g. nested parameterisation handles material and transfomation of voxels.)
	D1NestedPhantomParameterisation* paramPhantom
	= new D1NestedPhantomParameterisation(sensSize/2.,nzCells,phantomMat);
	//G4VPhysicalVolume * physiPhantomSens =
	new G4PVParameterised("PhantomSens",     // their name
			fLVPhantomSens,    // their logical volume
			logXRep,           // Mother logical volume
			kUndefined,        // Are placed along this axis
			nzCells,           // Number of cells
			paramPhantom);     // Parameterisation.
	//   Optimization flag is avaiable for,
	//    kUndefined, kXAxis, kYAxis, kZAxis.
	//

	//===============================
	//   Visualization attributes
	//===============================

	G4VisAttributes* boxVisAtt = new G4VisAttributes(G4Colour(1.0,1.0,1.0));
	logicWorld  ->SetVisAttributes(boxVisAtt);
	//logicWorld->SetVisAttributes(G4VisAttributes::GetInvisible());

	// Mother volume of WaterPhantom
	G4VisAttributes* phantomVisAtt = new G4VisAttributes(G4Colour(1.0,1.0,0.0));
	logicPhantom->SetVisAttributes(phantomVisAtt);

	// Replica
	G4VisAttributes* yRepVisAtt = new G4VisAttributes(G4Colour(0.0,1.0,0.0));
	logYRep->SetVisAttributes(yRepVisAtt);
	G4VisAttributes* xRepVisAtt = new G4VisAttributes(G4Colour(0.0,1.0,0.0));
	logXRep->SetVisAttributes(xRepVisAtt);

	// Skip the visualization for those voxels.
	fLVPhantomSens->SetVisAttributes(G4VisAttributes::GetInvisible());


	fScoringVolumeVector.push_back(fLVPhantomSens);

	if (!fpRegion)
	{
		fpRegion = new G4Region("Target");
		fLVPhantomSens -> SetRegion(fpRegion);
		fpRegion->AddRootLogicalVolume( fLVPhantomSens );
	}



	return physiWorld;
}

void D1DetectorConstruction::ConstructSDandField() {

	//================================================
	// Sensitive detectors : MultiFunctionalDetector
	//================================================
	//
	//  Sensitive Detector Manager.
	G4SDManager* pSDman = G4SDManager::GetSDMpointer();
	//
	// Sensitive Detector Name
	G4String phantomSDname = "PhantomSD";

	//------------------------
	// MultiFunctionalDetector
	//------------------------
	//
	// Define MultiFunctionalDetector with name.
	G4MultiFunctionalDetector* mFDet
	= new G4MultiFunctionalDetector(phantomSDname);
	pSDman->AddNewDetector( mFDet );                // Register SD to SDManager.
	fLVPhantomSens->SetSensitiveDetector(mFDet);    // Assign SD to the logical volume.

	//---------------------------------------
	// SDFilter : Sensitive Detector Filters
	//---------------------------------------
	//
	// Particle Filter for Primitive Scorer with filter name(fltName)
	// and particle name(particleName),
	// or particle names are given by add("particle name"); method.
	//
	G4String fltName,particleName;
	//
	//-- proton filter
	G4SDParticleFilter* protonFilter =
			new G4SDParticleFilter(fltName="protonFilter", particleName="proton");
	//
	//-- electron filter
	G4SDParticleFilter* electronFilter =
			new G4SDParticleFilter(fltName="electronFilter");
	electronFilter->add(particleName="e+");   // accept electrons.
	electronFilter->add(particleName="e-");   // accept positorons.
	//
	//-- charged particle filter
	G4SDChargedFilter* chargedFilter =
			new G4SDChargedFilter(fltName="chargedFilter");

	//------------------------
	// PS : Primitive Scorers
	//------------------------
	// Primitive Scorers are used with SDFilters according to your purpose.
	//
	//
	//-- Primitive Scorer for Energy Deposit.
	//      Total, by protons, by electrons.
	G4String psName;
	G4PSEnergyDeposit3D * scorer0 = new G4PSEnergyDeposit3D(psName="totalEDep",
			fNx,fNy,fNz);
	G4PSEnergyDeposit3D * scorer1 = new G4PSEnergyDeposit3D(psName="protonEDep",
			fNx,fNy,fNz);
	scorer1->SetFilter(protonFilter);

	//
	//------------------------------------------------------------
	//  Register primitive scorers to MultiFunctionalDetector
	//------------------------------------------------------------
	mFDet->RegisterPrimitive(scorer0);
	mFDet->RegisterPrimitive(scorer1);

}

