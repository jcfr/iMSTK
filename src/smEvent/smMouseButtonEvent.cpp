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

#include "smMouseButtonEvent.h"

namespace smtk {
namespace Event {

EventType smMouseButtonEvent::EventName = EventType::MouseButton;

smMouseButtonEvent::smMouseButtonEvent(const smMouseButton& button): pressed(false), mouseButton(button)
{}
const smMouseButton& smMouseButtonEvent::getMouseButton()
{
    return mouseButton;
}
void smMouseButtonEvent::setPresed(const bool& press)
{
    this->pressed = press;
}
const bool& smMouseButtonEvent::getPressed()
{
    return this->pressed;
}
const bool& smMouseButtonEvent::togglePressed()
{
    return this->pressed = !this->pressed;
}
void smMouseButtonEvent::setWindowCoord(const smVec2d& coordinates)
{
    this->coord = coordinates;
}
const smVec2d& smMouseButtonEvent::getWindowCoord()
{
    return this->coord;
}

} // Event namespace
} // smtk namespace
