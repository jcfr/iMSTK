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

#include "imstkTaskNode.h"
#include "imstkTimer.h"

namespace imstk
{
void
TaskNode::execute()
{
    if (m_enabled && m_func != nullptr)
    {
        if (!m_enableTiming)
        {
            m_func();
        }
        else
        {
            StopWatch timer;
            timer.start();
            m_func();
            m_computeTime = timer.getTimeElapsed();
            timer.stop();
        }
    }
    else
    {
        m_computeTime = 0.0;
    }
}
}