// ============================
// src/ActionInitialization.cc
// ============================

#include "../include/ActionInitialization.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "SteppingAction.hh"
#include "DetectorConstruction.hh"

ActionInitialization::ActionInitialization(DetectorConstruction* detConstruction)
: G4VUserActionInitialization(),
  fDetectorConstruction(detConstruction)
{
}

ActionInitialization::~ActionInitialization()
{
}

void ActionInitialization::BuildForMaster() const
{
  SetUserAction(new RunAction());
}

void ActionInitialization::Build() const
{
  // Primary generator
  SetUserAction(new PrimaryGeneratorAction());
  
  // Run action
  RunAction* runAction = new RunAction();
  SetUserAction(runAction);
  
  // Event action
  EventAction* eventAction = new EventAction();
  SetUserAction(eventAction);
  
  // Stepping action
  SetUserAction(new SteppingAction(eventAction));
}
