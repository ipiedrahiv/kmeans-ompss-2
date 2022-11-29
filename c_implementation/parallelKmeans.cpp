#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <chrono>
#include <nanos6/debug.h>
#include "util.cpp"

using namespace std;

void kMeansClustering(Point* points, int N, int k, size_t maxIter, dtype tol) {

    Point* centroids = randomInit(points, N, k);
    dtype t = tolerance(points, N, tol);
    //cout << "tolerancia " << t << endl;

    
    // Recompute centroids
    int* nPoints = (int*) malloc(sizeof(int) * k);
    dtype* sumX = (dtype*) malloc(sizeof(dtype) * k);
    dtype* sumY = (dtype*) malloc(sizeof(dtype) * k);
    dtype* centroidShift = (dtype*) malloc(sizeof(dtype) * k);

    size_t ncpus = nanos6_get_total_num_cpus();
    cout << "NCPUS=" << ncpus<< endl;

    // START TIMER
    auto start = std::chrono::high_resolution_clock::now();

    for (size_t iter = 0; iter < maxIter; iter++) {

        // print centroids
       // cout << "Centroides: [";
       // for (int i = 0; i < k-1; i++) {
       //     cout << " [" << centroids[i].x << ", " << centroids[i].y << "]," << endl;
       // }
       // cout << " [" << centroids[k-1].x << ", " << centroids[k-1].y << "]]" << endl;

        // Set aux vectors to 0
        for (int j = 0; j < k; j++) {
            nPoints[j] = 0;
            sumX[j] = 0;
            sumY[j] = 0;
            centroidShift[j] = 0;
        }

        size_t BS = N/(ncpus/8);
        // Assign each point to a centroid
        #pragma oss taskloop grainsize(BS) inout(points[i;BS]) in(centroids[0;k])
        for (int i = 0; i < N; i++) {
            Point* currentPoint = &points[i];
            for (int j = 0; j < k; j++) {
                dtype dist = centroids[j].distance(currentPoint);
                if (dist < currentPoint->minDist) {
                    currentPoint->minDist = dist;
                    currentPoint->cluster = j;
                }
            }
        }
        
        #pragma oss taskwait

        // Iterate over points to append to cada centroids. Keep track of number of points per centroid, sum of total x values per centroid and y values per centroid
        //#pragma oss taskloop 
        for (int j = 0; j < k; j++) {
            for (int i = 0; i < N; i++) {
                if (points[i].cluster == j) {

                    nPoints[j] += 1;
                    sumX[j] += points[i].x;
                    sumY[j] += points[i].y;
                }
            }

        }


        // Compute the new centroids
        for (int j = 0; j < k; j++) {
            Point oldCentroid = centroids[j];

            centroids[j].x = sumX[j] / nPoints[j];
            centroids[j].y = sumY[j] / nPoints[j];

            centroidShift[j] = oldCentroid.shift(&centroids[j]);
        } // */

        dtype totalShift = 0.0;
        for (int j = 0; j < k; j++) {
            totalShift += centroidShift[j];
            //cout << "centroid shift for " << j << ": " << centroidShift[j] << endl;
        }
        //cout << std::setprecision(20) << totalShift << endl;

        if (iter > 0 && totalShift < t) {
            cout << std::setprecision(20) << std::fixed << "Converged at iteration " << iter << endl;
            cout << std::setprecision(20) << std::fixed << totalShift << " within tolerance " << t << endl;
            break;
        } // */
    }

    // END TIMER
    auto finish = std::chrono::high_resolution_clock::now();

    ofstream time;
    time.open("time.csv");

    time << std::chrono::duration<double,std::milli>(finish - start).count()/1000. << "s\n";
    cout << std::chrono::duration<double,std::milli>(finish - start).count()/1000. << "s\n";

    time.close();
    // */

    ofstream myfile;
    myfile.open("pointlabel.csv");
    for (int i = 0; i < N; i++) {
        myfile << points[i].x << "," << points[i].y << "," << points[i].cluster << endl;
    }
    myfile.close();
    // */

    ofstream myfile1;
    myfile1.open("centroids.txt");
    myfile1 << "[" << endl;
    for (int j = 0; j < k; j++) {
        myfile1 << "[" << centroids[j].x << "," << centroids[j].y << "]," << endl;
    }
    myfile1 << "]" << endl;
    myfile1.close();
    // */

}

int main() {

    Point* source = readFile("/gpfs/projects/bsc15/bsc15889/coopnov24/kmeans-ompss-2/c_implementation/uniform_small_2d_lowstddev_samples.csv", 1000000);
    kMeansClustering( source, 1000000, 7, 300, 0.0001);
    return 0;

}
