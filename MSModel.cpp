#include "MSModel.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include "MSHeap.h"

bool solveEquation(MSMatrix<3, 4>& equation, MSVector3D& result)
{
	//将以（k,k）位置的元素为基准进行消去
	for (int k = 0; k < equation.getRows(); k++)
	{
		//尽量选择在该位置最大的行
		double maxValue = 0.0;
		int selectedRow = -1;
		for (int i = k; i < equation.getRows(); i++)
		{
			if (qAbs(equation(i, k)) > qAbs(maxValue))
			{
				maxValue = equation(i, k);
				selectedRow = i;
			}
		}
		if (selectedRow == -1) return false;

		//交换
		if (k != selectedRow)
			for (int j = 0; j < equation.getColumns(); j++)
			{
				qSwap(equation(k, j), equation(selectedRow, j));
			}

		//归一
		for (int j = k + 1; j < equation.getColumns(); j++)
		{
			equation(k, j) /= equation(k, k);
		}
		equation(k, k) = 1;

		//消元
		for (int i = 0; i < equation.getRows(); i++)
		{
			if (i == k) continue;
			double ratio = equation(i, k) / equation(k, k);
			for (int j = k; j < equation.getColumns(); j++)
			{
				equation(i, j) -= equation(k, j)*ratio;
			}
		}
	}
	for (int i = 0; i < 3; i++)
	{
		result(i) = equation(i, 3);
	}
	return true;
}

void MSModel::Triangle::updateGeometry()
{
	onePoint = edges[0]->vertices[0]->position;
	normal = MSVector3D(edges[0]->vertices[0]->position - edges[0]->vertices[1]->position).crossProduct
		(edges[1]->vertices[0]->position - edges[1]->vertices[1]->position);
}

double MSModel::Edge::evaluate()
{
	MSMatrix<3, 4> equation;
	equation.fill(0);
	QSet<MSModel::Triangle*> relatedTriangles;

	int limit = 10;
	for (int i = 0; i < 2; i++)
	{
		foreach(MSModel::Edge* edge, vertices[i]->referedByEdges) relatedTriangles.unite(edge->referedByTriangles);
	}
	foreach(Triangle* triangle, relatedTriangles)
	{
		triangle->normal.vectorNormalize();
		double term[4];
		term[0] = triangle->normal.x(); term[1] = triangle->normal.y(); term[2] = triangle->normal.z();
		term[3] = triangle->onePoint.dotProduct(triangle->normal);
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 4; j++) equation(i, j) += triangle->normal(i)*term[j];
		}
	}

	if (!solveEquation(equation, bestPoint))
	{
		double bestFailEpsilon = 1e100;
		MSVector3D failCandidates[3] = { vertices[0]->position, (vertices[0]->position + vertices[1]->position) / 2, vertices[1]->position };
		bestPoint = failCandidates[1];
		for (int i = 0; i < 3; i++)
		{
			double currentFailEpsilon = 0.0;
			foreach(Triangle* triangle, relatedTriangles)
			{
				triangle->normal.vectorNormalize();
				currentFailEpsilon += qPow((failCandidates[i] - triangle->onePoint).dotProduct(triangle->normal), 2);
			}
			if (currentFailEpsilon < bestFailEpsilon)
			{
				bestPoint = failCandidates[i];
				bestFailEpsilon = currentFailEpsilon;
			}
		}
		return bestFailEpsilon;
	}

	double epsilon = 0.0;
	foreach(Triangle* triangle, relatedTriangles)
	{
		triangle->normal.vectorNormalize();
		epsilon += qPow((bestPoint - triangle->onePoint).dotProduct(triangle->normal), 2);
	}
	return epsilon;
}

MSModel::Triangle::~Triangle()
{
	for (int i = 0; i < 3; i++)
	{
		edges[i]->referedByTriangles.remove(this);
	}
}

MSModel::Edge::~Edge()
{
	foreach(MSModel::Triangle* triangle, referedByTriangles) delete triangle;
	for (int i = 0; i < 2; i++) vertices[i]->referedByEdges.remove(this);
}

MSModel::Vertex::~Vertex()
{
	foreach(MSModel::Edge* edge, referedByEdges)
	{
		delete edge;
	}
}

MSModel::Vertex* MSModel::addVertex(const MSVector3D& coordinate)
{
	Vertex* vertex = new Vertex();
	vertex->position = coordinate;
	vertices.insert(vertex);
	return vertex;
}

MSModel::Vertex* MSModel::addVertex(double _x, double _y, double _z)
{
	return addVertex(MSVector3D(_x, _y, _z));
}

MSModel::Edge* MSModel::addEdge(Vertex* vertex1, Vertex* vertex2)
{
	foreach (Edge* edge, vertex1->referedByEdges)
	{
		if (edge->vertices[0] == vertex2 || edge->vertices[1] == vertex2)
			return edge;
	}
	Edge* edge = new Edge();
	edge->vertices[0] = vertex1;
	edge->vertices[1] = vertex2;
	vertex1->referedByEdges.insert(edge);
	vertex2->referedByEdges.insert(edge);
	edges.insert(edge);
	return edge;
}

void MSModel::updateEdge(Edge* edge)
{
	Q_ASSERT(edges.find(edge) != edges.end());
	heap->remove(edge->heapIndex);
	heap->insert(edge);
}

void MSModel::removeEdge(Edge* edge, bool inHeap)
{
	foreach(MSModel::Triangle* triangle, edge->referedByTriangles)
	{
		removeTriangle(triangle);
	}
	if(inHeap) heap->remove(edge->heapIndex);
	edges.remove(edge);
	delete edge;
}

MSModel::Triangle* MSModel::addTriangle(Vertex* vertex1, Vertex* vertex2, Vertex* vertex3)
{
	Triangle* triangle = new Triangle();
	triangle->edges[0] = addEdge(vertex1, vertex2);
	triangle->edges[1] = addEdge(vertex1, vertex3);
	triangle->edges[2] = addEdge(vertex2, vertex3);
	for (int i = 0; i < 3; i++) triangle->edges[i]->referedByTriangles.insert(triangle);
	triangles.insert(triangle);
	return triangle;
}

void MSModel::removeTriangle(Triangle* triangle)
{
	triangles.remove(triangle);
	delete triangle;
}

void MSModel::vertexReplace(Vertex* oldVertex, Vertex* newVertex)
{
	QSet<Edge*> toRemove;
	foreach(Edge* edge, oldVertex->referedByEdges)
	{
		if (edge->vertices[0] == oldVertex) edge->vertices[0] = newVertex;
		if (edge->vertices[1] == oldVertex) edge->vertices[1] = newVertex;
		if (edge->vertices[0] == edge->vertices[1]) toRemove.insert(edge);
		newVertex->referedByEdges.insert(edge);
	}
	oldVertex->referedByEdges.clear();
	delete oldVertex;
	vertices.remove(oldVertex);
	foreach(Edge* edge, toRemove) removeEdge(edge, true);
}

void MSModel::edgeCollapse(Edge* edge, const MSVector3D& newPosition)
{
	Vertex* vertex = addVertex(newPosition);
	Vertex* oldVertex1 = edge->vertices[0];
	Vertex* oldVertex2 = edge->vertices[1];
	QVector<Edge*> duplicateEdge;
	foreach(MSModel::Triangle* triangle, edge->referedByTriangles)
	{
		int flag = 0;
		for (int i = 0; i < 3; i++)
		{
			if (triangle->edges[i] != edge)
			{
				duplicateEdge.append(triangle->edges[i]);
				flag++;
			}
		}
		if (flag % 2 != 0) duplicateEdge.removeLast();
	}
	removeEdge(edge, false);
	vertexReplace(oldVertex1, vertex);
	if (oldVertex1!=oldVertex2)
		vertexReplace(oldVertex2, vertex);
	for (int i = 0; i < duplicateEdge.size(); i += 2)
	{
		if (edges.find(duplicateEdge[i])==edges.end() ||  edges.find(duplicateEdge[i + 1]) == edges.end())
		{
			continue;
		}
		foreach(MSModel::Triangle* triangle, duplicateEdge[i + 1]->referedByTriangles)
		{
			for (int j = 0; j < 3; j++)
			{
				if (triangle->edges[j] == duplicateEdge[i + 1])
				{
					triangle->edges[j] = duplicateEdge[i];
					duplicateEdge[i]->referedByTriangles.insert(triangle);
					duplicateEdge[i + 1]->referedByTriangles.remove(triangle);
				}
			}
		}
		removeEdge(duplicateEdge[i + 1], true);
	}
	QSet<Edge*> affectedEdges;
	QSet<Vertex*> affectedVertices;
	QSet<Triangle*> affectedTriangles;
	affectedEdges = vertex->referedByEdges;
	foreach(Edge* edge, affectedEdges)
	{
		affectedVertices.insert(edge->vertices[0]);
		affectedVertices.insert(edge->vertices[1]);
		affectedTriangles.unite(edge->referedByTriangles);
	}
	foreach(Vertex* vertex, affectedVertices) affectedEdges.unite(vertex->referedByEdges);
	foreach(Edge* edge, affectedEdges) updateEdge(edge);
	foreach(Triangle* triangle, affectedTriangles) triangle->updateGeometry();
}

void MSModel::simplify(int count)
{
	QTextStream consoleOutput(stdout);
	double epsilon = 0.0;
	for (int i = 0; i < count; i++)
	{
		Edge* edge = heap->getExtreme()->edge;
		epsilon += edge->evaluate();
		edgeCollapse(edge, edge->bestPoint);
		consoleOutput << "\r Simplifying...Remaining vertices:" << vertices.size() << "...           ";
	}
	consoleOutput << "\r Simplifying...Remaining vertices:" << vertices.size() << "...Done\n";
	consoleOutput << "\n";
	consoleOutput << "Total error = " << epsilon << "\n";
}

void MSModel::simplify(double ratio)
{
	simplify(qRound(ratio*vertices.size()));
}

bool MSModel::loadModelFromObjFile(const QString& filePath)
{
	QFileInfo info(filePath);
	modelPath = info.dir().path();
	QFile objModelFile(filePath);
	if(!objModelFile.open(QIODevice::ReadOnly))
	{
		return false;
	}
	QTextStream objModelStream(&objModelFile);
	QTextStream consoleOutput(stdout);
	QVector<Vertex*> tempVertexPositions;
	bool vertexLoadFlag = false;
	while(!objModelStream.atEnd())
	{
		//Read a line, spaces and begin and end are trimmed
		QString line = objModelStream.readLine().trimmed();

		//Ignore empty lines and comment lines
		if (line.isEmpty()) continue;
		if (line[0] == '#') continue;


		QStringList param = line.split(' ', QString::SkipEmptyParts);
		QString command = param[0];
		command.toLower();
		param.removeFirst();
		if (command == "v")
		{
			tempVertexPositions.append(
				addVertex(param[0].toDouble(), param[1].toDouble(), param[2].toDouble()));
			consoleOutput << "\rLoading vertices..." << tempVertexPositions.size() << " vertices loaded...";
		}
		else if (command == "f")
		{
			if (!vertexLoadFlag)
			{
				consoleOutput << "\rLoading vertices..." << tempVertexPositions.size() << " vertices loaded. Done.\n";
				vertexLoadFlag = true;
			}
			if (param.size() != 3)
			{
				consoleOutput << "Warning : only triangle mesh is supported!\n";
			}
			else
			{
				Vertex* v[3];
				for (int i = 0; i < param.size(); i++)
				{
					QStringList list = param[i].split("/");
					v[i] = tempVertexPositions[list[0].toInt() - 1];
				}
				addTriangle(v[0], v[1], v[2]);
			}
			consoleOutput << "\rLoading triangles..." << triangles.size() << " triangles loaded...";
		}
		else
		{
			qDebug() << "Ignoring Unsupported Line : " << command << param;
		}
	}
	objModelFile.close();
	consoleOutput << "\rLoading triangles..." << triangles.size() << " triangles loaded. Done.\n";

	//构建用于加速的Heap
	heap = new MSHeap(edges.size(), this);
	foreach(Triangle* triangle, triangles) triangle->updateGeometry();
	foreach (Edge* edge, edges) heap->insert(edge);
	return true;
}

bool MSModel::saveModelToObjFile(const QString& filePath)
{
	QTextStream consoleOutput(stdout);
	QFile objOutputFile(filePath);
	if (!objOutputFile.open(QIODevice::WriteOnly))
	{
		return false;
	}
	QMap<Vertex*, int> tempVertexPositions;
	QTextStream objOutputStream(&objOutputFile);
	int vertexOrder = 1;
	foreach(Vertex* vertex, vertices)
	{
		objOutputStream << "v " << vertex->position.x() << " " << vertex->position.y() << " " << vertex->position.z() << "\n";
		if (vertexOrder % 1000 == 0) consoleOutput << "\rWriting vertices..." << vertexOrder << "/" << vertices.size() << "...";
		tempVertexPositions.insert(vertex, vertexOrder);
		vertexOrder++;
	}
	int triangleOrder = 1;
	consoleOutput << "\rWriting vertices..." << vertices.size() << "/" << vertices.size() << "...Done.\n";
	foreach (Triangle* triangle, triangles)
	{
		Vertex* v[3];
		v[0] = triangle->edges[0]->vertices[0];
		v[1] = triangle->edges[0]->vertices[1];
		if (triangle->edges[1]->vertices[0] == triangle->edges[0]->vertices[0]
			|| triangle->edges[1]->vertices[0] == triangle->edges[0]->vertices[1])
		{
			v[2] = triangle->edges[1]->vertices[1];
		}
		else v[2] = triangle->edges[1]->vertices[0];
		objOutputStream << "f " << tempVertexPositions[v[0]] << " " << tempVertexPositions[v[1]] << " " << tempVertexPositions[v[2]] << "\n";
		if (triangleOrder % 1000 == 0) consoleOutput << "\rWriting triangles..." << triangleOrder << "/" << triangles.size() << "...";
		triangleOrder++;
	}
	consoleOutput << "\rWriting triangles..." << triangles.size() << "/" << triangles.size() << "...Done.\n";
	objOutputFile.close();
	return false;
}