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

#include "imstkSurfaceMeshSubdivide.h"
#include "imstkGeometryUtilities.h"
#include "imstkSurfaceMesh.h"
#include "imstkLogger.h"

#include <vtkButterflySubdivisionFilter.h>
#include <vtkLinearSubdivisionFilter.h>
#include <vtkLoopSubdivisionFilter.h>

namespace imstk
{
SurfaceMeshSubdivide::SurfaceMeshSubdivide()
{
    setNumberOfInputPorts(1);
    setRequiredInputType<SurfaceMesh>(0);

    setNumberOfOutputPorts(1);
    setOutput(std::make_shared<SurfaceMesh>());
}

std::shared_ptr<SurfaceMesh>
SurfaceMeshSubdivide::getOutputMesh()
{
    return std::dynamic_pointer_cast<SurfaceMesh>(getOutput(0));
}

void
SurfaceMeshSubdivide::setInputMesh(std::shared_ptr<SurfaceMesh> inputMesh)
{
    setInput(inputMesh, 0);
}

void
SurfaceMeshSubdivide::requestUpdate()
{
    std::shared_ptr<SurfaceMesh> inputMesh = std::dynamic_pointer_cast<SurfaceMesh>(getInput(0));

    if (inputMesh == nullptr)
    {
        LOG(WARNING) << "Missing required SurfaceMesh input";
        return;
    }

    vtkSmartPointer<vtkSubdivisionFilter> filter = nullptr;
    if (m_SubdivisionType == Type::BUTTERFLY)
    {
        filter = vtkSmartPointer<vtkButterflySubdivisionFilter>::New();
    }
    else if (m_SubdivisionType == Type::LOOP)
    {
        filter = vtkSmartPointer<vtkLoopSubdivisionFilter>::New();
    }
    else
    {
        filter = vtkSmartPointer<vtkLinearSubdivisionFilter>::New();
    }

    filter->SetInputData(GeometryUtils::copyToVtkPolyData(inputMesh));
    filter->SetNumberOfSubdivisions(m_NumberOfSubdivisions);
    filter->Update();

    setOutput(GeometryUtils::copyToSurfaceMesh(filter->GetOutput()));
}
}