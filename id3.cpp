//	Daniel Chang
//	CSCI 4350
//	11/15/2018
//	id3.cpp
//	This program takes 3 command line argument, 	integer: the number of real-valued features in the data set, 
//													string: input training data filename, 
//													string: input testing data filename 
//
//	Program then read in training data to build a decision tree then use the decision tree to test the tree using test data.

#include <vector>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <sstream>
#include <fstream>
#include <set>
#include <math.h>
using namespace std;

//Decision Node struct
struct Node
{
	bool terminal;
	int classification;
	int category;
	double value;
	Node* left;
	Node* right;
	
	Node() {}
	
	~Node()
	{
		delete left;
		delete right;
	}
};

int numClassification;	//number of Classification
int numCategory;		//number of Categories/Columns

// Released into Public Domain by Joshua L. Phillips 2016-10-27
// Attribute sorting
vector<vector<int> > sort_attributes(vector<vector<double> > data) {
	vector<vector<int> > indices;
	vector<double> *ptr;
	indices.resize(data.size());
	for (int x = 0; x < indices.size(); x++) 
	{
		indices[x].resize(data[x].size());
		iota(indices[x].begin(),indices[x].end(),0);
		ptr = &(data[x]);
		sort(indices[x].begin(),indices[x].end(),
		[&](size_t i, size_t j){ return (*ptr)[i] < (*ptr)[j]; });
	}
	return indices;
}

double calcInfo(int head, int last, int column, vector<vector<double> > data, vector<vector<int>> indices)
{
	double numClass, p, I;
	double info = 0;
	
	for(int i = 0; i < numClassification; i++)
	{
		numClass = 0;
		for(int j = head; j < last; j++){
			if (data[numCategory][indices[column][j]] == i)
			{
				numClass++;
			}
		}
		
		p = (double)numClass / (double)last;

		//Check for log2(0)
		if(numClass != 0)
		{
			I = -1*p*log2(p);
		}
		else
		{
			I = 0;
		}
		info += I;
	}
	return info;
}

//Gets the index on split category
int getSplit(int last, double info, vector<vector<int>> sorted, vector<vector<double>> data)
{
	double E, min = info;
	int splitCate, head = 0;

	for(int i = 0; i < numCategory; i++)
	{
		for(int j = head; j < last-1; j++)
		{
			if(data[i][sorted[i][j]] != data[i][sorted[i][j+1]])
			{
				E = ((double)(j+1)/(double)last) * calcInfo(head, j+1, i, data, sorted) + 
					((double)(last-j+1)/(double)last) * calcInfo(j+1, last, i, data, sorted);

				if(E < min)//Finding min E for max Gain
				{
					min = E;
					splitCate = i;
				}
			}
		}
	}
	return splitCate;
}

//Calc the value for split
double calcSplit(int last, double info, vector<vector<int>> sorted, vector<vector<double>> data)
{
	double splitAvg, E, min = info;
	int head = 0;

	for(int i = 0; i < numCategory; i++)
	{
		for(int j = head; j < last-1; j++)
		{
			if(data[i][sorted[i][j]] != data[i][sorted[i][j+1]])
			{
				E = ((double)(j+1)/(double)last) * calcInfo(head, j+1, i, data, sorted) + 
					((double)(last-j+1)/(double)last) * calcInfo(j+1, last, i, data, sorted);

				if(E < min)//Finding min E for max Gain
				{
					min = E;
					splitAvg = (data[i][sorted[i][j]] + data[i][sorted[i][j+1]]) / 2;	//attribute value changes
				}
			}
		}
	}
	return splitAvg;
}

//Build the decision tree
Node* build(vector<vector<double>> data)
{
	vector<vector<int>> sorted;
	int head = 0;
	int last = data[0].size();
	sorted = sort_attributes(data);

	double info = calcInfo(0, last, 0, data, sorted);

	//Terminal Node
	if(!info){
		Node *newNode = new Node;
		newNode->terminal = true;
		newNode->classification = data[numCategory][sorted[0][0]];
		newNode->category = -1;
		newNode->value = -1;
		newNode->left = NULL;
		newNode->right = NULL;
		return newNode;
	}
	
	//Making children nodes
	Node *newNode = new Node;
	newNode->terminal = false;
	newNode->classification = -1;
	newNode->category = getSplit(last, info, sorted, data);
	newNode->value = calcSplit(last, info, sorted, data);
	
	vector<vector<double>> leftData;
	vector<vector<double>> rightData;
	leftData.resize(numCategory+1);
	rightData.resize(numCategory+1);
	
	//Loading data for children nodes
	for(int i = 0; i < last; i++){
		for(int j = 0; j <  numCategory+1; j++)
		{
			if(data[newNode->category][sorted[newNode->category][i]] < newNode->value)
			{
				leftData[j].push_back(data[j][sorted[newNode->category][i]]);
			}
			else
			{
				rightData[j].push_back(data[j][sorted[newNode->category][i]]);
			}
		}
	}
	
	newNode->left = build(leftData);
	newNode->right = build(rightData);

	return newNode;
}

void loadData(string filePath, vector<vector<double> >& data, set<int>& classification) 
{
	fstream fs(filePath);
	string line;
	double value;
    
	getline(fs,line);
	stringstream ss(line);
  
	while (!ss.eof()) 
	{
		ss >> value;
		data.push_back(vector<double>());
	}
  
	while (!fs.eof()) 
	{
		stringstream ss(line);
		for (int i = 0; i < data.size(); i++) 
		{
			ss >> value;
			data[i].push_back(value);
			if(i == data.size()-1)
			{
				classification.insert(value);
			}
		}
	getline(fs,line);
	}

  fs.close();
}

void runTest(string filePath, Node* head) 
{
	double testData[numCategory];
	int testCase;
	int correct = 0;
	Node* curr;
	ifstream file;
	
	file.open(filePath);
	
	for(int i = 0; i < numCategory; i++)
	{
		file >> testData[i];
	}
	file >> testCase;
	
	while(!file.eof())
	{
		curr = head;
		bool done = false;
		while(!done)
		{
			//At terminal node and returned correct class
			if(curr->terminal && curr->classification == testCase)
			{
				correct++;
				done = true;
			}
			//Left then right
			else if(testData[curr->category] < curr->value && curr->left != NULL)
			{
				curr = curr->left;
			}
			else if(curr->right != NULL)
			{
				curr = curr->right;
			}
			else
			{
				done = true;
			}
		}
		for(int i = 0; i < numCategory; i++)
		{
			file >> testData[i];
		}
		file >> testCase;
	}
	
	file.close();
	cout << correct << endl;
}

int main(int argc, char* argv[]) 
{
	vector<vector<double>> data;
	ifstream test;
	set<int> classification;
	Node* head;
	
	numCategory = atoi(argv[1]);				//command line argument for number of column/category in set
	loadData(argv[2],data,classification);		//training data
	numClassification = classification.size();
	
	head = build(data);							//Build the tree
	
	runTest(argv[3], head);						//Test the tree
	
	
	return 0;
}
