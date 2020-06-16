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

#include "imstkVisualModel.h"
#include "imstkGeometry.h"
#include "imstkRenderMaterial.h"
#include "imstkDebugRenderGeometry.h"

namespace imstk
{
VisualModel::VisualModel(std::shared_ptr<Geometry> geometry) : m_geometry(geometry), m_renderMaterial(std::make_shared<RenderMaterial>())
{
}

VisualModel::VisualModel(std::shared_ptr<Geometry>       geometry,
                         std::shared_ptr<RenderMaterial> renderMaterial) :
    m_geometry(geometry),
    m_renderMaterial(renderMaterial)
{
}

VisualModel::VisualModel(std::shared_ptr<DebugRenderGeometry> geometry) : m_DbgGeometry(geometry), m_renderMaterial(std::make_shared<RenderMaterial>())
{
}

VisualModel::VisualModel(std::shared_ptr<DebugRenderGeometry> geometry,
                         std::shared_ptr<RenderMaterial>      renderMaterial) :
    m_DbgGeometry(geometry),
    m_renderMaterial(renderMaterial)
{
}

std::shared_ptr<Geometry>
VisualModel::getGeometry()
{
    return m_geometry;
}

void
VisualModel::setGeometry(std::shared_ptr<Geometry> geometry)
{
    m_geometry = geometry;
}

std::shared_ptr<DebugRenderGeometry>
VisualModel::getDebugGeometry()
{
    return m_DbgGeometry;
}

void
VisualModel::setDebugGeometry(std::shared_ptr<DebugRenderGeometry> dbgGeometry)
{
    m_DbgGeometry = dbgGeometry;
}

void
VisualModel::setRenderMaterial(std::shared_ptr<RenderMaterial> renderMaterial)
{
    m_renderMaterial = renderMaterial;
}

std::shared_ptr<RenderMaterial>
VisualModel::getRenderMaterial() const
{
    return m_renderMaterial;
}

void
VisualModel::show()
{
    m_isVisible = true;
}

void
VisualModel::hide()
{
    m_isVisible = false;
}

bool
VisualModel::isVisible() const
{
    return m_isVisible;
}

bool
VisualModel::isRenderDelegateCreated()
{
    return m_renderDelegateCreated;
}
}