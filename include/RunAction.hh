// ========================
// include/RunAction.hh
// ========================

#ifndef RunAction_h
#define RunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"
#include <map>
#include <string>
#include <mutex>

class G4Run;

class RunAction : public G4UserRunAction
{
  public:
    RunAction();
    virtual ~RunAction();
    
    virtual void BeginOfRunAction(const G4Run*);
    virtual void EndOfRunAction(const G4Run*);
    
    // Methods to add particles to the counters
    // Note: This method is thread-safe
    void AddParticle(const G4String& detectorName, const G4String& particleName, G4double energy);
    
  private:
    // Map to store particle counts: [detector_name][particle_name] = count
    std::map<G4String, std::map<G4String, int>> fParticleCounts;
    
    // Map to store total energy: [detector_name][particle_name] = total_energy
    std::map<G4String, std::map<G4String, G4double>> fTotalEnergy;
    
    // Mutex to protect thread access to the maps
    std::mutex fMutex;
    
    // Method to print the particle summary
    void PrintParticleSummary();
};

#endif