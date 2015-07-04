#include "MSVector2D.h"
#include <QtCore>

MSVector2D::MSVector2D()
{
	x() = y() = 0.0;
}

MSVector2D::MSVector2D(double _x, double _y)
{
	x() = _x;
	y() = _y;
}

MSVector2D::MSVector2D(const MSMatrix& matrix)
{
	Q_ASSERT(matrix.isVector() && matrix.getSize() == 2);
	x() = matrix(0);
	y() = matrix(1);
}

double& MSVector2D::x()
{
	return elementAt(0);
}

double MSVector2D::x() const
{
	return elementAt(0);
}

double& MSVector2D::y()
{
	return elementAt(1);
}

double MSVector2D::y() const
{
	return elementAt(1);
}

double MSVector2D::crossProduct(const MSVector2D& other)
{
	return x()*other.y() - y()*other.x();
}