#ifndef PrimaryGeneratorAction2_h
#define PrimaryGeneratorAction2_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"
#include <vector>

class G4ParticleGun;
class G4Event;
class DetectorConstruction;

class PrimaryGeneratorAction: public G4VUserPrimaryGeneratorAction
{
  public:
    PrimaryGeneratorAction();    
   ~PrimaryGeneratorAction();
  
  public:
    void GeneratePrimaries(G4Event*);
    G4ParticleGun* GetParticleGun() { return particleGun; };
    
  private:    
    G4ParticleGun*         particleGun;
    DetectorConstruction*    Detector;     //pointer to the geometry
 
    std::vector<G4int>     thetaArray;
    std::vector<G4double>  thetaPDF;     
    std::vector<G4double>  thetaSlope;           //slopes      

    std::vector<G4double>  particleInitialPosR;
    std::vector<G4double>  particleInitialPosZ;
    std::vector<G4double>  particleInitialPosTheta;
  
    std::vector<G4double>  particleInitialDirTheta;
    std::vector<G4double>  particleInitialDirPhi;
    std::vector<G4double>  particleInitialEnergy;
    
  private:
   void InitPsi();
    void InitTheta();
    void InitEnergy();
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
