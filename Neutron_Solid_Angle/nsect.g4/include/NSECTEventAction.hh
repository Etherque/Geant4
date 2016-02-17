#ifndef NSECTEventAction_h
#define NSECTEventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"

class G4Event;
class NSECTTrackerHit;

class NSECTEventAction : public G4UserEventAction
{
  public:
  NSECTEventAction();
   ~NSECTEventAction();

  public:
   
    void BeginOfEventAction(const G4Event*);
    void EndOfEventAction(const G4Event*);

private:
  void PrintHit(NSECTTrackerHit * the_hit);
  
};

#endif

    
