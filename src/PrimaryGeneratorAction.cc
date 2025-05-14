// =================================
// src/PrimaryGeneratorAction.cc
// =================================

#include "PrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4Event.hh"
#include "G4RotationMatrix.hh"
#include "G4GeneralParticleSource.hh"

PrimaryGeneratorAction::PrimaryGeneratorAction()
: G4VUserPrimaryGeneratorAction(),
  fParticleGun(nullptr)
{
  // Use particle gun with higher energy and more particles
  G4int nParticles = 1;
  fParticleGun = new G4ParticleGun(nParticles);
  
  // Default particle: proton with higher energy
  G4ParticleDefinition* particleDefinition = 
    G4ParticleTable::GetParticleTable()->FindParticle("proton");
  fParticleGun->SetParticleDefinition(particleDefinition);
  
  // Increase initial energy to 50 GeV for better particle production
  fParticleGun->SetParticleEnergy(8.*GeV);
  
  // Set initial position (start before the tungsten block)
  fParticleGun->SetParticlePosition(G4ThreeVector(0, 0, -10.*cm));
  
  // Set direction aligned with the tungsten block rotation
  G4double angle = 10.*deg;
  G4ThreeVector direction(0, std::sin(angle), std::cos(angle));
  fParticleGun->SetParticleMomentumDirection(direction);
  
  // Print configuration
  G4cout << "Primary particle: proton" << G4endl;
  G4cout << "Energy: " << 50.0 << " GeV" << G4endl;
  G4cout << "Position: (0, 0, -50) cm" << G4endl;
  G4cout << "Direction: " << direction << G4endl;
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete fParticleGun;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
  // Generate the primary vertex
  fParticleGun->GeneratePrimaryVertex(event);
}