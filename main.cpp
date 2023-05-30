#include "christofides.hpp"
#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

#define DISTANCE(m, i, j, num) m[i][j]
#define MAX_INT 2147483647
#define DEADLINE CLOCKS_PER_SEC * 1.996


inline void reverse2(short path[], short start, short end, short num) {
    while (start < end) {
        short temp = path[start];
        path[start] = path[end];
        path[end] = temp;
        start++;
        end--;
    }
}

inline void shift_reverse(short path[], short num) {
    short temp = path[num-1];
    for (short i = num-1; i > 0; i--) {
        path[i] = path[i-1];
    }
    path[0] = temp;
}


inline int opt2(vector<vector<int> > &m, short path[], short N, short *skippable) {
    short start = *skippable;
    int improvement = 0;
    for (short i=start; i < N-1; i++) {
        int a = DISTANCE(m, path[i], path[i+1], N);
        
        for (int j=i+2; j < N-1; j++) {
            int b = DISTANCE(m, path[j], path[j+1], N);

            int c = DISTANCE(m, path[i], path[j], N);
            int d = DISTANCE(m, path[i+1], path[j+1], N);

            int original_dist = a + b;
            int new_dist = c + d;

            if (original_dist > new_dist) {
                reverse2(path, i+1, j, N);
                a = DISTANCE(m, path[i], path[i+1], N);
                improvement += original_dist - new_dist;
            }
        }
        if (improvement == 0) {
            *skippable = i;
        }
    }
    return improvement;
}

inline int path_distance(vector<vector<int> > &m, short path[], short N) {
    int distance = 0;
    for (short i=0; i < N-1; i++) {
        distance += DISTANCE(m, path[i], path[i+1], N);
}
    distance += DISTANCE(m, path[N-1], path[0], N);
    return distance;
}

inline int small_change(vector<vector<int> > &m, short path[], int path_dist, short N, float swapping_factor) {
    int path_dist_change = 0;
    short num_changes = int(N * swapping_factor);
    for (short i = 0; i < num_changes; ++i) {
        short random = (rand() % (N-3)) + 1;
        int old_edge_length = DISTANCE(m, path[random-1], path[random], N) + DISTANCE(m, path[random+1], path[random+2], N);
        int new_edge_length = DISTANCE(m, path[random-1], path[random+1], N) + DISTANCE(m, path[random], path[random+2], N);
        path_dist_change += new_edge_length - old_edge_length;
        short temp = path[random];
        path[random] = path[random+1];
        path[random+1] = temp;
    }
    return path_dist_change;
}

inline int update_if_better(short best_path[], int best_path_dist, short tourArray [], int path_dist, short num) {
    if (path_dist < best_path_dist) {
        best_path_dist = path_dist;
        for (short i = 0; i < num; ++i) {
            best_path[i] = tourArray[i];
        }
    }
    return best_path_dist;
}


int main(int argc, char *argv[]) {
    const clock_t begin_time = clock();
    srand (time(NULL));
    // read in the number of points
    short num;
    cin >> num;
    if (num <= 1) {
        cout << 0 << endl;
        return 0;
    }

    // create a vector to store the coordinates of the points
    vector<pair<double, double> > points(num);
    for (short i = 0; i < num; ++i) {
        double x, y;
        cin >> x >> y;
        pair<double, double> point = make_pair(x, y);
        points[i] = point;
    }

    // calculate distanceMatrix
    vector<vector<int> > distanceMatrix(num, vector<int>(num, 0));
    for (short i = 0; i < num; ++i) {
        for (short j = 0; j < i; ++j) {
            double square = pow(points[i].first - points[j].first, 2) + pow(points[i].second - points[j].second, 2);
            distanceMatrix[i][j] = distanceMatrix[j][i] = round(sqrt(square));
        }
    }

    // greedy algorithm
    vector<int> tour = christofides(distanceMatrix, num);
    

    // make tour into an array because idk how vectors work
    short tourArray[num];
    for (int i = 0; i < num; ++i) {
        tourArray[i] = tour[i];
    }
    short best_path[num];
    int best_path_dist = MAX_INT;

    int path_dist = path_distance(distanceMatrix, tourArray, num);
    short loop_count = 0;
    short skippable = 0;
    short reset_to_best_counter = 0;
    float swapping_factor = 0.5;
    while (float(clock()-begin_time) < DEADLINE) {
        if (loop_count > 50) {
            loop_count = 0;
            shift_reverse(tourArray, num);
        }

        int improvment = opt2(distanceMatrix, tourArray, num, &skippable);
        path_dist -= improvment;

        if (improvment == 0) { 
            best_path_dist = update_if_better(best_path, best_path_dist, tourArray, path_dist, num);
            if (best_path_dist != path_dist) {
                reset_to_best_counter++;
                if (reset_to_best_counter > 25) {
                    swapping_factor *= 1.01;
                    reset_to_best_counter = 0;
                    for (short i = 0; i < num; ++i) {
                        tourArray[i] = best_path[i];
                    }
                    path_dist = best_path_dist;
                }
            } else {
                reset_to_best_counter = 0;
                swapping_factor = 0.5;
            }

            int change = small_change(distanceMatrix, tourArray, path_dist, num, swapping_factor);
            path_dist += change;
            skippable = 0;
        }
        loop_count++;
    }

    // // // // print tour
    for (short i = 0; i < num; ++i) {
        cout << best_path[i] << endl;
    }
    cout << endl;
    return 0;
}
