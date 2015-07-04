#ifndef MSVECTOR2D_H
#define MSVECTOR2D_H

#include "MSMatrix.h"

class QColor;

class MSVector2D : public MSMatrix<2,1>
{
public:
	MSVector2D();
	MSVector2D(double _x, double _y);
	MSVector2D(const MSMatrix<2,1>& matrix);
	double& x();
	double x()const;
	double& y();
	double y() const;
	double crossProduct(const MSVector2D& other);
};

#endif