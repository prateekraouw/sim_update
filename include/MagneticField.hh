// ==========================
// include/MagneticField.hh
// ==========================

#ifndef MagneticField_h
#define MagneticField_h 1

#include "G4ElectroMagneticField.hh"
#include "G4ThreeVector.hh"

class MagneticField : public G4ElectroMagneticField  // Changed from G4MagneticField
{
  public:
    MagneticField();
    virtual ~MagneticField();
    
    // Implementation of GetFieldValue from G4ElectroMagneticField
    virtual void GetFieldValue(const G4double point[4], G4double* field) const;
    
    // Implementation of pure virtual function from G4ElectroMagneticField
    virtual G4bool DoesFieldChangeEnergy() const { return false; }
    
  private:
    G4double fMaxField;      // Maximum field strength (7 Tesla)
    G4double fZMin;          // Field start position (-200 cm)
    G4double fZMax;          // Field end position (1000 cm)
    G4double fZTaperLength;  // Tapering length at edges
};

#endif