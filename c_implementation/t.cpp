#include<iostream>

int main(){
    double* centroids = (double *)malloc(7 * 2 * sizeof(double));

        centroids = {-0.8297, 2.2032, -4.9989, -1.9767, -3.5324, -4.0766, 3.1374, -1.5444, -1.0323, 0.3882, -0.8081, 1.8522, -2.9555, 3.7811}; // Optimal centroids */


        std::cout << centroids[13] << std::endl;
    return 0;
}
