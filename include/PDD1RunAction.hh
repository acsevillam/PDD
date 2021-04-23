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

#ifndef PDD1RunAction_h
#define PDD1RunAction_h 1

#include "G4UserRunAction.hh"
#include "G4Accumulable.hh"
#include "globals.hh"
#include <vector>

class G4Run;

/// Run action class
///
/// In EndOfRunAction(), it calculates the dose in the selected volume 
/// from the energy deposit accumulated via stepping and event actions.
/// The computed dose is then printed on the screen.

class PDD1RunAction : public G4UserRunAction
{
  public:
    PDD1RunAction();
    virtual ~PDD1RunAction();

    virtual void BeginOfRunAction(const G4Run*);
    virtual void EndOfRunAction(const G4Run*);

    void CreateHistos();
    void CreateNTuples();
    inline void AddEdep(G4double edep){fEdep += edep; fEdep2 += edep*edep;}

  private:

    G4Accumulable<G4double> fEdep;
    G4Accumulable<G4double> fEdep2;
};

#endif

