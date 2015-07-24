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

#ifndef SMMYSTYLUS_H
#define SMMYSTYLUS_H

// STL includes
#include <memory>

// SimMedTK includes
#include "Simulators/StylusObject.h"

class MeshContainer;

/// \brief class for avatar controlled by phantom omni in virtual space
class MyStylus: public StylusRigidSceneObject
{

public:
    size_t phantomID; ///< ID for phantom omni
    bool buttonState[4]; ///< state of the buttons
    float angle; ///< angle of the jaw (if applicable)
    float maxangle; ///< maximum angle of the jaw (if applicable)
    MeshContainer meshContainer, meshContainerLower, meshContainerUpper; ///< !!

    /// \brief handle keyboard and omni button presses
    void handleEvent(std::shared_ptr<core::Event> p_event) override;

    /// \brief constructor
    MyStylus(const std::string& p_shaft = "../../resources/models/blunt_diss_pivot.3DS",
             const std::string& p_lower = "../../resources/models/blunt_diss_lower.3DS",
             const std::string& p_upper = "../../resources/models/blunt_diss_upper.3DS");

    /// \brief update the closing and opening of jaws (if applicable)
    void updateOpenClose();
};

/// \brief class for cauter tool avatar in virtual space
class HookCautery: public StylusRigidSceneObject
{

public:
    size_t phantomID; ///< ID of the phantom omni
    bool buttonState[4]; ///< state of the buttons
    MeshContainer meshContainer; ///< !!

    /// \brief handle keyboard and omni button presses
    void handleEvent(std::shared_ptr<core::Event> p_event) override;

    /// \brief constructor
    HookCautery(const std::string& p_pivot = "../../resources/models/hook_cautery_new.3DS");
};

#endif