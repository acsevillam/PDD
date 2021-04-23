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

// PDD1 Headers
#include "DetectorMatrix.hh"

// Geant4 Headers
#include "globals.hh"
#include "G4SystemOfUnits.hh"
#include "G4RunManager.hh"
#include "G4ParticleGun.hh"
#include "G4SystemOfUnits.hh"
#include "G4RunManager.hh"
#include "G4Run.hh"
#include "G4EmCalculator.hh"

// C++ Headers
#include <time.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <array>

DetectorMatrix* DetectorMatrix::instance = NULL;
G4bool DetectorMatrix::secondary = true;
G4String DetectorMatrix::parent_folder = "data";


// Only return a pointer to matrix
DetectorMatrix* DetectorMatrix::GetInstance()
{
	return instance;
}

// TODO A check on the parameters is required!
DetectorMatrix* DetectorMatrix::GetInstance(G4int voxelX, G4int voxelY, G4int voxelZ, G4double mass)
{
	if (instance) delete instance;
	instance = new DetectorMatrix(voxelX, voxelY, voxelZ, mass);
	instance -> Initialize();
	return instance;
}

DetectorMatrix::DetectorMatrix(G4int voxelX, G4int voxelY, G4int voxelZ, G4double mass)
{
	// Number of the voxels of the phantom
	// For Y = Z = 1 the phantom is divided in slices (and not in voxels)
	// orthogonal to the beam axis
	fNX = voxelX;
	fNY = voxelY;
	fNZ = voxelZ;
	fMassOfVoxel = mass;

	G4cout << "DetectorMatrix: Memory space to store physical variables into " <<
			fNX*fNY*fNZ <<
			" voxels has been allocated " << G4endl;


	// Hit voxel (TrackID) marker
	// This array mark the status of voxel, if a hit occur, with the trackID of the particle
	// Must be initialized

	hitTrack = new G4int[fNX*fNY*fNZ];
	ClearHitTrack();
}

DetectorMatrix::~DetectorMatrix()
{
	delete[] hitTrack;
	Clear();
}

void DetectorMatrix::Clear()
{
	for (size_t i=0; i<ionStore.size(); i++)
	{
		delete[] ionStore[i].eDep;
		delete[] ionStore[i].letN;
		delete[] ionStore[i].letD;
		delete[] ionStore[i].fluence;
	}
	ionStore.clear();
}

// Initialize the elements of the matrix to zero
void DetectorMatrix::Initialize()
{
	// Clear ions store
	Clear();

}

// Print generated nuclides list
void DetectorMatrix::PrintNuclides()
{
	for (size_t i=0; i<ionStore.size(); i++)
	{
		G4cout << ionStore[i].name << G4endl;
	}
}

// Clear Hit voxel (TrackID) markers
void DetectorMatrix::ClearHitTrack()
{
	for(G4int i=0; i<fNX*fNY*fNZ; i++) hitTrack[i] = 0;
}

// Return Hit status
G4int* DetectorMatrix::GetHitTrack(G4int i, G4int j, G4int k)
{
	return &(hitTrack[Index(i,j,k)]);
}

// General method to store matrix data to filename
void DetectorMatrix::StoreAscii(G4String filename, G4double** data,G4double unit, G4double scale=1.0)
{

	//std::sort(ionStore.begin(), ionStore.end());

	if(data)
	{
		ofs.open(filename, std::ios::out);
		if (ofs.is_open())
		{
			// Write the voxels index and the list of particles/ions
			ofs << "i" << '\t' << "j" << '\t' << "k";
			ofs <<'\t' << "Total";

			if (secondary)
			{
				for (size_t l=0; l < ionStore.size(); l++)
				{
					G4String a = (ionStore[l].isPrimary) ? "_1":"";     // is it a primary?
					// Total dose
					ofs <<'\t' <<ionStore[l].name + a;
				}
			}

			// Write data
			for(G4int i = 0; i < fNX; i++)
				for(G4int j = 0; j < fNY; j++)
					for(G4int k = 0; k < fNZ; k++)
					{

						G4int n = Index(i, j, k);
						G4double** pdata = (G4double**) data;

						G4double total=0;

						for (size_t l=0; l < ionStore.size(); l++)
						{
							if (pdata[l][n])
							{
								total+=pdata[l][n];
							}
						}

						if(total){
							ofs << G4endl;
							ofs << i << '\t' << j << '\t' << k;
							ofs << '\t' << total*scale/unit;

							if (secondary)
							{
								for (size_t l=0; l < ionStore.size(); l++)
								{
									ofs << '\t' << pdata[l][n]*scale/unit;
								}
							}
						}

					}
			ofs.close();
		}
	}
}

// General method to store matrix data to filename
void DetectorMatrix::StoreAscii(G4String filename, G4int** data,G4double unit, G4double scale)
{

	//std::sort(ionStore.begin(), ionStore.end());

	if(data)
	{
		ofs.open(filename, std::ios::out);
		if (ofs.is_open())
		{
			// Write the voxels index and the list of particles/ions
			ofs << "i" << '\t' << "j" << '\t' << "k";
			ofs <<'\t' << "Total";

			if (secondary)
			{
				for (size_t l=0; l < ionStore.size(); l++)
				{
					G4String a = (ionStore[l].isPrimary) ? "_1":"";     // is it a primary?
					// Total dose
					ofs <<'\t' <<ionStore[l].name + a;
				}
			}

			// Write data
			for(G4int i = 0; i < fNX; i++)
				for(G4int j = 0; j < fNY; j++)
					for(G4int k = 0; k < fNZ; k++)
					{

						G4int n = Index(i, j, k);
						G4int** pdata = (G4int**) data;

						G4int total=0;

						for (size_t l=0; l < ionStore.size(); l++)
						{
							if (pdata[l][n])
							{
								total+=pdata[l][n];
							}
						}

						if(total){
							ofs << G4endl;
							ofs << i << '\t' << j << '\t' << k;
							ofs << '\t' << G4double(total)*scale/unit;

							if (secondary)
							{
								for (size_t l=0; l < ionStore.size(); l++)
								{
									ofs << '\t' << G4double(pdata[l][n])*scale/unit;
								}
							}
						}

					}
			ofs.close();
		}
	}
}

void DetectorMatrix::StoreEDepAscii()
{

	G4int nEvents = G4RunManager::GetRunManager()->GetCurrentRun()->GetNumberOfEventToBeProcessed() ;

	auto eDep = new G4double*[ionStore.size()];

	for (size_t l=0; l < ionStore.size(); l++){
		eDep[l] = new G4double[fNX*fNY*fNZ];
	}

	for(G4int i = 0; i < fNX; i++)
		for(G4int j = 0; j < fNY; j++)
			for(G4int k = 0; k < fNZ; k++)
			{
				G4int n = Index(i, j, k);
				for (size_t l=0; l < ionStore.size(); l++){
					eDep[l][n]=ionStore[l].eDep[n];
				}
			}

	StoreAscii(parent_folder+"/"+"Edep.out", eDep,MeV, 1./nEvents);
}

void DetectorMatrix::StoreLetAscii()
{
	auto let = new G4double*[ionStore.size()];

	for (size_t l=0; l < ionStore.size(); l++){
		let[l] = new G4double[fNX*fNY*fNZ];
	}

	for(G4int i = 0; i < fNX; i++)
		for(G4int j = 0; j < fNY; j++)
			for(G4int k = 0; k < fNZ; k++)
			{
				G4int n = Index(i, j, k);
				for (size_t l=0; l < ionStore.size(); l++){
					if (ionStore[l].letD[n]>0.){
						let[l][n]=ionStore[l].letN[n]/ionStore[l].letD[n];
					}
					else{
						let[l][n]=0.;
					};
				}
			}

	StoreAscii(parent_folder+"/"+"Let.out", let,keV/um, 1.0);
}

void DetectorMatrix::StoreFluenceAscii()
{

	auto fluence = new G4double*[ionStore.size()];

	for (size_t l=0; l < ionStore.size(); l++){
		fluence[l] = new G4double[fNX*fNY*fNZ];
	}

	for(G4int i = 0; i < fNX; i++)
		for(G4int j = 0; j < fNY; j++)
			for(G4int k = 0; k < fNZ; k++)
			{
				G4int n = Index(i, j, k);
				for (size_t l=0; l < ionStore.size(); l++){
					fluence[l][n]=ionStore[l].fluence[n];
				}
			}

	StoreAscii(parent_folder+"/"+"Fluence.out", fluence,1./cm2, 1.0);
}

G4bool DetectorMatrix::FillEdep(G4int i, G4int j, G4int k, G4double energyDeposit, G4int trackID, G4ParticleDefinition* particleDef){

	// Get Particle Data Group particle ID
	G4int PDGencoding = particleDef -> GetPDGEncoding();
	PDGencoding -= PDGencoding%10;

	// Search for already allocated data...
	for (size_t l=0; l < ionStore.size(); l++)
	{
		if (ionStore[l].PDGencoding == PDGencoding )
		{   // Is it a primary or a secondary particle?

			if ( (trackID ==1 && ionStore[l].isPrimary) || (trackID !=1 && !ionStore[l].isPrimary))
			{

				// Fill a matrix per each ion with the fluence

				ionStore[l].eDep[Index(i, j, k)]+= energyDeposit;

				return true;
			}
		}
	}

	G4int Z = particleDef-> GetAtomicNumber();
	G4int A = particleDef-> GetAtomicMass();
	G4String fullName = particleDef -> GetParticleName();
	G4String name = fullName.substr (0, fullName.find("[") ); // cut excitation energy

	// Let's put a new particle in our store...
	ion newIon = {
			(trackID == 1) ? true:false,
					PDGencoding,
					name,
					name.length(),
					Z,
					A,
					new G4double[fNX * fNY * fNZ],
					new G4double[fNX * fNY * fNZ],
					new G4double[fNX * fNY * fNZ],
					new G4double[fNX * fNY * fNZ]
	};

	// Initialize data
	if (newIon.eDep && newIon.letN && newIon.letD && newIon.fluence)
	{
		for(G4int q=0; q<fNX * fNY * fNZ; q++)
		{
			newIon.eDep[q] = 0;
			newIon.letN[q] = 0;
			newIon.letD[q] = 0;
			newIon.fluence[q] = 0;
		}

		newIon.eDep[Index(i, j, k)]+=energyDeposit;

		ionStore.push_back(newIon);
		return true;
	}

	else // XXX Out of memory! XXX

	{
		return false;
	}

}


G4bool DetectorMatrix::FillLet(G4int i, G4int j, G4int k, G4double energyDeposit, G4double /*dx*/, G4double kinEMean, G4int trackID, G4ParticleDefinition* particleDef, G4Material* mat){

	// Get Particle Data Group particle ID
	G4int PDGencoding = particleDef -> GetPDGEncoding();
	PDGencoding -= PDGencoding%10;

	// ICRU stopping power calculation
	G4EmCalculator emCal;
	// use the mean kinetic energy of ions in a step to calculate ICRU stopping power
	G4double Lsn = emCal.ComputeElectronicDEDX(kinEMean, particleDef, mat);

	// Search for already allocated data...
	for (size_t l=0; l < ionStore.size(); l++)
	{
		if (ionStore[l].PDGencoding == PDGencoding )
		{   // Is it a primary or a secondary particle?

			if ( (trackID ==1 && ionStore[l].isPrimary) || (trackID !=1 && !ionStore[l].isPrimary))
			{

				// Fill a matrix per each ion with the fluence

				ionStore[l].letN[Index(i, j, k)]+=energyDeposit * Lsn;
				ionStore[l].letD[Index(i, j, k)]+=energyDeposit;
				return true;
			}
		}
	}

	G4int Z = particleDef-> GetAtomicNumber();
	G4int A = particleDef-> GetAtomicMass();
	G4String fullName = particleDef -> GetParticleName();
	G4String name = fullName.substr (0, fullName.find("[") ); // cut excitation energy

	// Let's put a new particle in our store...
	ion newIon =
	{
			(trackID == 1) ? true:false,
					PDGencoding,
					name,
					name.length(),
					Z,
					A,
					new G4double[fNX * fNY * fNZ],
					new G4double[fNX * fNY * fNZ],
					new G4double[fNX * fNY * fNZ],
					new G4double[fNX * fNY * fNZ]
	};

	// Initialize data
	if (newIon.eDep && newIon.letN && newIon.letD && newIon.fluence)
	{
		for(G4int q=0; q<fNX * fNY * fNZ; q++)
		{
			newIon.eDep[q] = 0;
			newIon.letN[q] = 0;
			newIon.letD[q] = 0;
			newIon.fluence[q] = 0;
		}

		newIon.letN[Index(i, j, k)]+=energyDeposit * Lsn;
		newIon.letD[Index(i, j, k)]+=energyDeposit;

		ionStore.push_back(newIon);
		return true;
	}

	else // XXX Out of memory! XXX

	{
		return false;
	}

}

G4bool DetectorMatrix::FillFluence(G4int i, G4int j, G4int k, G4double dx, G4double vol,  G4int trackID, G4ParticleDefinition* particleDef){

	// Get Particle Data Group particle ID
	G4int PDGencoding = particleDef -> GetPDGEncoding();
	PDGencoding -= PDGencoding%10;

	// Search for already allocated data...
	for (size_t l=0; l < ionStore.size(); l++)
	{
		if (ionStore[l].PDGencoding == PDGencoding )
		{   // Is it a primary or a secondary particle?

			if ( (trackID ==1 && ionStore[l].isPrimary) || (trackID !=1 && !ionStore[l].isPrimary))
			{

				// Fill a matrix per each ion with the fluence

				ionStore[l].fluence[Index(i, j, k)]+=dx/vol;
				return true;
			}
		}
	}

	G4int Z = particleDef-> GetAtomicNumber();
	G4int A = particleDef-> GetAtomicMass();
	G4String fullName = particleDef -> GetParticleName();
	G4String name = fullName.substr (0, fullName.find("[") ); // cut excitation energy

	// Let's put a new particle in our store...
	ion newIon =
	{
			(trackID == 1) ? true:false,
					PDGencoding,
					name,
					name.length(),
					Z,
					A,
					new G4double[fNX * fNY * fNZ],
					new G4double[fNX * fNY * fNZ],
					new G4double[fNX * fNY * fNZ],
					new G4double[fNX * fNY * fNZ]
	};

	// Initialize data
	if (newIon.eDep && newIon.letN && newIon.letD && newIon.fluence)
	{
		for(G4int q=0; q<fNX * fNY * fNZ; q++)
		{
			newIon.eDep[q] = 0;
			newIon.letN[q] = 0;
			newIon.letD[q] = 0;
			newIon.fluence[q] = 0;
		}

		newIon.fluence[Index(i, j, k)]+=dx/vol;

		ionStore.push_back(newIon);
		return true;
	}

	else // XXX Out of memory! XXX

	{
		return false;
	}

}
