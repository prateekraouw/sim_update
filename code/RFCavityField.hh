// ================================
// include/RFCavityField.hh
// ================================

#ifndef RFCavityField_h
#define RFCavityField_h 1

#include "G4ElectroMagneticField.hh"
#include "G4ThreeVector.hh"

class RFCavityField : public G4ElectroMagneticField
{
  public:
    RFCavityField();
    virtual ~RFCavityField();
    
    // Method to compute both electric and magnetic field at a given point
    virtual void GetFieldValue(const G4double point[4], G4double* field) const;
    
  private:
    G4double fAmplitude;     // Field amplitude
    G4double fFrequency;     // RF frequency
    G4double fPhase;         // RF phase
    G4double fWavelength;    // RF wavelength
    G4double fZmin;          // Start of RF cavity
    G4double fZmax;          // End of RF cavity
    G4double fMaxRadius;     // Maximum radius of field
};

#endif
