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

#ifndef PDD1TrackInformation_h
#define PDD1TrackInformation_h 1

#include "G4Types.hh"
#include "G4VUserTrackInformation.hh"

class PDD1TrackInformation : public G4VUserTrackInformation
{
public:

	PDD1TrackInformation();

	virtual ~PDD1TrackInformation();

	void Print() const {};

	inline void SetParentName(G4String value) { fParentName = value; }
	inline G4String GetParentName() const { return fParentName; }

private:
	G4String fParentName;
};
#endif // PDD1TrackInformation_h
