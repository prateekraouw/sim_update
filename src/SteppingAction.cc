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
#include "G4LogicalVolumeStore.hh"
#include "G4ios.hh"

SteppingAction::SteppingAction(EventAction* eventAction)
: G4UserSteppingAction(),
  fEventAction(eventAction),
  fDetector1LV(nullptr),
  fDetector2LV(nullptr),
  fDetector3LV(nullptr),
  fHeliumCloudLV(nullptr),
  fRFCavityLV(nullptr)
{
  G4cout << "SteppingAction constructor called" << G4endl;
}

SteppingAction::~SteppingAction()
{
}

void SteppingAction::UserSteppingAction(const G4Step* step)
{
  // Get logical volumes if not yet set
  if (!fDetector1LV) {
    G4cout << "First step - initializing logical volume pointers..." << G4endl;
    G4LogicalVolumeStore* lvStore = G4LogicalVolumeStore::GetInstance();
    fDetector1LV = lvStore->GetVolume("Detector1");
    fDetector2LV = lvStore->GetVolume("Detector2");
    fDetector3LV = lvStore->GetVolume("Detector3");
    fHeliumCloudLV = lvStore->GetVolume("HeliumCloud");
    fRFCavityLV = lvStore->GetVolume("RFCavity");
    
    // Print status of logical volume pointers
    G4cout << "Detector1LV: " << (fDetector1LV ? "Found" : "Not Found") << G4endl;
    G4cout << "Detector2LV: " << (fDetector2LV ? "Found" : "Not Found") << G4endl;
    G4cout << "Detector3LV: " << (fDetector3LV ? "Found" : "Not Found") << G4endl;
    G4cout << "HeliumCloudLV: " << (fHeliumCloudLV ? "Found" : "Not Found") << G4endl;
    G4cout << "RFCavityLV: " << (fRFCavityLV ? "Found" : "Not Found") << G4endl;
  }
  
  // Get track and event information
  G4Track* track = step->GetTrack();
  G4int trackID = track->GetTrackID();
  G4int eventID = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
  
  // Get particle information
  G4ParticleDefinition* particle = track->GetDefinition();
  G4String particleName = particle->GetParticleName();
  G4double energy = track->GetKineticEnergy();
  
  // Get step information
  G4StepPoint* prePoint = step->GetPreStepPoint();
  G4StepPoint* postPoint = step->GetPostStepPoint();
  G4ThreeVector prePos = prePoint->GetPosition();
  G4ThreeVector postPos = postPoint->GetPosition();
  G4ThreeVector momentum = track->GetMomentum();
  G4double stepLength = step->GetStepLength();
  G4String processName = "InitialStep";
  if (step->GetPostStepPoint()->GetProcessDefinedStep()) {
    processName = step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();
  }
  
  // Get volume information
  G4LogicalVolume* volume = prePoint->GetTouchableHandle()->GetVolume()->GetLogicalVolume();
  G4String volumeName = "Unknown";
  if (volume) {
    volumeName = volume->GetName();
  }
  
  // Print step information
  G4cout << "========== Step Information ==========" << G4endl;
  G4cout << "Event ID: " << eventID << ", Track ID: " << trackID << G4endl;
  G4cout << "Particle: " << particleName << ", Energy: " << energy/GeV << " GeV" << G4endl;
  G4cout << "Volume: " << volumeName << G4endl;
  G4cout << "Process: " << processName << G4endl;
  G4cout << "Step Length: " << stepLength/cm << " cm" << G4endl;
  G4cout << "Pre-position: " << prePos/cm << " cm" << G4endl;
  G4cout << "Post-position: " << postPos/cm << " cm" << G4endl;
  G4cout << "Momentum: " << momentum/GeV << " GeV" << G4endl;
  G4cout << "========================================" << G4endl;
  
  // Energy threshold for neutrons, electrons, and photons (8 GeV)
  G4double energyThreshold = 8.0*GeV;
  
  // Kill neutrons, electrons, and gammas below threshold
  if ((particle == G4Neutron::Definition() || 
       particle == G4Electron::Definition() || 
       particle == G4Gamma::Definition()) && 
      energy < energyThreshold) {
    G4cout << "Killing particle: " << particleName << " with energy " << energy/GeV << " GeV (below threshold)" << G4endl;
    track->SetTrackStatus(fStopAndKill);
    return;
  }
  
  // Process hits in the detectors
  G4String detectorName = "";
  if (volume == fDetector1LV) {
    detectorName = "Detector1";
    G4cout << "Hit detected in Detector1" << G4endl;
  } else if (volume == fDetector2LV) {
    detectorName = "Detector2";
    G4cout << "Hit detected in Detector2" << G4endl;
  } else if (volume == fDetector3LV) {
    detectorName = "Detector3";
    G4cout << "Hit detected in Detector3" << G4endl;
  } else {
    // Not in a detector, apply other physics
    
    // Apply helium cloud physics (reduce transverse momentum)
    if (volume == fHeliumCloudLV) {
      G4cout << "Particle in Helium Cloud - reducing transverse momentum" << G4endl;
      
      // Get momentum components
      G4ThreeVector momentum = track->GetMomentum();
      G4double px = momentum.x();
      G4double py = momentum.y();
      G4double pz = momentum.z();
      
      G4cout << "Before reduction - px: " << px/GeV << " GeV, py: " << py/GeV << " GeV, pz: " << pz/GeV << " GeV" << G4endl;
      
      // Apply reduction to transverse momentum components
      // Simple model: Reduce by 2% per step
      G4double reductionFactor = 0.98;
      px *= reductionFactor;
      py *= reductionFactor;
      
      G4cout << "After reduction - px: " << px/GeV << " GeV, py: " << py/GeV << " GeV, pz: " << pz/GeV << " GeV" << G4endl;
      
      // Create a new momentum vector
      G4ThreeVector newMomentum(px, py, pz);
      
      // Calculate new momentum direction and magnitude
      G4ThreeVector newDirection = newMomentum.unit();
      G4double newMomentumMag = newMomentum.mag();
      
      // Set the new momentum direction
      track->SetMomentumDirection(newDirection);
      
      // Set the new kinetic energy based on the new momentum
      G4double mass = track->GetDefinition()->GetPDGMass();
      G4double newEnergy = std::sqrt(newMomentumMag*newMomentumMag + mass*mass) - mass;
      track->SetKineticEnergy(newEnergy);
      
      G4cout << "New momentum direction: " << newDirection << G4endl;
      G4cout << "New energy: " << newEnergy/GeV << " GeV" << G4endl;
    }
    
    // Apply RF cavity physics (accelerate in z direction)
    if (volume == fRFCavityLV) {
      G4cout << "Particle in RF Cavity - accelerating in z direction" << G4endl;
      
      // Get momentum components
      G4ThreeVector momentum = track->GetMomentum();
      G4double px = momentum.x();
      G4double py = momentum.y();
      G4double pz = momentum.z();
      
      G4cout << "Before acceleration - px: " << px/GeV << " GeV, py: " << py/GeV << " GeV, pz: " << pz/GeV << " GeV" << G4endl;
      
      // Accelerate in z direction
      // Simple model: Increase z momentum by 0.5% per step
      G4double accelerationFactor = 1.005;
      pz *= accelerationFactor;
      
      G4cout << "After acceleration - px: " << px/GeV << " GeV, py: " << py/GeV << " GeV, pz: " << pz/GeV << " GeV" << G4endl;
      
      // Create a new momentum vector
      G4ThreeVector newMomentum(px, py, pz);
      
      // Calculate new momentum direction and magnitude
      G4ThreeVector newDirection = newMomentum.unit();
      G4double newMomentumMag = newMomentum.mag();
      
      // Set the new momentum direction
      track->SetMomentumDirection(newDirection);
      
      // Set the new kinetic energy based on the new momentum
      G4double mass = track->GetDefinition()->GetPDGMass();
      G4double newEnergy = std::sqrt(newMomentumMag*newMomentumMag + mass*mass) - mass;
      track->SetKineticEnergy(newEnergy);
      
      G4cout << "New momentum direction: " << newDirection << G4endl;
      G4cout << "New energy: " << newEnergy/GeV << " GeV" << G4endl;
    }
    
    return;
  }
  
  // Record particle data in detector
  G4cout << "Recording " << particleName << " in " << detectorName << " with energy " << energy/GeV << " GeV" << G4endl;
  fEventAction->AddParticleData(detectorName, particleName, energy);
  
  // Record 6D vector at detector
  G4ThreeVector position = prePoint->GetPosition();
  G4ThreeVector detector_momentum = prePoint->GetMomentum();
  
  G4cout << "Recording 6D vector: " << G4endl;
  G4cout << "  Position: (" << position.x()/cm << ", " << position.y()/cm << ", " << position.z()/cm << ") cm" << G4endl;
  G4cout << "  Momentum: (" << detector_momentum.x()/GeV << ", " << detector_momentum.y()/GeV << ", " << detector_momentum.z()/GeV << ") GeV" << G4endl;
  
  fEventAction->Add6DVector(detectorName, 
                          position.x(), detector_momentum.x(),
                          position.y(), detector_momentum.y(),
                          position.z(), detector_momentum.z());
}