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

#include "imstkPbdConstantDensityConstraint.h"
#include "imstkParallelUtils.h"

namespace imstk
{
void
PbdConstantDensityConstraint::initConstraint(const VecDataArray<double, 3>& initVertexPositions, const double)
{
    const size_t numParticles = initVertexPositions.size();
    m_lambdas.resize(numParticles);
    m_densities.resize(numParticles);
    m_deltaPositions.resize(numParticles);
    m_neighborList.resize(numParticles);

    // Initialize neighbor searcher
    m_NeighborSearcher = std::make_shared<NeighborSearch>(m_NeighborSearchMethod, m_maxDist);
}

void
PbdConstantDensityConstraint::projectConstraint(const DataArray<double>& imstkNotUsed(currInvMasses),
                                                const double imstkNotUsed(dt),
                                                const PbdConstraint::SolverType& imstkNotUsed(type),
                                                VecDataArray<double, 3>& currVertexPositions)
{
    const size_t numParticles = currVertexPositions.size();

    // Search neighbor for each particle
    m_NeighborSearcher->getNeighbors(m_neighborList, currVertexPositions);

    ParallelUtils::parallelFor(numParticles,
        [&](const size_t idx) {
            computeDensity(currVertexPositions[idx], idx, currVertexPositions);
    });

    ParallelUtils::parallelFor(numParticles,
        [&](const size_t idx) {
            computeLambdaScalingFactor(currVertexPositions[idx], idx, currVertexPositions);
    });

    ParallelUtils::parallelFor(numParticles,
        [&](const size_t idx) {
            updatePositions(currVertexPositions[idx], idx, currVertexPositions);
    });
}

void
PbdConstantDensityConstraint::computeDensity(const Vec3d& pi,
                                             const size_t index,
                                             const VecDataArray<double, 3>& positions)
{
    double densitySum = 0.0;
    for (auto q : m_neighborList[index])
    {
        densitySum += wPoly6(pi, positions[q]);
    }

    m_densities[index] = densitySum;
}

void
PbdConstantDensityConstraint::computeLambdaScalingFactor(const Vec3d& pi,
                                                         const size_t index,
                                                         const VecDataArray<double, 3>& positions)
{
    const double invRestDensity    = 1.0 / m_restDensity;
    const double densityConstraint = (m_densities[index] * invRestDensity) - 1.0;
    double       gradientSum       = 0.0;

    for (auto q : m_neighborList[index])
    {
        gradientSum += gradSpiky(pi, positions[q]).squaredNorm() * invRestDensity;
    }

    m_lambdas[index] = densityConstraint / (gradientSum + m_relaxationParameter);
}

void
PbdConstantDensityConstraint::updatePositions(const Vec3d& pi,
                                              const size_t index,
                                              VecDataArray<double, 3>& positions)
{
    // Make sure the point is valid
    Vec3d gradientLambdaSum(0.0, 0.0, 0.0);
    for (auto q : m_neighborList[index])
    {
        const double lambdasDiff = (m_lambdas[index] + m_lambdas[q]);
        const Vec3d  gradKernal  = gradSpiky(pi, positions[q]);
        gradientLambdaSum += (gradKernal * lambdasDiff);
    }

    m_deltaPositions[index] = gradientLambdaSum / m_restDensity;
    positions[index] += m_deltaPositions[index];
}

void
PbdConstantDensityConstraint::setMaxNeighborDistance(const double dist)
{
    m_maxDist     = dist;
    m_maxDistSqr  = dist * dist;
    m_wPoly6Coeff = 315.0 / (64.0 * PI * pow(m_maxDist, 9));
    m_wSpikyCoeff = 15.0 / (PI * pow(m_maxDist, 6)) * -3.0;
    if (m_NeighborSearcher)
    {
        m_NeighborSearcher->setSearchRadius(m_maxDist);
    }
}
} // imstk
