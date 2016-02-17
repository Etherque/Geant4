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
   

  public:        
    G4double ThetaRejectAccept();
    G4double PsiRejectAccept();
    G4double EnergyRejectAccept(G4double);
    G4double InverseCumul();

  public:
    G4double GetTheta() { return theta; }
    G4double GetPsi() { return psi; }            
    
  private:    
    G4ParticleGun*         particleGun;
    DetectorConstruction*    Detector;     //pointer to the geometry
 
    std::vector<G4int>     thetaArray;
    std::vector<G4double>  thetaPDF;     
    std::vector<G4double>  thetaSlope;           //slopes      

    std::vector<G4double>  psiArray;
    std::vector<G4double>  psiPDF;
    std::vector<G4double>  psiSlope;

    G4double		   maxEnergy;
    G4double		   energyIncrement;
    G4int		   sizeOfEnergyPDF;
    std::vector<G4double>  energyArray;
    std::vector< std::vector<G4double> >  energyPDF;
    std::vector< std::vector<G4double> >  energySlope;

    std::vector<G4double>  Fc;          //cumulative of f
    G4double               thetaMax;        //max(f)
    G4double		   psiMax;
    G4double		   energyMax[181];
    G4double		   theta;
    G4double		   psi;
    G4int		   sizeOfEnergyArray;

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
