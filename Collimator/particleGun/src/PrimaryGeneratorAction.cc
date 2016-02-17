#include "PrimaryGeneratorAction.hh"
#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include "DetectorConstruction.hh"
#include "G4RunManager.hh"

#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4Event.hh"
#include "G4UnitsTable.hh"

#define NUMBER_OF_POSSIBLE_THETA 181
#define NUMBER_OF_POSSIBLE_PSI 360

//include standard C++ tools for opening numeric data files
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iterator>
#include <algorithm>

//include math library for making a sine squared distribution for psi
#include <math.h>
#define PI 3.1415926535
#define lists 8
#define totalevents 48

   
using namespace std;
int thetaArraySize;
int psiArraySize;
int numofevents=0;
double acceptevent[totalevents][lists];

static ifstream fileinput, eventsinput;
static string ln, lnevt;

PrimaryGeneratorAction::PrimaryGeneratorAction()
{    
  G4int n_particle = 1;
  particleGun  = new G4ParticleGun(n_particle);
        
  // Give the particle some default parameters that are changed later in 
  // GeneratePrimaries()

    //fileinput.open("/var/phy/project/ntof/zh27/new_theory_calculation/3H17EVENTGEANT/Nonpoloarized_3H17fm_exclusive.txt");
    fileinput.open("/var/phy/project/ntof/zh27/new_theory_calculation/3H19EVENTGEANT/Nonpoloarized_3H19fm_exclusive.txt");

}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
   delete particleGun;
}  

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{  

   G4double energy1, energy2, theta1, theta2, psi1, psi2;
   G4double p1R, p1Z, p1Ang, p2R, p2Z, p2Ang;
   double tmparray[12];
  
   getline(fileinput,ln);
   
   sscanf(ln.c_str(), "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf", &tmparray[0], &tmparray[1], &tmparray[2], &tmparray[3], &tmparray[4], &tmparray[5], &tmparray[6], &tmparray[7], &tmparray[8], &tmparray[9], &tmparray[10], &tmparray[11]);

   //--------------particle 1 event----------------//
   G4ParticleDefinition* particle1 = G4ParticleTable::GetParticleTable()->FindParticle("proton");
   particleGun->SetParticleDefinition(particle1);
   energy1= tmparray[0]*MeV;
   particleGun->SetParticleEnergy(energy1);
   theta1= tmparray[1]*deg;
   psi1= tmparray[2]*deg;
   //Set the initial direction of propagation 
   G4double cosTheta1 = std::cos(theta1);
   G4double sinTheta1 = std::sqrt(1. - cosTheta1*cosTheta1);  
   G4ThreeVector dir1(sinTheta1*std::cos(psi1),sinTheta1*std::sin(psi1),cosTheta1);//(x=sin(theta)cos(phi),y=sin(theta)sin(phi),z=cos(theta)
   particleGun->SetParticleMomentumDirection(dir1);

   p1R = tmparray[3]*mm;
   p1Z = 0.8 * tmparray[4]*cm;
   p1Ang = tmparray[5]*deg;
   
   G4ThreeVector Pos1(p1R * std::cos(p1Ang), p1R * std::sin(p1Ang), p1Z);
   particleGun->SetParticlePosition(Pos1);  
   particleGun->GeneratePrimaryVertex(anEvent);

   //-----------------------------particle 2 event--------------------------//
   G4ParticleDefinition* particle2 = G4ParticleTable::GetParticleTable()->FindParticle("neutron");
   particleGun->SetParticleDefinition(particle2);
   energy2= tmparray[6]*MeV;
   particleGun->SetParticleEnergy(energy2);

   theta2= tmparray[7]*deg;
   psi2= tmparray[8]*deg;
   //Set the initial direction of propagation 
   G4double cosTheta2 = std::cos(theta2);
   G4double sinTheta2 = std::sqrt(1. - cosTheta2*cosTheta2);  
   G4ThreeVector dir2(sinTheta2*std::cos(psi2),sinTheta2*std::sin(psi2),cosTheta2);//(x=sin(theta)cos(phi),y=sin(theta)sin(phi),z=cos(theta)
   particleGun->SetParticleMomentumDirection(dir2);

   p2R = tmparray[9]*mm;
   p2Z = 0.8 * tmparray[10]*cm;
   p2Ang = tmparray[11]*deg;
   G4ThreeVector Pos2(p2R * std::cos(p2Ang), p2R * std::sin(p2Ang), p2Z);
   particleGun->SetParticlePosition(Pos2);  
   particleGun->GeneratePrimaryVertex(anEvent);
   numofevents++;
}

