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

#ifndef IOMESHDELEGATE_H
#define IOMESHDELEGATE_H

#include <cstdint>
#include <memory>

#include "IO/IOMesh.h"
#include "Core/Config.h"
#include "Mesh/SurfaceMesh.h"
#include "Mesh/VegaVolumetricMesh.h"

// VegaFEM includes
#include "tetMesh.h"
#include "cubicMesh.h"
#include "generateSurfaceMesh.h"
#include "objMesh.h"

///
/// \brief Base class for mesh IO delegates.
///
class IOMeshDelegate
{
public:
    typedef std::shared_ptr<IOMeshDelegate> Ptr;

public:
    IOMeshDelegate() : meshProps(0){}
    void setSource(IOMesh *src)
    {
        meshIO = src;
    }

    ///
    /// \brief Performs the actual reading of the mesh.
    ///     It populates the local mesh datastructure.
    ///
    virtual void read() { }

    ///
    /// \brief Write mesh.
    ///  You can use one of the boundled writers or any customized one.
    ///
    virtual void write(){ }

    /// \brief Mesh type and property variable
    enum MeshType
    {
        Unknown         = 0,
        Tri             = 1,
        Tetra           = 1 << 1,
        Hexa            = 1 << 2,
        hasMaterials    = 1 << 3,
        hasBDConditions = 1 << 4,
        hasDensity      = 1 << 4,
        hasPoisson      = 1 << 5,
        hasYoung        = 1 << 6

    };

    // Used to know what type of mesh is being read.
    unsigned int meshProps;

protected:
    ///
    /// \brief Creates a new surface mesh with vertices and triangleArray and
    /// stores it in the meshIO.
    ///
    void setSurfaceMesh(const std::vector<core::Vec3d> &vertices,
                        const std::vector<std::array<size_t,3>> &triangleArray)
    {
        auto mesh = std::make_shared<SurfaceMesh>();
        mesh->setVertices(vertices);
        mesh->setTriangles(triangleArray);
        mesh->computeTriangleNormals();
        mesh->computeVertexNormals();
        this->meshIO->setMesh(mesh);
    }

    ///
    /// \brief Creates a new vega volumetric mesh with vertices and tetraArray
    /// and stores it in the meshIO. Uses the vega mesh constructor that takes
    /// boundary conditions and material properties.
    ///
    void setVegaTetraMesh(const std::vector<core::Vec3d> &vertices,
                          const std::vector<std::array<size_t,4>> &tetraArray,
                          const std::vector<size_t> &bdConditions,
                          const core::Vec3d &material)
    {
        auto tetraMesh = std::make_shared<VegaVolumetricMesh>();
        std::vector<int> vegaElements;

        std::for_each(tetraArray.begin(),tetraArray.end(),
                      [&](const std::array<size_t,4> &i)
                      {
                          vegaElements.push_back(i[0]); vegaElements.push_back(i[1]);
                          vegaElements.push_back(i[2]); vegaElements.push_back(i[3]);
                      });

        double *v = const_cast<double*>(vertices.data()->data());
        int *e = vegaElements.data();

        int v_size = vertices.size();
        int e_size = tetraArray.size();
        auto vegaMesh = std::make_shared<TetMesh>(v_size,
                                                  v,
                                                  e_size,
                                                  e,
                                                  material[0],
                                                  material[1],
                                                  material[2]);
        tetraMesh->setVegaMesh(vegaMesh);
        tetraMesh->setFixedVertices(bdConditions);
        this->meshIO->setMesh(tetraMesh);

    }

    ///
    /// \brief Creates a new vega volumetric mesh with vertices and tetraArray
    /// and stores it in the meshIO.
    ///
    void setVegaTetraMesh(const std::vector<core::Vec3d> &vertices,
                          const std::vector<std::array<size_t,4>> &tetraArray)
    {
        auto tetraMesh = std::make_shared<VegaVolumetricMesh>();
        std::vector<int> vegaElements;

        std::for_each(tetraArray.begin(),tetraArray.end(),
                      [&](const std::array<size_t,4> &i)
                      {
                          vegaElements.push_back(i[0]); vegaElements.push_back(i[1]);
                          vegaElements.push_back(i[2]); vegaElements.push_back(i[3]);
                      });

        double *v = const_cast<double*>(vertices.data()->data());
        int *e = vegaElements.data();
        int v_size = vertices.size();
        int e_size = tetraArray.size();
        auto vegaMesh = std::make_shared<TetMesh>(v_size,
                                                  v,
                                                  e_size,
                                                  e);
        tetraMesh->setVegaMesh(vegaMesh);
        this->meshIO->setMesh(tetraMesh);
    }

    ///
    /// \brief Creates a new vega volumetric mesh with vertices and hexaArray
    /// and stores it in the meshIO. Uses the vega mesh constructor that takes
    /// boundary conditions and material properties.
    ///
    void setVegaHexaMesh(const std::vector<core::Vec3d> &vertices,
                         const std::vector<std::array<size_t,8>> &hexaArray,
                         const std::vector<size_t> &bdConditions,
                         const std::array<double,3> &material)
    {
        auto hexaMesh = std::make_shared<VegaVolumetricMesh>();
        std::vector<int> vegaElements;

        std::for_each(hexaArray.begin(),hexaArray.end(),
                      [&](const std::array<size_t,8> &i)
                      {
                          vegaElements.push_back(i[0]); vegaElements.push_back(i[1]);
                          vegaElements.push_back(i[2]); vegaElements.push_back(i[3]);
                          vegaElements.push_back(i[4]); vegaElements.push_back(i[5]);
                          vegaElements.push_back(i[6]); vegaElements.push_back(i[7]);
                      });

        double *v = const_cast<double*>(vertices.data()->data());
        int *e = vegaElements.data();
        int v_size = vertices.size();
        int e_size = hexaArray.size();
        auto vegaMesh = std::make_shared<CubicMesh>(v_size,
                                                    v,
                                                    e_size,
                                                    e,
                                                    material[0],
                                                    material[1],
                                                    material[2]);
        hexaMesh->setVegaMesh(vegaMesh);
        hexaMesh->setFixedVertices(bdConditions);
        this->meshIO->setMesh(hexaMesh);
    }

    ///
    /// \brief Creates a new vega volumetric mesh with vertices and hexaArray
    /// and stores it in the meshIO.
    /// \param vertices Vertex array
    /// \param hexaArray Hexahedron array
    ///
    void setVegaHexaMesh(const std::vector<core::Vec3d> &vertices,
                         const std::vector<std::array<size_t,8>> &hexaArray)
    {
        auto hexaMesh = std::make_shared<VegaVolumetricMesh>();
        std::vector<int> vegaElements;

        std::for_each(hexaArray.begin(),hexaArray.end(),
                      [&](const std::array<size_t,8> &i)
                      {
                          vegaElements.push_back(i[0]); vegaElements.push_back(i[1]);
                          vegaElements.push_back(i[2]); vegaElements.push_back(i[3]);
                          vegaElements.push_back(i[4]); vegaElements.push_back(i[5]);
                          vegaElements.push_back(i[6]); vegaElements.push_back(i[7]);
                      });

        double *v = const_cast<double*>(vertices.data()->data());
        int *e = vegaElements.data();
        int v_size = vertices.size();
        int e_size = hexaArray.size();
        auto vegaMesh = std::make_shared<CubicMesh>(v_size,
                                                    v,
                                                    e_size,
                                                    e);
        hexaMesh->setVegaMesh(vegaMesh);
        this->meshIO->setMesh(hexaMesh);
    }

    ///
    /// \brief Utility function to help extract a surface mesh from a volume
    ///      mesh.
    /// \param triangleArray Triangle array of the surface of the volumetric
    ///     mesh. This array will be sorted and updated with new indices.
    /// \param vertices Entire volumetric mesh vertex array.
    /// \param surfaceVertices Ouput array containing the vertex array for
    ///     the surface mesh.
    /// \param uniqueVertexArray Output map of the vertices. Maps indices
    ///     from the surfaceVertices to vertices.
    ///
    void reorderSurfaceTopology(const std::vector<core::Vec3d> &vertices,
                                std::vector<core::Vec3d> &surfaceVertices,
                                std::vector<std::array<size_t,3>> &triangleArray,
                                std::unordered_map<size_t,size_t> &uniqueVertexArray
                               )
    {
        // First, sort the triangle array to speed thing up later.
        std::sort(std::begin(triangleArray),std::end(triangleArray));

        // Use a set to discard repeated indices in the surface triangles. T
        // This also sort the vertex index.
        std::set<size_t> uniqueVertexSet;
        for(const auto &t : triangleArray)
        {
            for(const auto &v : t)
            {
                uniqueVertexSet.insert(v);
            }
        }

        // Create a map between the volumetric mesh vertex indices and surface
        // vertex indices.
        size_t index = 0;
        for(const auto &v : uniqueVertexSet)
        {
            uniqueVertexArray[index++] = v;
        }
        for(auto &t : triangleArray)
        {
            for(auto &v : t)
            {
                v = uniqueVertexArray[v];
            }
        }

        for(const auto &v : uniqueVertexSet)
        {
            surfaceVertices.emplace_back(vertices[v]);
        }
    }

protected:
    IOMesh *meshIO;
};
#endif // READERDELEGATE_H
