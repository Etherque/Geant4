#include "GenericParametrization.hh"

#include "G4VPhysicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4Box.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GenericParametrization::GenericParametrization(  
        G4int    NoChambers, 
        G4double startX,          //  Z of center of first 
        G4double spacingX,        //  Z spacing of centers
        G4double widthChamber)
{
   fNoChambers =  NoChambers; 
   fStartZ     =  startZ; 
   fHalfWidth  =  widthChamber*0.5;
   fSpacing    =  spacingZ;

   // fHalfLengthLast = lengthFinal;
   if( NoChambers > 0 )
   {
      fHalfLengthIncr =  0.5 * (lengthFinal-lengthInitial)/NoChambers;
     
   }
   
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GenericParametrization::~GenericParametrization()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GenericParametrization::ComputeTransformation
(const G4int copyNo, G4VPhysicalVolume* physVol) const
{
  G4double      Zposition= fStartZ + (copyNo+1) * fSpacing;
  G4ThreeVector origin(0,0,Zposition);
  physVol->SetTranslation(origin);
  physVol->SetRotation(0);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GenericParametrization::ComputeDimensions
(G4Box& trackerChamber, const G4int copyNo, const G4VPhysicalVolume*) const
{
  G4double  halfLength= fHalfLengthFirst + copyNo * fHalfLengthIncr;
  trackerChamber.SetXHalfLength(halfLength);
  trackerChamber.SetYHalfLength(halfLength);
  trackerChamber.SetZHalfLength(fHalfWidth);
}
