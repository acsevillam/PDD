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
#include "DetectorHit.hh"

// Geant4 Headers
#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

// C++ Headers
#include <iomanip>

G4ThreadLocal G4Allocator<DetectorHit>* PDD1HitAllocator=0;

DetectorHit::DetectorHit()
: G4VHit(),
  fTrackID(-1),
  fix(-1),
  fiy(-1),
  fiz(-1),
  fEDep(0.),
  fSecondariesEDep(0.),
  fDX(0.),
  fKinEMean(0.),
  fPos(G4ThreeVector()),
  fParticleDef(0),
  fMat(0),
  fVol(0.)
{}

DetectorHit::~DetectorHit()
{}

DetectorHit::DetectorHit(const DetectorHit& right)
: G4VHit()
{
	fTrackID			= right.fTrackID;
	fix					= right.fix;
	fiy					= right.fiy;
	fiz					= right.fiz;
	fEDep				= right.fEDep;
	fSecondariesEDep	= right.fSecondariesEDep;
	fDX					= right.fDX;
	fKinEMean			= right.fKinEMean;
	fPos				= right.fPos;
	fParticleDef		= right.fParticleDef;
	fMat				= right.fMat;
	fVol				= right.fVol;
}

const DetectorHit& DetectorHit::operator=(const DetectorHit& right)
{
	fTrackID			= right.fTrackID;
	fix					= right.fix;
	fiy					= right.fiy;
	fiz					= right.fiz;
	fEDep				= right.fEDep;
	fSecondariesEDep	= right.fSecondariesEDep;
	fDX					= right.fDX;
	fKinEMean			= right.fKinEMean;
	fPos				= right.fPos;
	fParticleDef		= right.fParticleDef;
	fMat				= right.fMat;
	fVol				= right.fVol;

	return *this;
}

G4bool DetectorHit::operator==(const DetectorHit& right) const
								{
	return ( this == &right ) ? true : false;
								}

void DetectorHit::Draw()
{
	G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
	if(pVVisManager)
	{
		G4Circle circle(fPos);
		circle.SetScreenSize(4.);
		circle.SetFillStyle(G4Circle::filled);
		G4Colour colour(1.,0.,0.);
		G4VisAttributes attribs(colour);
		circle.SetVisAttributes(attribs);
		pVVisManager->Draw(circle);
	}
}

void DetectorHit::Print()
{
	G4cout
	<< "TrackID: " << std::setw(3) << fTrackID << " "
	<< "ix: " << std::setw(3) << fix << " "
	<< "iy: " << std::setw(3) << fiy << " "
	<< "iz: " << std::setw(3) << fiz << " "
	<< "Edep: " << std::setw(7) << G4BestUnit(fEDep,"Energy") << " "
	<< "SecondariesEdep: " << std::setw(7) << G4BestUnit(fSecondariesEDep,"Energy") << " "
	<< "KinEMean: " << std::setw(7) << G4BestUnit(fKinEMean,"Energy") << " "
	<< "Position: " << std::setw(7) << G4BestUnit( fPos,"Length")
	<< "ParticleName: " << std::setw(7) << fParticleDef->GetParticleName()
	<< "Mat: " << std::setw(7) << fMat->GetName()
	<< G4endl;
}
