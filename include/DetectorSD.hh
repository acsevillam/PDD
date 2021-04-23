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

#ifndef SensitiveDetector_h
#define SensitiveDetector_h 1

// PDD1 Headers
#include "DetectorHit.hh"

// Geant4 Headers
#include "G4VSensitiveDetector.hh"

// C++ Headers
#include <vector>

class G4Step;
class G4HCofThisEvent;

// Sensitive detector class

class DetectorSD : public G4VSensitiveDetector
{
  public:
    DetectorSD(const G4String& name,
                const G4String& hitsCollectionName);
    virtual ~DetectorSD();
  
    // methods from base class
    virtual void   Initialize(G4HCofThisEvent* hitCollection);
    virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history);
    virtual void   EndOfEvent(G4HCofThisEvent* hitCollection);

  private:
    PDD1HitsCollection* fHitsCollection;

};


#endif // SensitiveDetector_h
