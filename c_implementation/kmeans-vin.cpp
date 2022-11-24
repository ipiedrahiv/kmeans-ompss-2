#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <chrono>

using namespace std;

typedef double dtype;

/* Using struct to represent datapoints. Structs are very much like classes, but all members are public by default.
A point will contain the information of its coordinates, its closest centroid and how far away it is from that centroid.
It will also implement a distance function, that will allow it to calculate the euclidean distance between itself and another given point.

Reference:

How to deine a new point in the origin
Point p1 = Point(0, 0);
cout << p1.x << endl;

How to define a point in another place and define the square distance
Point p2 = Point(3, 4);
cout << p1.distance(p2) << endl;

*/

struct Point {
    dtype x, y; // Two dimentional points, in order to turn them into n-dimensional points we will later use vectors
    int cluster; // Which cluster is this point associated to
    dtype minDist; // How far away is this point from its nearest cluster. Default to max double.

    Point():
        x(0.0),
        y(0.0),
        cluster(-1),
        minDist(__DBL_MAX__) {} // Default initialisation list

    Point(dtype x, dtype y):
        x(x),
        y(y),
        cluster(-1),
        minDist(__DBL_MAX__) {} // User value initialisation list

    dtype distance(Point *p) {
        return sqrt((p->x - x) * (p->x - x) + (p->y - y) * (p->y - y)); // Return euclidean distance from the current point vs another given point. When we torn Point into an n-dim Point, we will replace this with a loop.
    }

    dtype shift(Point *p) {
        return ((p->x - x) * (p->x - x) + (p->y - y) * (p->y - y));
    }

};

/* Reading data from the .csv file and using it to create a vector of Points. */

Point* readFile(string filename, size_t N) {
    Point* points = (Point* ) malloc(N*sizeof(Point));
    string line;
    ifstream file(filename);
    size_t pId = 0;
    while (getline(file, line)) {
        stringstream lineStream(line);
        string bit;
        dtype x, y;
        getline(lineStream, bit, ',');
        x = stof(bit);
        getline(lineStream, bit, '\n');
        y = stof(bit);

        points[pId] = Point(x, y);
        pId++;
    }

    //for (int i =  0; i < N; i++) {
    //    cout << points[i].x << "," << points[i].y << endl;
    //}

    return points;
}

dtype tolerance(Point* points, size_t N, dtype tol) {
    if (tol == 0) {
        return 0;
    } else { // Partiendo del principio de que no le vamos a pasar datos sparse
        dtype sumX = 0;
        dtype sumY = 0;

        for (int i = 0; i < N; i++) {
            sumX += points[i].x;
            sumY += points[i].y;
        }

        dtype meanX = sumX / N;
        dtype meanY = sumY / N;

        dtype vX = 0;
        dtype vY = 0;

        for (int i = 0; i < N; i++) {
            vX += ((points[i].x - meanX) * (points[i].x - meanX));
            vY += ((points[i].y - meanY) * (points[i].y - meanY));
        }

        dtype varX = vX / (N - 1);
        dtype varY = vY / (N - 1);

        dtype meanVar = (varX + varY) / 2;

        //return meanVar * tol;
        return 0.00043888131958223433;
    }
}

/* Randomly inicializated centroids to k different points loaded from the file. This function will probably not be used, since we need to ensure that these centroids are exactly the same as those used in the python version. */

Point* randomInit(Point* points, int N, int k) {
    Point* centroids = (Point *)malloc(k * sizeof(Point));
    srand(time(NULL));

    /*
	for (int i = 0; i < k; i++) {
		centroids[i] = points[rand() % N];
	} // Random centroids */

  /*
	for (int i = 0; i < k; i++) {
        cout << centroids[i].x << ", " << centroids[i].y << endl;
    } // Print randomly generated centroids */

  /*
    		centroids.push_back(Point(-0.8297, 2.2032));
        centroids.push_back(Point(-4.9989, -1.9767));
        centroids.push_back(Point(-3.5324, -4.0766));
        centroids.push_back(Point(-3.1374, -1.5444));
        centroids.push_back(Point(-1.0323, 0.3882));
    	centroids.push_back(Point(-0.8081, 1.8522));
    	centroids.push_back(Point(-2.9555, 3.7811)); // Optimal centroids */

    centroids[0] = Point(-1.08756, 0.480849);
    centroids[1] = Point(-3.12783, -1.43567);
    centroids[2] = Point(-4.99813, -1.80588);
    centroids[3] = Point(-3.5161, -4.12267);
    centroids[4] = Point(-3.60242, -4.02816);
    centroids[5] = Point(-0.912942, 0.309218);
    centroids[6] = Point(-3.4735, -4.13939); // Bad centroids */

    return centroids;

}

void kMeansClustering(Point* points, int N, int k, size_t maxIter, dtype tol) {

    Point* centroids = randomInit(points, N, k);
    dtype t = tolerance(points, N, tol);
    cout << "tolerancia " << t << endl;

    
    // Recompute centroids
    int* nPoints = (int*) malloc(sizeof(int) * k);
    dtype* sumX = (dtype*) malloc(sizeof(dtype) * k);
    dtype* sumY = (dtype*) malloc(sizeof(dtype) * k);
    dtype* centroidShift = (dtype*) malloc(sizeof(dtype) * k);

    // START TIMER
    auto start = std::chrono::high_resolution_clock::now();

    for (size_t iter = 0; iter < maxIter; iter++) {

        // Set aux vectors to 0
        for (int j = 0; j < k; j++) {
            nPoints[j] = 0;
            sumX[j] = 0;
            sumY[j] = 0;
            centroidShift[j] = 0;
        }

        // Assign each point to a centroid
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


        // Iterate over points to append to cada centroids. Keep track of number of points per centroid, sum of total x values per centroid and y values per centroid
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
        }
        cout << std::setprecision(20) << totalShift << endl;

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

    time << std::chrono::duration_cast < std::chrono::nanoseconds > (finish - start).count() << "ns\n";

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

    Point* source = readFile("/home/vincent/Descargas/Telegram Desktop/uniform_small_2d_lowstddev_samples.csv", 10000);
    kMeansClustering( source, 10000, 7, 300, 0.0001);
    return 0;

}
