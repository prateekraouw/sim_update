// ================================
// src/DetectorConstruction.cc
// ================================

#include "DetectorConstruction.hh"
#include "MagneticField.hh"
#include "RFCavityField.hh"

#include "G4Material.hh"
#include "G4Element.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4NistManager.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"
#include "G4ChordFinder.hh"
#include "G4SDManager.hh"
#include "G4RotationMatrix.hh"
#include "G4ClassicalRK4.hh"
#include "G4UniformElectricField.hh"
#include "G4EqMagElectricField.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4MagIntegratorDriver.hh"

DetectorConstruction::DetectorConstruction()
 : G4VUserDetectorConstruction(),
   fWorldLV(nullptr),
   fCylinderLV(nullptr),
   fTungstenBlockLV(nullptr),
   fDetector1LV(nullptr),
   fDetector2LV(nullptr),
   fDetector3LV(nullptr),
   fHeliumCloudLV(nullptr),
   fRFCavityLV(nullptr),
   fMagneticField(nullptr),
   fRFField(nullptr),
   fFieldMgr(nullptr)
{
}

DetectorConstruction::~DetectorConstruction()
{
  delete fMagneticField;
  delete fRFField;
}

void DetectorConstruction::DefineMaterials()
{
  G4NistManager* nistManager = G4NistManager::Instance();
  
  // Define materials
  nistManager->FindOrBuildMaterial("G4_AIR");
  nistManager->FindOrBuildMaterial("G4_W");  // Tungsten
  nistManager->FindOrBuildMaterial("G4_He"); // Helium
  nistManager->FindOrBuildMaterial("G4_Cu"); // Copper for RF cavity
  nistManager->FindOrBuildMaterial("G4_Si"); // Silicon for detectors
  
  // Print materials
  G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

G4VPhysicalVolume* DetectorConstruction::DefineVolumes()
{
  // Get materials
  G4Material* air = G4Material::GetMaterial("G4_AIR");
  G4Material* tungsten = G4Material::GetMaterial("G4_W");
  G4Material* silicon = G4Material::GetMaterial("G4_Si");
  G4Material* helium = G4Material::GetMaterial("G4_He");
  G4Material* copper = G4Material::GetMaterial("G4_Cu");
  
  // Parameters
  G4double world_sizeXY = 600*cm;
  G4double world_sizeZ  = 6000*cm;
  
  G4double cylinder_radius = 100*cm;
  G4double cylinder_length = 5000*cm;
  
  // Updated tungsten block dimensions to 5×5×75 cm
  G4double tungsten_block_sizeX = 5*cm;
  G4double tungsten_block_sizeY = 5*cm;
  G4double tungsten_block_sizeZ = 75*cm;
  
  // Updated detector parameters for circular detectors
  G4double detector_radius = 80*cm;
  G4double detector_thickness = 1*cm;
  
  G4double helium_cloud_length = 100*cm; // 250 cm to 350 cm from block
  
  G4double rf_cavity_length = 50*cm;
  G4double rf_cavity_inner_radius = 40*cm;
  G4double rf_cavity_outer_radius = 45*cm;
  
  // Rotation for tungsten block (10 degrees from yz axis)
  G4RotationMatrix* rotationMatrix = new G4RotationMatrix();
  rotationMatrix->rotateX(10.*deg);
  
  // World
  G4Box* worldS = new G4Box("World", 0.5*world_sizeXY, 0.5*world_sizeXY, 0.5*world_sizeZ);
  fWorldLV = new G4LogicalVolume(worldS, air, "World");
  G4VPhysicalVolume* worldPV 
    = new G4PVPlacement(nullptr, G4ThreeVector(), fWorldLV, "World", nullptr, false, 0, true);
  
  // Cylinder (experimental hall)
  G4Tubs* cylinderS = new G4Tubs("Cylinder", 0, cylinder_radius, 0.5*cylinder_length, 0.*deg, 360.*deg);
  fCylinderLV = new G4LogicalVolume(cylinderS, air, "Cylinder");
  new G4PVPlacement(nullptr, G4ThreeVector(0, 0, 0), fCylinderLV, "Cylinder", fWorldLV, false, 0, true);
  
  // Tungsten Block at origin - updated to rectangular shape with specified dimensions
  G4ThreeVector tungstenBlockPos = G4ThreeVector(0, 0, 0);
  G4Box* tungstenBlockS = new G4Box("TungstenBlock", 
                               0.5*tungsten_block_sizeX, 
                               0.5*tungsten_block_sizeY, 
                               0.5*tungsten_block_sizeZ);
  fTungstenBlockLV = new G4LogicalVolume(tungstenBlockS, tungsten, "TungstenBlock");
  new G4PVPlacement(rotationMatrix, tungstenBlockPos, fTungstenBlockLV, "TungstenBlock", 
                   fCylinderLV, false, 0, true);
  
  // Detector 1 (10 cm from block)
  G4ThreeVector detector1Pos = G4ThreeVector(0, 0, 10*cm);
  G4Tubs* detector1S = new G4Tubs("Detector1", 
                             0,                 // inner radius
                             detector_radius,   // outer radius
                             0.5*detector_thickness, // half height
                             0.*deg, 360.*deg); // start and span angles
  fDetector1LV = new G4LogicalVolume(detector1S, silicon, "Detector1");
  new G4PVPlacement(nullptr, detector1Pos, fDetector1LV, "Detector1", 
                   fCylinderLV, false, 0, true);
  
  // Detector 2 (200 cm from block)
  G4ThreeVector detector2Pos = G4ThreeVector(0, 0, 200*cm);
  G4Tubs* detector2S = new G4Tubs("Detector2", 
                             0,                 // inner radius
                             detector_radius,   // outer radius
                             0.5*detector_thickness, // half height
                             0.*deg, 360.*deg); // start and span angles
  fDetector2LV = new G4LogicalVolume(detector2S, silicon, "Detector2");
  new G4PVPlacement(nullptr, detector2Pos, fDetector2LV, "Detector2", 
                   fCylinderLV, false, 0, true);
  
  // Helium Cloud (250-350 cm from block)
  G4ThreeVector heliumCloudPos = G4ThreeVector(0, 0, 300*cm); // center at 300 cm (250-350)
  G4Tubs* heliumCloudS = new G4Tubs("HeliumCloud", 0, cylinder_radius*0.9, 0.5*helium_cloud_length, 0.*deg, 360.*deg);
  fHeliumCloudLV = new G4LogicalVolume(heliumCloudS, helium, "HeliumCloud");
  new G4PVPlacement(nullptr, heliumCloudPos, fHeliumCloudLV, "HeliumCloud", 
                   fCylinderLV, false, 0, true);
  
  // RF Cavity (at 350 cm from block)
  G4ThreeVector rfCavityPos = G4ThreeVector(0, 0, 350*cm);
  G4Tubs* rfCavityS = new G4Tubs("RFCavity", 
                             rf_cavity_inner_radius, 
                             rf_cavity_outer_radius, 
                             0.5*rf_cavity_length, 
                             0.*deg, 360.*deg);
  fRFCavityLV = new G4LogicalVolume(rfCavityS, copper, "RFCavity");
  new G4PVPlacement(nullptr, rfCavityPos, fRFCavityLV, "RFCavity", 
                   fCylinderLV, false, 0, true);
  
  // Detector 3 (500 cm from block)
  G4ThreeVector detector3Pos = G4ThreeVector(0, 0, 500*cm);
  G4Tubs* detector3S = new G4Tubs("Detector3", 
                             0,                 // inner radius
                             detector_radius,   // outer radius
                             0.5*detector_thickness, // half height
                             0.*deg, 360.*deg); // start and span angles
  fDetector3LV = new G4LogicalVolume(detector3S, silicon, "Detector3");
  new G4PVPlacement(nullptr, detector3Pos, fDetector3LV, "Detector3", 
                   fCylinderLV, false, 0, true);
  
  // Visualization attributes
  G4VisAttributes* visAttributes = new G4VisAttributes(G4Colour(1.0, 1.0, 1.0));
  visAttributes->SetVisibility(false);
  fWorldLV->SetVisAttributes(visAttributes);
  
  fCylinderLV->SetVisAttributes(new G4VisAttributes(G4Colour(0.9, 0.9, 0.9, 0.1)));
  
  fTungstenBlockLV->SetVisAttributes(new G4VisAttributes(G4Colour(0.5, 0.5, 0.5)));
  
  G4VisAttributes* detectorVis = new G4VisAttributes(G4Colour(0.0, 1.0, 0.0));
  fDetector1LV->SetVisAttributes(detectorVis);
  fDetector2LV->SetVisAttributes(detectorVis);
  fDetector3LV->SetVisAttributes(detectorVis);
  
  fHeliumCloudLV->SetVisAttributes(new G4VisAttributes(G4Colour(0.0, 0.0, 1.0, 0.2)));
  
  fRFCavityLV->SetVisAttributes(new G4VisAttributes(G4Colour(1.0, 0.6, 0.0)));
  
  return worldPV;
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  // Define materials
  DefineMaterials();
  
  // Define volumes
  return DefineVolumes();
}

void DetectorConstruction::ConstructSDandField()
{
  // Create global magnetic field
  fMagneticField = new MagneticField();
  
  // Create global field manager
  G4FieldManager* globalFieldMgr = G4TransportationManager::GetTransportationManager()->GetFieldManager();
  
  // Configure field manager with a chord finder
  globalFieldMgr->SetDetectorField(fMagneticField);
  G4MagIntegratorStepper* stepper = new G4ClassicalRK4(new G4EqMagElectricField(fMagneticField));
  G4double minStep = 0.01*mm;
  
  G4MagInt_Driver* driver = new G4MagInt_Driver(minStep, stepper, stepper->GetNumberOfVariables());
  G4ChordFinder* chordFinder = new G4ChordFinder(driver);
  globalFieldMgr->SetChordFinder(chordFinder);
  
  // Create RF cavity field
  fRFField = new RFCavityField();
  
  // Create local field manager for RF cavity
  G4FieldManager* rfFieldManager = new G4FieldManager();
  rfFieldManager->SetDetectorField(fRFField);
  
  // Configure RF cavity field manager with appropriate chord finder
  G4MagIntegratorStepper* rfStepper = new G4ClassicalRK4(new G4EqMagElectricField(fRFField));
  G4MagInt_Driver* rfDriver = new G4MagInt_Driver(minStep, rfStepper, rfStepper->GetNumberOfVariables());
  G4ChordFinder* rfChordFinder = new G4ChordFinder(rfDriver);
  rfFieldManager->SetChordFinder(rfChordFinder);
  
  // Assign field manager to RF cavity logical volume
  fRFCavityLV->SetFieldManager(rfFieldManager, true);
}