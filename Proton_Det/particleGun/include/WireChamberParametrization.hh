#ifndef WireChamberParametrization_H
#define WireChamberParametrization_H 1

#include "globals.hh"
#include "G4VPVParameterisation.hh"
#include "G4ThreeVector.hh"

class G4VPhysicalVolume;
class G4Box;

class WireChamberParametrization : public G4VPVParameterisation
{
   public:
	
     WireChamberParametrization(G4int NumOfWires,
			      G4ThreeVector startPos,
			      G4double spacing,
			      G4double WireDiameter);

     void ComputeTransformation (const G4int copyNo, G4VPhysicalVolume* physVol) const;

     virtual ~WireChamberParametrization();

   private:

   // G4int fNumberOfChambers;
    G4int fNbOfWires;
    G4ThreeVector fStartPos;
    G4double fHalfWidth;
    G4double fSpacing;
};

#endif
