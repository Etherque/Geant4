#ifndef NSECTTrackerHit_h
#define NSECTTrackerHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

class NSECTTrackerHit : public G4VHit
{
public:
  
  NSECTTrackerHit();
  ~NSECTTrackerHit();
  NSECTTrackerHit(const NSECTTrackerHit&);
  const NSECTTrackerHit& operator=(const NSECTTrackerHit&);
  G4int operator==(const NSECTTrackerHit&) const;
  
  inline void* operator new(size_t);
  inline void  operator delete(void*);
  
  void Draw();
  void Print();
  
public:
  
  void SetTrackID    (G4int track)      { trackID = track; };
  void SetChamberNb  (G4int chamb)      { chamberNb = chamb; };  
  void SetChamberName(G4String Name)    { chamberName = Name; };  
  void SetProcessName(G4String Name)    { processName = Name; };  
  void SetEkinetic   (G4double ke)      { ekinetic = ke; };
  void SetEdep       (G4double de)      { edep = de; };
  void SetPos        (G4ThreeVector xyz){ pos = xyz; };
  void SetParticleName(G4String Name)    { particleName = Name; };  
  
  G4int GetTrackID()         { return trackID; };
  G4int GetChamberNb()       { return chamberNb; };
  G4String GetChamberName()  { return chamberName; };
  G4String GetProcessName()  { return processName; };
  G4double GetEkinetic()     { return ekinetic; };      
  G4double GetEdep()         { return edep; };      
  G4ThreeVector GetPos()     { return pos; };
  G4String GetParticleName()  { return particleName; };
  
private:
  
  G4int         trackID;
  G4int         chamberNb;
  G4double      ekinetic;
  G4double      edep;
  G4ThreeVector pos;
  G4String      chamberName;
  G4String      processName;
  G4String      particleName;
};

typedef G4THitsCollection<NSECTTrackerHit> NSECTTrackerHitsCollection;

extern G4Allocator<NSECTTrackerHit> NSECTTrackerHitAllocator;

inline void* NSECTTrackerHit::operator new(size_t)
{
  void *aHit;
  aHit = (void *) NSECTTrackerHitAllocator.MallocSingle();
  return aHit;
}

inline void NSECTTrackerHit::operator delete(void *aHit)
{
  NSECTTrackerHitAllocator.FreeSingle((NSECTTrackerHit*) aHit);
}

#endif
