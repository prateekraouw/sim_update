// ==========================
// include/MagneticField.hh
// ==========================

#ifndef MagneticField_h
#define MagneticField_h 1

#include "G4MagneticField.hh"
#include "G4ThreeVector.hh"

class MagneticField : public G4MagneticField
{
  public:
    MagneticField();
    virtual ~MagneticField();
    
    virtual void GetFieldValue(const G4double point[4], G4double* field) const;
    
  private:
    G4double fMaxField;      // Maximum field strength (7 Tesla)
    G4double fZMin;          // Field start position (-200 cm)
    G4double fZMax;          // Field end position (1000 cm)
    G4double fZTaperLength;  // Tapering length at edges
};

#endif
