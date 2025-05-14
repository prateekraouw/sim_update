// ================================
// src/RFCavityField.cc
// ================================

#include "RFCavityField.hh"
#include "G4SystemOfUnits.hh"
#include <cmath>

RFCavityField::RFCavityField()
: G4ElectroMagneticField(),
  fAmplitude(1.0*megavolt/m),  // Field amplitude - adjust as needed
  fFrequency(2856*megahertz),  // Standard S-band RF frequency
  fPhase(0*deg),               // Initial phase
  fZmin(350*cm - 25*cm),       // Start of RF cavity
  fZmax(350*cm + 25*cm),       // End of RF cavity
  fMaxRadius(40*cm)            // Maximum radius of field
{
  // Calculate wavelength from frequency
  fWavelength = c_light / fFrequency;
}

RFCavityField::~RFCavityField()
{
}

void RFCavityField::GetFieldValue(const G4double point[4], G4double* field) const
{
  // Clear the field array
  for (G4int i=0; i<6; i++) {
    field[i] = 0.0;
  }
  
  // Get position and time
  G4double x = point[0];
  G4double y = point[1];
  G4double z = point[2];
  G4double t = point[3];
  
  // Calculate distance from z-axis
  G4double r = std::sqrt(x*x + y*y);
  
  // Check if point is inside the cavity volume
  if (z < fZmin || z > fZmax || r > fMaxRadius) {
    return;
  }
  
  // Apply a smooth transition at the edges (to avoid discontinuities)
  G4double zFactor = 1.0;
  G4double edgeWidth = 5.0*cm;
  if (z < fZmin + edgeWidth) {
    zFactor = (z - fZmin) / edgeWidth;
  } else if (z > fZmax - edgeWidth) {
    zFactor = (fZmax - z) / edgeWidth;
  }
  
  // Radial attenuation factor
  G4double rFactor = 1.0;
  if (r > 0.0) {
    rFactor = std::exp(-r*r/(2.0*fMaxRadius*fMaxRadius/4.0));
  }
  
  // Standing wave pattern along z
  G4double kz = 2.0 * M_PI / fWavelength;
  G4double zPosition = z - fZmin;  // Position relative to cavity start
  
  // Time-dependent phase factor
  G4double omega = 2.0 * M_PI * fFrequency;
  G4double phase = omega * t + fPhase;
  
  // Electric field - longitudinal component (Ez)
  G4double Ez = fAmplitude * std::cos(kz * zPosition) * std::cos(phase);
  
  // Apply attenuation factors
  Ez *= zFactor * rFactor;
  
  // Set the electric field (Ez along z-axis)
  field[3] = 0;     // Ex
  field[4] = 0;     // Ey
  field[5] = Ez;    // Ez
  
  // Magnetic field components would be derived from Maxwell's equations
  // For simplicity and optimal acceleration, we're focusing on the electric field
  field[0] = 0;     // Bx
  field[1] = 0;     // By
  field[2] = 0;     // Bz
}
