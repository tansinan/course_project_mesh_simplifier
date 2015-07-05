#ifndef MSHEAP_H
#define MSHEAP_H

class MSHeap
{
	struct Node
	{
		double value;
		MSModel::Edge* edge;
	};

protected:
	Node** data;

	MSModel* model;

	int capacity;

	int size;
public:

	MSHeap(int _capacity, MSModel* _model)
	{
		capacity = _capacity;
		size = 0;
		data = new Node*[capacity];
		model = _model;
	}

	void swapData(int index1, int index2)
	{
		Q_ASSERT(index1 >= 0 && index2 >= 0);
		std::swap(data[index1], data[index2]);
		data[index1]->edge->heapIndex = index1;
		data[index2]->edge->heapIndex = index2;
	}

	int getParent(int node)
	{
		return (node - 1) / 2;
	}
	int getLeft(int node)
	{
		return node * 2 + 1;
	}
	int getRight(int node)
	{
		return node * 2 + 2;
	}

	void percolateUp(int index)
	{
		while (index != 0 && (data[index]->value < data[getParent(index)]->value))
		{
			swapData(index, getParent(index));
			index = getParent(index);
		}
	}

	void percolateDown(int index)
	{
		while (true)
		{
			int left = getLeft(index);
			int right = getRight(index);
			int maxChild;
			if (left >= size) return;
			if (right >= size) maxChild = left;
			else maxChild = ((data[left]->value < data[right]->value)) ? left : right;
			if ((data[index]->value > data[maxChild]->value))
			{
				swapData(index, maxChild);
				index = maxChild;
			}
			else return;
		}
	}

	bool insert(MSModel::Edge *edge)
	{
		if (size >= capacity) return false;
		size++;
		Node* node = new Node();
		node->edge = edge;
		node->value = edge->evaluate();
		edge->heapIndex = size - 1;
		data[size - 1] = node;
		percolateUp(size - 1);
		return true;
	}

	Node* getExtreme()
	{
		Node* ret = data[0];
		swapData(0, size - 1);
		size--;
		percolateDown(0);
		data[size] = NULL;
		return ret;
	}

	void remove(int index)
	{
		Node* ret = data[index];
		swapData(index, size - 1);
		size--;
		data[size] = NULL;
		if (index != size)
		{
			percolateDown(index);
			percolateUp(index);
		}
	}
};

#endif