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

#include "imstkCollisionPair.h"

#include <string>

namespace imstk
{
class RigidObject2;
template<typename T, int N> class VecDataArray;

///
/// \class RigidObjectObjectCollision
///
/// \brief This class defines a collision interaction between two RigidObjects
/// This involves a RigidObjCH which will generate 2 way or 1 way constraints for the RigidBodyModel/s
/// depending on which system they belong too
///
class RigidObjectCollision : public CollisionPair
{
public:
    RigidObjectCollision(std::shared_ptr<RigidObject2> obj1, std::shared_ptr<CollidingObject> obj2, std::string cdType);
    virtual ~RigidObjectCollision() override = default;

public:
    void setStiffness(double stiffness);
    const double getStiffness() const;

    void setFriction(double frictionalCoefficient);
    const double getFriction() const;

public:
    void apply() override;

    void copyVertsToPrevious();

    void measureDisplacementFromPrevious();

public:
    std::shared_ptr<VecDataArray<double, 3>> m_prevVertices;
};
}