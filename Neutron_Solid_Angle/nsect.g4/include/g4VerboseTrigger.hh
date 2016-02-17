#ifndef _g4VerboseTrigger_hh
#define _g4VerboseTrigger_hh

#include "G4Track.hh"
#include "G4Step.hh"
#include "G4VPhysicalVolume.hh"
#include "G4UnitsTable.hh"
#include "G4VProcess.hh"

// bits
#define RAYLEIGH 0x01
#define DETECTOR 0x02
#define COMPTON  0x04
#define PHOTO    0x08

using std::vector;
class g4VerboseInfo;

class g4VerboseTrigger 
{
public:

  g4VerboseTrigger();
  ~g4VerboseTrigger();

  void AddStep(const G4Step * fStep);

  void Print();

  void Clear();

  static G4int Trigger; // the flag

private:

  void PrintInit(g4VerboseInfo * step_1);

  vector<g4VerboseInfo*> StoredTrackInfo;

};


class g4VerboseInfo 
{
public:

  g4VerboseInfo(const G4Step * fStep);
  g4VerboseInfo(const g4VerboseInfo * other_info);

  ~g4VerboseInfo();

  void          DumpTrack(G4int print_banner);

  G4int         GetTrackID() { return track_id; }
  G4int         GetParentID() { return parent_id; }
  G4int         GetStepNumber() { return step_number; }
  G4double      GetTLength() { return t_length; }
  G4ThreeVector GetPosition() { return position; }

  void  SetStepNumber(G4int s) { step_number = s; }
  void  SetKEnergy(G4double e) { k_energy = e; }
  void  SetTEnergy(G4double e) { t_energy = e; }
  void  SetTLength(G4double tl) { t_length = tl; }
  void  SetSLength(G4double sl) { s_length = sl; }
  void  SetProcess(G4String p) { process = p; }
  void  SetVolume(G4String v) { volume_name = v; }
  void  SetPosition(G4ThreeVector p) { position = p; }

private:

  // prints banner
  void TrackBanner();

  G4int         step_number;
  G4ThreeVector position;
  G4double      k_energy;
  G4double      t_energy;
  G4double      s_length;
  G4double      t_length;

  G4VPhysicalVolume * n_vol_pt;

  G4String volume_name;

  G4String      particle_name;
  G4int         track_id;
  G4int         parent_id;

  G4String      process;

};

#endif
