#ifndef SiliconDetectorTrackerSD_h
#define SiliconDetectorTrackerSD_h 1

#include "G4VSensitiveDetector.hh"
#include "SiliconDetectorTrackerHit.hh"
#include <vector>

class G4Step;
class G4HCofThisEvent;
class EventAction;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class SiliconDetectorTrackerSD : public G4VSensitiveDetector
{
  public:
      SiliconDetectorTrackerSD(G4String);
     ~SiliconDetectorTrackerSD();

      void Initialize(G4HCofThisEvent*);
      G4bool ProcessHits(G4Step*, G4TouchableHistory*);
      void EndOfEvent(G4HCofThisEvent*);

  private:
      SiliconDetectorTrackerHitsCollection* trackerCollection;
      EventAction* event;
      std::vector<G4double> depPosX;
      std::vector<G4double> depPosY;
      std::vector<G4String> depParticleType;
      std::vector<G4double> energyLeft;
      std::vector<G4int>    currentTrackID;
      std::vector<G4int>    parentTrackID;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

