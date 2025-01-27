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

#include "imstkModule.h"
#include "imstkLogger.h"

namespace imstk
{
void
Module::setSleepDelay(const double ms)
{
    CHECK(ms >= 0.0);
    m_sleepDelay = ms;
}

void
Module::update()
{
    if (m_init && !m_paused)
    {
        if (m_sleepDelay != 0.0)
        {
            std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(m_sleepDelay));
        }

        if (m_muteUpdateEvents)
        {
            this->updateModule();
        }
        else
        {
            this->postEvent(Event(Module::preUpdate()));
            this->updateModule();
            this->postEvent(Event(Module::postUpdate()));
        }
    }
}

void
Module::uninit()
{
    // Can only uninit if, init'd
    if (m_init)
    {
        uninitModule();
        m_init = false;
    }
}
}// imstk
