#ifndef MSMODEL_H
#define MSMODEL_H

#include <QVector>
#include <QString>
#include "MSMatrix.h"
#include "MSVector2D.h"
#include "MSVector3D.h"

class MSHeap;
class MSModel
{
public:
	struct Vertex;
	struct Edge;
	struct Triangle;
	struct Vertex
	{
		MSVector3D position;
		QSet<Edge*> referedByEdges;
		~Vertex();
	};
	struct Edge
	{
		int heapIndex;
		Vertex* vertices[2];
		QSet<Triangle*> referedByTriangles;
		MSVector3D bestPoint;
		double evaluate();
		~Edge();
	};
	struct Triangle
	{
		MSVector3D onePoint;
		MSVector3D normal;
		Edge* edges[3];
		~Triangle();
		void updateGeometry();
	};
public:
	QSet<Vertex*> vertices;
	QSet<Edge*> edges;
	QSet<Triangle*> triangles;
public:
	QString modelPath;
public:
	MSHeap* heap;
	bool loadModelFromObjFile(const QString& filePath);
	bool saveModelToObjFile(const QString& filePath);
	Vertex* addVertex(const MSVector3D& coordinate);
	Vertex* addVertex(double _x, double _y, double _z);
	Edge* addEdge(Vertex* vertex1, Vertex* vertex2);
	void removeEdge(Edge* edge, bool inHeap);
	Triangle* addTriangle(Vertex* vertex1, Vertex* vertex2, Vertex* vertex3);
	void removeTriangle(Triangle* triangle);
	void updateEdge(Edge* edge);
	void edgeCollapse(Edge* edge, const MSVector3D& newPosition);
	void vertexReplace(Vertex* oldVertex, Vertex* newVertex);
	void simplify(int count);
	void simplify(double ratio);
};

#endif // MSMODEL_H
