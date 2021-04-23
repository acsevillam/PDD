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

#ifndef DetectorMatrix_h
#define DetectorMatrix_h 1
#include <G4ParticleDefinition.hh>
#include "globals.hh"
#include <vector>
#include <fstream>


class G4Material;
// The information: energy deposit and position in the phantom
// is stored in a matrix

// type struct useful to store nucludes data


struct ion
{ 
  G4bool isPrimary;   			// true if particle is primary
  G4int PDGencoding;  			// Particle data group id for the particle
  //G4String extName; 			//  AZ[excitation energy]: like He3[1277.4], He4[0.0], Li7[231.4], ...
  G4String name;   	 			// simple name without excitation energy: He3, He4, Li7, ...
  std::string::size_type len; 	// name length
  G4int Z; 		 				// atomic number
  G4int A; 		 				// mass number
  G4double* eDep;				// pointer to edep matrix
  G4double* letN;				// pointer to let dose numerator matrix
  G4double* letD;				// pointer to let dose denominator matrix
  G4double* fluence;			// pointer to fluence matrix
  //friend G4bool operator<(const ion& a, const ion& b) {return (a.Z == b.Z) ? b.A < a.A : b.Z < a.Z ;}
  G4bool operator<(const ion& a) const{return (this->Z == a.Z) ? this-> A < a.A : this->Z < a.Z ;}
};

class DetectorMatrix
{
private:
  DetectorMatrix(G4int nX, G4int nY, G4int nZ,  G4double massOfVoxel);


public:

  ~DetectorMatrix();

  // Get object instance only
  static DetectorMatrix* GetInstance();

  // Make & Get instance
  static DetectorMatrix* GetInstance(G4int nX, G4int nY, G4int nZ, G4double massOfVoxel);

  // All the elements of the matrix are initialized to zero
  void Initialize();
  void Clear();

  // Full list of generated nuclides
  void PrintNuclides(); 

  // Hit array marker (useful to avoid multiple counts of fluence)
  void ClearHitTrack();
  G4int* GetHitTrack(G4int i, G4int j, G4int k);

  // Fill energy deposit matrix
  G4bool FillEdep(G4int i, G4int j, G4int k, G4double energyDeposit, G4int trackID, G4ParticleDefinition* particleDef);

  // Fill fluence matrix
  G4bool FillLet(G4int i, G4int j, G4int k, G4double energyDeposit, G4double dx, G4double kinEMean, G4int trackID, G4ParticleDefinition* particleDef, G4Material* mat);

  // Fill fluence matrix
  G4bool FillFluence(G4int i, G4int j, G4int k, G4double dx,G4double vol, G4int trackID, G4ParticleDefinition* particleDef);
   
  // General store matrix data to filename
  void StoreAscii(G4String file, G4double** data, G4double unit, G4double scale);
  void StoreAscii(G4String file, G4int** data, G4double unit, G4double scale);

  // Store all energy deposit data to filename
  void StoreEDepAscii();

  // Store all energy deposit data to filename
  void StoreLetAscii();

  // Store all fluence data to filename
  void StoreFluenceAscii();

  // Get index from voxel position
  inline G4int Index(G4int i, G4int j, G4int k) { return (i * fNY + j) * fNZ + k; }

  // Total number of voxels read only access
  G4int GetNvoxel(){return fNX*fNY*fNZ;}


public:

  static G4bool secondary;
  static G4String parent_folder;

private:

  std::ofstream ofs;

  static DetectorMatrix* instance;

  G4int fNX, fNY, fNZ;

  G4double fMassOfVoxel;

  G4int* hitTrack;

  // data store
  std::vector <ion> ionStore;

};
#endif // DetectorMatrix_h

