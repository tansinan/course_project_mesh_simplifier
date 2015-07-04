#ifndef MSVECTOR3D_H
#define MSVECTOR3D_H

#include "MSMatrix.h"

class QColor;

class MSVector3D : public MSMatrix<3,1>
{
public:
	MSVector3D();
	MSVector3D(double _x, double _y, double _z);
	MSVector3D(const MSMatrix<3,1>& matrix);
	double& x();
	double x()const;
	double& y();
	double y() const;
	double& z();
	double z() const;
	MSVector3D crossProduct(const MSVector3D &other) const;
};

#endif