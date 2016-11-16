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

#include "imstkVTKSurfaceMeshRenderDelegate.h"

#include "imstkSurfaceMesh.h"

#include <vtkPolyData.h>
#include <vtkPolyDataNormals.h>
#include <vtkPolyDataMapper.h>
#include <vtkPoints.h>
#include <vtkDoubleArray.h>
#include <vtkCellArray.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>
#include <vtkImageReader2Factory.h>
#include <vtkImageReader2.h>
#include <vtkTexture.h>
#include <vtkProperty.h>

#include "g3log/g3log.hpp"

namespace imstk
{

VTKSurfaceMeshRenderDelegate::VTKSurfaceMeshRenderDelegate(std::shared_ptr<SurfaceMesh> surfaceMesh) :
    m_geometry(surfaceMesh)
{
    // Map vertices
    StdVectorOfVec3d& vertices = m_geometry->getVerticesPositionsNotConst();
    double* vertData = reinterpret_cast<double*>(vertices.data());
    m_mappedVertexArray = vtkSmartPointer<vtkDoubleArray>::New();
    m_mappedVertexArray->SetNumberOfComponents(3);
    m_mappedVertexArray->SetArray(vertData, vertices.size()*3, 1);

    // Create points
    auto points = vtkSmartPointer<vtkPoints>::New();
    points->SetNumberOfPoints(m_geometry->getNumVertices());
    points->SetData(m_mappedVertexArray);

    // Copy cells
    auto cells = vtkSmartPointer<vtkCellArray>::New();
    vtkIdType cell[3];
    for(const auto &t : m_geometry->getTrianglesVertices())
    {
        for(size_t i = 0; i < 3; ++i)
        {
            cell[i] = t[i];
        }
        cells->InsertNextCell(3,cell);
    }

    // Create PolyData
    auto polydata = vtkSmartPointer<vtkPolyData>::New();
    polydata->SetPoints(points);
    polydata->SetPolys(cells);

    // Compute Normals
    auto normalGen = vtkSmartPointer<vtkPolyDataNormals>::New();
    normalGen->SetInputData(polydata);
    normalGen->SplittingOff();

    // Mapper
    auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(normalGen->GetOutputPort());

    // Copy textures
    int unit = 0;
    auto readerFactory = vtkSmartPointer<vtkImageReader2Factory>::New();
    for (auto const &texturePair : surfaceMesh->getTextureMap())
    {
        std::string tCoordsName = texturePair.first;
        std::string tFileName = texturePair.second;

        // Convert texture coordinates
        auto tcoords = surfaceMesh->getPointDataArray(tCoordsName);
        auto vtkTCoords = vtkSmartPointer<vtkFloatArray>::New();
        vtkTCoords->SetNumberOfComponents(2);
        vtkTCoords->SetName(tCoordsName.c_str());
        for (auto const tcoord : tcoords)
        {
            double tuple[2] = {tcoord[0], tcoord[1]};
            vtkTCoords->InsertNextTuple(tuple);
        }
        polydata->GetPointData()->SetTCoords(vtkTCoords);

        // Read texture image
        auto imgReader = readerFactory->CreateImageReader2(tFileName.c_str());
        if (!imgReader)
        {
            LOG(WARNING) << "Could not find reader for " << tFileName;
            continue;
        }
        imgReader->SetFileName(tFileName.c_str());
        imgReader->Update();
        auto texture = vtkSmartPointer<vtkTexture>::New();
        texture->SetInputConnection(imgReader->GetOutputPort());
        texture->SetBlendingMode(vtkTexture::VTK_TEXTURE_BLENDING_MODE_ADD);
        texture->SetWrapMode(vtkTexture::VTKTextureWrapMode::ClampToBorder);

        // Link textures
        mapper->MapDataArrayToMultiTextureAttribute(unit, tCoordsName.c_str(),
                    vtkDataObject::FIELD_ASSOCIATION_POINTS);
        m_actor->GetProperty()->SetTexture(unit, texture);
        unit++;
    }

    // Actor
    m_actor->SetMapper(mapper);

    // Transform
    this->updateActorTransform();
}

void
VTKSurfaceMeshRenderDelegate::update()
{
    // Base class update
    VTKRenderDelegate::update();

    m_mappedVertexArray->Modified(); // TODO: only modify if vertices change
}

std::shared_ptr<Geometry>
VTKSurfaceMeshRenderDelegate::getGeometry() const
{
    return m_geometry;
}

} // imstk
