// This file is part of the SimMedTK project.
// Copyright (c) Center for Modeling, Simulation, and Imaging in Medicine,
//                        Rensselaer Polytechnic Institute
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//---------------------------------------------------------------------------
//
// Authors:
//
// Contact:
//---------------------------------------------------------------------------

#include "smCore/smMakeUnique.h"
#include <bandit/bandit.h>

#include "smGeometry/smMeshModel.h"

using namespace bandit;

std::shared_ptr<smMeshModel> getModel(const smStdVector3d &vertices)
{
    std::shared_ptr<smMeshModel> model = std::make_shared<smMeshModel>();
    std::shared_ptr<smMesh> mesh = std::make_shared<smSurfaceMesh>();
    model->setModelMesh(mesh);

    // Add two triangles to the data structure
    mesh->initVertexArrays(3);
    mesh->initTriangleArrays(1);

    mesh->vertices.push_back(vertices[0]);
    mesh->vertices.push_back(vertices[1]);
    mesh->vertices.push_back(vertices[2]);

    mesh->triangles[0].vert[0] = 0;
    mesh->triangles[0].vert[1] = 1;
    mesh->triangles[0].vert[2] = 2;

    mesh->initVertexNeighbors();
    mesh->updateTriangleNormals();
    mesh->updateVertexNormals();

    return model;
}

go_bandit([](){
    describe("BVH Collision Detection Algorithm", []() {
        it("constructs ", []() {
            auto model = make_unique<smMeshModel>();
            AssertThat(model != nullptr, IsTrue());
        });
        it("can access mesh vertices ", []() {

            smStdVector3d vertices;
            vertices.emplace_back(1.0,2.0,0);
            vertices.emplace_back(2.0,3.0,0);
            vertices.emplace_back(2.0,1.0,0);

            auto model = getModel(vertices);

            AssertThat(model->getTrianglePositions(0)[0], Equals(vertices[0]));
            AssertThat(model->getTrianglePositions(0)[1], Equals(vertices[1]));
            AssertThat(model->getTrianglePositions(0)[2], Equals(vertices[2]));

            AssertThat(model->getTrianglePositions(1)[0], Equals(vertices[1]));
            AssertThat(model->getTrianglePositions(1)[1], Equals(vertices[2]));
            AssertThat(model->getTrianglePositions(1)[2], Equals(vertices[3]));

        });
        it("can access mesh face normals ", []() {

            smStdVector3d vertices;
            vertices.emplace_back(1.0,2.0,0);
            vertices.emplace_back(2.0,3.0,0);
            vertices.emplace_back(2.0,1.0,0);

            auto model = getModel(vertices);

            smVec3d normalA = (vertices[1]-vertices[0]).cross(vertices[2]-vertices[0]).normalized();
            smVec3d normalB = (vertices[2]-vertices[1]).cross(vertices[3]-vertices[1]).normalized();

            AssertThat((model->getNormal(0)-normalA).squaredNorm(), EqualsWithDelta(0.0,.00001));
            AssertThat((model->getNormal(1)-normalB).squaredNorm(), EqualsWithDelta(0.0,.00001));

        });

    });

});
