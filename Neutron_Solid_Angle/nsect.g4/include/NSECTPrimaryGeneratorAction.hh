#ifndef NSECTPrimaryGeneratorAction_h
#define NSECTPrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "Randomize.hh"

class G4ParticleGun;
class G4Event;

class NSECTPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
  NSECTPrimaryGeneratorAction();
  ~NSECTPrimaryGeneratorAction();
  
  void GeneratePrimaries(G4Event*);
  
private:
  G4ParticleGun * particleGun;

  G4double x_max, x_min;
  G4double y_max, y_min;
  G4double energy;
};

#endif


