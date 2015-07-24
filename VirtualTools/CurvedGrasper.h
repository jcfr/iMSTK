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

#ifndef CURVEDGRASPER_H
#define CURVEDGRASPER_H

// SimMedTK includes
#include "Simulators/StylusObject.h"
#include "Mesh/SurfaceMesh.h"

namespace core {
    class Event;
}

/// \brief Cruver Grasper tool
class CurvedGrasper: public StylusRigidSceneObject
{
public:
    /// \brief constrcutor that gest hatpic device ID (e.g. 0 or 1), pivot, lower and upper mesh file names
    CurvedGrasper(size_t ID,
                  const std::string& p_pivotModelFileName = "../../resources/models/curved_pivot.3DS",
                  const std::string& p_lowerModelFileName = "../../resources/models/curved_upper.3DS",
                  const std::string& p_upperModelFileName = "../../resources/models/curved_lower.3DS");

    /// \brief event handler
    void handleEvent(std::shared_ptr<core::Event> p_event) override;

    /// \brief for open and close motion
    void updateOpenClose();

public:
    size_t phantomID; // phantom device ID that will be listened
    bool buttonState[2]; // buttons states of haptic device
    double angle; // angle of the jaws
    double maxangle; // maximum angle that jaws can open
    MeshContainer meshContainer_pivot; // the pivto mesh container
    MeshContainer meshContainer_lowerJaw; // lower jaw container
    MeshContainer meshContainer_upperJaw; // upper jaw container
    SurfaceMesh *mesh_pivot; // stores the pivot mesh
    SurfaceMesh *mesh_lowerJaw; // stores lower jaw mesh
    SurfaceMesh *mesh_upperJaw; // stores upper mesh jaw

    core::Vec3d godPos; // god object position
    double godMat[9]; // god object matrix
    int DAQdataID; // interface for DAQ
    double minValue; // read  min  data value
    double maxValue; // read  max  data value
    double invRange; // 1/range value
};

#endif