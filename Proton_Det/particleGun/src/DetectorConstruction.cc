#include "DetectorConstruction.hh"
#include "WireChamberParametrization.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVParameterised.hh"
#include "G4SystemOfUnits.hh"
#include "G4Material.hh"

// For defining silicon and helium from the database
#include "G4NistManager.hh"


#include "G4VisAttributes.hh"
#include "G4Colour.hh"

DetectorConstruction::DetectorConstruction()
{
}

DetectorConstruction::~DetectorConstruction()
{
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{


// -------    Material definitions ------------- //

   G4double z, a, density, fractionmass;
   G4int ncomponents;
   G4Material* Silicon = new G4Material("Silicon", z=14., a=28.0855*g/mole, density=2.330*g/cm3);
   G4Material* Helium = new G4Material("Helium", z = 2, a=4.001*g/mole, density=0.0001785*g/cm3);
   
   G4Element* Cr = new G4Element("Chrome", "Cr", z=25, a=  51.996*g/mole);
   G4Element* Fe = new G4Element("Iron"  , "Fe", z=26, a=  55.845*g/mole);
   G4Element* Co = new G4Element("Cobalt", "Co", z=27, a=  58.933*g/mole);
   G4Element* Ni = new G4Element("Nickel", "Ni", z=28, a=  58.693*g/mole);
   G4Element* W  = new G4Element("Tungsten","W", z=74, a= 183.850*g/mole);
   G4Element* H  = new G4Element("Hydrogen","H", z=1, a= 1.01*g/mole);
   G4Element* C  = new G4Element("Carbon","C", z=6, a= 12.01*g/mole);
   G4Element* O  = new G4Element("Oxygen","O", z=8, a= 16.00*g/mole);
   
   G4Material* Havar = new G4Material("Havar", density= 8.3*g/cm3, ncomponents=5);
   Havar->AddElement(Cr, fractionmass=0.1785);
   Havar->AddElement(Fe, fractionmass=0.1822);
   Havar->AddElement(Co, fractionmass=0.4452);
   Havar->AddElement(Ni, fractionmass=0.1310);
   Havar->AddElement(W , fractionmass=0.0631);
   
   G4Material* Mylar = new G4Material("Mylar", density= 1.38*g/cm3, ncomponents=3);
   Mylar->AddElement(C,fractionmass=0.6250);
   Mylar->AddElement(H,fractionmass=0.0417);
   Mylar->AddElement(O,fractionmass=0.3333);

// ----------- Definitions of all dimensions -------------//
// Currently unused values commented out to save warnings //

   // Helium world box
   G4double WorldDimX = 1.0*m, WorldDimY = 1.0*m, WorldDimZ = 1.0*m;

   // Actual wire chamber
   G4double WireChamberDimX = 0.003*mm, WireChamberDimY=30*cm, WireChamberDimZ = 30*cm;

   // Size of the wire chamber system
   G4double WireChamberSystemDimX = 10*cm, WireChamberSystemDimY = 35*cm,
            WireChamberSystemDimZ = 60*cm;

   // Wire chamber frame
   //G4double WireChamberFrameDimX, WireChamberFrameDimY, WireChamberFrameDimZ;

   // Wire Chamber Mylar sheet
  // G4double WireChamberMylarSheetDimX, WireChamberMylarSheetDimY, WireChamberMylarSheetDimZ;


// ------------ Definitions of positions ----------------//
 
   // Wire chamber
   G4double WireChamberPosX =  5*cm, WireChamberPosY = 0*cm, WireChamberPosZ = 0*cm;

   // Silicon detector
   G4double SiliconDetectorPosX = 14*cm, SiliconDetectorPosY = 0*cm, 
            SiliconDetectorPosZ = 0*cm;


// ----------- Construct a world box made of helium --------------- //
   G4Box* solidWorld = new G4Box("world", WorldDimX/2, WorldDimY/2, WorldDimZ/2);
   G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld, Helium, "World", 0, 0, 0);
   G4PVPlacement* physWorld = new G4PVPlacement(0,
				  G4ThreeVector(0,0,0),
				  logicWorld,
				  "World",
				  0,
				  false,
				  0);

// ----------- Since the world volume cannot contain both replicated volumes and regular volumes,
// need to create a subworld "Tracker" where I want the first replicated volume to be
// located. This is made invisible in the world.

  G4Box* solidTracker = new G4Box("tracker", WireChamberSystemDimX/2,
					      WireChamberSystemDimY/2,
					      WireChamberSystemDimZ/2);
   G4LogicalVolume* logicTracker = new G4LogicalVolume(solidTracker,
						       Helium,
						       "Tracker");
   G4PVPlacement* physTracker = new G4PVPlacement(0,
			 G4ThreeVector(WireChamberPosX, WireChamberPosY, WireChamberPosZ),
				    logicTracker,
				    "Tracker",
				    logicWorld,
				    false,
  				    0);
  
 logicTracker->SetVisAttributes(G4VisAttributes::GetInvisible());						


// ----------- Construct the wire chamber solid and logical volumes. Wire chambers are nearly
// transparent, so I'll make them of helium. These are red. -----------//

   G4int NumOfWires = 80;
   G4ThreeVector firstPosition(0*cm, 0*cm, -20*cm);
   G4double spacingZ = 0.5*cm;
   G4double WireDiameter = 1.0*mm;

   G4Box* solidWireChamber = new G4Box("WireChamber", WireDiameter/2, WireChamberDimY/2,
WireDiameter/2);
   G4LogicalVolume* logicWireChamber = new G4LogicalVolume(solidWireChamber, Helium, "WireChamber", 0,0,0);//Helium


   WireChamberParam = new WireChamberParametrization(
				NumOfWires,
				firstPosition,
				spacingZ,
				WireDiameter);
				
   fWireChamber = new G4PVParameterised(		
				"Chamber",
				logicWireChamber,
				logicTracker,
				kXAxis,
				NumOfWires,
				WireChamberParam);
					     
   G4VisAttributes* BoxVisAttributes = new G4VisAttributes(G4Colour(1.0, 1.0, 1.0));
   logicWireChamber->SetVisAttributes(BoxVisAttributes);

// ---------- Construct the silicon detector and make it blue ------------------//

   
   // Silicon Detector
   G4double SiliconDetectorDimX = 5.0*mm, 
            SiliconDetectorDimY = 30.0*cm, 
            SiliconDetectorDimZ = 30.0*cm;

   G4Box* solidSiliconDetector = new G4Box("siliconDetector", SiliconDetectorDimX/2,
				           SiliconDetectorDimY/2, SiliconDetectorDimZ/2);
   G4LogicalVolume* logicSiliconDetector = new G4LogicalVolume(
					       solidSiliconDetector, 
					       Silicon,
					       "SiliconDetector", 
					       0,0,0);
 
   fSiliconDetector = new G4PVPlacement(0,
		G4ThreeVector(SiliconDetectorPosX, SiliconDetectorPosY, SiliconDetectorPosZ),
					   logicSiliconDetector,
					   "SDetector",
						logicWorld,
						false,
						0);

   G4VisAttributes* BoxVisAttGreen = new G4VisAttributes(G4Colour(0., 1., 0.));
   BoxVisAttGreen->SetForceSolid(TRUE);
   logicSiliconDetector->SetVisAttributes(BoxVisAttGreen);
 
  return physWorld;
  
}
