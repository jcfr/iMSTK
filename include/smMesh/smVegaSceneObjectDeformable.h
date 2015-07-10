/*
  A general deformable scene object.
  See also sceneObject.h .
*/

#ifndef _SIMMEDTK_VEGASCENEOBJECTDEFORMABLE_H_
#define _SIMMEDTK_VEGASCENEOBJECTDEFORMABLE_H_

#include "smMesh/smVegaSceneObjectWithRestPosition.h"

class smVegaSceneObjectDeformable : public virtual smVegaSceneObjectWithRestPosition
{
public:
  smVegaSceneObjectDeformable(char * filenameOBJ);
  virtual ~smVegaSceneObjectDeformable();

  // sets the current dynamic vertex positions to the rest position + specified deformation
  void SetVertexDeformations(double * u);
  void SetVertexDeformations(float * u);

  // adds deformations to current dynamicPosition of the vertices
  void AddVertexDeformations(double * u);

  void ResetDeformationToRest();

  inline void GetSingleVertexRestPosition(int vertex, double * x, double * y, double * z);
  inline void SetSingleVertexRestPosition(int vertex, double x, double y, double z);
  inline void GetSingleVertexPositionFromBuffer(int vertex, double * x, double * y, double * z);
};

inline void smVegaSceneObjectDeformable::GetSingleVertexRestPosition(int vertex, double * x, double * y, double * z)
{
  *x = restPosition[3*vertex+0];
  *y = restPosition[3 * vertex + 1];
  *z = restPosition[3 * vertex + 2];
}

inline void smVegaSceneObjectDeformable::SetSingleVertexRestPosition(int vertex, double x, double y, double z)
{
    restPosition[3 * vertex + 0] = x;
    restPosition[3 * vertex + 1] = y;
    restPosition[3 * vertex + 2] = z;
}

inline void smVegaSceneObjectDeformable::GetSingleVertexPositionFromBuffer(int vertex, double * x, double * y, double * z)
{
  Vec3d pos = mesh->getPosition(vertex);
  *x = pos[0];
  *y = pos[1];
  *z = pos[2];
}

#endif
