// =================================
// src/PrimaryGeneratorAction.cc
// =================================

#include "PrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4Event.hh"
#include "G4RotationMatrix.hh"

PrimaryGeneratorAction::PrimaryGeneratorAction()
: G4VUserPrimaryGeneratorAction(),
  fParticleGun(nullptr)
{
  G4int nParticles = 1;
  fParticleGun = new G4ParticleGun(nParticles);
  
  // Default particle
  G4ParticleDefinition* particleDefinition = 
    G4ParticleTable::GetParticleTable()->FindParticle("proton");
  fParticleGun->SetParticleDefinition(particleDefinition);
  
  // Set initial energy (Example: 10 GeV protons)
  fParticleGun->SetParticleEnergy(8.*GeV);
  
  // Set initial position (start before the tungsten block)
  fParticleGun->SetParticlePosition(G4ThreeVector(0, 0, -50.*cm));
  
  // Set direction (toward the tungsten block with 10 degrees angle)
  G4double angle = 10.*deg;
  G4ThreeVector direction(0, std::sin(angle), std::cos(angle));
  fParticleGun->SetParticleMomentumDirection(direction);
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete fParticleGun;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
  fParticleGun->GeneratePrimaryVertex(event);
}
