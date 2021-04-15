#include <iostream>
#include<vector>
#include<string>
using namespace std;

struct Result
{
    string cluster;
    int dissimilarity;
    Result(string cluster, int dissimilarity):cluster(cluster),dissimilarity(dissimilarity) {}
    friend ostream& operator<<(ostream& o, const Result& result);
};
ostream& operator<<(ostream& o, const Result& result)
{
    o << "Cluster: "<<result.cluster<<" ; "<<flush;
    o << "Dissimilarity: " << result.dissimilarity;
    return o;
}

int* getElements(int K,int M) 
{
    int* cls = new int[2];
    int stop,i,j;
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

int* performLinkage(string* method, int* first, int* second, int size)
{
    int* linkedPtr = new int[size];
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
    else if (*method == "average")
    {
        for (int i = 0; i < size; i++)
        {

            linkedPtr[i] = (first[i] + second[i]) / 2;
        }
    }
    else { cout << "Linkage sbagliato"<<endl; }
    first = nullptr;
    second = nullptr;
    method = nullptr;
    return linkedPtr;
}

int* updateArray(int* original,int M,int cl1,int cl2)   
{
    string link = "single";
    int* ptrFirst = new int[M - 2];
    int* ptrSecond = new int[M - 2];
    int* Linked;
    int l, k,o, i, j;
    for (o=k=l=i=0; i < M; i++)
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
    Linked = performLinkage(&link,ptrFirst, ptrSecond, M - 2);
    delete[] ptrFirst;
    delete[] ptrSecond;
    ptrFirst = nullptr;
    ptrSecond = nullptr;
    original = nullptr;
    return Linked;
}

int* fillArray(int* original, int* linkages,int M,int cl1,int cl2)
{
    int* linkArray = new int[(M - 1) * (M - 2) / 2];
    int i, n, j, k,l;
    for (int l=n=k=i=0; i < M; i++)
    {
        for (int j = i + 1; j < M; j++)
        {
            if ((i != cl1 && j != cl2)&&(i != cl2 && j != cl1))
            {
                linkArray[k++] = original[n];
            }
            if (i != cl1 && i != cl2 && j == M-1) linkArray[k++] = linkages[l++];
            n++;
        }
    }
    linkArray[(M-1)*(M-2)/2-1] = linkages[M-3];
    original = nullptr;
    linkages = nullptr;
    return linkArray;
}

int getArgmin(int* iterable,int M)
{
    int min = iterable[0];
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

int main()
{
    vector<Result> results;
    vector<string> clusters;
    int CL1, CL2,argmin;
    int N = 5;
    // temporary initialization
    for (int i = 0; i < N; i++)
    {
        clusters.push_back(to_string(i));
    }
    int A[5][5]{ {0,638,426,31,896},{638,0,725,125,69},{426,725,0,226,733},
        {31,125,226,0,121},{896,69,733,121,0} };
    int i, j, k;
    int* oldDissimilar = new int[N * (N - 1) / 2];
    for (int k = i = 0; i < N; i++)
    {
        for (int j = i + 1; j < N; j++)
        {
            oldDissimilar[k] = A[i][j];
            k++;
        }
    }
    do
    {
        argmin = getArgmin(oldDissimilar, N * (N - 1) / 2);
        int* CL = getElements(argmin, N);
        clusters.push_back(clusters[CL[0]]+"-"+clusters[CL[1]]);
        results.push_back(Result(clusters[CL[0]] + "-" + clusters[CL[1]],oldDissimilar[argmin]));
        clusters.erase(clusters.begin()+CL[1]);
        clusters.erase(clusters.begin() + CL[0]);
        int* Linkages = updateArray(oldDissimilar, N, CL[0], CL[1]);
        int* newDissimilar = fillArray(oldDissimilar, Linkages, N, CL[0], CL[1]);
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
    } while (N!=1);
    for (auto& result : results) { cout << result << endl; }
    return 0;
}