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

#ifndef SMSTYLUSOBJECT_H
#define SMSTYLUSOBJECT_H

// STD includes
#include <unordered_map>

// Eigen includes
#include "Eigen/Geometry"

// SimMedTK includes
#include "Core/Config.h"
#include "Mesh/Mesh.h"
#include "Core/SceneObject.h"
#include "External/tree.hh"

namespace core {
    class Event;
    class EventHandler;
}

template<typename SurfaceTreeCell> class SurfaceTree;
class OctreeCell;

/// \brief !!
class MeshContainer
{
public:
    using SurfaceTreeType = SurfaceTree<OctreeCell>;

public:
    /// \brief constructor
    MeshContainer(std::string p_name = "");

    /// \brief constructor
    MeshContainer(std::string p_name, Mesh *p_mesh, core::Vec3d p_prePos, core::Vec3d p_posPos, float p_offsetRotX, float p_offsetRotY, float p_offsetRotZ);

    void computeCurrentMatrix();

public:
    std::string name;
    float offsetRotX; // offset in rotation in x-direction
    float offsetRotY; // offset in rotation in y-direction
    float offsetRotZ; // offset in rotation in z-direction
    core::Vec3d preOffsetPos; // !!
    core::Vec3d posOffsetPos; // !!
    Matrix44d accumulatedMatrix; // !!
    Matrix44d accumulatedDeviceMatrix; // !!

    Matrix44d currentMatrix; // !!
    Matrix44d currentViewerMatrix; // !!
    Matrix44d currentDeviceMatrix; // !!
    Matrix44d tempCurrentMatrix; // !!
    Matrix44d tempCurrentDeviceMatrix; // !!
    Mesh * mesh; // mesh
    std::shared_ptr<SurfaceTreeType> colModel; // octree of surface
};

/// \brief points on the stylus
struct StylusPoints
{
    /// \brief constructor
    StylusPoints();

    core::Vec3d point; // co-ordinates of points on stylus
    MeshContainer *container; // !!
};

/// \brief stylus object of the scene (typically used for laparascopic VR simulations)
class StylusSceneObject: public SceneObject
{
public:
    /// \brief constructor
    StylusSceneObject(std::shared_ptr<ErrorLog> p_log = nullptr);

    /// \brief !!
    virtual void serialize(void *p_memoryBlock) override;

    /// \brief !!
    virtual void unSerialize(void *p_memoryBlock) override;

    /// \brief handle the events such as button presses related to stylus
    void handleEvent(std::shared_ptr<core::Event> p_event) override;

public:
    core::Vec3d pos; // position of stylus
    core::Vec3d vel; // velocity of stylus
    Matrix33d rot; // rotation of stylus
    Matrix44d transRot; // !! translation and rotation matrix of stylus
    Matrix44d transRotDevice; // translation and rotation matrix of devide controlling the stylus
    bool toolEnabled; // !!

protected:
    std::shared_ptr<core::EventHandler> eventHandler;
};

/// \brief !!
class StylusRigidSceneObject: public StylusSceneObject
{
public:
    tree<MeshContainer*> meshes; // meshes representing the stylus
    tree<MeshContainer*>::iterator rootIterator; // !!
    volatile bool updateViewerMatrixEnabled; // !!

    /// \brief to show the device tool..It is for debugging god object
    bool enableDeviceManipulatedTool;

    /// \brief enabling post traverse callback enabled for each node.
    bool posTraverseCallbackEnabled;

    /// \brief post traverse callback for each node
    virtual void posTraverseCallBack(MeshContainer &p_container);

    /// \brief Post Traverse callback for the entire object.
    virtual void posTraverseCallBack();

    bool posCallBackEnabledForEntireObject; // !!

    /// \brief !!
    StylusRigidSceneObject(std::shared_ptr<ErrorLog> p_log = nullptr);

    /// \brief !!
    tree<MeshContainer*>::iterator addMeshContainer(MeshContainer *p_meshContainer);

    /// \brief !!
    bool addMeshContainer(std::string p_ParentName, MeshContainer *p_meshContainer);

    /// \brief !!
    tree<MeshContainer*>::iterator addMeshContainer(tree<MeshContainer*>::iterator p_iterator, MeshContainer *p_meshContainer);

    /// \brief !!
    MeshContainer *getMeshContainer(std::string p_string) const;

    virtual void handleEvent(std::shared_ptr<core::Event> p_event) override;

    /// \brief !!
    std::shared_ptr<SceneObject> clone() override;

    /// \brief !!
    virtual void initialize() override {};

    void loadInitialStates() override{};

    bool configure(const std::string /*ConfigFile*/)
    {
        return false;
    }

    void printInfo() const override
    {
        std::cout << "\t-------------------------------------\n";
        std::cout << "\t Name        : " << this->getName() << std::endl;
        std::cout << "\t-------------------------------------\n";
    }

private:
    std::unordered_map<std::string, tree<MeshContainer*>::iterator> indexIterators;
};




/// \brief !!
class StylusDeformableSceneObject: public StylusSceneObject
{

public:
    StylusDeformableSceneObject(std::shared_ptr<ErrorLog> p_log = nullptr);

    ~StylusDeformableSceneObject(){};

    virtual void initialize() override {};

    void loadInitialStates() {};

    bool configure(std::string /*ConfigFile*/)
    {
        return false;
    }

    std::shared_ptr<void> duplicateAtRuntime()
    {
        std::shared_ptr<StylusDeformableSceneObject> newSO =
            std::make_shared<StylusDeformableSceneObject>();

        return std::static_pointer_cast<void>(newSO);
    }
    std::shared_ptr<void> duplicateAtInitialization()
    {
        std::shared_ptr<StylusDeformableSceneObject> newSO =
            std::make_shared<StylusDeformableSceneObject>();

        return std::static_pointer_cast<void>(newSO);
    }
    std::shared_ptr<SceneObject> clone() override
    {
        // WARNING: What is the purpose of this function
        std::shared_ptr<StylusDeformableSceneObject> ret = std::make_shared<StylusDeformableSceneObject>();
        return ret;
    }

    void printInfo() const override
    {
        std::cout << "\t-------------------------------------\n";
        std::cout << "\t Name        : " << this->getName() << std::endl;
        std::cout << "\t-------------------------------------\n";
    }
};

#endif
