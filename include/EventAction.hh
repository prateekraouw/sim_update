// ============================
// include/EventAction.hh
// ============================

#ifndef EventAction_h
#define EventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"
#include <map>
#include <vector>



class G4Event;

class EventAction : public G4UserEventAction
{
  public:
    EventAction();
    virtual ~EventAction();
    
    virtual void BeginOfEventAction(const G4Event*);
    virtual void EndOfEventAction(const G4Event*);
    
    // Methods to store 6D vector data for later output
    void Add6DVector(const G4String& detectorName, 
                    G4double x, G4double px, G4double y, G4double py, G4double z, G4double pz);
    
    // Method to store particle data
    void AddParticleData(const G4String& detectorName,
                        const G4String& particleName, G4double energy);
    
  private:
    // Structure to hold 6D vector data (x, px, y, py, z, pz)
    struct Vector6D {
      G4double x, px, y, py, z, pz;
    };
    
    // Structure for particle data
    struct ParticleData {
      G4String name;
      G4double energy;
    };
    
    // Maps to store data by detector
    std::map<G4String, std::vector<Vector6D>> fVector6DMap;
    std::map<G4String, std::vector<ParticleData>> fParticleDataMap;
};

#endif
