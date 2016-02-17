#ifndef WireChamberTrackerSD_h
#define WireChamberTrackerSD_h 1

#include "G4VSensitiveDetector.hh"
#include "WireChamberTrackerHit.hh"
#include <vector>


class G4Step;
class G4HCofThisEvent;
class EventAction;
class G4Event;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class WireChamberTrackerSD : public G4VSensitiveDetector
{
  public:
      WireChamberTrackerSD(G4String);
     ~WireChamberTrackerSD();

      void Initialize(G4HCofThisEvent*);
      G4bool ProcessHits(G4Step*, G4TouchableHistory*);
      void EndOfEvent(G4HCofThisEvent*);

  private:
      WireChamberTrackerHitsCollection* WiretrackerCollection;
      EventAction* event;
      std::vector<G4double> depPosX;
      std::vector<G4double> depPosY;
      std::vector<G4String> depParticleType;
      std::vector<G4double> energyLeft;
      std::vector<G4int>    currentTrackID;
      std::vector<G4int>    parentTrackID;
      std::vector<G4int>    chamberNumber;
      std::vector<G4double> timeOfCurrentHit;
      std::vector<G4double> eventID;

      // Energy deposited by one proton (including secondaries)
      std::vector<G4double> energyDepositedInFirstChamber;
      std::vector<G4double> energyDepositedInSecondChamber;

      G4double energyBeforeFirstWireChamber;
      G4double energyAfterFirstWireChamber;
     
      G4double energyBeforeSecondWireChamber;
      G4double energyAfterSecondWireChamber;       
 
      std::vector<G4double> energyBeforeWC;
      G4double totalEDep;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
