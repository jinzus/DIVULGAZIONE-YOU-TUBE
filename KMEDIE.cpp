#include <iostream>
#include<fstream>
#include<string>
#include<vector>
#include<stdlib.h>
using namespace std;

int find_min(double* arr,const int N)
{
    int argmin = 0;
    double min = arr[0];
    for (int index = 0; index < N; index++)
    {
        if (arr[index] < min)
        {
            min = arr[index];
            argmin = index;
        }
    }
    return argmin;
}

double compute_inertia(const vector<vector<double>>& data,int* labels, vector<double>* centers, const int n_centers)
{
    const int n_features = data[0].size();
    double inertia = 0;
    for (int cl = 0; cl < n_centers; cl++)
    {
        for (int i = 0; i < data.size(); i++)
        {
            if (labels[i] == cl)
            {
                for (int j = 0; j < n_features; j++) inertia += pow(data[i][j] - centers[cl][j], 2);
            }
        }
    }
    labels = nullptr;
    return inertia;
}

vector<double>* update_centroids(const vector<vector<double>>& data, const int* ptr, const int n_centers)
{
    int count;
    const int n_features = data[0].size();
    vector<double>* newCentroids = new vector<double>[n_centers];
    double* sums = new double[n_features];
    for (int center = 0; center < n_centers; center++)
    {
        for (int zero = 0; zero < n_features; zero++) sums[zero] = 0;
        count = 0;
        for (int i = 0; i < data.size(); i++)
        {
            if (ptr[i] == center)
            {
                count++;
                for (int j = 0; j < n_features; j++) sums[j] += data[i][j];
            }
        }
        for (int k = 0; k < n_features; k++) sums[k] /= count;
        for (int item = 0; item < n_features; item++) newCentroids[center].push_back(sums[item]);
    }
    ptr = nullptr;
    delete[]sums;
    sums = nullptr;
    return newCentroids;
}

int* compute_distances(const vector<vector<double>>& data, vector<double>* centers, const int n_centers)
{
    int* labels = new int[data.size()];
    const int n_features = data[0].size();
    for (int i = 0; i < data.size(); i++)
    {
        double* distances = new double[n_centers];
        for (int k = 0; k < n_centers; k++)
        {
            distances[k] = 0;
            for (int j = 0; j < n_features; j++) distances[k] += pow(data[i][j] - centers[k][j], 2);
        }
        labels[i] = find_min(distances,n_centers);
        delete[]distances;
        distances = nullptr;
    }
    centers = nullptr;
    return labels;
}

class KMeans
{
    int n_clusters;
    int n_iterations{ 30 };
    int random_inits{ 10 };
    long int seed{ 10 };
    double* inertias = new double[random_inits];
    vector<double>* centroids = new vector<double>[n_clusters];
    int* cluster_labels; 
public:
    KMeans(int n_clusters);
    KMeans(int n_clusters, long int seed);
    KMeans(int n_clusters, long int seed, int n_iterations,int random_inits);
    void fit(const vector<vector<double>>& vec);
    int* getLabels() 
    {
        return cluster_labels;
    }
    vector<double>* getCenters()
    {
        return centroids;
    }
    vector<int> predict(const vector<vector<double>>& vec);
    ~KMeans();
};

KMeans::KMeans(int n_clusters) :n_clusters(n_clusters) {}

KMeans::KMeans(int n_clusters, long int seed) : n_clusters(n_clusters), seed(seed) {}

KMeans::KMeans(int n_clusters, long int seed,int n_iterations,int random_inits) : n_clusters(n_clusters), seed(seed),n_iterations(n_iterations),random_inits(random_inits) {}

void KMeans::fit(const vector<vector<double>>& vec)
{
    int count, n;
    for (int rInit = 0; rInit < random_inits; rInit++)
    {
        count = 0;
        while (count != n_clusters) {
            srand(count+ rInit);
            n = rand() % vec.size();
            centroids[count++] = vec[n];
        }
        for (int iter = 0; iter < n_iterations; iter++)
        {
            cluster_labels = compute_distances(vec, centroids, n_clusters);
            vector<double>* updatedCentroids = update_centroids(vec, cluster_labels, n_clusters);
            for (int i = 0; i < n_clusters; i++)
            {
                for (int j = 0; j < vec[0].size(); j++) centroids[i][j] = updatedCentroids[i][j];
            }
            if (iter == n_iterations - 1) inertias[rInit] = compute_inertia(vec,cluster_labels, centroids, n_clusters);
            delete[]updatedCentroids;
            updatedCentroids = nullptr;  
        }
    }
    int optimal_seed=find_min(inertias, random_inits);
    count = 0;
    while (count != n_clusters) {
        srand(count + optimal_seed);
        n = rand() % vec.size();
        centroids[count++] = vec[n];
    }
    for (int iter = 0; iter < n_iterations; iter++)
    {
        cluster_labels = compute_distances(vec, centroids, n_clusters);
        vector<double>* updatedCentroids = update_centroids(vec, cluster_labels, n_clusters);
        for (int i = 0; i < n_clusters; i++)
        {
            for (int j = 0; j < vec[0].size(); j++) centroids[i][j] = updatedCentroids[i][j];
        }
        delete[]updatedCentroids;
        updatedCentroids = nullptr;
    }
}

vector<int> KMeans::predict(const vector<vector<double>>& vec)
{
    vector<int> predictLabels(vec.size());
    const int n_features = vec[0].size();
    for (int i = 0; i < vec.size(); i++)
    {
        double* distances = new double[n_clusters];
        for (int cl = 0; cl < n_clusters; cl++)
        {
            distances[cl] = 0;
            for (int j = 0; j < n_features; j++) distances[cl] += pow(vec[i][j]-centroids[cl][j],2);
        }
        predictLabels[i]=find_min(distances,n_clusters);
        delete[]distances;
        distances = nullptr;
    }
    return predictLabels;
}

KMeans::~KMeans()
{
    delete[]cluster_labels;
    delete[]inertias;
    delete[]centroids;
    inertias = nullptr;
    centroids = nullptr;
    cluster_labels = nullptr;
}

vector<vector<double>> file_parser(string fileName)
{
    vector<vector<double>> data;
    vector<double> row;
    char w_s = ' ';
    string str, number;
    double numberD;
    ifstream infile;
    infile.open(fileName);
    if (!infile.is_open()) { cout << "File" << fileName << " not found"; }
    while (!infile.eof())
    {
        getline(infile, str, '\n');
        for (const auto& c : str)
        {
            if (c != w_s)
            {
                number += c;
            }
            else
            {
                numberD = stod(number);
                row.push_back(numberD);
                number = "";
            }
        }
        data.push_back(row);
        row.clear();
    }
    infile.close();
    return data;
}

void write_files(string &file1,string&file2,vector<int> labels,vector<double>* centers,const int n_cl,const int N)
{
    ofstream outfile1;
    outfile1.open(file1);
    for (int i = 0; i < N; i++)
    {
        outfile1 << labels[i]<<'\n';
    }
    outfile1.close();
    ofstream outfile2;
    outfile2.open(file2);
    for (int i = 0; i < n_cl; i++)
    {
        for (const auto value : centers[i])
        {
            outfile2 << value << '\n';
        }
    }
    cout << "Txt file delle train labels con " << n_cl << " clusters pronto!"<<'\n';
    cout << "Txt file con feature vectors dei " << n_cl << " clusters pronto!"<<'\n';
    outfile2.close();
    centers = nullptr;
}

void write_files(string& file, vector<int> labels,const int N)
{
    ofstream outfile;
    outfile.open(file);
    for (int i = 0; i < N; i++)
    {
        outfile << labels[i] << '\n';
    }
    cout << "Txt file delle test labels pronto!";
    outfile.close();
}

int main()
{
    vector<vector<double>> trainData,testData;
    string filenameInSample = "cerchiInSample.txt";
    string out1= "labelsInSample.txt",out2="centroids.txt";
    string filenameOutOfSample = "cerchiOutOfSample.txt";
    string out3 = "labelsOutOfSample.txt";
    trainData = file_parser(filenameInSample);
    testData = file_parser(filenameOutOfSample);
    int cl;
    cout << "Numero di clusters: "<<flush;
    cin >> cl;
    cout << endl;
    KMeans kmeans(cl);
    kmeans.fit(trainData);
    int* clusterLabels=kmeans.getLabels();
    vector<double>* centroids =kmeans.getCenters();
    vector<int> labsInSample=kmeans.predict(trainData), labsOutOfSample = kmeans.predict(testData);
    write_files(out1, out2, labsInSample, centroids, cl,trainData.size());
    write_files(out3, labsOutOfSample,testData.size());
    centroids = nullptr;
    clusterLabels = nullptr;
    return 0;
}