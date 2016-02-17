#ifndef NSECTDetectorMessenger_h
#define NSECTDetectorMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class NSECTDetectorConstruction;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithAnInteger;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithADouble;
class G4UIcmdWithoutParameter;

class NSECTDetectorMessenger: public G4UImessenger
{
public:
  NSECTDetectorMessenger(NSECTDetectorConstruction*);
  ~NSECTDetectorMessenger();

  void SetNewValue(G4UIcommand* command,G4String newValue);
  
// G4UImanager  * the_ui;

private:
  NSECTDetectorConstruction* myDetector;
  
  G4UIdirectory*             NSECTDir;
  G4UIcmdWithAnInteger*      PositionCmd;
};

#endif

