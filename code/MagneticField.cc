// ==========================
// src/MagneticField.cc
// ==========================

#include "MagneticField.hh"
#include "G4SystemOfUnits.hh"
#include "G4GenericMessenger.hh"
#include <cmath>

MagneticField::MagneticField()
: G4MagneticField(),
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
  // Initialize field to zero
  field[0] = 0.0;
  field[1] = 0.0;
  field[2] = 0.0;
  
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
    
    // Apply the field in the z direction
    field[2] = strength;
  }
}
