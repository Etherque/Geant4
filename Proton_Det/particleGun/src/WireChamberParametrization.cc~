#include "WireChamberParametrization.hh"

#include "G4VPhysicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4Box.hh"

WireChamberParametrization::WireChamberParametrization (
	G4int NumOfWires,
	G4ThreeVector startPos,
	G4double spacingZ,
	G4double WireDiameter )

{
   fNbOfWires = NumOfWires;
   fStartPos = startPos;
   fHalfWidth = WireDiameter * 0.5;
   fSpacing = spacingZ;
   
}

WireChamberParametrization::~WireChamberParametrization()
{}

void WireChamberParametrization::ComputeTransformation
  (const G4int copyNo, G4VPhysicalVolume* physVol) const
{
    G4double zPosition = fStartPos.z() + (copyNo + 1) * fSpacing;
    G4ThreeVector newOrigin(fStartPos.x(), fStartPos.y(), fStartPos.z()+ zPosition);
    physVol->SetTranslation(newOrigin);
}
