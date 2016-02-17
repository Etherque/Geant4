#ifndef WireChamberTrackerHit_h
#define WireChamberTrackerHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

#include <vector>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class EventAction;

class WireChamberTrackerHit : public G4VHit
{
  public:

      WireChamberTrackerHit(EventAction*);
     ~WireChamberTrackerHit();
      WireChamberTrackerHit(const WireChamberTrackerHit&);
      const WireChamberTrackerHit& operator=(const WireChamberTrackerHit&);
      G4int operator==(const WireChamberTrackerHit&) const;

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

typedef G4THitsCollection<WireChamberTrackerHit> WireChamberTrackerHitsCollection;

extern G4Allocator<WireChamberTrackerHit> WireChamberTrackerHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void* WireChamberTrackerHit::operator new(size_t)
{
  void *aHit;
  aHit = (void *) WireChamberTrackerHitAllocator.MallocSingle();
  return aHit;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void WireChamberTrackerHit::operator delete(void *aHit)
{
  WireChamberTrackerHitAllocator.FreeSingle((WireChamberTrackerHit*) aHit);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
