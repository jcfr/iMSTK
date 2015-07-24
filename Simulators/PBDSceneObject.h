// This file is part of the SimMedTK project.
// Copyright (c) Center for Modeling, Simulation, and Imaging in Medicine,
//                        Rensselaer Polytechnic Institute
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//---------------------------------------------------------------------------
//
// Authors:
//
// Contact:
//---------------------------------------------------------------------------

#ifndef SMPBDSCENEOBJECT_H
#define SMPBDSCENEOBJECT_H

// SimMedTK includes
#include "Core/Config.h"
#include "Core/SceneObject.h"
#include "Mesh/SurfaceMesh.h"
#include "Mesh/Mesh.h"

/// \brief Position based dynamics (PBD) object
class PBDSceneObject: public SceneObject
{
public:
    /// \brief constructor
    PBDSceneObject(std::shared_ptr<ErrorLog> p_log = nullptr);

    /// \brief !!
    virtual std::shared_ptr<SceneObject> clone();

    /// \brief !!
    virtual void serialize(void *p_memoryBlock);

    /// \brief !!
    virtual void unSerialize(void *p_memoryBlock);

    /// \brief find the masses that are fixed
    void findFixedMass();

public:
    float dT; ///< size of time step
    float paraK; ///< !!
    float Damp; ///< damping values
    int nbrMass; ///< number of masses
    int **massIdx; ///< !!
    core::StdVector3d P; ///< !! position
    core::StdVector3d V; ///< !! velocity
    core::StdVector3d exF; ///< external force
    int nbrSpr; ///< !! number of spheres
    float *L0; ///< !! Initial length
    bool *fixedMass; ///< true if masses are fixed
    int nbrFixedMass; ///< number of fixed masses
    int *listFixedMass; ///< list of IDs of masses that are fixed

    core::Vec3d ball_pos; ///< !! position of ball
    core::Vec3d ball_vel; ///< !! velocity of ball
    core::Vec3d ball_frc; ///< !!

    float ball_mass; ///< !! mass of ball
    float ball_rad; ///< !! radius of ball
};

/// \brief Position based dynamics (PBD) object for surface mesh (eg. cloth)
class PBDSurfaceSceneObject: public PBDSceneObject
{
public:

    /// \brief constructor
    PBDSurfaceSceneObject(std::shared_ptr<ErrorLog> p_log = nullptr);

    /// \brief !!
    virtual std::shared_ptr<SceneObject> clone() override;

    /// \brief !!
    virtual void serialize(void *p_memoryBlock) override;

    /// \brief !!
    virtual void unSerialize(void *p_memoryBlock) override;

    /// \brief initialize the mesh structure
    void initMeshStructure();

    /// \brief initialize surface PBD object
    void InitSurfaceObject();

    /// \brief destructor
    ~PBDSurfaceSceneObject();

    /// \brief find the masses that will be fixed based on the spheres
    void findFixedMassWrtSphere(core::Vec3d p_center, float pos);

    /// \brief find fixed corners
    void findFixedCorners();

public:
    friend class PBDSurfaceRenderDetail;

    SurfaceMesh *mesh; ///< surface mesh
    int nbrTri; ///< number of surface triangles
    int **triVertIdx; ///< !!
    int **sprInTris; ///< triangles that include a spring
};

#endif