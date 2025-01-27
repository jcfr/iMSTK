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

#include "imstkRigidObject.h"
#include "imstkLogger.h"
#include "imstkRigidBodyModel.h"

namespace imstk
{
bool
RigidObject::initialize()
{
    m_rigidBodyModel = std::dynamic_pointer_cast<RigidBodyModel>(m_dynamicalModel);
    if (m_rigidBodyModel == nullptr)
    {
        LOG(FATAL) << "Dynamics pointer cast failure in RigidObject::initialize()";
        return false;
    }

    DynamicObject::initialize();
    m_rigidBodyModel->initialize();

    return true;
}

void
RigidObject::addForce(const Vec3d& force, const Vec3d& pos, bool wakeup)
{
    m_rigidBodyModel->addForce(force, pos, wakeup);
}

std::shared_ptr<RigidBodyModel>
RigidObject::getRigidBodyModel()
{
    m_rigidBodyModel = std::dynamic_pointer_cast<RigidBodyModel>(m_dynamicalModel);
    return m_rigidBodyModel;
}
} // imstk
