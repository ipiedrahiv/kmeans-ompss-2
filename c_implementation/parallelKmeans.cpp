#include <ctime>

#include <fstream>

#include <iostream>

#include <sstream>

#include <vector>

#include <cmath>

#include <iomanip>

#include <chrono>

using namespace std;

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
    long double x, y; // Two dimentional points, in order to turn them into n-dimensional points we will later use vectors
    int cluster; // Which cluster is this point associated to
    long double minDist; // How far away is this point from its nearest cluster. Default to max double.

    Point():
        x(0.0),
        y(0.0),
        cluster(-1),
        minDist(__DBL_MAX__) {} // Default initialisation list

    Point(long double x, long double y):
        x(x),
        y(y),
        cluster(-1),
        minDist(__DBL_MAX__) {} // User value initialisation list

    long double distance(Point p) {
        return sqrt((p.x - x) * (p.x - x) + (p.y - y) * (p.y - y)); // Return euclidean distance from the current point vs another given point. When we torn Point into an n-dim Point, we will replace this with a loop.
    }

    long double shift(Point p) {
        return ((p.x - x) * (p.x - x) + (p.y - y) * (p.y - y));
    }

};

/* Reading data from the .csv file and using it to create a vector of Points. */

vector < Point > readFile(string filename) {
    vector < Point > points;
    string line;
    ifstream file(filename);

    while (getline(file, line)) {
        stringstream lineStream(line);
        string bit;
        long double x, y;
        getline(lineStream, bit, ',');
        x = stof(bit);
        getline(lineStream, bit, '\n');
        y = stof(bit);

        points.push_back(Point(x, y));
    }

    //for (int i =  0; i < points.size(); i++) {
    //cout << points[i].x << "," << points[i].y << endl;
    //}

    return points;
}

long double tolerance(vector < Point > * points, long double tol) {
    if (tol == 0) {
        return 0;
    } else { // Partiendo del principio de que no le vamos a pasar datos sparse
        int n = points -> size();
        long double sumX;
        long double sumY;

        for (int i = 0; i < n; i++) {
            sumX = points -> at(i).x;
            sumY = points -> at(i).y;
        }

        long double meanX = sumX / n;
        long double meanY = sumY / n;

        long double vX;
        long double vY;

        for (int i = 0; i < n; i++) {
            vX += ((points -> at(i).x - meanX) * (points -> at(i).x - meanX));
            vY += ((points -> at(i).y - meanY) * (points -> at(i).y - meanY));
        }

        long double varX = vX / (n - 1);
        long double varY = vX / (n - 1);

        long double meanVar = (varX + varY) / 2;

        return meanVar * tol;

    }
}

/*void writeFile() {
    fstream myfile;
    myfile.open("/gpfs/projects/bsc28/bsc28912/k-means/c_implementation/output.csv");
    myfile << "x,y,c" << endl;

    for (vector<Point>::iterator it = points->begin();
        it != points->end(); ++it) {
        myfile << it->x << "," << it->y << "," << it->cluster << endl;
        cout << it->x << "," << it->y << "," << it->cluster << endl;
    }
    myfile.close();

}*/

/* Randomly inicializated centroids to k different points loaded from the file. This function will probably not be used, since we need to ensure that these centroids are exactly the same as those used in the python version. */

vector < Point > randomInit(vector < Point > * points, int k) {
    vector < Point > centroids;
    srand(time(NULL));

    /*
	for (int i = 0; i < k; i++) {
		centroids.push_back(points->at(rand() % points->size())); // vector->at[n] is a syntactic shortcut for (*points)[i] which is dereferencing the vector and then accesing the position
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

    centroids.push_back(Point(-1.08756, 0.480849));
    centroids.push_back(Point(-3.12783, -1.43567));
    centroids.push_back(Point(-4.99813, -1.80588));
    centroids.push_back(Point(-3.5161, -4.12267));
    centroids.push_back(Point(-3.60242, -4.02816));
    centroids.push_back(Point(-0.912942, 0.309218));
    centroids.push_back(Point(-3.4735, -4.13939)); // Bad centroids */

    return centroids;

}

void kMeansClustering(vector < Point > * points, int k, size_t maxIter, long double tol) {

    vector < Point > centroids = randomInit(points, k);
    long double t = tolerance(points, tol);
    cout << "tolerancia " << t << endl;

    // START TIMER
    auto start = std::chrono::high_resolution_clock::now();

    for (size_t iter = 0; iter < maxIter; iter++) {

        // Assign each point to a centroid
        for (int i = 0; i < points -> size(); i++) {
            Point currentPoint = points -> at(i);
            for (int j = 0; j < k; j++) {
                long double dist = centroids[j].distance(currentPoint);
                if (dist < currentPoint.minDist) {
                    currentPoint.minDist = dist;
                    currentPoint.cluster = j;
                }
            }
            points -> at(i) = currentPoint;
        }

        // Recompute centroids
        vector < int > nPoints;
        vector < long double > sumX, sumY, centroidShift; // Keeps track of centroids that have already converged

        // Initialise with zeroes
        for (int i = 0; i < k; i++) {
            nPoints.push_back(0);
            sumX.push_back(0.0);
            sumY.push_back(0.0);
            centroidShift.push_back(0.0);
        }

        // Iterate over points to append to cada centroids. Keep track of number of points per centroid, sum of total x values per centroid and y values per centroid
        for (int i = 0; i < k; i++) {
            for (int j = 0; j < points -> size(); j++) {
                if (points -> at(j).cluster == i) {
                    nPoints[i] += 1;
                    sumX[i] += points -> at(j).x;
                    sumY[i] += points -> at(j).y;
                }
            }

        }

        //Point oldCentroid;
        //Point newCentroid;
        // Compute the new centroids
        for (int i = 0; i < centroids.size(); i++) {
            Point oldCentroid = centroids[i];

            Point newCentroid;
            newCentroid.x = sumX[i] / nPoints[i];
            newCentroid.y = sumY[i] / nPoints[i];

            centroids[i] = newCentroid;

            centroidShift[i] = oldCentroid.shift(newCentroid);
        } // */

        long double totalShift = 0.0;
        for (int i = 0; i < centroidShift.size(); i++) {
            totalShift += centroidShift[i];
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

    time << std::chrono::duration_cast < std::chrono::nanoseconds > (finish - start).count() << "ns\n";

    time.close();
    // */

    ofstream myfile;
    myfile.open("pointlabel.csv");
    for (int i = 0; i < points -> size(); i++) {
        myfile << points -> at(i).x << "," << points -> at(i).y << "," << points -> at(i).cluster << endl;
    }
    myfile.close();
    // */

    ofstream myfile1;
    myfile1.open("centroids.txt");
    myfile1 << "[" << endl;
    for (int i = 0; i < k; i++) {
        myfile1 << "[" << centroids[i].x << "," << centroids[i].y << "]," << endl;
    }
    myfile1 << "]" << endl;
    myfile1.close();
    // */

}

main() {

    vector < Point > source = readFile("/gpfs/projects/bsc28/bsc28912/k-means/c_implementation/uniform_small_2d_lowstddev/uniform_small_2d_lowstddev_samples.csv");
    kMeansClustering( & source, 7, 10000, 0.0001);
    return 0;

}


