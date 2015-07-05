#include <QCoreApplication>
#include "MSModel.h"

int main(int argc, char *argv[])
{
	QTextStream consoleOutput(stdout);
	if (argc <= 3)
	{
		consoleOutput << "Too few arguments!" << endl;
		consoleOutput << "Usage: MeshSimplifier.exe <input> <output> <ratio>" << endl;
		return 1;
	}
	MSModel model;
	if (!model.loadModelFromObjFile(QString(argv[1])))
	{
		consoleOutput << "Fail to open input file!" << endl;
		return 1;
	}
	model.simplify(1 - QString(argv[3]).toDouble());
	model.saveModelToObjFile(QString(argv[2]));
	system("pause");
	return 0;
}
