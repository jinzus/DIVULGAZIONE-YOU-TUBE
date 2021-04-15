#include<iostream>
#include<vector>
#include<fstream>
#include<string>
using namespace std;

struct Result
{
	string cluster;
	float dissimilarity;
	Result(string cluster, float dissimilarity) :cluster(cluster), dissimilarity(dissimilarity) {}
	friend ostream& operator<<(ostream& o, const Result& result);
};
ostream& operator<<(ostream& o, const Result& result)
{
	o << "Cluster: " << result.cluster << " ; " << flush;
	o << "Dissimilarity: " << result.dissimilarity;
	return o;
}

class Flower 
{
	static int num_flower;
	string ID; 
	float SepalLength, SepalWidth, PetalLength, PetalWidth;
public:
	Flower () {}
	Flower(float SepalLength, float SepalWidth, float PetalLength, float PetalWidth);
	static int getNumFlowers() { return num_flower; }
	float getSepalLength() const { return SepalLength; }
	float getSepalWidth() const { return SepalWidth; }
	float getPetalLength() const { return PetalLength; }
	float getPetalWidth() const { return PetalWidth; };
	string getId() const { return ID; }
	friend ostream& operator<<(ostream& o,const Flower& flower);
};

class Cluster
{
    string ID;
public:
    Cluster(string ID) :ID(ID) {}
    string getId() const { return ID; }
};

Flower::Flower(float SepalLength, float SepalWidth, float PetalLength, float PetalWidth)
{
	this->SepalLength =SepalLength;
	this->SepalWidth =SepalWidth;
	this->PetalLength = PetalLength;
	this->PetalWidth =PetalWidth;
	ID= to_string(num_flower++);
}
int Flower::num_flower=0;

ostream& operator<<(ostream& o, const Flower& flower) 
{
	o << flower.ID<<": "<<flush ;
	o << flower.SepalLength << "; "<<flush;
	o << flower.SepalWidth << "; "<<flush;
	o << flower.PetalLength <<"; "<<flush;
	o << flower.PetalWidth;
	return o;
}

float getDistance(Flower& flower1,Flower& flower2) {
	float distance = pow(flower1.getPetalLength() - flower2.getPetalLength(), 2) + pow(flower1.getPetalWidth() - flower2.getPetalWidth(), 2) + pow(flower1.getSepalWidth() - flower2.getSepalWidth(), 2) + pow(flower1.getSepalLength() - flower2.getSepalLength(), 2);
	return pow(distance,.5);
}

int* getElements(int K, int M)
{
    int* cls = new int[2];
    int stop, i, j;
    for (stop = i = 0; i < M; i++)
    {
        for (j = i + 1; j < M; j++)
        {
            if (stop == K)
            {
                cls[0] = i;
                cls[1] = j;
            }
            stop++;
        }
    }
    return cls;
}

float* performLinkage(string* method,float* first, float* second, int size)
{
    float* linkedPtr = new float[size];
    if (*method == "single")
    {
        for (int i = 0; i < size; i++)
        {
            if (first[i] < second[i])
            {
                linkedPtr[i] = first[i];
            }
            else
            {
                linkedPtr[i] = second[i];
            }
        }
    }
    else if (*method == "complete")
    {
        for (int i = 0; i < size; i++)
        {
            if (first[i] > second[i])
            {
                linkedPtr[i] = first[i];
            }
            else
            {
                linkedPtr[i] = second[i];
            }
        }
    }
    else
    {
        for (int i = 0; i < size; i++)
        {

            linkedPtr[i] = (first[i] + second[i]) / 2;
        }
    }
    first = nullptr;
    second = nullptr;
    method = nullptr;
    return linkedPtr;
}

float* updateArray(string* method,float* original, int M, int cl1, int cl2)
{
    float* ptrFirst = new float[M - 2];
    float* ptrSecond = new float[M - 2];
    float* Linked;
    int l, k, o, i, j;
    for (o = k = l = i = 0; i < M; i++)
    {
        for (j = i + 1; j < M; j++)
        {
            if ((i == cl1 && j != cl2) || (j == cl1))
            {
                ptrFirst[k++] = original[o];
            }
            if ((i != cl1 && j == cl2) || (i == cl2))
            {
                ptrSecond[l++] = original[o];
            }
            o++;
        }
    }
    Linked = performLinkage(method,ptrFirst, ptrSecond, M - 2);
    delete[] ptrFirst;
    delete[] ptrSecond;
    ptrFirst = nullptr;
    ptrSecond = nullptr;
    original = nullptr;
    method = nullptr;
    return Linked;
}

float* fillArray(float* original, float* linkages, int M, int cl1, int cl2)
{
    float* linkArray = new float[(M - 1) * (M - 2) / 2];
    int i, n, j, k, l;
    for (int l = n = k = i = 0; i < M; i++)
    {
        for (int j = i + 1; j < M; j++)
        {
            if ((i != cl1 && j != cl2) && (i != cl2 && j != cl1))
            {
                linkArray[k++] = original[n];
            }
            if (i != cl1 && i != cl2 && j == M - 1) linkArray[k++] = linkages[l++];
            n++;
        }
    }
    linkArray[(M - 1) * (M - 2) / 2 - 1] = linkages[M - 3];
    original = nullptr;
    linkages = nullptr;
    return linkArray;
}

int getArgmin(float* iterable, int M)
{
    float min = iterable[0];
    int argmin = 0;
    for (int i = 0; i < M; i++)
    {
        if (iterable[i] < min)
        {
            min = iterable[i];
            argmin = i;
        }
    }
    iterable = nullptr;
    return argmin;
}

class HierarchicalClustering {
    vector<Flower> flowers;
	vector<Cluster> clusters;
	vector<Result> results;
    string link;
public:
    HierarchicalClustering(vector<Flower>& flowers,string link);
    void fit() {
        if (link == "average" || link == "single" || link == "complete")
        {
            int CL1, CL2, argmin;
            int N = Flower::getNumFlowers();
            int i, j, k;
            float* oldDissimilar = new float[N * (N - 1) / 2];
            for (int k = i = 0; i < N; i++)
            {
                for (int j = i + 1; j < N; j++)
                {
                    oldDissimilar[k] = getDistance(flowers[i], flowers[j]);
                    k++;
                }
            }
            do
            {
                argmin = getArgmin(oldDissimilar, N * (N - 1) / 2);
                int* CL = getElements(argmin, N);
                clusters.push_back(clusters[CL[0]].getId() + "-" + clusters[CL[1]].getId());
                results.push_back(Result(clusters[CL[0]].getId() + "-" + clusters[CL[1]].getId(), oldDissimilar[argmin]));
                clusters.erase(clusters.begin() + CL[1]);
                clusters.erase(clusters.begin() + CL[0]);
                float* Linkages = updateArray(&link, oldDissimilar, N, CL[0], CL[1]);
                float* newDissimilar = fillArray(oldDissimilar, Linkages, N, CL[0], CL[1]);
                delete[] CL;
                CL = nullptr;
                delete[] Linkages;
                Linkages = nullptr;
                for (int i = 0; i < (N - 1) * (N - 2) / 2; i++)
                {
                    oldDissimilar[i] = newDissimilar[i];
                }
                N--;
                if (N == 1)
                {
                    delete[] oldDissimilar;
                    delete[] newDissimilar;
                    newDissimilar = nullptr;
                    oldDissimilar = nullptr;
                }
            } while (N != 1);
        }
        else
        {
            cout << "Invalid linkage!";
            return;
        }
    }
    void printResult();
};

HierarchicalClustering::HierarchicalClustering(vector<Flower>& flowers, string link) :flowers(flowers), link(link)
{
    for (vector<Flower>::iterator itr = flowers.begin(); itr != flowers.end(); itr++)
    {
        clusters.push_back(itr->getId());
    }
}

void HierarchicalClustering::printResult()
{
    for (vector<Result>::iterator itr=results.begin(); itr != results.end();itr++) { cout << *itr<<endl; }
}

vector<Flower> text_parser(string fileName)
{
	typedef float N;
	vector<Flower> flowers;
	string str;
	N n1, n2, n3, n4;
	char comma;
	ifstream infile;
	infile.open(fileName);
	if (!infile) {
		cout << "File " << fileName << " not found" << endl;
		return flowers;
	}
	while (!infile.eof())
	{
		getline(infile, str, ',');
		infile >> n1;
		infile >> comma;
		infile >> n2;
		infile >> comma;
		infile >> n3;
		infile >> comma;
		infile >> n4;
		flowers.push_back(Flower(n1, n2, n3, n4));
	}
	return flowers;
}

int main()
{
    string str;
    cout << "Metodo di linkaggio: ";
    cin >> str;
    cout << endl;
	string filename= "IRIS.txt";
	vector<Flower> flowers = text_parser(filename);
	HierarchicalClustering HC(flowers,str);
    HC.fit();
    HC.printResult();
    cout << endl;
    /*for (auto& flower : flowers)
    {
        cout << flower<<endl;
    }*/
	return 0;
}