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

#ifndef PDD1NestedParameterisation_hh
#define PDD1NestedParameterisation_hh

#include "G4Types.hh"
#include "G4VNestedParameterisation.hh" 
#include "G4ThreeVector.hh"
#include <vector>

class G4VPhysicalVolume;
class G4VTouchable; 
class G4VSolid;
class G4Material;

// CSG Entities which may be parameterised/replicated
//
class G4Box;
class G4Tubs;
class G4Trd;
class G4Trap;
class G4Cons;
class G4Sphere;
class G4Orb;
class G4Ellipsoid;
class G4Torus;
class G4Para;
class G4Polycone;
class G4Polyhedra;
class G4Hype;


class PDD1NestedPhantomParameterisation: public G4VNestedParameterisation
{
  public:  // with description

    PDD1NestedPhantomParameterisation(const G4ThreeVector& voxelSize,
                                      G4int nz,
                                      std::vector<G4Material*>& mat);
   ~PDD1NestedPhantomParameterisation();

    // Methods required in derived classes
    // -----------------------------------
    G4Material* ComputeMaterial(G4VPhysicalVolume *currentVol,
                                const G4int repNo, 
                                const G4VTouchable *parentTouch=0
                                        );
  // Required method, as it is the reason for this class.
  //   Must cope with parentTouch=0 for navigator's SetupHierarchy

    G4int       GetNumberOfMaterials() const;
    G4Material* GetMaterial(G4int idx) const;
      // Needed to define materials for instances of Nested Parameterisation 
      //   Current convention: each call should return the materials 
      //   of all instances with the same mother/ancestor volume.

    void ComputeTransformation(const G4int no,
                               G4VPhysicalVolume *currentPV) const;

    // Methods optional in derived classes
    // -----------------------------------

    // Additional standard Parameterisation methods, 
    //   which can be optionally defined, in case solid is used.

    void ComputeDimensions(G4Box &,
                                   const G4int,
                                   const G4VPhysicalVolume *) const;

private:  // Dummy declarations to get rid of warnings ...

  void ComputeDimensions (G4Trd&,const G4int,const G4VPhysicalVolume*) 
    const {}
  void ComputeDimensions (G4Trap&,const G4int,const G4VPhysicalVolume*) 
    const {}
  void ComputeDimensions (G4Cons&,const G4int,const G4VPhysicalVolume*) 
    const {}
  void ComputeDimensions (G4Sphere&,const G4int,const G4VPhysicalVolume*) 
    const {}
  void ComputeDimensions (G4Orb&,const G4int,const G4VPhysicalVolume*) 
    const {}
  void ComputeDimensions (G4Ellipsoid&,const G4int,const G4VPhysicalVolume*) 
    const {}
  void ComputeDimensions (G4Torus&,const G4int,const G4VPhysicalVolume*) 
    const {}
  void ComputeDimensions (G4Para&,const G4int,const G4VPhysicalVolume*) 
    const {}
  void ComputeDimensions (G4Hype&,const G4int,const G4VPhysicalVolume*) 
    const {}
  void ComputeDimensions (G4Tubs&,const G4int,const G4VPhysicalVolume*) 
    const {}
  void ComputeDimensions (G4Polycone&,const G4int,const G4VPhysicalVolume*)
    const {}
  void ComputeDimensions (G4Polyhedra&,const G4int,const G4VPhysicalVolume*) 
    const {}
//  G4Material* ComputeMaterial(const G4int repNo,
//                              G4VPhysicalVolume* currentVol,
//                              const G4VTouchable* parentTouch)
//  { return ComputeMaterial( currentVol, repNo, parentTouch ); }
  using G4VNestedParameterisation::ComputeMaterial;

private:

  G4double fdX,fdY,fdZ;
  G4int fNz;
  //
  std::vector<G4double>  fpZ;
  std::vector<G4Material*> fMat;
};

#endif //PDD1NestedParameterisation_hh
