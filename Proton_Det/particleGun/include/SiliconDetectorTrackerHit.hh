#ifndef SiliconDetectorTrackerHit_h
#define SiliconDetectorTrackerHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

#include <vector>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class EventAction;

class SiliconDetectorTrackerHit : public G4VHit
{
  public:

      SiliconDetectorTrackerHit(EventAction*);
     ~SiliconDetectorTrackerHit();
      SiliconDetectorTrackerHit(const SiliconDetectorTrackerHit&);
      const SiliconDetectorTrackerHit& operator=(const SiliconDetectorTrackerHit&);
      G4int operator==(const SiliconDetectorTrackerHit&) const;

      inline void* operator new(size_t);
      inline void  operator delete(void*);

      void Draw();
      void Print();

  public:
  
      void SetTrackID  (G4int track)      { trackID = track; };
      void SetChamberNb(G4int chamb)      { chamberNb = chamb; };  
      void SetEdep     (G4double de)      { edep = de; };
      void SetPos      (G4ThreeVector xyz){ pos = xyz; };
      
      G4int GetTrackID()    { return trackID; };
      G4int GetChamberNb()  { return chamberNb; };
      G4double GetEdep()    { return edep; };      
      G4ThreeVector GetPos(){ return pos; };
      
  private:
  
      G4int         trackID;
      G4int         chamberNb;
      G4double      edep;
      G4ThreeVector pos;
   
      std::vector <G4double> DepositPosX;
      std::vector <G4double> DepositPosY;

      //Include a pointer to the event action
      EventAction* fEventAction;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

typedef G4THitsCollection<SiliconDetectorTrackerHit> SiliconDetectorTrackerHitsCollection;

extern G4Allocator<SiliconDetectorTrackerHit> SiliconDetectorTrackerHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void* SiliconDetectorTrackerHit::operator new(size_t)
{
  void *aHit;
  aHit = (void *) SiliconDetectorTrackerHitAllocator.MallocSingle();
  return aHit;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void SiliconDetectorTrackerHit::operator delete(void *aHit)
{
  SiliconDetectorTrackerHitAllocator.FreeSingle((SiliconDetectorTrackerHit*) aHit);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
