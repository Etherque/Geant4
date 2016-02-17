#include "NSECTEventAction.hh"
#include "NSECTTrackerHit.hh"

#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Trajectory.hh"
#include "G4VVisManager.hh"
#include "G4ios.hh"
#include "G4HCofThisEvent.hh"

#include "G4UnitsTable.hh"

NSECTEventAction::NSECTEventAction()
{
}

NSECTEventAction::~NSECTEventAction()
{;}

void NSECTEventAction::BeginOfEventAction(const G4Event*)
{

//G4cout << ">>> Event " << evt->GetEventID() << G4endl;
}

void NSECTEventAction::EndOfEventAction(const G4Event*)
{
 // G4cout << ">>> Event " << evt->GetEventID() << G4endl;
}

void NSECTEventAction::PrintHit(NSECTTrackerHit * the_hit)
{
  G4cout << the_hit->GetChamberName() << " ";
  G4cout << the_hit->GetParticleName()  << " ";
  G4cout << G4BestUnit(the_hit->GetEkinetic(),"Energy")  << " ";
  G4cout << the_hit->GetProcessName()  << " ";
  G4cout << G4BestUnit(the_hit->GetPos(),"Length") << " ";
  G4cout << G4endl;
}

