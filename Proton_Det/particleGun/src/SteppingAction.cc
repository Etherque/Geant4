#include "SteppingAction.hh"

#include "DetectorConstruction.hh"
#include "EventAction.hh"

#include "G4SteppingManager.hh"
#include "G4Step.hh"
#include "G4VVisManager.hh"
#include "G4Colour.hh"
#include "G4Polyline.hh" 
#include "G4VisAttributes.hh"
#include "G4VPhysicalVolume.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4Event.hh"
#include "G4UnitsTable.hh"

SteppingAction::SteppingAction()                                      
{ }

SteppingAction::~SteppingAction()
{ }

void SteppingAction::UserSteppingAction(const G4Step* aStep)
{
 
  G4Track * fTrack  = aStep->GetTrack();
  G4VPhysicalVolume * n_vol_pt = fTrack->GetNextVolume();
  G4String name = aStep->GetTrack()->GetDefinition()->GetParticleName();
  G4String volume_name, next_volume_name;
  G4VPhysicalVolume* volume = aStep->GetPreStepPoint()->GetTouchableHandle()->GetVolume();
// G4String procName = aStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();
  G4double current_energy = fTrack->GetKineticEnergy();
                  
  
  if(n_vol_pt){
       next_volume_name = fTrack->GetNextVolume()->GetName();
       volume_name = fTrack->GetVolume()->GetName();
 }
  
  if((name=="proton")&&(next_volume_name.contains("Chamber"))){ 
       G4cout << "Chamber_proton_: " 
              << G4BestUnit(current_energy,"Energy")
              << G4endl;
       
 }
  
  //collect energy and track length step by step
  //G4double edep = aStep->GetTotalEnergyDeposit();
}

