#include <QCoreApplication>
#include "MSModel.h"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	MSModel model;
	model.loadModelFromObjFile("D:\\Downloads\\models\\test_data\\fixed.perfect.dragon.100K.0.07.obj");
	model.simplify(0.99);
	model.saveModelToObjFile("D:\\dragon.obj");

	return a.exec();
}
