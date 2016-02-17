#include "math.h"

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

extern G4int currentEventID;
extern G4int scatterCnt;

SteppingAction::SteppingAction()                                      
{ }

SteppingAction::~SteppingAction()
{ }

void SteppingAction::UserSteppingAction(const G4Step* aStep)
{
  G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
  G4Track * fTrack  = aStep->GetTrack();
  G4VPhysicalVolume * n_vol_pt = fTrack->GetNextVolume();
  G4String name = aStep->GetTrack()->GetDefinition()->GetParticleName();
  G4String prestep, volume_name, next_volume_name;
  G4VPhysicalVolume* volume = aStep->GetPreStepPoint()->GetTouchableHandle()->GetVolume();
  G4String procName;
  G4int parentID = fTrack->GetParentID();     
  G4int trackID = fTrack->GetTrackID();              
  G4int stepNum = fTrack->GetCurrentStepNumber();
  G4double chamber_energy,SDet_energy,MFilm_energy,chamberouter_energy,MFilmOuter_energy,MFilm_energy_prev_R1;
  if(n_vol_pt){
       next_volume_name = fTrack->GetNextVolume()->GetName();
       volume_name = fTrack->GetVolume()->GetName();
 }
   
 //if((name=="proton")&&(parentID==0)){
 if((name=="proton" || name == "neutron")&&(parentID==0)){
    if(aStep->GetPostStepPoint()->GetProcessDefinedStep()!=0)
          procName=aStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();
    //if(procName=="initStep"){
    if(stepNum==1){
                positionIn = fTrack->GetMomentumDirection();
                Intx = positionIn.x(), Inty = positionIn.y(), Intz = positionIn.z();
                Eint = fTrack->GetKineticEnergy();
               }

    if(next_volume_name.contains("Colimators") && (procName == "hadElastic")) {
       scatterCnt++;
   }
    
    if(next_volume_name.contains("Plastic")&&(procName=="Transportation")){
                   positionHT = fTrack->GetPosition();
                  // Htx = positionHT.x(), Hty = positionHT.y(), Htz = positionHT.z(); 
                   HT_energy = fTrack->GetKineticEnergy();  
    }
   
    if(next_volume_name.contains("Behind")&&(procName=="Transportation")){
                   plB_energy = fTrack->GetKineticEnergy();  
    }
 
    if((next_volume_name.contains("FDetector"))&&(procName=="Transportation")){
                   positionSD = fTrack->GetPosition();
                   Sdx = positionSD.x(), Sdy = positionSD.y(), Sdz = positionSD.z(); 
                   SDet_energy = fTrack->GetKineticEnergy();
                   G4double Thetap = acos(Intz);                   
                   G4double Phi = acos(Intx/sin(Thetap));
                                     
                   G4cout << "Theta_proton_: " <<"\t" <<G4BestUnit(Thetap, "Angle") <<"\t"
                          << "Phi_proton_: " <<"\t" <<G4BestUnit(Phi, "Angle") <<"\t" 
                          << "Plasetic_proton_: " << G4BestUnit(HT_energy,"Energy")<<"\t"
                          << "PlasBehind_proton_: " << G4BestUnit(plB_energy,"Energy")<<"\t"
                          << "FDetector_"+name+"_: " << G4BestUnit(SDet_energy,"Energy")<<"\t"; 
                   fTrack->SetTrackStatus(fStopAndKill);
         }        
} 
  
if(pVVisManager) {
    
    // particle colour in a string
    G4Colour colour;
    
    if(name=="proton") {
      // violet
      colour=G4Colour(0.4, 0.0, 1.0);
    } else if (name=="gamma") {
      // green
      colour=G4Colour(0.0, 1.0, 0.0);
    } else if (name=="e-") {
      // red  
      colour=G4Colour(1.0, 0.0, 0.0);
    }
    else if (name=="e+")
      // yellow
      colour=G4Colour(1.0, 1.0, 0.0);
    else
      // gray
      colour=G4Colour(0.5, 0.5, 0.5);
    
    // create line with colour
    G4VisAttributes attribs(colour);
    G4Polyline polyline;
    polyline.SetVisAttributes(attribs);
    
    // draw line
    G4Point3D start(aStep->GetPreStepPoint()->GetPosition());
    G4Point3D end(aStep->GetPostStepPoint()->GetPosition());
    polyline.push_back(start);
    polyline.push_back(end);
    pVVisManager->Draw(polyline);
  }
}

