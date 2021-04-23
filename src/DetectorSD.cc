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
#include "DetectorSD.hh"
#include "DetectorMatrix.hh"

// Geant4 Headers
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include "G4Box.hh"


DetectorSD::DetectorSD(const G4String& name,
		const G4String& hitsCollectionName)
: G4VSensitiveDetector(name),
  fHitsCollection(NULL)
{
	collectionName.insert(hitsCollectionName);
}

DetectorSD::~DetectorSD()
{}

void DetectorSD::Initialize(G4HCofThisEvent* HCE)
{
	// Create hits collection
	fHitsCollection = new PDD1HitsCollection(SensitiveDetectorName, collectionName[0]);

	// Add this collection in hce
	G4int HCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
	HCE->AddHitsCollection( HCID, fHitsCollection );
}

G4bool DetectorSD::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{  

	// Track
	G4Track * theTrack = aStep -> GetTrack();

	// Particle definition
	G4ParticleDefinition* particleDef = theTrack->GetDefinition();

	// Particle code
	G4int PDGCode = particleDef->GetPDGEncoding();

	if (PDGCode == 11) return false;

	// Energy deposit
	G4double eDep = aStep->GetTotalEnergyDeposit();

	// Get the secondary particles in current step
	G4Step fstep = *theTrack -> GetStep();

	// Store all the secondary particles in current step
	const std::vector<const G4Track*> * secondary = fstep.GetSecondaryInCurrentStep();

	size_t secondariesSize = (*secondary).size();
	G4double secondariesEDep = 0.;

	// Get secondary electrons energy deposited
	if (secondariesSize)
	{
		for (size_t numsec = 0; numsec< secondariesSize ; numsec ++)
		{
			G4int secondaryPDGCode=(*secondary)[numsec]->GetDefinition()->GetPDGEncoding();

			if(secondaryPDGCode == 11)
			{
				// Calculate the energy deposit of secondary electrons in current step
				secondariesEDep += (*secondary)[numsec]->GetKineticEnergy();
			}
		}

	}

	if (eDep==0. && secondariesEDep==0.) return false;

	// Particle name
	G4String particleName =  particleDef -> GetParticleName();

	// TrackID
	G4int trackID = theTrack -> GetTrackID();

	// Step length
	G4double DX = aStep -> GetStepLength();

	// Atomic number
	//G4int Z = particleDef-> GetAtomicNumber();

	// Atomic mass
	//G4int A = particleDef-> GetAtomicMass();

	// Read voxel indexes: i is the x index, k is the z index
	const G4VTouchable* touchable = aStep->GetPreStepPoint()->GetTouchable();
	G4int k  = touchable->GetReplicaNumber(0);
	G4int j  = touchable->GetReplicaNumber(1);
	G4int i  = touchable->GetReplicaNumber(2);

    // Pre-step kinetic energy
    G4double kinEPre = aStep -> GetPreStepPoint() -> GetKineticEnergy();

    // Post-step kinetic energy
    G4double kinEPost = aStep -> GetPostStepPoint() -> GetKineticEnergy();

    // Step average kinetic energy
    G4double kinEMean = (kinEPre + kinEPost) * 0.5;

    // Material
    G4Material * mat = aStep -> GetPreStepPoint() -> GetMaterial();

    // Volume

    G4Box* voxel_geo = (G4Box*) touchable->GetVolume(0)->GetLogicalVolume()->GetSolid();
    G4double vol = voxel_geo->GetCubicVolume();

	DetectorMatrix* matrix = DetectorMatrix::GetInstance();

	if (matrix)
	{

		DetectorHit* detectorHit = new DetectorHit();

		detectorHit->SetTrackID (trackID);
		detectorHit->SetIx (i);
		detectorHit->SetIy (j);
		detectorHit->SetIz (k);
		detectorHit->SetEdep (eDep);
		detectorHit->SetSecondariesEdep (secondariesEDep);
		detectorHit->SetDX (DX);
		detectorHit->SetKinEMean (kinEMean);
		detectorHit->SetPos (aStep->GetPreStepPoint()->GetPosition());
		detectorHit->SetParticleDef (particleDef);
		detectorHit->SetMat (mat);
		detectorHit->SetVol (vol);

		fHitsCollection -> insert(detectorHit);

		//detectorHit->Print();
	}

	return true;
}

void DetectorSD::EndOfEvent(G4HCofThisEvent* HCE)
{
	if ( verboseLevel>1 ) {
		G4int nofHits = fHitsCollection->entries();
		G4cout
		<< G4endl
		<< "-------->Hits Collection: in this event they are " << nofHits
		<< " hits in the tracker chambers: " << G4endl;
		for ( G4int i=0; i<nofHits; i++ ) (*fHitsCollection)[i]->Print();
	}

	static G4int HCID = -1;
	if(HCID < 0)
	{
		HCID = GetCollectionID(0);
	}

	HCE -> AddHitsCollection(HCID,fHitsCollection);

}
