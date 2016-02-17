#include "Randomize.hh"

#include "DetectorConstruction.hh"
//#include "PhysicsList.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "SteppingAction.hh"

#include "G4VisManager.hh"
#include "G4VisExecutive.hh"
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIterminal.hh"
#include "G4UItcsh.hh"
#include "G4PhysListFactory.hh"

#include "QGSP_BIC_HP.hh"
#include "FTFP_BERT.hh"


G4int currentEventID;
G4int scatterCnt;

int main(int argc,char** argv) {

  //choose the Random engine
  CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);

  // Construct the default run manager
  G4RunManager * runManager = new G4RunManager;

  // set mandatory initialization classes
  //
  DetectorConstruction* detector = new DetectorConstruction;
  runManager->SetUserInitialization(detector);

  G4PhysListFactory *physList_Factory = new G4PhysListFactory(); 
  
 // G4VUserPhysicsList* physics_list = new FTFP_BERT();  
  G4VUserPhysicsList* physics_list = new QGSP_BIC_HP();
  //PhysicsList* physics = new PhysicsList;
  runManager->SetUserInitialization(physics_list);
      
  PrimaryGeneratorAction* prim = new PrimaryGeneratorAction();
  runManager->SetUserAction(prim);
  
  RunAction* run_action = new RunAction();
  runManager->SetUserAction(run_action);
  //
  EventAction* event_action = new EventAction();

  SteppingAction* stepping_action = new SteppingAction();
        
  runManager->SetUserAction(event_action);
  runManager->SetUserAction(stepping_action);
    
  //Initialize G4 kernel
  runManager->Initialize();
    
  
    G4UImanager* UI = G4UImanager::GetUIpointer();  

  if (argc==1)   // batch mode  
    { 
     G4VisManager* visManager = new G4VisExecutive;
     visManager->Initialize();
     
     G4UIsession *session =0;
     session = new G4UIterminal(new G4UItcsh);
     
     UI->ApplyCommand("/control/execute vis.mac");    
     session->SessionStart();
     
     delete visManager;
     delete session;
    }
    
  else           // define visualization and UI terminal for interactive mode 
    { 
     G4String command = "/control/execute ";
     G4String fileName = argv[1];     
     UI->ApplyCommand(command+fileName); 
    }

  // job termination
  //
  delete runManager;

  return 0;
}


