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

#include "PDD1DetectorConstruction.hh"
#include "LET.hh"

#include "PDD1PrimaryGeneratorAction.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4EmCalculator.hh"
#include <cmath>
#include "PDD1Analysis.hh"

LET* LET::instance = NULL;
G4bool LET::doCalculation = true;

LET* LET::GetInstance(PDD1DetectorConstruction *pDet)
{
    if (instance) delete instance;
    instance = new LET(pDet);
    return instance;
}

LET* LET::GetInstance()
{
    return instance;
}

LET::LET(PDD1DetectorConstruction* pDet)
:filename("Let.out"),fDet(pDet) // Default output filename
{


    nVoxels = pDet->GetNumberOfVoxelAlongX()*pDet->GetNumberOfVoxelAlongY()*pDet->GetNumberOfVoxelAlongZ();

    numberOfVoxelAlongX = pDet -> GetNumberOfVoxelAlongX();
    numberOfVoxelAlongY = pDet -> GetNumberOfVoxelAlongY();
    numberOfVoxelAlongZ = pDet -> GetNumberOfVoxelAlongZ();

    // Pointer to the detector material
    detectorMat = NULL;

    nVoxels = numberOfVoxelAlongX * numberOfVoxelAlongY * numberOfVoxelAlongZ;
    // Instances for Total LET
    totalLetD =      new G4double[nVoxels];
    DtotalLetD =     new G4double[nVoxels];
    totalLetT =      new G4double[nVoxels];
    DtotalLetT =     new G4double[nVoxels];
    
}

LET::~LET()
{
    Clear();
    delete [] totalLetD;
    delete [] DtotalLetD;
    delete [] totalLetT;
    delete [] DtotalLetT;
}

// Fill energy spectrum for every voxel (local energy spectrum)
void LET::Initialize()
{
    for(G4int v=0; v < nVoxels; v++) totalLetD[v] = DtotalLetD[v] = totalLetT[v] = DtotalLetT[v] = 0.;
    Clear();

}
/**
 * Clear all stored data
 */
void LET::Clear()
{
    for (size_t i=0; i < ionLetStore.size(); i++)
    {
        delete [] ionLetStore[i].letDN; // Let Dose Numerator
        delete [] ionLetStore[i].letDD; // Let Dose Denominator
        delete [] ionLetStore[i].letTN; // Let Track Numerator
        delete [] ionLetStore[i].letTD; // Let Track Denominator
    }
    ionLetStore.clear();
}

void  LET::FillEnergySpectrum(G4int trackID,
                                           G4ParticleDefinition* particleDef,
                                           G4double ekinMean,
                                           G4Material* mat,
                                           G4double DE,
                                           G4double DEEletrons,
                                           G4double DX,
                                           G4int i, G4int j, G4int k)
{

	if (DE <= 0. || DX <=0. ) return; // calculate only energy deposit
    if (!doCalculation) return;
    
    // atomic number
    G4int Z = particleDef -> GetAtomicNumber();
    if (Z<1) return; // calculate only protons and ions
    
    G4int PDGencoding = particleDef -> GetPDGEncoding();
    PDGencoding -= PDGencoding%10; // simple Particle data group id  without excitation level
    
    G4int voxel = fDet -> Index(i,j,k);
    
    // ICRU stopping power calculation
    G4EmCalculator emCal;
    // use the mean kinetic energy of ions in a step to calculate ICRU stopping power
    G4double Lsn = emCal.ComputeElectronicDEDX(ekinMean, particleDef, mat);
    

    // Total LET calculation...
    totalLetD[voxel]  += (DE + DEEletrons) * Lsn; // total dose Let Numerator, including secondary electrons energy deposit
    DtotalLetD[voxel] += DE + DEEletrons;         // total dose Let Denominator, including secondary electrons energy deposit
    totalLetT[voxel]  += DX * Lsn;                // total track Let Numerator
    DtotalLetT[voxel] += DX;                      // total track Let Denominator
    

    // store primary ions and secondary ions
    size_t l;
    for (l=0; l < ionLetStore.size(); l++)
    {
        // judge species of the current particle and store it
        if (ionLetStore[l].PDGencoding == PDGencoding)
            if ( ((trackID ==1) && (ionLetStore[l].isPrimary)) || ((trackID !=1) && (!ionLetStore[l].isPrimary)))
                break;
    }

    if (l == ionLetStore.size()) // Just another type of ion/particle for our store...
    {

        // mass number
        G4int A = particleDef -> GetAtomicMass();
        
        // particle name
        G4String fullName = particleDef -> GetParticleName();
        G4String name = fullName.substr (0, fullName.find("[") ); // cut excitation energy [x.y]

        ionLet ion =
        {
            (trackID == 1) ? true:false, // is it the primary particle?
            PDGencoding,
            fullName,
            name,
            Z,
            A,
            new G4double[nVoxels], // Let Dose Numerator
            new G4double[nVoxels],  // Let Dose Denominator
            new G4double[nVoxels], // Let Track Numerator
            new G4double[nVoxels],  // Let Track Denominator
        };

        G4int size = ionLetStore.size();

        // Initialize let and other parameters
        for(G4int v=0; v < nVoxels; v++)
        {
            ion.letDN[v] = ion.letDD[v] = ion.letTN[v] = ion.letTD[v] = 0.;
        }
        
        ionLetStore.push_back(ion);

    }

    // calculate ions let and store them
    ionLetStore[l].letDN[voxel] += (DE + DEEletrons)* Lsn; // ions dose Let Numerator, including secondary electrons energy deposit
    ionLetStore[l].letDD[voxel] += DE + DEEletrons;        // ions dose Let Denominator, including secondary electrons energy deposit
    ionLetStore[l].letTN[voxel] += DX* Lsn;                // ions track Let Numerator
    ionLetStore[l].letTD[voxel] += DX;                     // ions track Let Denominator

}

void LET::LetOutput()
{
    for(G4int v=0; v < nVoxels; v++)
    {
        // compute total let
        if (DtotalLetD[v]>0.) totalLetD[v] = totalLetD[v]/DtotalLetD[v];
        if (DtotalLetT[v]>0.) totalLetT[v] = totalLetT[v]/DtotalLetT[v];

    }
    
    // Sort ions by A and then by Z ...
    std::sort(ionLetStore.begin(), ionLetStore.end());
    
    
    // Compute Let Track and Let Dose for ions
    
    for(G4int v=0; v < nVoxels; v++)
    {
        
        for (size_t ion=0; ion < ionLetStore.size(); ion++)
        {
            // compute ions let
            if (ionLetStore[ion].letDD[v] >0.) ionLetStore[ion].letDN[v] = ionLetStore[ion].letDN[v] / ionLetStore[ion].letDD[v];
            if (ionLetStore[ion].letTD[v] >0.) ionLetStore[ion].letTN[v] = ionLetStore[ion].letTN[v] / ionLetStore[ion].letTD[v];
        } // end loop over ions
    }
} // end loop over voxels



void LET::StoreLetAscii()
{
#define width 15L
    
    if(ionLetStore.size())
    {
        ofs.open(filename, std::ios::out);
        if (ofs.is_open())
        {
            
            // Write the voxels index and total Lets and the list of particles/ions
            ofs << std::setprecision(6) << std::left <<
            "i\tj\tk\t";
            ofs <<  std::setw(width) << "LDT";
            ofs <<  std::setw(width) << "LTT";
            
            for (size_t l=0; l < ionLetStore.size(); l++) // write ions name
            {
                G4String a = (ionLetStore[l].isPrimary) ? "_1_D":"_D";
                ofs << std::setw(width) << ionLetStore[l].name  + a ;
                G4String b = (ionLetStore[l].isPrimary) ? "_1_T":"_T";
                ofs << std::setw(width) << ionLetStore[l].name  + b ;
            }
            ofs << G4endl;


            for(G4int i = 0; i < numberOfVoxelAlongX; i++)
                for(G4int j = 0; j < numberOfVoxelAlongY; j++)
                    for(G4int k = 0; k < numberOfVoxelAlongZ; k++)
                    {

                        G4int v = fDet -> Index(i, j, k);

                        // write total Lets and voxels index
                        ofs << G4endl;
                        ofs << i << '\t' << j << '\t' << k << '\t';
                        ofs << std::setw(width) << totalLetD[v]/(keV/um);
                        ofs << std::setw(width) << totalLetT[v]/(keV/um);


                        // write ions Lets
                        for (size_t l=0; l < ionLetStore.size(); l++)
                        {

                            // Write only not identically null data line
                            if(ionLetStore[l].letDN)
                            {
                                // write ions Lets
                                ofs << std::setw(width) << ionLetStore[l].letDN[v]/(keV/um) ;
                                ofs << std::setw(width) << ionLetStore[l].letTN[v]/(keV/um);
                            }
                        }

                    }
            ofs.close();

        }
        
    }
    
}

