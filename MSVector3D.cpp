#include "MSVector3D.h"
#include <QtCore>

MSVector3D::MSVector3D()
{
	x() = y() = z() = 0.0;
}

MSVector3D::MSVector3D(double _x, double _y, double _z)
{
	x() = _x;
	y() = _y;
	z() = _z;
}

MSVector3D::MSVector3D(const MSMatrix<3, 1>& matrix)
{
	x() = matrix(0);
	y() = matrix(1);
	z() = matrix(2);
}

double& MSVector3D::x()
{
	return elementAt(0);
}

double MSVector3D::x() const
{
	return elementAt(0);
}

double& MSVector3D::y()
{
	return elementAt(1);
}

double MSVector3D::y() const
{
	return elementAt(1);
}

double& MSVector3D::z()
{
	return elementAt(2);
}

double MSVector3D::z() const
{
	return elementAt(2);
}

MSVector3D MSVector3D::crossProduct(const MSVector3D& other) const
{
	return MSVector3D(
		y()*other.z() - z()*other.y(), 
		z()*other.x() - x()*other.z(), 
		x()*other.y() - y()*other.x()
		);
}