#include <cuda_runtime.h>
#include <iostream>
#include <vector>
#include "LlyodCentralisation.hpp"

__global__ void centralePointLLoyd(float* pointList, int* adjacencySize, int* pointAdjacencyList)
{
    int idx               = threadIdx.x * 2;
    int idxAdjacencyStart = adjacencySize[idx];
    int nbrOfAdjacencies  = adjacencySize[idx + 1];

    if (nbrOfAdjacencies <= 0)
    {
        // If there are no adjacencies, we can skip the computation
        return;
    }

    float moyenneX = 0.0f;
    float moyenneY = 0.0f;

    for (int i = 0; i < nbrOfAdjacencies; ++i)
    {
        int currentAdjacencyIndex = 2 * pointAdjacencyList[idxAdjacencyStart + i];
        moyenneX += pointList[currentAdjacencyIndex];
        moyenneY += pointList[currentAdjacencyIndex + 1];
    }

    if (nbrOfAdjacencies > 0)
    {
        moyenneX /= nbrOfAdjacencies;
        moyenneY /= nbrOfAdjacencies;
    }

    pointList[idx]     = moyenneX;
    pointList[idx + 1] = moyenneY;
}

template<typename T>
__host__ void print_matrix(T* matrix, int size)
{
    for (int i = 0; i < size; ++i)
    {
        std::cout << matrix[i] << " ";
    }
    std::cout << std::endl;
}

template<typename T>
T* vectorToCArray(const std::vector<T>& vec)
{
    T* arr = new T[vec.size()];
    std::copy(vec.begin(), vec.end(), arr);
    return arr;
}

template<typename T>
std::vector<T> cArrayToVector(const T* arr, size_t size)
{
    return std::vector<T>(arr, arr + size);
}

template<typename T, size_t N>
T* arrayToCArray(const std::array<T, N>& arr)
{
    T* cArr = new T[N];
    std::copy(arr.begin(), arr.end(), cArr);
    return cArr;
}

template<typename T, size_t N>
std::array<T, N> cArrayToArray(const T* cArr)
{
    std::array<T, N> arr;
    std::copy(cArr, cArr + N, arr.begin());
    return arr;
}

void Graphe::centralisation()
{
    std::cout << "Starting centralisation : ------------------------------------------------------ " << std::endl;

    if (pointList.size() == 0 || adjacencySize.size() == 0 || pointAdjacencyList.size() == 0)
    {
        std::cerr << "Error: One of the input vectors is empty." << std::endl;
        return;
    }

    if (pointList.size() != adjacencySize.size())
    {
        std::cerr << "Error: Wrong vector." << std::endl;
        std::cerr << "pointList.size() = " << pointList.size() << ", adjacencySize.size() = " << adjacencySize.size() << std::endl;
        return;
    }

    float* pointListCARRAY          = vectorToCArray<float>(pointList);
    int*   adjacencySizeCARRAY      = vectorToCArray<int>(adjacencySize);
    int*   pointAdjacencyListCARRAY = vectorToCArray<int>(pointAdjacencyList);

    // std::cout << "pointListCARRAY =\n";
    // print_matrix<float>(pointListCARRAY, pointList.size());
    // std::cout << "adjacencySizeCARRAY =\n";
    // print_matrix<int>(adjacencySizeCARRAY, adjacencySize.size());
    // std::cout << "pointAdjacencyListCARRAY =\n";
    // print_matrix<int>(pointAdjacencyListCARRAY, pointAdjacencyList.size());

    /* Setting up variables on device. i.e. GPU */
    float* pointListCUDA;
    int *  adjacencySizeCUDA, *pointAdjacencyListCUDA;
    cudaMalloc((void**)&pointListCUDA, pointList.size() * sizeof(float));
    cudaMalloc((void**)&adjacencySizeCUDA, adjacencySize.size() * sizeof(int));
    cudaMalloc((void**)&pointAdjacencyListCUDA, pointAdjacencyList.size() * sizeof(int));

    /* Copy data from host to device */
    cudaMemcpy(pointListCUDA, pointListCARRAY, pointList.size() * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(adjacencySizeCUDA, adjacencySizeCARRAY, adjacencySize.size() * sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(pointAdjacencyListCUDA, pointAdjacencyListCARRAY, pointAdjacencyList.size() * sizeof(int), cudaMemcpyHostToDevice);

    /*
    Kernel Launch
    Grid contains M*N blocks
    Each block has 1 thread
    Hence index of matrix element is
    blockIdx.x* gridSize.y + blockIdx.y
    */
    int  nbrThreads = pointList.size() / 2;                             // Assuming each block has 2 threads
    dim3 gridSize((pointList.size() + nbrThreads - 1) / nbrThreads, 1); // Assuming each block has 64 threads
    centralePointLLoyd<<<gridSize, nbrThreads>>>(pointListCUDA, adjacencySizeCUDA, pointAdjacencyListCUDA);
    cudaDeviceSynchronize();

    /* Copy result from GPU device to host */
    float* pointListResult = new float[pointList.size()];
    cudaMemcpy(pointListResult, pointListCUDA, pointList.size() * sizeof(float), cudaMemcpyDeviceToHost);

    /* Print result */
    // std::cout << "Resulting pointList after centralisation:\n";
    // print_matrix<float>(pointListResult, pointList.size());

    /* Convert result back to vector */
    pointList = cArrayToVector<float>(pointListResult, pointList.size());

    /* Cleanup device and host memory */
    cudaFree(pointListCUDA);
    cudaFree(adjacencySizeCUDA);
    cudaFree(pointAdjacencyListCUDA);
    delete[] pointListCARRAY;
    delete[] adjacencySizeCARRAY;
    delete[] pointAdjacencyListCARRAY;
    delete[] pointListResult;

    std::cout << "End of centralisation : ------------------------------------------------------ " << std::endl;
}

void Graphe::init_from_bad_format(const std::vector<Point>& points, const std::vector<Adjacency>& adjacencies)
{
    std::cout << "Initializing from bad format: ------------------------------------------------------ " << std::endl;

    pointList.clear();
    adjacencySize.clear();
    pointAdjacencyList.clear();

    for (const Point& point : points)
    {
        pointList.emplace_back(point.first);
        pointList.emplace_back(point.second);

    }

    std::vector<std::pair<int,int>> adjacenciesIdx(adjacencies.size());

    for (int k = 0; k < adjacencies.size(); k++)
    {
        Adjacency adjacency = adjacencies[k];

        for(int i = 0; i < pointList.size(); i += 2)
        {
            Point currentPoint = std::pair<float,float>(pointList[i], pointList[i+1]);


            if(pointEqual(currentPoint, adjacency.first))
            {
                adjacenciesIdx[k].first = i/2;
            }
        
        
            if(pointEqual(currentPoint, adjacency.second))
            {
                adjacenciesIdx[k].second = i/2;
            }
        }
    }

    std::vector<std::vector<int>> AdjacencyList(points.size());


    for (int k = 0; k < adjacenciesIdx.size(); k++)
    {

        if(std::find(AdjacencyList[adjacenciesIdx[k].first].begin(), AdjacencyList[adjacenciesIdx[k].first].end(), adjacenciesIdx[k].second) == AdjacencyList[adjacenciesIdx[k].first].end())
        {
            AdjacencyList[adjacenciesIdx[k].first].emplace_back(adjacenciesIdx[k].second);
        }
        if(std::find(AdjacencyList[adjacenciesIdx[k].second].begin(), AdjacencyList[adjacenciesIdx[k].second].end(), adjacenciesIdx[k].first) == AdjacencyList[adjacenciesIdx[k].second].end())
        {
            AdjacencyList[adjacenciesIdx[k].second].emplace_back(adjacenciesIdx[k].first);
        }
    }

    for (int k = 0; k < AdjacencyList.size(); k++)
    {
        std::vector<int> adjacency = AdjacencyList[k];

        if(k == 0)
            adjacencySize.emplace_back(0);

        adjacencySize.emplace_back(int(adjacency.size()));

        if(k != AdjacencyList.size() - 1)
            adjacencySize.emplace_back(adjacencySize.back() + adjacencySize[adjacencySize.size() - 1]);
        
        for (int idx : adjacency)
        {
            pointAdjacencyList.emplace_back(idx);
        }
    }

    std::cout << "Initialization complete." << std::endl;
}
