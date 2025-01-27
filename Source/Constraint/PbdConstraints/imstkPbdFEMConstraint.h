/*=========================================================================

   Library: iMSTK

   Copyright (c) Kitware, Inc. & Center for Modeling, Simulation,
   & Imaging in Medicine, Rensselaer Polytechnic Institute.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

=========================================================================*/

#pragma once

#include "imstkPbdConstraint.h"

namespace imstk
{
///
/// \class PbdFEMConstraintConfig
///
/// \brief Either mu/lambda used, may be computed from youngs modulus and poissons ratio
///
struct PbdFEMConstraintConfig
{
    PbdFEMConstraintConfig(double mu, double lambda, double youngModulus, double poissonRatio) :
        m_mu(mu), m_lambda(lambda), m_YoungModulus(youngModulus), m_PoissonRatio(poissonRatio)
    {
    }

    double m_mu     = 0.0;        ///> Lame constant, if constraint type is FEM
    double m_lambda = 0.0;        ///> Lame constant, if constraint type is FEM

    double m_YoungModulus = 1000; ///> FEM parameter, if constraint type is FEM
    double m_PoissonRatio = 0.2;  ///> FEM parameter, if constraint type is FEM
};

///
/// \class PbdFEMConstraint
///
/// \brief The PbdFEMConstraint class for constraint as the elastic energy
/// computed by linear shape functions with tetrahedral mesh.
/// We provide several model for elastic energy including:
/// Linear, Co-rotation, St Venant-Kirchhof and NeoHookean
///
class PbdFEMConstraint : public PbdConstraint
{
public:
    // Material type
    enum class MaterialType
    {
        Linear,
        Corotation,
        StVK,
        NeoHookean
    };

    ///
    /// \brief Constructor
    ///
    PbdFEMConstraint(const unsigned int cardinality, MaterialType mtype = MaterialType::StVK);

public:
    double       m_elementVolume = 0.0; ///> Volume of the element
    MaterialType m_material;            ///> Material type
    Mat3d m_invRestMat;

    std::shared_ptr<PbdFEMConstraintConfig> m_config = nullptr;
};
} // imstk
