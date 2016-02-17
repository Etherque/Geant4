#include "NSECTTrackerHit.hh"
#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

G4Allocator<NSECTTrackerHit> NSECTTrackerHitAllocator;

NSECTTrackerHit::NSECTTrackerHit() {}

NSECTTrackerHit::~NSECTTrackerHit() {}

NSECTTrackerHit::NSECTTrackerHit(const NSECTTrackerHit& right)
  : G4VHit()
{
  trackID   = right.trackID;
  chamberNb = right.chamberNb;
  edep      = right.edep;
  pos       = right.pos;
  chamberName = right.chamberName;
  particleName = right.particleName;
}

const NSECTTrackerHit& NSECTTrackerHit::operator=(const NSECTTrackerHit& right)
{
  trackID   = right.trackID;
  chamberNb = right.chamberNb;
  edep      = right.edep;
  pos       = right.pos;
  chamberName = right.chamberName;
  particleName = right.particleName;

  return *this;
}

G4int NSECTTrackerHit::operator==(const NSECTTrackerHit& right) const
{
  return (this==&right) ? 1 : 0;
}

void NSECTTrackerHit::Draw()
{
  G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
  if(pVVisManager)
  {
    G4Circle circle(pos);
    circle.SetScreenSize(0.04);
    circle.SetFillStyle(G4Circle::filled);
    G4Colour colour(1.,0.,0.);
    G4VisAttributes attribs(colour);
    circle.SetVisAttributes(attribs);
    pVVisManager->Draw(circle);
  }
}

void NSECTTrackerHit::Print()
{
  G4cout << "  TrackID:  " << trackID 
         << "  Particle: " << particleName
	 << "  Detector: " << chamberName
         << "  Energy deposit: " << G4BestUnit(edep,"Energy") << G4endl;
    //<< "  Position: " << G4BestUnit(pos,"Length") << G4endl;
}

