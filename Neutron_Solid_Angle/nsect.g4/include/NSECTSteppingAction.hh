#ifndef NSECTSteppingAction_h
#define NSECTSteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "g4VerboseTrigger.hh"

class NSECTSteppingAction : public G4UserSteppingAction
{
  public:
  NSECTSteppingAction(g4VerboseTrigger * trigger);

   ~NSECTSteppingAction(){};

    void UserSteppingAction(const G4Step*);

private:

  G4String prestep, getstep, volume_name, next_volume_name;
  G4ThreeVector positionT2, positionFD, position;
  G4double T2x, T2y, T2z,Fdx, Fdy, Fdz, FdEg;
  G4double pdx, pdy, pdz, tdz_numerator, tdz_denominator, tdz, tdy;

  g4VerboseTrigger  * the_verbosetrigger;
};

#endif
