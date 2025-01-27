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

#include "imstkVTKCapsuleRenderDelegate.h"
#include "imstkCapsule.h"
#include "imstkVisualModel.h"

#include <vtkActor.h>
#include <vtkCapsuleSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkTransform.h>

namespace imstk
{
VTKCapsuleRenderDelegate::VTKCapsuleRenderDelegate(std::shared_ptr<VisualModel> visualModel) : VTKPolyDataRenderDelegate(visualModel),
    m_capsuleSource(vtkSmartPointer<vtkCapsuleSource>::New())
{
    auto geometry = std::dynamic_pointer_cast<Capsule>(visualModel->getGeometry());

    m_capsuleSource->SetCenter(0.0, 0.0, 0.0);
    m_capsuleSource->SetRadius(geometry->getRadius(Geometry::DataType::PreTransform));
    m_capsuleSource->SetCylinderLength(geometry->getLength(Geometry::DataType::PreTransform));
    m_capsuleSource->SetLatLongTessellation(20);
    m_capsuleSource->SetPhiResolution(20);
    m_capsuleSource->SetThetaResolution(20);

    const Mat4d& transform = geometry->getTransform().transpose();
    m_transform->SetMatrix(transform.data());

    // Setup mapper
    {
        vtkNew<vtkPolyDataMapper> mapper;
        mapper->SetInputConnection(m_capsuleSource->GetOutputPort());
        vtkNew<vtkActor> actor;
        actor->SetMapper(mapper);
        actor->SetUserTransform(m_transform);
        m_mapper = mapper;
        m_actor  = actor;
    }

    update();
    updateRenderProperties();
}

void
VTKCapsuleRenderDelegate::processEvents()
{
    VTKRenderDelegate::processEvents();

    // Don't use events for primitives, just always update
    auto geometry = std::dynamic_pointer_cast<Capsule>(m_visualModel->getGeometry());

    m_capsuleSource->SetRadius(geometry->getRadius(Geometry::DataType::PreTransform));
    m_capsuleSource->SetCylinderLength(geometry->getLength(Geometry::DataType::PreTransform));
    m_capsuleSource->Modified();

    AffineTransform3d T = AffineTransform3d::Identity();
    T.translate(geometry->getPosition(Geometry::DataType::PostTransform));
    T.rotate(geometry->getOrientation(Geometry::DataType::PostTransform));
    T.scale(geometry->getScaling());
    T.matrix().transposeInPlace();
    m_transform->SetMatrix(T.data());
}
} // imstk
