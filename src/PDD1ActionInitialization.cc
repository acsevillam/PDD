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
//

#include "PDD1ActionInitialization.hh"
#include "PDD1EventAction.hh"
#include "PDD1PrimaryGeneratorAction.hh"
#include "PDD1RunAction.hh"
#include "PDD1SteppingAction.hh"
#include "PDD1TrackingAction.hh"

PDD1ActionInitialization::PDD1ActionInitialization()
{;}

PDD1ActionInitialization::~PDD1ActionInitialization()
{;}

void PDD1ActionInitialization::Build() const
{

  SetUserAction(new PDD1PrimaryGeneratorAction);

  PDD1RunAction* runAction = new PDD1RunAction();
  SetUserAction(runAction);

  PDD1EventAction* eventAction = new PDD1EventAction(runAction);
  SetUserAction(eventAction);

  PDD1TrackingAction* trackingAction = new PDD1TrackingAction(eventAction);
  SetUserAction(trackingAction);

  SetUserAction(new PDD1SteppingAction(trackingAction));

}

void PDD1ActionInitialization::BuildForMaster() const
{

  G4UserRunAction* runAction = new PDD1RunAction;
  SetUserAction(runAction);

}

