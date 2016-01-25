// This file is part of the iMSTK project.
//
// Copyright (c) Kitware, Inc.
//
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

#ifndef SCENE_MODELS_VEGAFEMDEFORMABLESCENEOBJECT_H
#define SCENE_MODELS_VEGAFEMDEFORMABLESCENEOBJECT_H

// STL includes
#include <memory>

// iMSTK includes
#include "SceneModels/DeformableSceneObject.h"

// iMSTK forward declarations
class VegaVolumetricMesh;

// Vega forward declarations
class SparseMatrix;
class VolumetricMesh;
class ForceModel;
class StVKInternalForces;
class CorotationalLinearFEM;
class StVKStiffnessMatrix;
class IsotropicMaterial;
class IsotropicHyperelasticFEM;

class VegaFEMDeformableSceneObject : public DeformableSceneObject
{
public:
    ///
    /// \brief Constructor. This is the only constructor and takes a valid mesh file name
    ///     and a valid VegaFEM configuration file.
    ///
    VegaFEMDeformableSceneObject(const std::string &meshFilename,
                                 const std::string &vegaConfigFileName);

    VegaFEMDeformableSceneObject();
    ~VegaFEMDeformableSceneObject();

    ///
    /// \brief Load the Vega volume. This is a .veg file containing vertices, connections
    ///     and material properties.
    ///
    /// \param fileName Path to Vega (.veg) mesh.
    ///
    void loadVolumeMesh(const std::string &fileName);

    ///
    /// \brief Set the initial state of the system. It assumes there is a valid mesh and
    ///     that numOfDOF is greater than zero.
    ///
    void loadInitialStates() override;

    ///
    /// \brief Initialize the ode solver.
    ///
    void initialize() override;

    ///
    /// \brief Helper function to setup components.
    ///
    /// \param configFile Vega configuration file.
    ///
    bool configure(const std::string &configFile) override;

    ///
    /// \brief Initialize the mass matrix and convert it to an Eigen container.
    ///
    /// \param saveToDisk Dave mass matrix to disk if true.
    ///
    void initMassMatrix(bool saveToDisk = false);

    ///
    /// \brief Initialize the tangent stiffness matrix and convert it to an Eigen
    ///  container. Upon return the this->M matrix will contain the matrix entries.
    ///
    void initTangentStiffnessMatrix();

    ///
    /// \brief Initialize the Lagrangian matrix and convert it to an Eigen
    ///  container.
    ///
    void initDampingMatrix();

    ///
    /// \brief Initialize the constitutive model used by Vega's force model.
    ///
    void initConstitutiveModel();

    ///
    /// \brief Instantiate the Vega force model.
    ///
    void initForceModel();

    ///
    /// \brief Load boundary conditions. The file contains the indices of the degrees of
    ///     freedom that are subjected to Dirichlet boundary conditions.
    ///
    std::vector<size_t> loadBoundaryConditions();

    ///
    /// \brief Helper function to interface Vega sparse matrices with Eigen matrices.
    ///
    void updateValuesFromMatrix(std::shared_ptr<SparseMatrix> matrix,
                                double *values);

    ///
    /// \brief Set the ODE system right hand side function.
    ///
    void setOdeRHS();

    ///
    /// \brief Set the the tangent stiffness matrix function to be evaluated by the ODE solver.
    ///
    void setTangentStiffnessMatrix();

    ///
    /// \brief Set the the mass matrix function to be evaluated by the ODE solver.
    ///
    void setMassMatrix();

    ///
    /// \brief Set the the Raleigh damping matrix function to be evaluated by the ODE solver.
    ///
    void setDampingMatrices();

    ///
    /// \brief Update underlying meshes with current state positions.
    ///
    void updateMesh() override;

private:
    // Volumetric mesh, local copy of the actual mesh (stored in the model).
    std::shared_ptr<VegaVolumetricMesh> volumetricMesh;

    // Vega force model
    std::shared_ptr<ForceModel> forceModel;

    // Vega internal constitutive models
    std::shared_ptr<StVKInternalForces> stVKInternalForces;
    std::shared_ptr<CorotationalLinearFEM> corrotationalInternalForces;
    std::shared_ptr<StVKStiffnessMatrix> stVKStiffnessMatrix;
    std::shared_ptr<IsotropicMaterial> isotropicMaterial;
    std::shared_ptr<IsotropicHyperelasticFEM> isotropicHyperElasticForces;

    // Vega mass matrix
    std::shared_ptr<SparseMatrix> vegaMassMatrix;

    // Vega Tangent stiffness matrix
    std::shared_ptr<SparseMatrix> vegaTangentStiffnessMatrix;

    // Vega Laplacian damping matrix
    std::shared_ptr<SparseMatrix> dampingMatrix;

    // Nodal vector of gravity forces
    core::Vectord gravityForce;

    // Vega configuration parser interface
    class VegaConfiguration;
    std::unique_ptr<VegaConfiguration> vegaFemConfig;
};


#endif // VEGAFEMDEFORMABLESCENEOBJECT_H