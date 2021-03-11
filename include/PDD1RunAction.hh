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

class PDD1RunAction : public G4UserRunAction
{
public:
  // constructor and destructor
  PDD1RunAction();
  virtual ~PDD1RunAction();

public:
  // virtual method from G4UserRunAction.
  virtual G4Run* GenerateRun();
  virtual void BeginOfRunAction(const G4Run*);
  virtual void EndOfRunAction(const G4Run*);

	void CreateHistos();
	void CreateNTuples();

	G4int CopyNo(G4int ix, G4int iy, G4int iz) {  return (iy*(fNx*fNz)+ix*fNz+iz); }

	inline void AddTotalLetT(G4double value, G4int i) { totalLetT[i] += value; }
	inline void AddDtotalLetT(G4double value, G4int i) { DtotalLetT[i] += value; }
	inline void AddTotalLetD(G4double value, G4int i) { DtotalLetD[i] += value; }
	inline void AddDtotalLetD(G4double value, G4int i) { totalLetD[i] += value; }

private:

	// vector of MultiFunctionalDetecor names.
	std::vector<G4String>		fSDName;

	// for conversion of segment number to copyNo.
	G4int						fNx, fNy, fNz;

	G4double *totalLetT, *DtotalLetT , *DtotalLetD, *totalLetD;
};

//

#endif





