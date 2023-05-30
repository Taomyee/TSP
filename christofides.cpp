#include "christofides.hpp"
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <chrono>
#include <stack>
#include <map>
#include <functional>
#include <unordered_set>
#include <climits>

using namespace std;

// find the idx of nearest neighbor that is not used
int minIndex(const vector<int> &key, const vector<int> &used, int num) {
    int ans = -1;
    int cur_min = INT_MAX;
    for (int i = 0; i < num; ++i) {
        // if not used and has smaller key
        if (used[i] == 0 && key[i] < cur_min) {
            // update
            ans = i;
            cur_min = key[i];
        }
    }
    return ans;
}

// generate the MST
// the return answer is a vector of size num, each one is the neighbors of that vertex
vector<vector<int> > MST(const vector<vector<int> > &distanceMatrix, int num) {
    vector<int> used(num, 0); 
    vector<int> parent(num, 0);
    vector<int> key(num, INT_MAX);
    vector<vector<int> > ans(num, vector<int>());

    // root of MST has distance of 0 and no parent
    parent[0] = -1;
    key[0] = 0;

    for (int i = 0; i < num - 1; ++i) {
        // find the idx of nearest neighbor that is not used
        int idx = minIndex(key, used, num);
        // set as used
        used[idx] = 1;
        for (int j = 0; j < num; ++j) {
            // if not the same vertex && not used && has smaller distance, then update
            if (j != idx && used[j] == 0 && distanceMatrix[idx][j] < key[j]) {
                parent[j] = idx;
                key[j] = distanceMatrix[idx][j];
            }
        }
    }

    // construct ans
    for (int i = 1; i < num; ++i) {
        int j = parent[i];
        ans[i].push_back(j);
        ans[j].push_back(i);
    }
    return ans;
}

// find vertices that have odd degree
vector<int> oddVertices(const vector<vector<int> > &neighbors, int num) {
    vector<int> ans;
    for (int i = 0; i < num; ++i) {
        if (neighbors[i].size() % 2 != 0) {
            ans.push_back(i);
        }
    }
    return ans;
}

// find a perfect matching use greedy algorithm
vector<pair<int, int> > perfectMatching(vector<int> &odd, const vector<vector<int> > &distanceMatrix) {
    // greedy algorithm is sensitive to the initial state, so a random shuffle is needed
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    shuffle(odd.begin(), odd.end(), default_random_engine(seed));

    int n = odd.size();
    if (n == 0) {
        return {};
    }

    vector<int> tour;
    vector<int> used(n, 0);
    vector<pair<int, int> > ans;
    tour.push_back(odd[0]);
    used[0] = 1;

    for (int i = 1; i < n; ++i) {
        int temp = -1;
        int idx = -1;
        for (int j = 0; j < n; ++j) {
            // not used && (first iteration || smaller distance)
            if (used[j] == 0 && (temp == -1 || distanceMatrix[odd[j]][tour[i - 1]] < distanceMatrix[temp][tour[i - 1]])) {
                temp = odd[j];
                idx = j;
            }
        }
        tour.push_back(temp);
        used[idx] = 1;
    }

    for (int i = 0; i < tour.size(); i += 2) {
        ans.push_back(make_pair(tour[i], tour[i + 1]));
    }
    return ans;
}

// connect MST graph with perfect matches
void multiGraph(vector<vector<int> > &neighbors, const vector<pair<int, int> > &match) {
    for (auto &i : match) {
        neighbors[i.first].push_back(i.second);
        neighbors[i.second].push_back(i.first);
    }
}

// get eulerian
vector<int> eulerian(const vector<vector<int> > &neighbors) {
    map<pair<int, int>, int> edgeNum;
    vector<int> ans;

    for (int i = 0; i < neighbors.size(); ++i) {
        for (int j = 0; j < neighbors[i].size(); ++j) {
            edgeNum[make_pair(i, neighbors[i][j])]++;
        }
    }
    
    // dfs search to find a path that will use all the edges
    function<void(int)> euler = [&edgeNum, &neighbors, &ans, &euler](int r) {
        for (int i = 0; i < neighbors[r].size(); ++i) {
            if (edgeNum[make_pair(r, neighbors[r][i])] != 0) {
                edgeNum[make_pair(r, neighbors[r][i])]--;
                edgeNum[make_pair(neighbors[r][i], r)]--;
                euler(neighbors[r][i]);
            }
        }
        ans.push_back(r);
    };

    euler(0);
    return ans;
}

// get hamiltonian and cost of this path
vector<int> hamiltonian(vector<int>& eulerPath, const vector<vector<int> > &distanceMatrix, int num) {
    vector<int> path;
    vector<int> used(num, 0);
    int v = eulerPath.back();
    path.push_back(v);
    used[v] = 1;
    eulerPath.pop_back();
    while (!eulerPath.empty()) {
        v = eulerPath.back();
        if (used[v] == 0) {
            path.push_back(v);
            used[v] = 1;
        }
        eulerPath.pop_back();
    }
    return path;
}

vector<int> christofides(const vector<vector<int> > &distanceMatrix, int num) {
    vector<vector<int> > neighbors = MST(distanceMatrix, num);
    vector<int> odd = oddVertices(neighbors, num);
    vector<pair<int, int> > match = perfectMatching(odd, distanceMatrix);
    multiGraph(neighbors, match);
    vector<int> e = eulerian(neighbors);
    vector<int> ans = hamiltonian(e, distanceMatrix, num);
    return ans;
}