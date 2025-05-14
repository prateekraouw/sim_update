// ================================
// include/DetectorConstruction.hh
// ================================

#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "G4SystemOfUnits.hh"
#include "globals.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;
class G4Material;
class G4FieldManager;
class MagneticField;
class RFCavityField;  // Added for RF cavity field

class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    DetectorConstruction();
    virtual ~DetectorConstruction();
    
    virtual G4VPhysicalVolume* Construct();
    virtual void ConstructSDandField();
    
    // Getters for logical volumes to attach SDs
    G4LogicalVolume* GetDetector1LV() const { return fDetector1LV; }
    G4LogicalVolume* GetDetector2LV() const { return fDetector2LV; }
    G4LogicalVolume* GetDetector3LV() const { return fDetector3LV; }
    
  private:
    void DefineMaterials();
    G4VPhysicalVolume* DefineVolumes();
    
    G4LogicalVolume* fWorldLV;
    G4LogicalVolume* fCylinderLV;
    G4LogicalVolume* fTungstenBlockLV;
    G4LogicalVolume* fDetector1LV;
    G4LogicalVolume* fDetector2LV;
    G4LogicalVolume* fDetector3LV;
    G4LogicalVolume* fHeliumCloudLV;
    G4LogicalVolume* fRFCavityLV;
    
    MagneticField* fMagneticField;
    RFCavityField* fRFField;  // Added field for RF cavity
    G4FieldManager* fFieldMgr;
};

#endif