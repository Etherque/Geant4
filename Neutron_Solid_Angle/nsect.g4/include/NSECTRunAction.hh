#ifndef NSECTRunAction_h
#define NSECTRunAction_h 1

#include "G4UserRunAction.hh"
#include "g4IdealDetector.hh"

class G4Run;

class NSECTRunAction : public G4UserRunAction
{
  public:
  NSECTRunAction(g4IdealDetector * i_detector);
  ~NSECTRunAction();

  public:
    void BeginOfRunAction(const G4Run*);
    void EndOfRunAction(const G4Run*);

private:

  g4IdealDetector * ideal_detector;

};

#endif





