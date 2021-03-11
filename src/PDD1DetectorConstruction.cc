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

// Geant4 headers
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

// PDD headers

#include "PDD1DetectorConstruction.hh"
#include "PDD1NestedPhantomParameterisation.hh"
#include "LET.hh"


PDD1DetectorConstruction::PDD1DetectorConstruction()
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
	fPhantomSize.setX(50.*cm);
	fPhantomSize.setY(50.*cm);
	fPhantomSize.setZ(50.*cm);
	fNx = 1;
	fNy = 1;
	fNz = 500;

	fConcentration=0*mg/g;

	//fPDD1DetectorMessenger = new PDD1DetectorMessenger2(this) ;

	// Comment out the line below if let calculation is not needed!
	// Initialize LET with energy of primaries and clear data inside
	if ( (let = LET::GetInstance(this)) )
	{
		LET::GetInstance() -> Initialize();
	}

}

PDD1DetectorConstruction::~PDD1DetectorConstruction()
{;}

G4VPhysicalVolume* PDD1DetectorConstruction::Construct()
{

	// General Attributes
	G4VisAttributes* simpleInvisibleSVisAtt;
	simpleInvisibleSVisAtt= new G4VisAttributes(G4Colour(0.,0.0,0.5,0.025));
	simpleInvisibleSVisAtt->SetVisibility(true);

	// Get nist material manager
	G4NistManager* nist = G4NistManager::Instance();

	// Elements
	G4Material* vacuum  = nist->FindOrBuildMaterial("G4_Galactic");

	// Compounds
	G4Material* WATER  = nist->FindOrBuildMaterial("G4_WATER");

	// Print all the materials defined.
	G4cout << G4endl << "The materials defined are : " << G4endl << G4endl;
	G4cout << *(G4Material::GetMaterialTable()) << G4endl;

	// Geometrical Volume =========================================================================================

	// World
	G4double world_size_X = (10/2.)*m;
	G4VSolid* world_geo = new G4Box("world_geo", world_size_X, world_size_X, world_size_X);

	//Logical Volume ==============================================================================================

	// World
	G4LogicalVolume* world_log = new G4LogicalVolume(world_geo,vacuum,"world_log");
	world_log -> SetVisAttributes(simpleInvisibleSVisAtt);

	//Physics Volume  =============================================================================================

	// World
	G4VPhysicalVolume* world_phys =
			new G4PVPlacement(0,                    //no rotation
					G4ThreeVector(),       			//at (0,0,0)
					world_log,      				//its logical volume
					"world_phys",   				//its name
					0,                     			//its mother  volume
					false,                 			//no boolean operation
					0,                     			//copy number
					0);      			    		//overlaps checking

	//................................
	// Mother Volume of Water Phantom
	//................................

	//--  Default size of water phantom is defined at constructor.
	G4ThreeVector phantomSize = fPhantomSize;

	G4Box * solidPhantom
	= new G4Box("phantom",
			phantomSize.x()/2., phantomSize.y()/2., phantomSize.z()/2.);
	G4LogicalVolume * logicPhantom
	= new G4LogicalVolume(solidPhantom, WATER, "Phantom", 0, 0, 0);

	G4RotationMatrix* rot = new G4RotationMatrix();
	//rot->rotateY(30.*deg);
	G4ThreeVector positionPhantom = G4ThreeVector(0,0,phantomSize.z()/2.);
	//G4VPhysicalVolume * physiPhantom =
	new G4PVPlacement(rot,             // no rotation
			positionPhantom, // at (x,y,z)
			logicPhantom,    // its logical volume
			"Phantom",       // its name
			world_log,      // its mother  volume
			false,           // no boolean operations
			0);              // copy number

	//..............................................
	// Phantom segmentation using Parameterisation
	//..............................................
	//
	G4cout << "<-- PDD1DetectorConstruction::Construct-------" <<G4endl;
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
			new G4LogicalVolume(solYRep,WATER,yRepName);
	//G4PVReplica* yReplica =
	new G4PVReplica(yRepName,logYRep,logicPhantom,kYAxis,fNy,sensSize.y());
	// X Slice
	G4String xRepName("RepX");
	G4VSolid* solXRep =
			new G4Box(xRepName,sensSize.x()/2.,sensSize.y()/2.,phantomSize.z()/2.);
	G4LogicalVolume* logXRep =
			new G4LogicalVolume(solXRep,WATER,xRepName);
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
	fLVPhantomSens = new G4LogicalVolume(solVoxel,WATER,zVoxName);
	//
	//
	std::vector<G4Material*> phantomMat(2,WATER);
	//
	// Parameterisation for transformation of voxels.
	//  (voxel size is fixed in this example.
	//  e.g. nested parameterisation handles material and transfomation of voxels.)
	PDD1NestedPhantomParameterisation* paramPhantom
	= new PDD1NestedPhantomParameterisation(sensSize/2.,nzCells,phantomMat);
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



	return world_phys;
}

void PDD1DetectorConstruction::ConstructSDandField() {


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
	/*G4SDParticleFilter* electronFilter =
			new G4SDParticleFilter(fltName="electronFilter");
	electronFilter->add(particleName="e+");   // accept electrons.
	electronFilter->add(particleName="e-");   // accept positorons.
	//
	//-- charged particle filter
	G4SDChargedFilter* chargedFilter =
			new G4SDChargedFilter(fltName="chargedFilter");*/

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

