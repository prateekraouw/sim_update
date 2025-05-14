// ========================
// src/RunAction.cc
// ========================

#include "RunAction.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include <fstream>
#include <iostream>
#include <iomanip>
// Add this at the top of RunAction.cc
#include <mutex>


RunAction::RunAction()
: G4UserRunAction()
{
}

RunAction::~RunAction()
{
}

void RunAction::BeginOfRunAction(const G4Run* run)
{
  // Inform the runManager to save random number seed
  G4RunManager::GetRunManager()->SetRandomNumberStore(false);
  
  // Clear particle counters
  fParticleCounts.clear();
  fTotalEnergy.clear();
  
  G4cout << "### Run " << run->GetRunID() << " starts." << G4endl;
}

void RunAction::EndOfRunAction(const G4Run* run)
{
  G4int nofEvents = run->GetNumberOfEvent();
  if (nofEvents == 0) return;
  
  G4cout << "### Run " << run->GetRunID() << " completed." << G4endl;
  
  // Print particle summary
  PrintParticleSummary();
}

// Then update the AddParticle method
void RunAction::AddParticle(const G4String& detectorName, const G4String& particleName, G4double energy)
{
  // Only count muons and pions
  if (particleName == "mu+" || particleName == "mu-" || 
      particleName == "pi+" || particleName == "pi-" || 
      particleName == "pi0") {
    
    // Lock the mutex to ensure thread safety
    std::lock_guard<std::mutex> lock(fMutex);
    
    // Increment count
    fParticleCounts[detectorName][particleName]++;
    
    // Add energy
    fTotalEnergy[detectorName][particleName] += energy;
  }
}
void RunAction::PrintParticleSummary()
{
  G4cout << "\n\n";
  G4cout << "================================================================" << G4endl;
  G4cout << "                   PARTICLE DETECTION SUMMARY                    " << G4endl;
  G4cout << "================================================================" << G4endl;
  G4cout << std::setw(12) << "Detector" << " | " 
         << std::setw(10) << "Particle" << " | " 
         << std::setw(10) << "Count" << " | " 
         << std::setw(15) << "Total Energy" << " | " 
         << std::setw(15) << "Average Energy" << G4endl;
  G4cout << "----------------------------------------------------------------" << G4endl;
  
  // List of detectors to report on
  std::vector<G4String> detectors = {"Detector1", "Detector2", "Detector3"};
  
  // List of particle types to report on
  std::vector<G4String> particles = {"mu+", "mu-", "pi+", "pi-", "pi0"};
  
  // Track totals
  int totalParticles = 0;
  G4double totalEnergy = 0.0;
  
  // Loop through detectors
  for (const auto& detector : detectors) {
    bool detectorHasParticles = false;
    int detectorTotal = 0;
    G4double detectorEnergy = 0.0;
    
    // Loop through particle types
    for (const auto& particle : particles) {
      int count = fParticleCounts[detector][particle];
      if (count > 0) {
        detectorHasParticles = true;
        G4double energy = fTotalEnergy[detector][particle];
        G4double avgEnergy = (count > 0) ? energy / count : 0.0;
        
        G4cout << std::setw(12) << detector << " | " 
               << std::setw(10) << particle << " | " 
               << std::setw(10) << count << " | " 
               << std::setw(15) << G4BestUnit(energy, "Energy") << " | " 
               << std::setw(15) << G4BestUnit(avgEnergy, "Energy") << G4endl;
        
        detectorTotal += count;
        detectorEnergy += energy;
      }
    }
    
    if (detectorHasParticles) {
      G4double detectorAvgEnergy = (detectorTotal > 0) ? detectorEnergy / detectorTotal : 0.0;
      G4cout << "----------------------------------------------------------------" << G4endl;
      G4cout << std::setw(12) << detector << " | " 
             << std::setw(10) << "TOTAL" << " | " 
             << std::setw(10) << detectorTotal << " | " 
             << std::setw(15) << G4BestUnit(detectorEnergy, "Energy") << " | " 
             << std::setw(15) << G4BestUnit(detectorAvgEnergy, "Energy") << G4endl;
      G4cout << "----------------------------------------------------------------" << G4endl;
      
      totalParticles += detectorTotal;
      totalEnergy += detectorEnergy;
    }
  }
  
  // Print overall total
  G4double overallAvgEnergy = (totalParticles > 0) ? totalEnergy / totalParticles : 0.0;
  G4cout << std::setw(12) << "ALL" << " | " 
         << std::setw(10) << "TOTAL" << " | " 
         << std::setw(10) << totalParticles << " | " 
         << std::setw(15) << G4BestUnit(totalEnergy, "Energy") << " | " 
         << std::setw(15) << G4BestUnit(overallAvgEnergy, "Energy") << G4endl;
  G4cout << "================================================================" << G4endl;
}