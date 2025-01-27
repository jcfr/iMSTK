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

#include "imstkPBDCollisionHandling.h"
#include "NeedleObject.h"

using namespace imstk;

///
/// \brief Surface collision disabled upon puncture
///
class NeedlePbdCH : public PBDCollisionHandling
{
public:
    NeedlePbdCH() = default;
    ~NeedlePbdCH() override = default;

    virtual const std::string getTypeName() const override { return "NeedlePbdCH"; }

protected:
    ///
    /// \brief Add a vertex-triangle constraint
    ///
    virtual void addVTConstraint(
        VertexMassPair ptA,
        VertexMassPair ptB1, VertexMassPair ptB2, VertexMassPair ptB3,
        double stiffnessA, double stiffnessB)
    {
        auto needleObj = std::dynamic_pointer_cast<NeedleObject>(getInputObjectB());
        if (needleObj->getCollisionState() == NeedleObject::CollisionState::TOUCHING)
        {
            PBDCollisionHandling::addVTConstraint(ptA, ptB1, ptB2, ptB3, stiffnessA, stiffnessB);
        }
    }

    ///
    /// \brief Add an edge-edge constraint
    ///
    virtual void addEEConstraint(
        VertexMassPair ptA1, VertexMassPair ptA2,
        VertexMassPair ptB1, VertexMassPair ptB2,
        double stiffnessA, double stiffnessB)
    {
        auto needleObj = std::dynamic_pointer_cast<NeedleObject>(getInputObjectB());
        if (needleObj->getCollisionState() == NeedleObject::CollisionState::TOUCHING)
        {
            PBDCollisionHandling::addEEConstraint(ptA1, ptA2, ptB1, ptB2, stiffnessA, stiffnessB);
        }
    }

    ///
    /// \brief Add a point-edge constraint
    ///
    virtual void addPEConstraint(
        VertexMassPair ptA1,
        VertexMassPair ptB1, VertexMassPair ptB2,
        double stiffnessA, double stiffnessB)
    {
        auto needleObj = std::dynamic_pointer_cast<NeedleObject>(getInputObjectB());
        if (needleObj->getCollisionState() == NeedleObject::CollisionState::TOUCHING)
        {
            PBDCollisionHandling::addPEConstraint(ptA1, ptB1, ptB2, stiffnessA, stiffnessB);
        }
    }

    ///
    /// \brief Add a point-point constraint
    ///
    virtual void addPPConstraint(
        VertexMassPair ptA, VertexMassPair ptB,
        double stiffnessA, double stiffnessB)
    {
        auto needleObj = std::dynamic_pointer_cast<NeedleObject>(getInputObjectB());
        if (needleObj->getCollisionState() == NeedleObject::CollisionState::TOUCHING)
        {
            PBDCollisionHandling::addPPConstraint(ptA, ptB, stiffnessA, stiffnessB);
        }
    }
};