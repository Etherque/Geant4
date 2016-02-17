#include "SiliconDetectorTrackerHit.hh"
#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

#include "EventAction.hh"


G4Allocator<SiliconDetectorTrackerHit> SiliconDetectorTrackerHitAllocator;

SiliconDetectorTrackerHit::SiliconDetectorTrackerHit(EventAction* event)
: fEventAction(event)
{
}

SiliconDetectorTrackerHit::~SiliconDetectorTrackerHit() {}

SiliconDetectorTrackerHit::SiliconDetectorTrackerHit(const SiliconDetectorTrackerHit& right)
  : G4VHit()
{
  trackID   = right.trackID;
  chamberNb = right.chamberNb;
  edep      = right.edep;
  pos       = right.pos;

}

const SiliconDetectorTrackerHit& SiliconDetectorTrackerHit::operator=(const SiliconDetectorTrackerHit& right)
{
  trackID   = right.trackID;
  chamberNb = right.chamberNb;
  edep      = right.edep;
  pos       = right.pos;
  return *this;
}

G4int SiliconDetectorTrackerHit::operator==(const SiliconDetectorTrackerHit& right) const
{
  return (this==&right) ? 1 : 0;
}

void SiliconDetectorTrackerHit::Draw()
{
  G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
  if(pVVisManager)
  {
    G4Circle circle(pos);
    circle.SetScreenSize(2.);
    circle.SetFillStyle(G4Circle::filled);
    G4Colour colour(1.,0.,0.);
    G4VisAttributes attribs(colour);
    circle.SetVisAttributes(attribs);
    pVVisManager->Draw(circle);
  }
}

void SiliconDetectorTrackerHit::Print()
{
  G4cout << "  trackID: " << trackID << "  chamberNb: " << chamberNb
         << "  energy deposit: " << G4BestUnit(edep,"Energy")
         << "  position: " << G4BestUnit(pos,"Length") << G4endl;

  G4int id = 4;
}
