#include "EventAction.hh"
#include "RunAction.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4SteppingManager.hh"

extern G4int currentEventID;
extern G4int scatterCnt;

EventAction::EventAction()
{
}

EventAction::~EventAction()
{
}

void EventAction::BeginOfEventAction(const G4Event* evt)
{  
    currentEventID = evt->GetEventID();
    G4cout<<"\n";
    //G4cout << "Event " << currentEventID<< "  "<< scatterCnt;//evt->GetEventID()<<" ";
    scatterCnt = 0;
}


void EventAction::EndOfEventAction(const G4Event*)
{
}  

