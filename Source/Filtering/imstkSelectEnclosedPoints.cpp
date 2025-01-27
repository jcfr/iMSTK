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

#include "imstkSelectEnclosedPoints.h"
#include "imstkGeometryUtilities.h"
#include "imstkLogger.h"
#include "imstkSurfaceMesh.h"
#include "imstkVecDataArray.h"

#include <vtkPolyData.h>
#include <vtkSelectEnclosedPoints.h>

namespace imstk
{
SelectEnclosedPoints::SelectEnclosedPoints()
{
    setNumberOfInputPorts(2);
    setRequiredInputType<SurfaceMesh>(0);
    setRequiredInputType<PointSet>(1);

    setNumberOfOutputPorts(1);
    setOutput(std::make_shared<PointSet>());
}

void
SelectEnclosedPoints::setInputMesh(std::shared_ptr<SurfaceMesh> inputMesh)
{
    setInput(inputMesh, 0);
}

void
SelectEnclosedPoints::setInputPoints(std::shared_ptr<PointSet> inputPts)
{
    setInput(inputPts, 1);
}

std::shared_ptr<PointSet>
SelectEnclosedPoints::getOutputPoints() const
{
    return std::static_pointer_cast<PointSet>(getOutput(0));
}

void
SelectEnclosedPoints::requestUpdate()
{
    std::shared_ptr<SurfaceMesh> inputSurfaceMesh = std::dynamic_pointer_cast<SurfaceMesh>(getInput(0));
    std::shared_ptr<PointSet>    inputPointSet    = std::dynamic_pointer_cast<PointSet>(getInput(1));
    m_IsInsideMask = nullptr;

    if (inputSurfaceMesh == nullptr || inputPointSet == nullptr)
    {
        LOG(WARNING) << "Missing inputs";
        return;
    }

    vtkNew<vtkSelectEnclosedPoints> filter;
    filter->SetInputData(GeometryUtils::copyToVtkPointSet(inputPointSet));
    filter->SetSurfaceData(GeometryUtils::copyToVtkPolyData(inputSurfaceMesh));
    filter->SetTolerance(m_Tolerance);
    filter->SetInsideOut(m_InsideOut);
    filter->Update();

    vtkSmartPointer<vtkPointSet> vtkResults = vtkPointSet::SafeDownCast(filter->GetOutput());

    if (m_UsePruning)
    {
        std::shared_ptr<VecDataArray<double, 3>> points = std::make_shared<VecDataArray<double, 3>>();
        points->reserve(vtkResults->GetNumberOfPoints());
        for (vtkIdType i = 0; i < vtkResults->GetNumberOfPoints(); i++)
        {
            if (filter->IsInside(i))
            {
                double pt[3];
                vtkResults->GetPoint(i, pt);
                points->push_back(Vec3d(pt[0], pt[1], pt[2]));
            }
        }
        points->squeeze();

        std::shared_ptr<PointSet> results = std::make_shared<PointSet>();
        results->initialize(points);
        setOutput(results, 0);
    }
    else
    {
        m_IsInsideMask = std::make_shared<DataArray<unsigned char>>(vtkResults->GetNumberOfPoints());
        DataArray<unsigned char>& mask = *m_IsInsideMask;
        for (vtkIdType i = 0; i < vtkResults->GetNumberOfPoints(); i++)
        {
            mask[i] = filter->IsInside(i);
        }
    }
}
}