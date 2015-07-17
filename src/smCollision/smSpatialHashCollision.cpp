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

#include "smSpatialHashCollision.h"

// STL includes
#include <cmath>

// SimMedTK includes
#include "smCore/smCollisionConfig.h"
#include "smCollision/smSurfaceTree.h"
#include "smCollision/smOctreeCell.h"
#include "smCollision/smCollisionMoller.h"
#include "smCore/smSDK.h"


struct smSpatialHashCollision::HashFunction
{
    HashFunction(smUInt hashConst1 = 73856093,
                 smUInt hashConst2 = 19349663,
                 smUInt hashConst3 = 83492791) : const1(hashConst1),const2(hashConst2),const3(hashConst3)
                 {}

    smUInt getKey(const smUInt size, const smUInt x, const smUInt y, const smUInt z)
    {
        return (((((x)* const1) ^ ((y)* const2) ^ ((z)* const3))) % (size));
    }
    smUInt const1;
    smUInt const2;
    smUInt const3;
};

smSpatialHashCollision::smSpatialHashCollision(smInt hashTableSize,
                             smFloat _cellSizeX,
                             smFloat _cellSizeY,
                             smFloat _cellSizeZ):
    cells(hashTableSize),
    cellLines(hashTableSize),
    cellsForTri2Line(hashTableSize),
    cellsForModel(hashTableSize),
    cellsForModelPoints(hashTableSize),
    hasher(make_unique<HashFunction>())
{
    cellSizeX = _cellSizeX;
    cellSizeY = _cellSizeY;
    cellSizeZ = _cellSizeZ;
}

smSpatialHashCollision::~smSpatialHashCollision()
{
}

void smSpatialHashCollision::addCollisionModel(std::shared_ptr<SurfaceTreeType> CollMode)
{
    colModel.push_back(CollMode);
}

void smSpatialHashCollision::addMesh(std::shared_ptr<smMesh> mesh)
{
    meshes.push_back(mesh);
    mesh->allocateAABBTris();
}

void smSpatialHashCollision::addMesh(std::shared_ptr<smLineMesh> mesh)
{
    lineMeshes.push_back(mesh);
}

void smSpatialHashCollision::removeMesh(std::shared_ptr<smMesh> mesh)
{
    auto it = std::find(meshes.begin(),meshes.end(),mesh);
    if(it != meshes.end())
        meshes.erase(it);
}

smBool smSpatialHashCollision::findCandidatePoints(std::shared_ptr<smMesh> mesh,
                                          std::shared_ptr<smSpatialHashCollision::SurfaceTreeType> colModel)
{
    smAABB tempAABB;
    tempAABB.aabbMin = colModel->root->getCube().leftMinCorner();
    tempAABB.aabbMax = colModel->root->getCube().rightMaxCorner();

    smBool  found = false;
    for (smInt i = 0; i < mesh->nbrVertices; i++)
    {
        if (smCollisionMoller::checkAABBPoint(tempAABB, mesh->vertices[i]))
        {
            addPoint(mesh, i, cellsForModelPoints);
            found = true;
        }
    }
    return found;
}

smBool smSpatialHashCollision::findCandidateTris(std::shared_ptr<smMesh> meshA, std::shared_ptr<smMesh> meshB)
{
    smAABB aabboverlap;

    if (smCollisionMoller::checkOverlapAABBAABB(meshA->aabb, meshB->aabb, aabboverlap) == false)
    {
        return false;
    }

    for (int i = 0; i < meshA->nbrTriangles; i++)
    {
        addTriangle(meshA, i, cells);
    }

    for (int i = 0; i < meshB->nbrTriangles; i++)
    {
        addTriangle(meshB, i, cells);
    }

    return true;
}

smBool smSpatialHashCollision::findCandidateTrisLines(std::shared_ptr<smMesh> meshA, std::shared_ptr<smLineMesh> meshB)
{
    smAABB aabboverlap;

    if (smCollisionMoller::checkOverlapAABBAABB(meshA->aabb, meshB->aabb, aabboverlap) == false)
    {
        return false;
    }

    for (int i = 0; i < meshA->nbrTriangles; i++)
    {
        if (smCollisionMoller::checkOverlapAABBAABB(aabboverlap, meshA->triAABBs[i]))
        {
            addTriangle(meshA, i, cellsForTri2Line);
        }
    }

    for (int i = 0; i < meshB->nbrEdges; i++)
    {
        if (smCollisionMoller::checkOverlapAABBAABB(aabboverlap, meshB->edgeAABBs[i]))
        {
            addLine(meshB, i, cellLines);
        }
    }

    return true;
}

void smSpatialHashCollision::computeCollisionTri2Tri()
{
    smHashIterator<smCellTriangle> iterator;
    smHashIterator<smCellTriangle> iterator1;

    smCellTriangle triA;
    smCellTriangle triB;
    smVec3d proj1, proj2, inter1, inter2;
    smShort point1, point2;
    smInt coPlanar;

    while (cells.next(iterator))
    {
        while (cells.nextBucketItem(iterator, triA))
        {
            iterator1.clone(iterator);

            while (cells.nextBucketItem(iterator1, triB))
            {
                if (triA.meshID == triB.meshID ||
                    !(meshes[0]->collisionGroup.isCollisionPermitted(meshes[1]->collisionGroup)))
                {
                    continue;
                }

                if (smCollisionMoller::tri2tri(triA.vert[0],
                                               triA.vert[1],
                                               triA.vert[2],
                                               triB.vert[0],
                                               triB.vert[1],
                                               triB.vert[2],
                                               coPlanar,
                                               inter1,
                                               inter2,
                                               point1,
                                               point2,
                                               proj1,
                                               proj2))
                {
                    auto collisionPair = std::make_shared<smCollidedTriangles>();
                    collisionPair->tri1 = triA;
                    collisionPair->tri2 = triB;
                    collidedTriangles.push_back(collisionPair);
                }
            }
        }
    }
}

///line to triangle collision
void  smSpatialHashCollision::computeCollisionLine2Tri()
{
    smHashIterator<smCellLine > iteratorLine;
    smHashIterator<smCellTriangle > iteratorTri;
    smCellLine line;
    smCellTriangle tri;
    smVec3d intersection;

    while (cellLines.next(iteratorLine) && cellsForTri2Line.next(iteratorTri))
    {
        while (cellLines.nextBucketItem(iteratorLine, line))
        {

            iteratorTri.resetBucketIteration();

            while (cellsForTri2Line.nextBucketItem(iteratorTri, tri))
            {
                if (tri.meshID == line.meshID ||
                    !(meshes[0]->collisionGroup.isCollisionPermitted(meshes[1]->collisionGroup)))
                {
                    continue;
                }

                if (smCollisionMoller::checkLineTri(line.vert[0],
                                                    line.vert[1],
                                                    tri.vert[0],
                                                    tri.vert[1],
                                                    tri.vert[2],
                                                    intersection))
                {
                    auto collisionPair = std::make_shared<smCollidedLineTris>();
                    collisionPair->line = line;
                    collisionPair->tri = tri;
                    collisionPair->intersection = intersection;
                    collidedLineTris.push_back(collisionPair);
                }
            }
        }
    }
}

void smSpatialHashCollision::computeCollisionModel2Points()
{
    smHashIterator<smCellModel > iteratorModel;
    smHashIterator<smCellPoint > iteratorPoint;
    smCellModel model;
    smCellPoint point;

    while (cellsForModel.next(iteratorModel) && cellsForModelPoints.next(iteratorPoint))
    {
        while (cellsForModel.nextBucketItem(iteratorModel, model))
        {

            iteratorPoint.resetBucketIteration();

            while (cellsForModelPoints.nextBucketItem(iteratorPoint, point))
            {

                smFloat distanceFromCenter = (model.center - point.vert).norm();

                if (distanceFromCenter < model.radius)
                {
                    auto collisionPair = std::make_shared<smCollidedModelPoints>();
                    collisionPair->penetration = model.radius - distanceFromCenter;
                    collisionPair->model = model;
                    collisionPair->point = point;
                    collidedModelPoints.push_back(collisionPair);
                }
            }
        }
    }
}

void smSpatialHashCollision::computeHash(std::shared_ptr<smMesh> mesh, const std::vector<smInt> &triangleIndexes)
{
    for(auto&& i : triangleIndexes)
    {
        smInt xStartIndex = static_cast<smInt>(std::floor(mesh->triAABBs[i].aabbMin[0]/cellSizeX));
        smInt yStartIndex = static_cast<smInt>(std::floor(mesh->triAABBs[i].aabbMin[1]/cellSizeY));
        smInt zStartIndex = static_cast<smInt>(std::floor(mesh->triAABBs[i].aabbMin[2]/cellSizeZ));

        smInt xEndIndex = static_cast<smInt>(std::floor(mesh->triAABBs[i].aabbMax[0]/cellSizeX));
        smInt yEndIndex = static_cast<smInt>(std::floor(mesh->triAABBs[i].aabbMax[1]/cellSizeY));
        smInt zEndIndex = static_cast<smInt>(std::floor(mesh->triAABBs[i].aabbMax[2]/cellSizeZ));

        for (smInt ix = xStartIndex; ix <= xEndIndex; ix++)
            for (smInt iy = yStartIndex; iy <= yEndIndex; iy++)
                for (smInt iz = zStartIndex; iz <= zEndIndex; iz++)
                {
                    cells.insert(smCellTriangle(i), hasher->getKey(cells.tableSize, ix, iy, iz));
                }
    }
}

void smSpatialHashCollision::addTriangle(std::shared_ptr<smMesh> mesh, smInt triangleId, smHash<smCellTriangle> &cells)
{
    smCellTriangle  triangle;
    triangle.meshID = mesh->getUniqueId();
    triangle.primID = triangleId;

    triangle.vert[0] = mesh->vertices[mesh->triangles[triangleId].vert[0]];
    triangle.vert[1] = mesh->vertices[mesh->triangles[triangleId].vert[1]];
    triangle.vert[2] = mesh->vertices[mesh->triangles[triangleId].vert[2]];

    smInt xStartIndex = static_cast<smInt>(std::floor(mesh->triAABBs[triangleId].aabbMin[0]/cellSizeX));
    smInt yStartIndex = static_cast<smInt>(std::floor(mesh->triAABBs[triangleId].aabbMin[1]/cellSizeY));
    smInt zStartIndex = static_cast<smInt>(std::floor(mesh->triAABBs[triangleId].aabbMin[2]/cellSizeZ));

    smInt xEndIndex = static_cast<smInt>(std::floor(mesh->triAABBs[triangleId].aabbMax[0]/cellSizeX));
    smInt yEndIndex = static_cast<smInt>(std::floor(mesh->triAABBs[triangleId].aabbMax[1]/cellSizeY));
    smInt zEndIndex = static_cast<smInt>(std::floor(mesh->triAABBs[triangleId].aabbMax[2]/cellSizeZ));

    for (smInt ix = xStartIndex; ix <= xEndIndex; ix++)
        for (smInt iy = yStartIndex; iy <= yEndIndex; iy++)
            for (smInt iz = zStartIndex; iz <= zEndIndex; iz++)
            {
                cells.checkAndInsert(triangle, hasher->getKey(cells.tableSize, ix, iy, iz));
            }
}

void smSpatialHashCollision::addLine(std::shared_ptr<smLineMesh> mesh,
                                   smInt edgeId, smHash<smCellLine> &cells)
{
    smCellLine  line;
    line.meshID = mesh->getUniqueId();
    line.primID = edgeId;
    line.vert[0] = mesh->vertices[mesh->edges[edgeId].vert[0]];
    line.vert[1] = mesh->vertices[mesh->edges[edgeId].vert[1]];

    smInt xStartIndex = static_cast<smInt>(std::floor(mesh->edgeAABBs[edgeId].aabbMin[0]/cellSizeX));
    smInt yStartIndex = static_cast<smInt>(std::floor(mesh->edgeAABBs[edgeId].aabbMin[1]/cellSizeY));
    smInt zStartIndex = static_cast<smInt>(std::floor(mesh->edgeAABBs[edgeId].aabbMin[2]/cellSizeZ));

    smInt xEndIndex = static_cast<smInt>(std::floor(mesh->edgeAABBs[edgeId].aabbMax[0]/cellSizeX));
    smInt yEndIndex = static_cast<smInt>(std::floor(mesh->edgeAABBs[edgeId].aabbMax[1]/cellSizeY));
    smInt zEndIndex = static_cast<smInt>(std::floor(mesh->edgeAABBs[edgeId].aabbMax[2]/cellSizeZ));

    for (smInt ix = xStartIndex; ix <= xEndIndex; ix++)
        for (smInt iy = yStartIndex; iy <= yEndIndex; iy++)
            for (smInt iz = zStartIndex; iz <= zEndIndex; iz++)
            {
                cells.checkAndInsert(line, hasher->getKey(cells.tableSize, ix, iy, iz));
            }
}

void smSpatialHashCollision::addPoint(std::shared_ptr<smMesh> mesh, smInt vertId, smHash<smCellPoint> &cells)
{
    smCellPoint cellPoint;
    cellPoint.meshID = mesh->getUniqueId();
    cellPoint.primID = vertId;
    cellPoint.vert = mesh->vertices[vertId];

    smInt xStartIndex = static_cast<smInt>(std::floor(mesh->vertices[vertId][0]/cellSizeX));
    smInt yStartIndex = static_cast<smInt>(std::floor(mesh->vertices[vertId][1]/cellSizeY));
    smInt zStartIndex = static_cast<smInt>(std::floor(mesh->vertices[vertId][2]/cellSizeZ));

    cells.checkAndInsert(cellPoint, hasher->getKey(cells.tableSize, xStartIndex, yStartIndex, zStartIndex));
}

void smSpatialHashCollision::addOctreeCell(std::shared_ptr<smSpatialHashCollision::SurfaceTreeType> colModel, smHash<smCellModel> &cells)
{
    smCellModel cellModel;
    smAABB temp;

    smSurfaceTreeIterator<smOctreeCell> iter = colModel->getLevelIterator();
    cellModel.meshID = colModel->getAttachedMeshID();

    for (smInt i = iter.start(); i != iter.end(); ++i)
    {
        if (!iter[i].isEmpty())
        {
            temp.aabbMin =  iter[i].getCube().leftMinCorner();
            temp.aabbMax =  iter[i].getCube().rightMaxCorner();

            smInt xStartIndex = static_cast<smInt>(std::floor(temp.aabbMin[0]/cellSizeX));
            smInt yStartIndex = static_cast<smInt>(std::floor(temp.aabbMin[1]/cellSizeY));
            smInt zStartIndex = static_cast<smInt>(std::floor(temp.aabbMin[2]/cellSizeZ));

            smInt xEndIndex = static_cast<smInt>(std::floor(temp.aabbMin[0]/cellSizeX));
            smInt yEndIndex = static_cast<smInt>(std::floor(temp.aabbMin[1]/cellSizeY));
            smInt zEndIndex = static_cast<smInt>(std::floor(temp.aabbMin[2]/cellSizeZ));

            cellModel.primID = i;
            cellModel.center = iter[i].getCube().center;
            cellModel.radius = iter[i].getCube().getCircumscribedSphere().getRadius();

            for (smInt ix = xStartIndex; ix <= xEndIndex; ix++)
                for (smInt iy = yStartIndex; iy <= yEndIndex; iy++)
                    for (smInt iz = zStartIndex; iz <= zEndIndex; iz++)
                    {
                        cells.checkAndInsert(cellModel, hasher->getKey(cells.tableSize, ix, iy, iz));
                    }
        }
    }
}

void smSpatialHashCollision::reset()
{
    cells.clearAll();
    cellLines.clearAll();
    cellsForTri2Line.clearAll();
    cellsForModelPoints.clearAll();
    lineMeshes.clear();
    collidedLineTris.clear();
    collidedModelPoints.clear();
    collidedTriangles.clear();
}
bool smSpatialHashCollision::findCandidates()
{
    for(size_t i = 0; i < colModel.size(); i++)
        for(size_t i = 0; i < meshes.size(); i++)
        {
            findCandidatePoints(meshes[i], colModel[i]);
            addOctreeCell(colModel[i], cellsForModel);
        }

    ///Triangle-Triangle collision
    for(size_t i = 0; i < meshes.size(); i++)
    {
        for(size_t j = i + 1; j < meshes.size(); j++)
        {
            if(meshes[i]->collisionGroup.isCollisionPermitted(meshes[j]->collisionGroup))
            {
                if(findCandidateTris(meshes[i], meshes[j]) == false)
                {
                    continue;
                }
            }
        }
    }

    ///Triangle-line Collision
    for(size_t i = 0; i < meshes.size(); i++)
        for(size_t j = 0; j < lineMeshes.size(); j++)
        {
            if(meshes[i]->collisionGroup.isCollisionPermitted(lineMeshes[j]->collisionGroup))
            {
                if(findCandidateTrisLines(meshes[i], lineMeshes[j]) == false)
                {
                    continue;
                }
            }
        }
    return 0;
}
void smSpatialHashCollision::updateBVH()
{
    for(size_t i = 0; i < meshes.size(); i++)
    {
        meshes[i]->updateTriangleAABB();
    }

    for(size_t i = 0; i < lineMeshes.size(); i++)
    {
        meshes[i]->upadateAABB();
    }
}
const std::vector< std::shared_ptr< smCollidedTriangles > >& smSpatialHashCollision::getCollidedTriangles() const
{
    return collidedTriangles;
}
std::vector< std::shared_ptr< smCollidedTriangles > >& smSpatialHashCollision::getCollidedTriangles()
{
    return collidedTriangles;
}
