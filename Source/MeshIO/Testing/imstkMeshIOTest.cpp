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

#include "gtest/gtest.h"

#include "imstkMeshIO.h"
#include "imstkGeometryUtilities.h"
#include "imstkSurfaceMesh.h"

using namespace imstk;

TEST(imstkMeshIODeathTest, FailOnMissingFile)
{
    EXPECT_DEATH(MeshIO::read<SurfaceMesh>(iMSTK_DATA_ROOT "doesntexist.obj"), "doesntexist.obj doesn't exist");
}

// Test for issue https://gitlab.kitware.com/iMSTK/iMSTK/-/issues/365
TEST(imstkMeshIOTest, conversionBug)
{
    auto mesh = MeshIO::read<SurfaceMesh>(iMSTK_DATA_ROOT "testing/MeshIO/bugs/membrane_model.vtk");
    ASSERT_TRUE(mesh);

    EXPECT_NO_FATAL_FAILURE(auto data = GeometryUtils::copyToVtkPolyData(mesh));
}