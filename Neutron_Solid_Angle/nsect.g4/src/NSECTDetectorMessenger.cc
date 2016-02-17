#include "NSECTDetectorMessenger.hh"

#include "NSECTDetectorConstruction.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "globals.hh"

NSECTDetectorMessenger::NSECTDetectorMessenger(NSECTDetectorConstruction* myDet)
  :myDetector(myDet)
{ 
  char num_positions[32];
  sprintf(num_positions, "%d", NSECT_NUM_POSITIONS);
 
  G4String help_string = "Select Position 1-";
  help_string += num_positions;

  NSECTDir = new G4UIdirectory("/NSECT/");
  NSECTDir->SetGuidance("NSECT commands.");
  
  PositionCmd = new G4UIcmdWithAnInteger("/NSECT/Position",this);
  PositionCmd->SetGuidance(help_string);
}

NSECTDetectorMessenger::~NSECTDetectorMessenger()
{
  delete NSECTDir;
  delete PositionCmd;
}

void NSECTDetectorMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{ 
  if( command == PositionCmd ) { 
    G4int position = PositionCmd->GetNewIntValue(newValue);
    myDetector->setTargetPosition(position); 
  }  

}
