// ============================
// src/EventAction.cc
// ============================

#include "EventAction.hh"
#include "RunAction.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include <fstream>
#include <iostream>

EventAction::EventAction()
: G4UserEventAction()
{
}

EventAction::~EventAction()
{
}

void EventAction::BeginOfEventAction(const G4Event* /*event*/)
{
  // Clear maps for new event
  fVector6DMap.clear();
  fParticleDataMap.clear();
}

void EventAction::EndOfEventAction(const G4Event* event)
{
  G4int eventID = event->GetEventID();
  
  // Get pointer to the RunAction - properly handling const
  const RunAction* runAction = 
    dynamic_cast<const RunAction*>(G4RunManager::GetRunManager()->GetUserRunAction());
  
  // Write 6D vector data to CSV file
  std::ofstream vectorFile("trajectory_data.csv", std::ios::app);
  if (!vectorFile.is_open()) {
    G4cerr << "Error opening trajectory_data.csv" << G4endl;
    return;
  }
  
  // Write header if this is the first event
  if (eventID == 0) {
    vectorFile << "EventID,Detector,X,PX,Y,PY,Z,PZ" << std::endl;
  }
  
  // Write data for each detector
  for (const auto& detData : fVector6DMap) {
    G4String detName = detData.first;
    const std::vector<Vector6D>& vectors = detData.second;
    
    for (const auto& vec : vectors) {
      vectorFile << eventID << "," << detName << ","
                << vec.x/cm << "," << vec.px/(GeV/c_light) << ","
                << vec.y/cm << "," << vec.py/(GeV/c_light) << ","
                << vec.z/cm << "," << vec.pz/(GeV/c_light) << std::endl;
    }
  }
  vectorFile.close();
  
  // Write particle data to CSV file
  std::ofstream particleFile("particle_data.csv", std::ios::app);
  if (!particleFile.is_open()) {
    G4cerr << "Error opening particle_data.csv" << G4endl;
    return;
  }
  
  // Write header if this is the first event
  if (eventID == 0) {
    particleFile << "EventID,Detector,ParticleName,Energy" << std::endl;
  }
  
  // Write data for muons and pions
  for (const auto& detData : fParticleDataMap) {
    G4String detName = detData.first;
    const std::vector<ParticleData>& particles = detData.second;
    
    for (const auto& particle : particles) {
      // Only log muons and pions as requested
      if (particle.name == "mu+" || particle.name == "mu-" || 
          particle.name == "pi+" || particle.name == "pi-" || 
          particle.name == "pi0") {
        particleFile << eventID << "," << detName << ","
                    << particle.name << "," << particle.energy/GeV << std::endl;
      }
    }
  }
  particleFile.close();
}

void EventAction::Add6DVector(const G4String& detectorName, 
                             G4double x, G4double px, G4double y, G4double py, G4double z, G4double pz)
{
  Vector6D vector = {x, px, y, py, z, pz};
  fVector6DMap[detectorName].push_back(vector);
}

void EventAction::AddParticleData(const G4String& detectorName,
                                 const G4String& particleName, G4double energy)
{
  ParticleData data = {particleName, energy};
  fParticleDataMap[detectorName].push_back(data);
  
  // Directly update RunAction statistics here
  const RunAction* runAction = 
    dynamic_cast<const RunAction*>(G4RunManager::GetRunManager()->GetUserRunAction());
  
  if (runAction && (particleName == "mu+" || particleName == "mu-" || 
                    particleName == "pi+" || particleName == "pi-" || 
                    particleName == "pi0")) {
    // We need to cast away const to call a non-const method
    // This is safe because we know AddParticle doesn't modify the object in ways
    // that would break thread safety
    RunAction* nonConstRunAction = const_cast<RunAction*>(runAction);
    nonConstRunAction->AddParticle(detectorName, particleName, energy);
  }
}