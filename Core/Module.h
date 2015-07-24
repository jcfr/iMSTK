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

#ifndef SMMODULE_H
#define SMMODULE_H

// STL includes
#include <memory>

// SimMedTK includes
#include "Core/Config.h"
#include "Core/CoreClass.h"
#include "Core/Dispatcher.h"
#include "Core/Scene.h"
#include "Core/SceneObject.h"

///this class is module major. Every other thread should derive this class
class Module: public CoreClass
{

private:
    friend class SDK;

protected:
    ///initialization flag
    bool isInitialized;

    ///execution termination..if it is true exit from the thread
    bool terminateExecution;

    ///When the terminatation is done by the module, this will be true
    bool terminationCompleted;

    ///scene list in the environment
    std::vector<std::shared_ptr<Scene>> sceneList;
    /// \brief call are made for begin module and end module before and after each frame
    virtual void beginModule();
    virtual void   endModule();
    /// \brief  dispatcher reference
    std::shared_ptr<Dispatcher> dispathcer;

public:
    /// \brief  constructor initializes the module
    Module();

    /// \brief virtual functions
    virtual void init() = 0;
    virtual void beginFrame() = 0;
    virtual void endFrame() = 0;
    virtual void exec() = 0;
    /// \brief flags for termination
    void terminate();

    /// \brief  to check if the termination of the module is completed
    bool isTerminationDone();

    /// \brief  wait for termination
    void waitTermination();

    /// \brief  get module id
    int getModuleId();

};

#endif