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

#ifndef DetectorHit_h
#define DetectorHit_h 1

// Geant4 Headers
#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4ParticleDefinition.hh"
#include "G4Material.hh"
#include "tls.hh"

class DetectorHit : public G4VHit
{
  public:
    DetectorHit();
    DetectorHit(const DetectorHit&);
    virtual ~DetectorHit();

    // operators
    const DetectorHit& operator=(const DetectorHit&);
    G4bool operator==(const DetectorHit&) const;

    inline void* operator new(size_t);
    inline void  operator delete(void*);

    // methods from base class
    virtual void Draw();
    virtual void Print();

    // Set methods
    void SetTrackID (G4int track) { fTrackID = track; };
    void SetIx (G4int ix) { fix = ix; };
    void SetIy (G4int iy) { fiy = iy; };
    void SetIz (G4int iz) { fiz = iz; };
    void SetEdep (G4double eDep) { fEDep = eDep; };
    void SetSecondariesEdep (G4double eDep) { fSecondariesEDep = eDep; };
    void SetDX (G4double dx) { fDX = dx; };
    void SetKinEMean (G4double kinEMean) { fKinEMean = kinEMean; };
    void SetPos (G4ThreeVector xyz)	{ fPos = xyz; };
    void SetParticleDef (G4ParticleDefinition* particleDef)	{ fParticleDef = particleDef; };
    void SetMat (G4Material* mat)	{ fMat = mat; };
    void SetVol (G4double vol)	{ fVol = vol; };

    // Get methods
    G4int GetTrackID() const { return fTrackID; };
    G4int GetIx() const { return fix; };
    G4int GetIy() const { return fiy; };
    G4int GetIz() const { return fiz; };
    G4double GetEdep() const { return fEDep; };
    G4double GetSecondariesEdep() const { return fSecondariesEDep; };
    G4double GetDX() const { return fDX; };
    G4double GetKinEMean() const { return fKinEMean; };
    G4ThreeVector GetPos()	const 				{ return fPos; };
    G4ParticleDefinition* GetParticleDef()	const 		{ return fParticleDef; };
    G4Material* GetMat() const { return fMat; };
    G4double GetVol() const { return fVol; };

  private:

      G4int         fTrackID;
      G4int         fix;
      G4int         fiy;
      G4int         fiz;
      G4double      fEDep;
      G4double      fSecondariesEDep;
      G4double      fDX;
      G4double      fKinEMean;
      G4ThreeVector fPos;
      G4ParticleDefinition* fParticleDef;
      G4Material* 	fMat;
      G4double 	fVol;
};

typedef G4THitsCollection<DetectorHit> PDD1HitsCollection;

extern G4ThreadLocal G4Allocator<DetectorHit>* PDD1HitAllocator;

inline void* DetectorHit::operator new(size_t)
{

 if(!PDD1HitAllocator)
	 PDD1HitAllocator= new G4Allocator<DetectorHit>;
 void *aHit;

 aHit = (void *) PDD1HitAllocator->MallocSingle();
 return aHit;

}

inline void DetectorHit::operator delete(void *aHit)
{
if(!PDD1HitAllocator)
	PDD1HitAllocator= new G4Allocator<DetectorHit>;

PDD1HitAllocator->FreeSingle((DetectorHit*) aHit);
}

#endif // DetectorHit_h
