#ifndef SteppingAction_h
#define SteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "g4VerboseTrigger.hh"

class SteppingAction : public G4UserSteppingAction
{
public:
  SteppingAction();
  virtual ~SteppingAction();

  void UserSteppingAction(const G4Step*);

private:
   G4ThreeVector positionIn, positionHT, positionSD;
   G4double Intx,Inty,Intz;//position of Initial Step
   G4double Htx, Hty, Htz;
   G4double Sdx, Sdy, Sdz;//position of Silicon Detector
   G4double HT_energy, plB_energy, Eint;

   g4VerboseTrigger  * the_verbosetrigger;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
