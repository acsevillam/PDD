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

// PDD headers
#include "PDD1DetectorConstruction.hh"
#include "PDD1NestedPhantomParameterisation.hh"
#include "DetectorSD.hh"
#include "DetectorMatrix.hh"
#include "Materials.hh"

// Geant4 headers
#include "G4RunManager.hh"
#include "G4PSEnergyDeposit3D.hh"
#include "G4PSNofStep3D.hh"
#include "G4PSCellFlux3D.hh"
#include "G4PSPassageCellFlux3D.hh"
#include "G4PSFlatSurfaceFlux3D.hh"
#include "G4PSFlatSurfaceCurrent3D.hh"
#include "G4PSPassageCellCurrent3D.hh"
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

PDD1DetectorConstruction::PDD1DetectorConstruction()
: G4VUserDetectorConstruction() ,
  fPhantomLogicalVolume(0),
  fpRegion(0),
  fDetectorSD(0),
  matrix(0)
{

	const G4double ug = 1.e-6*g;
	const G4double ppm = ug/g;
	new G4UnitDefinition("microgram", "ug" , "Mass", ug);
	new G4UnitDefinition("ppm", "ppm","Concentration", ppm);
	new G4UnitDefinition("miligram/gram", "mg/g","Concentration", mg/g);
	new G4UnitDefinition("microgram/gram", "ug/g","Concentration", ug/g);

	fConcentration=100*mg/g;

	// Set material of the water phantom and of the detector
	SetPhantomMaterial("G4_WATER");
	SetDetectorMaterial("G4_WATER");
	//SetDetectorMaterial("H2O+Au",fConcentration);

	// Set phantom size and position
	SetPhantomSize(30.*cm, 30.*cm, 30.*cm);
	SetPhantomPosition(G4ThreeVector(0. *cm, 0. *cm, 15. *cm));

	// Set detector size and position
	SetDetectorSize(10.*cm, 10.*cm, 30.*cm);
	SetDetectorToPhantomPosition(G4ThreeVector(0. *cm, 0. *cm, 0 *cm));
	//SetDetectorSize(5.*cm, 5.*cm, 5.*cm);
	//SetDetectorToPhantomPosition(G4ThreeVector(0. *cm, 0. *cm, -15*cm+16.5 *cm));
	SetDetectorSegmentation(100, 100, 300);
}

PDD1DetectorConstruction::~PDD1DetectorConstruction()
{}

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

	//--  Default size of phantom is defined at constructor.
	G4ThreeVector phantom_size = fPhantomSize;

	G4Box * phantom_geo
	= new G4Box("phantom",
			phantom_size.x()/2., phantom_size.y()/2., phantom_size.z()/2.);
	G4LogicalVolume * phantom_log
	= new G4LogicalVolume(phantom_geo, fPhantomMaterial, "phantom", 0, 0, 0);

	G4RotationMatrix* rot1 = new G4RotationMatrix();
	//rot1->rotateY(30.*deg);
	G4ThreeVector positionPhantom = fPhantomPosition; //G4ThreeVector(0,0,phantom_size.z()/2.);
	//G4VPhysicalVolume * physiPhantom =
	new G4PVPlacement(rot1,				// no rotation
			positionPhantom,			// at (x,y,z)
			phantom_log,				// its logical volume
			"phantom",					// its name
			world_log,					// its mother  volume
			false,						// no boolean operations
			0);							// copy number


	//--  Default size detector is defined at constructor.
	G4ThreeVector detector_size = fDetectorSize;

	G4Box * detector_geo
	= new G4Box("detector",
			detector_size.x()/2., detector_size.y()/2., detector_size.z()/2.);
	G4LogicalVolume * detector_log
	= new G4LogicalVolume(detector_geo, fPhantomMaterial, "detector", 0, 0, 0);

	G4RotationMatrix* rot2 = new G4RotationMatrix();
	//rot2->rotateY(30.*deg);
	G4ThreeVector positionDetectorToPhantom = fDetectorToPhantomPosition;// G4ThreeVector(0,0,-phantom_size.z()/2.+detector_size.z()/2.);
	//G4VPhysicalVolume * physiPhantom =
	new G4PVPlacement(rot2,				// no rotation
			positionDetectorToPhantom,	// at (x,y,z)
			detector_log,				// its logical volume
			"detector",					// its name
			phantom_log,				// its mother  volume
			false,						// no boolean operations
			0);							// copy number

	//..............................................
	// Phantom segmentation using Parameterisation
	//..............................................
	//
	G4cout << "<-- PDD1DetectorConstruction::Construct-------" <<G4endl;
	G4cout << "  Phantom Material " << fPhantomMaterial->GetName() << G4endl;
	G4cout << "  Phantom Size " << fPhantomSize/mm << G4endl;
	G4cout << "  Detector Material " << fDetectorMaterial->GetName() << G4endl;
	G4cout << "  Detector Size " << fDetectorSize/mm << G4endl;
	G4cout << "  Segmentation  ("<< fNX<<","<<fNY<<","<<fNZ<<")"<< G4endl;
	G4cout << "<---------------------------------------------"<< G4endl;
	// Number of segmentation.
	// - Default number of segmentation is defined at constructor.
	G4ThreeVector sensSize;
	sensSize.setX(fDetectorSize.x()/(G4double)fNX);
	sensSize.setY(fDetectorSize.y()/(G4double)fNY);
	sensSize.setZ(fDetectorSize.z()/(G4double)fNZ);

	// Replication of Water Phantom Volume.
	// Y Slice
	G4String yRepName("RepY");
	G4VSolid* solYRep =
			new G4Box(yRepName,fDetectorSize.x()/2.,sensSize.y()/2.,fDetectorSize.z()/2.);
	G4LogicalVolume* logYRep =
			new G4LogicalVolume(solYRep,fDetectorMaterial,yRepName);
	//G4PVReplica* yReplica =
	new G4PVReplica(yRepName,logYRep,detector_log,kYAxis,fNY,sensSize.y());
	// X Slice
	G4String xRepName("RepX");
	G4VSolid* solXRep =
			new G4Box(xRepName,sensSize.x()/2.,sensSize.y()/2.,fDetectorSize.z()/2.);
	G4LogicalVolume* logXRep =
			new G4LogicalVolume(solXRep,fDetectorMaterial,xRepName);
	//G4PVReplica* xReplica =
	new G4PVReplica(xRepName,logXRep,logYRep,kXAxis,fNX,sensSize.x());

	//
	//..................................
	// Voxel solid and logical volumes
	//..................................
	// Z Slice
	G4String zVoxName("phantomSens");
	G4VSolid* solVoxel =
			new G4Box(zVoxName,sensSize.x()/2.,sensSize.y()/2.,sensSize.z()/2.);
	fVoxelLogicalVolume = new G4LogicalVolume(solVoxel,fDetectorMaterial,zVoxName);
	//
	//
	std::vector<G4Material*> detectorMat(2,fDetectorMaterial);

	//
	// Parameterisation for transformation of voxels.
	//  (voxel size is fixed in this example.
	//  e.g. nested parameterisation handles material and transfomation of voxels.)
	PDD1NestedPhantomParameterisation* paramPhantom
	= new PDD1NestedPhantomParameterisation(sensSize/2.,fNZ,detectorMat);
	//G4VPhysicalVolume * physiPhantomSens =
	new G4PVParameterised("PhantomSens",    // their name
			fVoxelLogicalVolume,    		// their logical volume
			logXRep,           				// Mother logical volume
			kUndefined,        				// Are placed along this axis
			fNZ,           					// Number of cells
			paramPhantom);     				// Parameterisation.
	//   Optimization flag is avaiable for,
	//    kUndefined, kXAxis, kYAxis, kZAxis.
	//

	//===============================
	//   Visualization attributes
	//===============================

	// Mother volume of WaterPhantom
	G4VisAttributes* phantomVisAtt = new G4VisAttributes(G4Colour(1.0,1.0,0.0));
	detector_log->SetVisAttributes(phantomVisAtt);

	// Replica
	G4VisAttributes* yRepVisAtt = new G4VisAttributes(G4Colour(0.0,1.0,0.0));
	logYRep->SetVisAttributes(yRepVisAtt);
	G4VisAttributes* xRepVisAtt = new G4VisAttributes(G4Colour(0.0,1.0,0.0));
	logXRep->SetVisAttributes(xRepVisAtt);

	// Skip the visualization for those voxels.
	fVoxelLogicalVolume->SetVisAttributes(G4VisAttributes::GetInvisible());

	fScoringVolumeVector.push_back(fVoxelLogicalVolume);

	if (!fpRegion)
	{
		fpRegion = new G4Region("Target");
		fVoxelLogicalVolume -> SetRegion(fpRegion);
		fpRegion->AddRootLogicalVolume( fVoxelLogicalVolume );
	}

	fVolumeOfVoxel = fNX * fNY * fNZ;
	fMassOfVoxel = fDetectorMaterial -> GetDensity() * fVolumeOfVoxel;

	//  This will clear the existing matrix (together with all data inside it)!
	matrix = DetectorMatrix::GetInstance(fNX, fNY, fNZ, fMassOfVoxel);

	return world_phys;
}

void PDD1DetectorConstruction::ConstructSDandField() {

	//  Sensitive Detector Manager.
	G4SDManager* pSDman = G4SDManager::GetSDMpointer();


	// Sensitive Detector Name
	G4String phantomSDname1 = "PhantomSD1";


	// Define MultiFunctionalDetector with name
	G4MultiFunctionalDetector* mFDet = new G4MultiFunctionalDetector(phantomSDname1);
	pSDman->AddNewDetector( mFDet );                		// Register SD to SDManager.
	fVoxelLogicalVolume->SetSensitiveDetector(mFDet);    		// Assign SD to the logical volume.

	G4String fltName,particleName;
	// filters
	G4SDParticleFilter* protonFilter = new G4SDParticleFilter(fltName="protonFilter", particleName="proton");

	G4String psName;
	// scorers
	G4PSEnergyDeposit3D * scorer0 = new G4PSEnergyDeposit3D(psName="totalEDep",fNX,fNY,fNZ);
	G4PSPassageCellCurrent3D * scorer1 = new G4PSPassageCellCurrent3D(psName="protonEDep",fNX,fNY,fNZ);
	scorer1->SetFilter(protonFilter);

	// register scorers to MultiFunctionalDetector
	mFDet->RegisterPrimitive(scorer0);
	mFDet->RegisterPrimitive(scorer1);


	// Sensitive Detector Name
	G4String phantomSDname2 = "PhantomSD2";

	// Sensitive detectors
	DetectorSD* phantomSD = new DetectorSD(phantomSDname2,"PhantomHitsCollection");
	pSDman->AddNewDetector(phantomSD);                			// Register SD to SDManager.
	SetSensitiveDetector( fVoxelLogicalVolume, phantomSD );    	// Assign SD to the logical volume.

}

G4bool PDD1DetectorConstruction::SetPhantomMaterial(G4String material)
{

	if (G4Material* pMat = Materials::GetInstance()->GetMaterial(material))
	{
		fPhantomMaterial  = pMat;
		if (fPhantomLogicalVolume)
		{
			fPhantomLogicalVolume ->  SetMaterial(pMat);

			G4RunManager::GetRunManager() -> PhysicsHasBeenModified();
			G4RunManager::GetRunManager() -> GeometryHasBeenModified();
			G4cout << "The material of phantom has been changed to " << material << G4endl;
		}
	}
	else
	{
		G4cout << "WARNING: material \"" << material << "\" doesn't exist in NIST elements/materials"
				" table [located in $G4INSTALL/source/materials/src/G4NistMaterialBuilder.cc]" << G4endl;
		G4cout << "Use command \"/parameter/nist\" to see full materials list!" << G4endl;
		return false;
	}

	return true;
}

G4bool PDD1DetectorConstruction::SetPhantomMaterial(G4String material, G4double concentration)
{

	if (G4Material* pMat = Materials::GetInstance()->GetMaterial(material,concentration))
	{
		fPhantomMaterial  = pMat;
		if (fPhantomLogicalVolume)
		{
			fPhantomLogicalVolume ->  SetMaterial(pMat);

			G4RunManager::GetRunManager() -> PhysicsHasBeenModified();
			G4RunManager::GetRunManager() -> GeometryHasBeenModified();
			G4cout << "The material of phantom has been changed to " << material << G4endl;
		}
	}
	else
	{
		G4cout << "WARNING: material \"" << material << "\" doesn't exist in NIST elements/materials"
				" table [located in $G4INSTALL/source/materials/src/G4NistMaterialBuilder.cc]" << G4endl;
		G4cout << "Use command \"/parameter/nist\" to see full materials list!" << G4endl;
		return false;
	}

	return true;
}

G4bool PDD1DetectorConstruction::SetDetectorMaterial(G4String material)
{

	if (G4Material* pMat = Materials::GetInstance()->GetMaterial(material))
	{
		fDetectorMaterial = pMat;
		if (fDetectorLogicalVolume)
		{
			fDetectorLogicalVolume -> SetMaterial(pMat);

			G4RunManager::GetRunManager() -> PhysicsHasBeenModified();
			G4RunManager::GetRunManager() -> GeometryHasBeenModified();
			G4cout << "The material of detector has been changed to " << material << G4endl;
		}
	}
	else
	{
		G4cout << "WARNING: material \"" << material << "\" doesn't exist in NIST elements/materials"
				" table [located in $G4INSTALL/source/materials/src/G4NistMaterialBuilder.cc]" << G4endl;
		G4cout << "Use command \"/parameter/nist\" to see full materials list!" << G4endl;
		return false;
	}

	return true;
}

G4bool PDD1DetectorConstruction::SetDetectorMaterial(G4String material, G4double concentration)
{

	if (G4Material* pMat = Materials::GetInstance()->GetMaterial(material,concentration))
	{
		fDetectorMaterial = pMat;
		if (fDetectorLogicalVolume)
		{
			fDetectorLogicalVolume -> SetMaterial(pMat);

			G4RunManager::GetRunManager() -> PhysicsHasBeenModified();
			G4RunManager::GetRunManager() -> GeometryHasBeenModified();
			G4cout << "The material of detector has been changed to " << material << G4endl;
		}
	}
	else
	{
		G4cout << "WARNING: material \"" << material << "\" doesn't exist in NIST elements/materials"
				" table [located in $G4INSTALL/source/materials/src/G4NistMaterialBuilder.cc]" << G4endl;
		G4cout << "Use command \"/parameter/nist\" to see full materials list!" << G4endl;
		return false;
	}

	return true;
}

void PDD1DetectorConstruction::SetPhantomSize(G4double sizeX, G4double sizeY, G4double sizeZ)
{
	if (sizeX > 0.) fPhantomSize.setX(sizeX);
	if (sizeY > 0.) fPhantomSize.setY(sizeY);
	if (sizeZ > 0.) fPhantomSize.setZ(sizeZ);
}

void PDD1DetectorConstruction::SetDetectorSize(G4double sizeX, G4double sizeY, G4double sizeZ)
{
	if (sizeX > 0.) {fDetectorSize.setX(sizeX);}
	if (sizeY > 0.) {fDetectorSize.setY(sizeY);}
	if (sizeZ > 0.) {fDetectorSize.setZ(sizeZ);}
}
