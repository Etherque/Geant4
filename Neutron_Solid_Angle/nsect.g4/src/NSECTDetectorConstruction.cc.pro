#include <math.h>

#include "NSECTDetectorConstruction.hh"
#include "NSECTDetectorMessenger.hh"
#include "NSECTMagneticField.hh"

#include "G4Material.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"
#include "G4UnitsTable.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Orb.hh"
#include "G4Torus.hh"
#include "G4PVPlacement.hh"
#include "G4Transform3D.hh"
#include "G4PhysicalConstants.hh"

#include "G4SDManager.hh"
#include "G4VVisManager.hh"

#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4SolidStore.hh"
#include "G4RunManager.hh"
#include "G4GeometryManager.hh"
#include "G4SystemOfUnits.hh"

#define PI 3.14159265359	

NSECTDetectorConstruction::NSECTDetectorConstruction()
  :solidWorld(0),  logicWorld(0),  physiWorld(0), 
   currentRotation(0), currentTranslation(-1)
{
  detectorMessenger = new NSECTDetectorMessenger(this);
}

NSECTDetectorConstruction::~NSECTDetectorConstruction()
{
}

G4VPhysicalVolume* NSECTDetectorConstruction::Construct()
{ 
  Make_Materials();
  
  G4VPhysicalVolume * the_world = Make_World();
  
  setTargetPosition(0);

  return(the_world);
}

void NSECTDetectorConstruction::Make_Materials()
{
  G4double a, density;
  G4int    iz, n, ncomponents;
  
  //Iron
  G4Isotope* Fe54 = new G4Isotope("FE54", iz=26, n=54 , a=53.939614*g/mole);
  G4Isotope* Fe56 = new G4Isotope("FE56", iz=26, n=56, a=55.934941*g/mole);
  G4Isotope* Fe57 = new G4Isotope("FE57", iz=26, n=57, a=56.935398*g/mole);
  
  G4Element* elFe  = new G4Element("Natural Iron", "Fe",ncomponents=3);
  elFe->AddIsotope(Fe54, 5.8*perCent);
  elFe->AddIsotope(Fe56, 91.75*perCent);
  elFe->AddIsotope(Fe57, 2.45*perCent);
  
  Iron = new G4Material("Iron", 7.86*g/cm3, 1);
  Iron->AddElement(elFe, 100*perCent);
  
  // Copper
  G4Isotope* Cu63 = new G4Isotope("CU63", iz=29, n=63 , a=62.929600*g/mole);
  G4Isotope* Cu65 = new G4Isotope("CU65", iz=29, n=65, a=64.927793*g/mole);
  
  G4Element* elCu  = new G4Element("Natural Copper", "Cu",ncomponents=2);
  elCu->AddIsotope(Cu63, 69.17*perCent);
  elCu->AddIsotope(Cu65, 30.83*perCent);
 
  Copper =  new G4Material("Copper", 8.96*g/cm3, 1);
  Copper->AddElement(elCu, 100*perCent);

  // Air
  G4Element* elN  = new G4Element("Nitrogen", "N", iz=7, a= 14.01*g/mole);
  G4Element* elO  = new G4Element("elOxygen"  , "O", iz=8, a= 16.00*g/mole);

  density= 1.29*mg/cm3;
  ncomponents = 2;
  Air = new G4Material("Air", density, ncomponents);
  Air->AddElement(elN, 70*perCent);
  Air->AddElement(elO, 30*perCent);
  
  //CH2
  G4Element* elC = new G4Element ("elCarbon", "C", iz=6, a= 12.011*g/mole);
  G4Element* elH = new G4Element("elHydrogen", "H", iz=1, a=1.01*g/mole);
  
  density = 0.94*g/cm3;
  CH2 = new G4Material("CH2",density,ncomponents=2);
  CH2 ->AddElement(elC, 1);
  CH2 ->AddElement(elH, 2);
    
  //CD2
  G4Isotope* H2 = new G4Isotope("H2", iz=1, n=2, a=2.014101*g/mole);
  G4Isotope* H1 = new G4Isotope("H1", iz=1, n=1, a=1.007940*g/mole);
  G4Element* elD = new G4Element("Deuterium","D",ncomponents=2);
  elD->AddIsotope(H2, 99.1228*perCent); 
  elD->AddIsotope(H1, 0.8772*perCent); 
 
  density = 1.06*g/cm3;
  CD2 = new G4Material("CD2",density,ncomponents=2);
  CD2 ->AddElement(elC, 1);
  CD2 ->AddElement(elD, 2);
   
  //Deuterium
  density = 0.18*g/cm3;
  D2 = new G4Material("D2",density,ncomponents=1);
  D2 ->AddElement(elD, 100*perCent);
  
  //Graphite
  density = 2.267*g/cm3;
  Graphite = new G4Material("Graphite",density,ncomponents=1);
  Graphite ->AddElement(elC, 100*perCent);
   
  // Vacuum
  density = universe_mean_density;
  G4double pressure    = 3.e-18*pascal;
  G4double temperature = 2.73*kelvin;
  Vacuum = new G4Material("Vacuum", iz=1, a=1.01*g/mole,density,kStateGas,temperature,pressure);
}

G4VPhysicalVolume * NSECTDetectorConstruction::Make_World()
{
  // Clean old geometry when world is remade
  //
  G4GeometryManager::GetInstance()->OpenGeometry();
  G4PhysicalVolumeStore::GetInstance()->Clean();
  G4LogicalVolumeStore::GetInstance()->Clean();
  G4SolidStore::GetInstance()->Clean();
  
  // 50 cm world
  G4double HalfWorldLength = 0.5* 1000.0*cm;
  
  solidWorld= new G4Box("world", HalfWorldLength, HalfWorldLength, HalfWorldLength);
  
  logicWorld= new G4LogicalVolume(solidWorld, Air, "World", 0, 0, 0);
  
  physiWorld = new G4PVPlacement(0,               // no rotation
				 G4ThreeVector(), // at (0,0,0)
				 logicWorld,      // its logical volume
				 "World",         // its name
				 0,               // its mother  volume
				 false,           // no boolean operations
				 0);              // no field specific to vol
  
  logicWorld->SetVisAttributes (G4VisAttributes::Invisible);
 
  Make_The_N_();
  Make_Magnetic_Field();
  Make_Detector();

  return(physiWorld);
}

void NSECTDetectorConstruction::Make_The_N_()
{
  // 60 cm target
  G4double HalfTargetLength = 0.5* 40.0*cm;
  
  solidTarget= new G4Box("target", HalfTargetLength, HalfTargetLength, HalfTargetLength);
  
  logicTarget= new G4LogicalVolume(solidTarget, Air, "Target", 0, 0, 0);
  
  physiTarget = new G4PVPlacement(0,    // 
                                  G4ThreeVector(0,0,-75*cm),  // 
				  logicTarget,     // its logical volume
				  "Target",        // its name
				  logicWorld,      // its mother  volume
				  false,           // no boolean operations
				  0);              // no field specific to vol
  
  logicTarget->SetVisAttributes (G4VisAttributes::Invisible);

  G4double barLength = 20.0 * cm;
  G4double barWidth  = 0.2 * cm;
  G4double barHeight = 20.0 *cm;
  //T1
  G4double T1_z = -4*cm;//-79.0*cm; 
  //T2
  G4double T2_z = 0*cm;//-75.0*cm;
  //T3
  G4double T3_z = 4*cm;// -71.0*cm;
  //T4
  G4double T4_z = 8*cm;//-67.0*cm;
  
  G4Box * solidT1 = new G4Box("Target1", barHeight/2.0, barLength/2.0, barWidth/2.0);
    
  G4LogicalVolume * logicT1Volume = new G4LogicalVolume(solidT1, Air, "Target1", 0,0,0);

  G4Box * solidT2 = new G4Box("Target2", barHeight/2.0, barLength/2.0, barWidth/2.0);
  
  G4LogicalVolume * logicT2Volume = new G4LogicalVolume(solidT2, Air, "Target2", 0,0,0);
  
  G4Box * solidT3 = new G4Box("Target3", barHeight/2.0, barLength/2.0, barWidth/2.0);
 
  G4LogicalVolume * logicT3Volume = new G4LogicalVolume(solidT3, CH2, "Target3", 0,0,0);  

  G4Box * solidT4 = new G4Box("Target4",2.5*barHeight,2.5*barLength, barWidth/2.0);
  
  G4LogicalVolume * logicT4Volume = new G4LogicalVolume(solidT4, Air, "Target4", 0,0,0);

  G4ThreeVector    Ta[6];
  G4RotationMatrix Ra[6];
  
  // Target1 
  Ta[3].setX(0.0); Ta[3].setY(0.0); Ta[3].setZ(T1_z);
  Ra[3].rotateX(0.0); Ra[3].rotateY(0.0); Ra[3].rotateZ(0.0);

  new G4PVPlacement(G4Transform3D(Ra[3], Ta[3]),
                    logicT1Volume,
                    "Targetbar1",
                    logicTarget,
                    false,
                    0); 

  // Target2
  Ta[0].setX(0.0); Ta[0].setY(0.0); Ta[0].setZ(T2_z);
  Ra[0].rotateX(0.0); Ra[0].rotateY(0.0); Ra[0].rotateZ(0.0);
  
  new G4PVPlacement(G4Transform3D(Ra[0], Ta[0]),
		    logicT2Volume,
		    "Targetbar2",
		    logicTarget,
		    false,
		    0);
  
   // Target3
  Ta[2].setX(0.0); Ta[2].setY(0.0); Ta[2].setZ(T3_z);
  Ra[2].rotateX(0.0*deg); Ra[2].rotateY(0.0); Ra[2].rotateZ(0.0);

  new G4PVPlacement(G4Transform3D(Ra[2], Ta[2]),
                    logicT3Volume,
                    "Targetbar3",
                    logicTarget,
                    false,
                    0);

  // Target4
  Ta[1].setX(0.0); Ta[1].setY(0.0); Ta[1].setZ(T4_z);
  Ra[1].rotateX(0.0); Ra[1].rotateY(0.0); Ra[1].rotateZ(0.0);
  
  new G4PVPlacement(G4Transform3D(Ra[1], Ta[1]),
		    logicT4Volume,
		    "Targetbar4",
		     logicTarget,
		    false,
		    0);
  
 //color
  G4VisAttributes* BoxVisAttOrange =  new G4VisAttributes(G4Colour(1.0,0.6,0.0));
  G4VisAttributes* BoxVisAttGrey   =  new G4VisAttributes(G4Colour(0.5,0.5,0.5));
  
  BoxVisAttOrange->SetForceSolid(TRUE);
  BoxVisAttGrey->SetForceSolid(TRUE);
  
  logicT1Volume->SetVisAttributes(BoxVisAttOrange);
  logicT2Volume->SetVisAttributes(BoxVisAttOrange);
  logicT3Volume->SetVisAttributes(BoxVisAttOrange);
  logicT4Volume->SetVisAttributes(BoxVisAttOrange);

}

   NSECTMagneticField* NSECTDetectorConstruction::fMagneticField = 0;
   G4FieldManager* NSECTDetectorConstruction::fFieldMgr = 0;

void NSECTDetectorConstruction::Make_Magnetic_Field()
{
   G4double fz = 46.0*cm;
   G4double fx = 35.0*cm;
   G4double fy = 36.0*cm;

   G4Box* Unifmagnetic  = new G4Box("MagField",
                                    fx/2,
                                    fy/2,
                                    fz/2);
               
  G4LogicalVolume * logicMagnetic = new G4LogicalVolume(Unifmagnetic,
                                                        Air,
                                                       "LogicMagnetic");
  G4ThreeVector    Tm[1];
  G4RotationMatrix Rm[1];

  Tm[0].setX(0.0); Tm[0].setY(3.0*cm); Tm[0].setZ(0.0);
  Rm[0].rotateX(-11.2*deg); Rm[0].rotateY(0.0); Rm[0].rotateZ(0.0);

  new G4PVPlacement(G4Transform3D(Rm[0], Tm[0]),
                    logicMagnetic,
                    "MagneticField",
                    logicWorld,
                    false,
                    0);
  
  fMagneticField = new NSECTMagneticField();
  fFieldMgr = new G4FieldManager();
  fFieldMgr->SetDetectorField(fMagneticField);
  fFieldMgr->CreateChordFinder(fMagneticField);
  G4bool forceToAllDaughters = true;
  logicMagnetic->SetFieldManager(fFieldMgr, forceToAllDaughters);
  
  G4VisAttributes* BoxVisAttCyan = new G4VisAttributes(G4Colour(0.0,1.0,1.0));
  BoxVisAttCyan->SetForceSolid(TRUE);
  logicMagnetic->SetVisAttributes(BoxVisAttCyan);
}

void NSECTDetectorConstruction::Make_Detector()
{
  G4double detector_width =  27.3*cm;//8.9*3cm
  G4double detector_height = 45.7*cm;//8.9*5cm
  G4double detector_thick = 15.2*cm;
  G4double fakeDet_inner = 0.0*cm, fakeDet_outer =100.0*cm; //(fakeDet_z+79.5*cm)*tan(Agmin), (fakeDet_z+79.5*cm)*tan(Agmax);
  G4double startAngleOfTheTube = 0.*deg;
  G4double spanningAngleOfTheTube = 360.*deg; 
  G4double fakeDet_thick = 2.0*cm;
  
  G4Tubs* fakeTarget
    = new G4Tubs("Fake_Target",
                 fakeDet_inner,
                 fakeDet_outer,
                 fakeDet_thick,
                 startAngleOfTheTube,
                 spanningAngleOfTheTube);

  G4Tubs* fakeDetector
    = new G4Tubs("Fake_Detector",
                 fakeDet_inner,
                 fakeDet_outer,
                 fakeDet_thick,
                 startAngleOfTheTube,
                 spanningAngleOfTheTube);

  G4LogicalVolume * logicFakeDet = new G4LogicalVolume(fakeDetector,                                                       
                                                        Air,                                                                                                                         "LogicFakeDet",
                                                        0,0,0);

  G4LogicalVolume * logicFakeTar = new G4LogicalVolume(fakeTarget, 
                                                       Air,                
                                                       "LogicFakeTar",
                                                       0,0,0);
  G4Box* solidDetector
    = new G4Box("SolidDetector",
                 detector_width/2,
                 detector_height/2,
                 detector_thick/2);


  G4LogicalVolume * logicDetector = new G4LogicalVolume(solidDetector,
                                                        Air,
                                                        "LogicDetector",
                                                          0,0,0);
  G4ThreeVector    Td[3];
  G4RotationMatrix Rd[3];
 
 //DE
  Td[0].setX(0.0*cm); Td[0].setY(30.9*cm+sin(11.5*deg)*detector_thick/2);
  Td[0].setZ(86.4*cm+detector_thick/2*cos(11.5*deg));
  Rd[0].rotateX(-11.5*deg); Rd[0].rotateY(0.0); Rd[0].rotateZ(0.0);

  new G4PVPlacement(G4Transform3D(Rd[0], Td[0]),
                    logicDetector,
                    "EDetector",
                    logicWorld,
                    false,
                    0);

//Fake Target
  Td[2].setX(0.0); Td[2].setY(0.0); Td[2].setZ(-60.0*cm);
  Rd[2].rotateX(0.0); Rd[2].rotateY(0.0); Rd[2].rotateZ(0.0);

/*  new G4PVPlacement(G4Transform3D(Rd[2], Td[2]),
                    logicFakeTar,
                    "fakeTar",
                    logicWorld,
                    false,
                    0);
*/
//Fake Detector
  Td[1].setX(0.0); Td[1].setY(0.0);//Td[1].setZ(86.4*cm+detector_thick/2*cos(11.5*deg));
  Td[1].setZ(-25.0*cm);
  Rd[1].rotateX(0.0); Rd[1].rotateY(0.0); Rd[1].rotateZ(0.0);  
  
  new G4PVPlacement(G4Transform3D(Rd[1], Td[1]),
                    logicFakeDet,
                    "Fake_Det",
                    logicWorld,
                    false,
                    0);

  G4VisAttributes* BoxVisAttMagenta   =  new G4VisAttributes(G4Colour(1.0,0.0,1.0));
  G4VisAttributes* BoxVisAttWhite   =  new G4VisAttributes(G4Colour(1.0,1.0,1.0));
  G4VisAttributes* BoxVisAttRed   =  new G4VisAttributes(G4Colour(1.0,0.0,0.0));

  BoxVisAttMagenta->SetForceSolid(TRUE);
  BoxVisAttWhite->SetForceSolid(TRUE);
  BoxVisAttRed->SetForceSolid(TRUE);

  logicDetector->SetVisAttributes(BoxVisAttMagenta);
  logicFakeTar->SetVisAttributes(BoxVisAttRed);
  logicFakeDet->SetVisAttributes(BoxVisAttWhite);
}

// rotates the target volume
void NSECTDetectorConstruction::setTargetPosition(G4int position)
{
}
  
