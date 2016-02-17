#include "EventAction.hh"
#include "RunAction.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"

extern G4int currentEventID;

EventAction::EventAction()
{
}

EventAction::~EventAction()
{
}

void EventAction::BeginOfEventAction(const G4Event* evt)
{  
  currentEventID = evt->GetEventID();
}


void EventAction::EndOfEventAction(const G4Event*)
{
  
}  

