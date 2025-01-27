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

#include "imstkCollisionHandling.h"

namespace imstk
{
class CollidingObject;
class CollisionData;
class FeDeformableObject;
class RigidObject2;

///
/// \class PenaltyCH
///
/// \brief Implements penalty collision handling between FEM and Rigid models
///
class PenaltyCH : public CollisionHandling
{
public:
    PenaltyCH() = default;
    virtual ~PenaltyCH() override = default;

    virtual const std::string getTypeName() const override { return "PenaltyCH"; }

public:
    void setInputFeObject(std::shared_ptr<FeDeformableObject> feObj);
    void setInputRbdObject(std::shared_ptr<RigidObject2> rbdObj);

    std::shared_ptr<FeDeformableObject> getInputFeObject();
    std::shared_ptr<RigidObject2> getInputRbdObject();

public:
    ///
    /// \brief Set the contact stiffness
    ///
    void setContactStiffness(const double stiffness) { m_stiffness = stiffness; }

    ///
    /// \brief Set the contact velocity damping
    ///
    void setContactVelocityDamping(const double damping) { m_damping = damping; }

protected:
    void handle(
        const std::vector<CollisionElement>& elementsA,
        const std::vector<CollisionElement>& elementsB) override;

    ///
    /// \brief Given the collision data, applies contact as external force
    /// to the rigid body (onyl supports PointDirection contacts)
    ///
    void computeContactForcesAnalyticRigid(
        const std::vector<CollisionElement>& elements,
        std::shared_ptr<RigidObject2>        analyticObj);

    ///
    /// \brief Given the collision data, applies nodal forces in the FEM model
    ///
    void computeContactForcesDiscreteDeformable(
        const std::vector<CollisionElement>& elements,
        std::shared_ptr<FeDeformableObject>  deformableObj);

protected:
    double m_stiffness = 5.0e5; ///> Stiffness of contact
    double m_damping   = 0.5;   ///> Damping of the contact
};
}