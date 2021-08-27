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

#include "imstkSphereToSphereCD.h"
#include "imstkSphere.h"

using namespace imstk;

TEST(imstkSphereToSphereCDTest, IntersectionTestAB)
{
    auto sphere1 = std::make_shared<Sphere>(Vec3d(0.0, 0.0, 0.0), 0.5);
    auto sphere2 = std::make_shared<Sphere>(Vec3d(0.0, 0.5, 0.0), 0.5);

    SphereToSphereCD m_sphereToSphereCD;
    m_sphereToSphereCD.setInput(sphere1, 0);
    m_sphereToSphereCD.setInput(sphere2, 1);
    m_sphereToSphereCD.setGenerateCD(true, true); // Generate both A and B
    m_sphereToSphereCD.update();

    std::shared_ptr<CollisionData> colData = m_sphereToSphereCD.getCollisionData();

    // Should be one element per side
    EXPECT_EQ(1, colData->elementsA.size());
    EXPECT_EQ(1, colData->elementsB.size());

    // That element should be a point directional element
    EXPECT_EQ(CollisionElementType::PointDirection, colData->elementsA[0].m_type);
    EXPECT_EQ(CollisionElementType::PointDirection, colData->elementsB[0].m_type);

    // Sphere A (which is below B), should be pushed down, while sphere B (above A), should be pushed up
    EXPECT_EQ(Vec3d(0.0, -1.0, 0.0), colData->elementsA[0].m_element.m_PointDirectionElement.dir);
    EXPECT_EQ(Vec3d(0.0, 1.0, 0.0), colData->elementsB[0].m_element.m_PointDirectionElement.dir);

    // Should have depths of 0.5
    EXPECT_EQ(0.5, colData->elementsA[0].m_element.m_PointDirectionElement.penetrationDepth);
    EXPECT_EQ(0.5, colData->elementsB[0].m_element.m_PointDirectionElement.penetrationDepth);

    // Contacts should be on the surface of spheres
    EXPECT_EQ(Vec3d(0.0, 0.5, 0.0), colData->elementsA[0].m_element.m_PointDirectionElement.pt);
    EXPECT_EQ(Vec3d(0.0, 0.0, 0.0), colData->elementsB[0].m_element.m_PointDirectionElement.pt);
}

TEST(imstkSphereToSphereCDTest, NonIntersectionTestAB)
{
    auto sphere1 = std::make_shared<Sphere>(Vec3d(-1.0, 0.0, 0.0), 0.5);
    auto sphere2 = std::make_shared<Sphere>(Vec3d(1.0, 0.0, 0.0), 0.5);

    SphereToSphereCD m_sphereToSphereCD;
    m_sphereToSphereCD.setInput(sphere1, 0);
    m_sphereToSphereCD.setInput(sphere2, 1);
    m_sphereToSphereCD.setGenerateCD(true, true); // Generate both A and B
    m_sphereToSphereCD.update();

    std::shared_ptr<CollisionData> colData = m_sphereToSphereCD.getCollisionData();

    // Should be no elements
    EXPECT_EQ(0, colData->elementsA.size());
    EXPECT_EQ(0, colData->elementsB.size());
}
