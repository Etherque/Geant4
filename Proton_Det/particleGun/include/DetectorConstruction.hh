#ifndef DETECTORCONSTRUCTION_HH
#define DETECTORCONSTRUCTION_HH 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4LogicalVolume;
class G4VPhysicalVolume;
class G4VPVParameterisation;

class DetectorConstruction : public G4VUserDetectorConstruction {

public:
  
  // Constructor
  DetectorConstruction();
  
  // Destructor
  virtual ~DetectorConstruction();
  
  // Method
  virtual G4VPhysicalVolume* Construct();

  G4VPhysicalVolume* GetSiliconDetector() { return fSiliconDetector; }
  G4VPhysicalVolume* GetWireChamber() { return fWireChamber; }
  
private:

  // Helper methods
  void DefineMaterials();
  void SetupGeometry();

  // World logical and physical volumes
  G4LogicalVolume*   fpWorldLogical;
  G4VPhysicalVolume* fpWorldPhysical;

  // Pointers to the silicon detector and wire chamber detector
  G4VPhysicalVolume* fWireChamber;
  G4VPhysicalVolume* fWireChamber2;
  G4VPhysicalVolume* fMylarSheet;
  G4VPhysicalVolume* fMylarSheet2;
  G4VPhysicalVolume* fSiliconDetector;
  G4VPhysicalVolume* fSiliconDetector2;

  G4VPVParameterisation* WireChamberParam;
  G4VPVParameterisation* MylarParam;

  //Parser and input gdml file
  G4String fReadFile;
};

#endif
