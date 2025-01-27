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

#include "imstkQuadricDecimate.h"
#include "imstkSurfaceMesh.h"
#include "imstkLogger.h"
#include "imstkGeometryUtilities.h"

#include <vtkQuadricDecimation.h>

namespace imstk
{
QuadricDecimate::QuadricDecimate() :
    m_VolumePreserving(true), m_TargetReduction(0.6)
{
    setRequiredInputType<SurfaceMesh>(0);

    setNumberOfInputPorts(1);
    setNumberOfOutputPorts(1);
    setOutput(std::make_shared<SurfaceMesh>());
}

void
QuadricDecimate::setInputMesh(std::shared_ptr<SurfaceMesh> inputMesh)
{
    setInput(inputMesh, 0);
}

void
QuadricDecimate::requestUpdate()
{
    std::shared_ptr<SurfaceMesh> inputMesh = std::dynamic_pointer_cast<SurfaceMesh>(getInput(0));
    if (inputMesh == nullptr)
    {
        LOG(WARNING) << "No inputMesh to clean";
        return;
    }
    vtkSmartPointer<vtkPolyData> inputMeshVtk = GeometryUtils::copyToVtkPolyData(std::dynamic_pointer_cast<SurfaceMesh>(inputMesh));

    vtkNew<vtkQuadricDecimation> filter;
    filter->SetInputData(inputMeshVtk);
    filter->SetVolumePreservation(m_VolumePreserving);
    filter->SetTargetReduction(m_TargetReduction);
    filter->Update();

    setOutput(GeometryUtils::copyToSurfaceMesh(filter->GetOutput()));
}
}