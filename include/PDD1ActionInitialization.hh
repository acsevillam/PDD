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

#ifndef PDD1ActionInitialization_H
#define PDD1ActionInitialization_H 1

#include "globals.hh"
#include "G4VUserActionInitialization.hh"

class PDD1ActionInitialization : public G4VUserActionInitialization
{
  public:
  PDD1ActionInitialization();//G4bool bParallelWorld);
    virtual ~PDD1ActionInitialization();

    virtual void Build() const;
    virtual void BuildForMaster() const;

private:
};

#endif

