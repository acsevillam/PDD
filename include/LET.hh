/*
 * PDD 1.0
 * Copyright (c) 2020
 * Universidad Nacional de Colombia
 * Servicio Geológico Colombiano
 * All Right Reserved.
 *
 * Developed by Andrés Camilo Sevilla Moreno
 *
 * Use and copying of these libraries and preparation of derivative works
 * based upon these libraries are permitted. Any copy of these libraries
 * must include this copyright notice.
 *
 * Bogotá, Colombia.
 *
 */
#ifndef LET_h
#define LET_h 1
#endif

#include "G4ParticleDefinition.hh"
#include "globals.hh"
#include <fstream>
#include <vector>
#include <string>

using namespace std;

struct ionLet
{
    G4bool isPrimary;        // True if particle is primary
    G4int PDGencoding;      // Particle data group id for the particle
    G4String fullName;      // AZ[excitation energy]: like He3[1277.4], He4[0.0], Li7[231.4], ...
    G4String name;          // simple name without excitation energy: He3, He4, Li7, ...
    G4int Z;                // atomic number
    G4int A;            // mass number
    G4double *letDN , *letDD, *letTN , *letTD; // Track averaged LET and Dose averaged LET
    //friend G4bool operator<(const ionLet& a, const ionLet& b) {return (a.Z == b.Z) ? b.A < a.A : b.Z < a.Z ;}
    G4bool operator<(const ionLet& a) const{return (this->Z == a.Z) ? this-> A < a.A : this->Z < a.Z ;}
    // For isotopes sort by the mass number, else sort by the atomic one.
};

class G4Material;
class PDD1PrimaryGeneratorAction;
class PDD1DetectorConstruction;

class LET
{
private:
    LET(PDD1DetectorConstruction*);
    
public:
    ~LET();
    static LET* GetInstance(PDD1DetectorConstruction*);
    static LET* GetInstance();
    static G4bool doCalculation;
    void Initialize();
    void Clear();
    
    void Fill(G4int i, G4int j, G4int k, G4double DE, G4double DX);
    void FillEnergySpectrum (G4int trackID,
                             G4ParticleDefinition* particleDef,
                             G4double ekinMean,
                             G4Material* mat,
                             G4double DE,
                             G4double DEEletrons,
                             G4double DX,
                             G4int i, G4int j, G4int k);
    void LetOutput();
    void StoreLetAscii();
    
    
private:
    static LET *instance;
    G4Material* detectorMat;
    G4String filename;
    
    std::ofstream ofs;
    std::ofstream stopFile;
    PDD1DetectorConstruction *fDet;
    G4int nVoxels, numberOfVoxelAlongX, numberOfVoxelAlongY, numberOfVoxelAlongZ ;
    G4double *totalLetT, *DtotalLetT , *DtotalLetD, *totalLetD;
    
    std::vector<ionLet> ionLetStore;
};
