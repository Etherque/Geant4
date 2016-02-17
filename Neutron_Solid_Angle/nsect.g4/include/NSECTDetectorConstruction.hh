#ifndef NSECTDetectorConstruction_h
#define NSECTDetectorConstruction_h 1

#include "globals.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4RotationMatrix.hh"
#include "G4FieldManager.hh"

#include <vector>

#define NSECT_NUM_ANGLES  10
#define NSECT_NUM_STOPS   128
#define NSECT_NUM_POSITIONS (NSECT_NUM_ANGLES*NSECT_NUM_STOPS)

#define FOV         8.5*cm

class NSECTMagneticField;
class G4Box;
class G4LogicalVolume;
class G4VPhysicalVolume;
class G4Material;
class NSECTDetectorMessenger;
class G4VisManager;
class G4UImanager;
class NSECTDetectorMessenger;
class NSECTTrackerSD;

class NSECTDetectorConstruction : public G4VUserDetectorConstruction
{
  public:
  
  NSECTDetectorConstruction();
  ~NSECTDetectorConstruction();
  
public:
  
  G4VPhysicalVolume * Construct();
  void                setTargetPosition(G4int position);
  int                 getNumberOfPositions(){ return NSECT_NUM_POSITIONS; }
 // void                Make_Magnetic_Field();
private:

  void                Make_Materials();
  void                Make_The_N_();
  void                Make_Magnetic_Field();
  void                Make_Detector();
  G4VPhysicalVolume * Make_World(void);


  // world
  G4Box*             solidWorld;
  G4LogicalVolume*   logicWorld;
  G4VPhysicalVolume* physiWorld;

  // target
  G4Box*             solidTarget;
  G4LogicalVolume*   logicTarget;
  G4VPhysicalVolume* physiTarget;

  //field
  G4double fx;
  G4double fy;
  G4double fz;
 
  // detector

  G4double Pos_x ;
  G4double Pos_y ;
  G4double Pos_z ;
  G4double rot ;

  G4double detector_height;
  G4double detector_distance;

    static NSECTMagneticField* fMagneticField; 
    static G4FieldManager* fFieldMgr;
 
 // static G4ThreadLocal NSECTMagneticField* fMagneticField;
 // static G4ThreadLocal G4FieldManager* fFieldMgr;

  // materials
  G4Material * Air;
  G4Material * Vacuum;
  G4Material * Water;
  G4Material * Iron;
  G4Material * Copper;
  G4Material * Carbon;
  G4Material * CH2;
  G4Material * CD2;
  G4Material * D2;
  G4Material * Graphite;  

  // state
  G4int            currentRotation;
  G4int            currentTranslation;

  NSECTDetectorMessenger * detectorMessenger;

};

#endif
