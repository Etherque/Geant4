#include "NSECTRunAction.hh"

#include "G4Run.hh"

using std::vector;

NSECTRunAction::NSECTRunAction(g4IdealDetector * i_detector)
{
  ideal_detector = i_detector;
}

NSECTRunAction::~NSECTRunAction()
{;}

void NSECTRunAction::BeginOfRunAction(const G4Run* )
{
  //G4cout << "Start of Run" << G4endl;
  ideal_detector->ZeroHistogram();
}

void NSECTRunAction::EndOfRunAction(const G4Run *)
{
  ideal_detector->PrintHistogram();
  //G4cout << "End of Run" << G4endl;
}




