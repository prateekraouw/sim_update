// ==========================
// src/MagneticField.cc
// ==========================

#include "MagneticField.hh"
#include "G4SystemOfUnits.hh"
#include "G4GenericMessenger.hh"
#include <cmath>

MagneticField::MagneticField()
: G4ElectroMagneticField(),  // Change from G4MagneticField to G4ElectroMagneticField
  fMaxField(7.0*tesla),
  fZMin(-200.0*cm),
  fZMax(1000.0*cm),
  fZTaperLength(50.0*cm)
{
}

MagneticField::~MagneticField()
{
}

void MagneticField::GetFieldValue(const G4double point[4], G4double* field) const
{
  // Initialize field to zero for all components (both magnetic and electric)
  for (G4int i=0; i<6; i++) {
    field[i] = 0.0;
  }
  
  // Get position
  G4double z = point[2];
  
  // Check if outside field region
  if (z < fZMin || z > fZMax) {
    return;
  }
  
  // Calculate field strength based on position (tapering at edges)
  G4double strength = fMaxField;
  
  // Linear tapering at start of field region
  if (z < fZMin + fZTaperLength) {
    strength *= (z - fZMin) / fZTaperLength;
  }
  // Linear tapering at end of field region
  else if (z > fZMax - fZTaperLength) {
    strength *= (fZMax - z) / fZTaperLength;
  }
  
  // Set field along z direction with concentric solenoid profile
  // For simplicity, using a linear increase from edge to center
  G4double distFromCenter = std::sqrt(point[0]*point[0] + point[1]*point[1]);
  G4double maxRadius = 90.0*cm; // Maximum radius of the field effect
  
  if (distFromCenter <= maxRadius) {
    // Create a profile with increasing field toward the center
    G4double radialFactor = 1.0 - distFromCenter/maxRadius;
    strength *= radialFactor;
    
    // Apply the field in the z direction (magnetic field component)
    field[2] = strength;
    
    // Note: For G4ElectroMagneticField, the field array has 6 components:
    // field[0,1,2] = Bx, By, Bz (magnetic field)
    // field[3,4,5] = Ex, Ey, Ez (electric field)
    // We're setting only Bz, leaving the electric field components at zero
  }
}