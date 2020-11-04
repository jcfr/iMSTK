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

#include "imstkPBDPickingCH.h"
#include "imstkCollisionData.h"

#include "imstkPbdModel.h"
#include "imstkPbdObject.h"
#include "imstkAnalyticalGeometry.h"
#include "imstkPointSet.h"

namespace imstk
{
PBDPickingCH::PBDPickingCH(const CollisionHandling::Side&       side,
                           const std::shared_ptr<CollisionData> colData,
                           std::shared_ptr<PbdObject>           pbdObj,
                           std::shared_ptr<CollidingObject>     pickObj) :
    CollisionHandling(Type::PBDPicking, side, colData),
    m_pbdObj(pbdObj),
    m_pickObj(pickObj)
{
    m_isPicking = false;
    m_pickedPtIdxOffset.clear();
}

void
PBDPickingCH::processCollisionData()
{
    CHECK(m_pbdObj != nullptr && m_pickObj != nullptr)
        << "PBDPickingCH::handleCollision error: "
        << "no picking collision handling available the object";

    if (m_isPicking)
    {
        this->updatePickConstraints();
    }
}

void
PBDPickingCH::updatePickConstraints()
{
    if (m_pickedPtIdxOffset.size() == 0)
    {
        this->removePickConstraints();
        return;
    }

    std::shared_ptr<PbdModel> model = m_pbdObj->getPbdModel();
    std::shared_ptr<AnalyticalGeometry> pickGeom = std::dynamic_pointer_cast<AnalyticalGeometry>(m_pickObj->getCollidingGeometry());
    for (auto iter = m_pickedPtIdxOffset.begin(); iter != m_pickedPtIdxOffset.end(); iter++)
    {
        auto rot = pickGeom->getRotation();
        model->getCurrentState()->setVertexPosition(iter->first, pickGeom->getPosition() + rot * iter->second);
    }
}

void
PBDPickingCH::addPickConstraints(std::shared_ptr<PbdObject> pbdObj, std::shared_ptr<CollidingObject> pickObj)
{
    if (m_colData->MAColData.isEmpty())
    {
        return;
    }

    CHECK(pbdObj != nullptr && pickObj != nullptr)
        << "PBDPickingCH:addPickConstraints error: "
        << "no pdb object or colliding object.";

    std::shared_ptr<PbdModel> model = pbdObj->getPbdModel();
    std::shared_ptr<AnalyticalGeometry> pickGeom = std::dynamic_pointer_cast<AnalyticalGeometry>(pickObj->getCollidingGeometry());
    CHECK(pickGeom != nullptr) << "Colliding geometry is analytical geometry ";

    ParallelUtils::SpinLock lock;
    ParallelUtils::parallelFor(m_colData->MAColData.getSize(),
        [&](const size_t idx) {

            const auto& cd = m_colData->MAColData[idx];
            if (m_pickedPtIdxOffset.find(cd.nodeIdx) == m_pickedPtIdxOffset.end())
            {
                
                lock.lock();

                auto rot = pickGeom->getRotation().transpose();
                auto relativePos = rot * (model->getCurrentState()->getVertexPosition(cd.nodeIdx) - pickGeom->getPosition());
                m_pickedPtIdxOffset[cd.nodeIdx] = relativePos;
                model->setFixedPoint(cd.nodeIdx);
                model->getCurrentState()->setVertexPosition(cd.nodeIdx, pickGeom->getPosition() + rot.transpose() * relativePos);
                lock.unlock();
            }
    });
}

void
PBDPickingCH::removePickConstraints()
{
    std::shared_ptr<PbdModel> model = m_pbdObj->getPbdModel();
    m_isPicking = false;
    for (auto iter = m_pickedPtIdxOffset.begin(); iter != m_pickedPtIdxOffset.end(); ++iter)
    {
        model->setPointUnfixed(iter->first);
    }
    m_pickedPtIdxOffset.clear();
}

void
PBDPickingCH::activatePickConstraints()
{
    if (!m_colData->MAColData.isEmpty())
    {        
        this->addPickConstraints(m_pbdObj, m_pickObj);
        m_isPicking = true;
    }
}
}
