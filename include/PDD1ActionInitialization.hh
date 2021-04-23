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

#ifndef PDD1ActionInitialization_h
#define PDD1ActionInitialization_h 1

#include "G4VUserActionInitialization.hh"

/// Action initialization class.

class PDD1ActionInitialization : public G4VUserActionInitialization
{
  public:
    PDD1ActionInitialization();
    virtual ~PDD1ActionInitialization();

    virtual void BuildForMaster() const;
    virtual void Build() const;
};

#endif // PDD1ActionInitialization_h

    
