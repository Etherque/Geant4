#include "NSECTPrimaryGeneratorAction.hh"
#include "NSECTDetectorConstruction.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

NSECTPrimaryGeneratorAction::NSECTPrimaryGeneratorAction()
{
  G4int n_particle = 1;
  particleGun = new G4ParticleGun(n_particle);
  
  // default particle  
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* particle = particleTable->FindParticle("neutron");

  G4double momX =  0.0;
  G4double momY =  0.0;
  G4double momZ =  1.0;
  
  particleGun->SetParticleDefinition(particle);
  particleGun->SetParticleMomentumDirection(G4ThreeVector(momX ,momY ,momZ));
  //particleGun->SetParticleEnergy(energy);
  
  // beam width - step_size = FOV / NUM_STOPS;
  G4double step_size = 10.0*cm;
  G4double half_step = step_size/2;

  x_max =  half_step;
  x_min = -half_step;

  y_max =  half_step;
  y_min = -half_step;

}

NSECTPrimaryGeneratorAction::~NSECTPrimaryGeneratorAction()
{
  delete particleGun;
}

void NSECTPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{   
  // x and y vary over range
  G4double positionX = x_min + G4UniformRand() * (x_max - x_min);
  G4double positionY = y_min + G4UniformRand() * (y_max - y_min);
  G4double positionZ = -100.0*cm;
  energy = 65.0*MeV+G4UniformRand()*5.0*MeV;
  
  particleGun->SetParticlePosition(G4ThreeVector(positionX, 
						 positionY, 
						 positionZ));
  particleGun->SetParticleEnergy(energy);
  particleGun->GeneratePrimaryVertex(anEvent);
}



