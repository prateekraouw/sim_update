// =============================
// include/SteppingAction.hh
// =============================

#ifndef SteppingAction_h
#define SteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "globals.hh"

class EventAction;
class G4LogicalVolume;

class SteppingAction : public G4UserSteppingAction
{
  public:
    SteppingAction(EventAction* eventAction);
    virtual ~SteppingAction();
    
    virtual void UserSteppingAction(const G4Step*);
    
  private:
    EventAction* fEventAction;
    G4LogicalVolume* fDetector1LV;
    G4LogicalVolume* fDetector2LV;
    G4LogicalVolume* fDetector3LV;
    G4LogicalVolume* fHeliumCloudLV;
    G4LogicalVolume* fRFCavityLV;
};

#endif
