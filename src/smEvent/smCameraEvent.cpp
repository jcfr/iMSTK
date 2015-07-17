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

#include "smCameraEvent.h"

namespace smtk {
namespace Event {

EventType smCameraEvent::EventName = EventType::CameraUpdate;

smCameraEvent::smCameraEvent()
{
    position = smVec3d::Zero();
    direction = smVec3d::Zero();
    upDirection = smVec3d::Zero();
}
void smCameraEvent::setPosition(const smVec3d& cameraPosition)
{
    this->position = cameraPosition;
}
const smVec3d& smCameraEvent::getPosition()
{
    return this->position;
}
void smCameraEvent::setDirection(const smVec3d& cameraDirection)
{
    this->direction = cameraDirection;
}
const smVec3d& smCameraEvent::getDirection()
{
    return this->direction;
}
void smCameraEvent::setUpDirection(const smVec3d& cameraUpDirection)
{
    this->upDirection = cameraUpDirection;
}
const smVec3d& smCameraEvent::getUpDirection()
{
    return this->upDirection;
}

} // Event namespace
} // smtk namespace
