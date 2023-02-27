#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <chrono>
#include "2dutils.cpp"

using namespace std;

typedef double dtype;

void kMeansClustering(double* points, int N, int k, size_t maxIter, double tol, int numFeatures) {

    double* centroids = randomInit(points, N, k, numFeatures);
    
    double t = tol;
    //double t = tolerance(points, N, tol);
    //cout << "tolerancia " << t << endl;

    
    // Recompute centroids
    int* nPoints = (int*) malloc(sizeof(int) * k);
    double* sum = (double*) malloc(sizeof(double) * k * numFeatures);
    double* centroidShift = (double*) malloc(sizeof(double) * k);
    int* label = (int*) malloc(sizeof(int)*N);
	double* oldCentroid = (double*) malloc(sizeof(double) * numFeatures);
	double* newCentroid = (double*) malloc(sizeof(double) * numFeatures);
	double* currentPoint = (double*) malloc(sizeof(double) * numFeatures);

    // START TIMER
    auto start = std::chrono::high_resolution_clock::now();

    for (size_t iter = 0; iter < maxIter; iter++) {

        // print centroids
        //cout << "Centroides: [";
        //for (int i = 0; i < k-1; i++) {
        //    cout << " [" << centroids[i].x << ", " << centroids[i].y << "]," << endl;
        //}
        //cout << " [" << centroids[k-1].x << ", " << centroids[k-1].y << "]]" << endl;

        // Set aux vectors to 0
        for (int centroid = 0; centroid < k; centroid++) {
            nPoints[centroid] = 0;
            centroidShift[centroid] = 0;
        }

        for (int centroid = 0; centroid < k; centroid++) {
            for (int feature = 0; feature < numFeatures; feature++)  {
                sum[centroid + feature] = 0;
            }
        }

        for (int point = 0; point < N; point++) {
            label[point] = -1;
        }

        // Assign each point to a centroid
        for (int point = 0; point < N; point++) {
            
			for (int feature = 0; feature < numFeatures; feature++) {
                currentPoint[feature] = points[point*numFeatures+feature];
            }

            double minDist = __DBL_MAX__;
            for (int centroid = 0; centroid < k; centroid++) {

                double* currentCentroid = &centroids[centroid*numFeatures];
                double dist = distance(currentPoint, currentCentroid, numFeatures);
				//cout << dist << endl;
                if (dist < minDist) {
                    minDist = dist;
                    label[point] = centroid;
                }
            }

            //nPoints[label[point]] += 1;

            for (int feature = 0; feature < numFeatures; feature++) {
                sum[label[point]*numFeatures+feature] += currentPoint[feature];
            }

        }


		for (int point = 0; point < N; point++) {
			nPoints[label[point]] += 1;
			cout << label[point] << endl;
		}

        for (int centroid = 0; centroid < k; centroid++) {
            //double* oldCentroid;

			for (int feature = 0; feature < numFeatures; feature++) {
				oldCentroid[feature] = centroids[centroid*numFeatures+feature];
			}

            for (int feature = 0; feature < numFeatures; feature++) {
                centroids[centroid*numFeatures+feature] = sum[centroid*numFeatures+feature]/nPoints[centroid];
				cout << nPoints[centroid] << endl;          
			}

            for (int feature = 0; feature < numFeatures; feature++) {
                newCentroid[feature] = centroids[centroid*numFeatures+feature];
            }

            double dist = distance(oldCentroid, newCentroid, numFeatures);

		//	cout << "Old centroid: "<< oldCentroid[0] << ", " << oldCentroid[1] << " New centroid: " <<  newCentroid[0] << ", " << newCentroid[1] << endl;

            centroidShift[centroid] = dist;

        }



        double totalShift = 0.0;
        for (int centroid = 0; centroid < k; centroid++) {
            totalShift += centroidShift[centroid];
            //cout << "centroid shift for " << j << ": " << centroidShift[j] << endl;
        }
        //cout << std::setprecision(20) << totalShift << endl;

        if ((iter > 0 && totalShift < t)||(iter==(maxIter-1))) {
            cout << std::setprecision(20) << std::fixed << "Converged at iteration " << iter << endl;
            cout << std::setprecision(20) << std::fixed << totalShift << " within tolerance " << t << endl;
            break;
        } //*/ 
    }

    
    // END TIMER
    auto finish = std::chrono::high_resolution_clock::now();

    ofstream time;
    time.open("time.txt");

        time << std::chrono::duration<double,std::milli>(finish - start).count()/1000. << "s\n";
        cout << std::chrono::duration<double,std::milli>(finish - start).count()/1000. << "s\n";


    time.close();
    // 

    ofstream myfile;
    myfile.open("pointlabel.csv");
    for (int i = 0; i < N; i++) {
        for (int j=0; j<numFeatures ; j++){
            myfile << points[i*numFeatures+j] << ",";
        }
        myfile << label[i] << endl;
    }
    myfile.close();
    // 
/*
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

    double* source = readFile("/home/bscuser/Workspace/kmeans-ompss-2/datasets/uniform_small_2d_lowstddev_samples.csv", 10000, 2);
    kMeansClustering( source, 10000, 7, 1, 0.00043888131958223433, 2);
    return 0;

}
