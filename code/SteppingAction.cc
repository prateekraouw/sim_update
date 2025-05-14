// =============================
// src/SteppingAction.cc
// =============================
#include "SteppingAction.hh"
#include "EventAction.hh"
#include "G4Step.hh"
#include "G4RunManager.hh"
#include "G4LogicalVolume.hh"
#include "G4SystemOfUnits.hh"
#include "G4StepPoint.hh"
#include "G4Track.hh"
#include "G4VPhysicalVolume.hh"
#include "G4ParticleDefinition.hh"
#include "G4Neutron.hh"
#include "G4Electron.hh"
#include "G4Gamma.hh"

SteppingAction::SteppingAction(EventAction* eventAction)
: G4UserSteppingAction(),
  fEventAction(eventAction),
  fDetector1LV(nullptr),
  fDetector2LV(nullptr),
  fDetector3LV(nullptr),
  fHeliumCloudLV(nullptr),
  fRFCavityLV(nullptr)
{
}

SteppingAction::~SteppingAction()
{
}

void SteppingAction::UserSteppingAction(const G4Step* step)
{
  // Get logical volumes if not yet set
  if (!fDetector1LV) {
    const G4LogicalVolumeStore* lvStore = G4LogicalVolumeStore::GetInstance();
    fDetector1LV = lvStore->GetVolume("Detector1");
    fDetector2LV = lvStore->GetVolume("Detector2");
    fDetector3LV = lvStore->GetVolume("Detector3");
    fHeliumCloudLV = lvStore->GetVolume("HeliumCloud");
    fRFCavityLV = lvStore->GetVolume("RFCavity");
  }
  
  // Get track and particle information
  G4Track* track = step->GetTrack();
  G4ParticleDefinition* particle = track->GetDefinition();
  G4String particleName = particle->GetParticleName();
  G4double energy = track->GetKineticEnergy();
  
  // Energy threshold for neutrons, electrons, and photons (8 GeV)
  G4double energyThreshold = 8.0*GeV;
  
  // Kill neutrons, electrons, and gammas below threshold
  if ((particle == G4Neutron::Definition() || 
       particle == G4Electron::Definition() || 
       particle == G4Gamma::Definition()) && 
      energy < energyThreshold) {
    track->SetTrackStatus(fStopAndKill);
    return;
  }
  
  // Get volume of the current step
  G4LogicalVolume* volume = step->GetPreStepPoint()->GetTouchableHandle()
                           ->GetVolume()->GetLogicalVolume();
                           
  // Process hits in the detectors
  G4String detectorName = "";
  if (volume == fDetector1LV) {
    detectorName = "Detector1";
  } else if (volume == fDetector2LV) {
    detectorName = "Detector2";
  } else if (volume == fDetector3LV) {
    detectorName = "Detector3";
  } else {
    // Not in a detector, apply other physics
    
    // Apply helium cloud physics (reduce transverse momentum)
    if (volume == fHeliumCloudLV) {
      // Get momentum components
      G4ThreeVector momentum = track->GetMomentum();
      G4double px = momentum.x();
      G4double py = momentum.y();
      G4double pz = momentum.z();
      
      // Apply reduction to transverse momentum components
      // Simple model: Reduce by 2% per step
      G4double reductionFactor = 0.98;
      px *= reductionFactor;
      py *= reductionFactor;
      
      // Set the new momentum
      track->SetMomentum(G4ThreeVector(px, py, pz));
    }
    
    // Apply RF cavity physics (accelerate in z direction)
    if (volume == fRFCavityLV) {
      // Get momentum components
      G4ThreeVector momentum = track->GetMomentum();
      G4double px = momentum.x();
      G4double py = momentum.y();
      G4double pz = momentum.z();
      
      // Accelerate in z direction
      // Simple model: Increase z momentum by 0.5% per step
      G4double accelerationFactor = 1.005;
      pz *= accelerationFactor;
      
      // Set the new momentum
      track->SetMomentum(G4ThreeVector(px, py, pz));
    }
    
    return;
  }
  
  // Record particle data in detector
  fEventAction->AddParticleData(detectorName, particleName, energy);
  
  // Record 6D vector at detector
  G4ThreeVector position = step->GetPreStepPoint()->GetPosition();
  G4ThreeVector momentum = step->GetPreStepPoint()->GetMomentum();
  
  fEventAction->Add6DVector(detectorName, 
                          position.x(), momentum.x(),
                          position.y(), momentum.y(),
                          position.z(), momentum.z());
}
