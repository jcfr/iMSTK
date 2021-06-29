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

#include "imstkPbdInflatableVolumeConstraint.h"

#include <cfloat>

namespace imstk
{
void
PbdInflatableVolumeConstraint::initConstraint(const VecDataArray<double, 3>& initVertexPositions,
                                              const size_t& pIdx0, const size_t& pIdx1,
                                              const size_t& pIdx2, const size_t& pIdx3,
                                              const double k)
{
    PbdVolumeConstraint::initConstraint(initVertexPositions, pIdx0, pIdx1, pIdx2, pIdx3, k);
    m_initialRestVolume = m_restVolume;
}

void
PbdInflatableVolumeConstraint::multiplyRestVolumeBy(const double ratio)
{
    // Multiplied ratio must be positive
    if (ratio < DBL_EPSILON)
    {
        return;
    }

    m_restVolume *= ratio;

    // Modified RestVolume can't be less than initial RestVolume
    if (abs(m_restVolume) < abs(m_initialRestVolume))
    {
        m_restVolume = m_initialRestVolume;
    }
}
} // imstk
