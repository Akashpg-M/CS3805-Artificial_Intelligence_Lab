#include <bits/stdc++.h>
using namespace std;
using namespace chrono;


void printPath(const vector<string> &path, double totalWeight) {
    if (path.empty()) {
        cout << "No path found\n";
        return;
    }
    for (size_t i = 0; i < path.size(); i++)
        cout << path[i] << (i + 1 == path.size() ? "\n" : " -> ");
    cout << "Total Path Weight: " << totalWeight << "\n";
}

void logStats(const string &algo, const vector<string> &path, double totalWeight, int openSize, const string &timeC, const string &spaceC, long long duration) {
    cout << "\n[" << algo << "]\n";
    printPath(path, totalWeight);
    cout << "Execution Time: " << duration << " microseconds\n";
    cout << "Time Complexity: " << timeC << "\n";
    cout << "Space Complexity: " << spaceC << "\n";
    cout << "Final Open List Size: " << openSize << "\n";
}



vector<string> bfs(unordered_map<string, vector<pair<string, double>>> &graph, string src, string dest, int &openSize, double &totalWeight) {
    queue<pair<vector<string>, double>> q;
    q.push({vector<string>{src}, 0.0});
    unordered_set<string> visited;
    visited.insert(src);

    while (!q.empty()) {
        openSize = max(openSize, (int)q.size());
        auto [path, currWeight] = q.front(); q.pop();
        string node = path.back();

        if (node == dest) {
            totalWeight = currWeight;
            return path;
        }

        for (const auto &[nbr, weight] : graph[node]) {
            if (!visited.count(nbr)) {
                visited.insert(nbr);
                vector<string> newPath = path;
                newPath.push_back(nbr);
                q.push({newPath, currWeight + weight});
            }
        }
    }
    totalWeight = 0.0;
    return vector<string>();
}

vector<string> dfs(unordered_map<string, vector<pair<string, double>>> &graph, string src, string dest, int &openSize, double &totalWeight) {
    unordered_set<string> visited;
    stack<pair<pair<string, vector<string>>, double>> st;
    st.push({{src, vector<string>{src}}, 0.0});
    visited.insert(src);

    while (!st.empty()) {
        openSize = max(openSize, (int)st.size());
        auto [top, currWeight] = st.top(); st.pop();
        string node = top.first;
        vector<string> path = top.second;

        if (node == dest) {
            totalWeight = currWeight;
            return path;
        }

        for (const auto &[nbr, weight] : graph[node]) {
            if (!visited.count(nbr)) {
                visited.insert(nbr);
                vector<string> newPath = path;
                newPath.push_back(nbr);
                st.push({{nbr, newPath}, currWeight + weight});
            }
        }
    }
    totalWeight = 0.0;
    return vector<string>();
}

vector<string> beam_search(unordered_map<string, vector<pair<string, double>>> &graph, string src, string dest, int beamWidth, int &openSize, double &totalWeight) {
    vector<pair<vector<string>, double>> q;
    q.push_back({vector<string>{src}, 0.0});
    unordered_set<string> visited;
    visited.insert(src);

    while (!q.empty()) {
        vector<pair<vector<string>, double>> candidates;
        openSize = max(openSize, (int)q.size());

        for (const auto &[path, currWeight] : q) {
            string node = path.back();

            if (node == dest) {
                totalWeight = currWeight;
                return path;
            }

            for (const auto &[nbr, weight] : graph[node]) {
                if (!visited.count(nbr)) {
                    visited.insert(nbr);
                    vector<string> newPath = path;
                    newPath.push_back(nbr);
                    candidates.push_back({newPath, currWeight + weight});
                }
            }
        }
        q.clear();

        sort(candidates.begin(), candidates.end(),
             [](const auto &a, const auto &b) {
                 return a.second < b.second; // Sort by path weight
             });

        for (int i = 0; i < min(beamWidth, (int)candidates.size()); i++)
            q.push_back(candidates[i]);
    }
    totalWeight = 0.0;
    return vector<string>();
}

vector<string> british_museum(unordered_map<string, vector<pair<string, double>>> &graph, string src, string dest, int &openSize, double &totalWeight) {
    vector<string> nodes;
    for (const auto &entry : graph)
        nodes.push_back(entry.first);

    srand((unsigned)time(0));
    int trials = 0, maxTrials = 100000;
    openSize = 0;

    while (trials < maxTrials) {
        trials++;
        vector<string> path(1, src);
        unordered_set<string> visited;
        visited.insert(src);
        string current = src;
        double currWeight = 0.0;

        while (!graph[current].empty() && visited.size() < nodes.size()) {
            openSize = max(openSize, (int)visited.size());
            const auto &adj = graph[current];
            int idx = rand() % adj.size();
            const auto &[nextNode, weight] = adj[idx];
            current = nextNode;
            if (visited.count(current)) break;
            visited.insert(current);
            path.push_back(current);
            currWeight += weight;

            if (current == dest) {
                totalWeight = currWeight;
                return path;
            }
        }
    }
    totalWeight = 0.0;
    return vector<string>();
}

vector<string> bidirectional_bfs(unordered_map<string, vector<pair<string, double>>> &graph, string src, string dest, int &openSize, double &totalWeight) {
    if (src == dest) {
        totalWeight = 0.0;
        return vector<string>(1, src);
    }

    unordered_map<string, string> parentF, parentB;
    unordered_map<string, double> weightF, weightB;
    unordered_set<string> visitedF, visitedB;
    queue<string> qF, qB;
    qF.push(src); qB.push(dest);
    visitedF.insert(src); visitedB.insert(dest);
    weightF[src] = 0.0; weightB[dest] = 0.0;

    while (!qF.empty() && !qB.empty()) {
        openSize = max(openSize, (int)(qF.size() + qB.size()));

        int szF = (int)qF.size();
        while (szF--) {
            string node = qF.front(); qF.pop();
            for (const auto &[nbr, weight] : graph[node]) {
                if (!visitedF.count(nbr)) {
                    visitedF.insert(nbr);
                    parentF[nbr] = node;
                    weightF[nbr] = weightF[node] + weight;
                    qF.push(nbr);
                    if (visitedB.count(nbr)) {
                        vector<string> path;
                        string x = nbr;
                        while (x != src) { path.push_back(x); x = parentF[x]; }
                        path.push_back(src);
                        reverse(path.begin(), path.end());
                        x = nbr;
                        while (x != dest) { x = parentB[x]; path.push_back(x); }
                        totalWeight = weightF[nbr] + weightB[nbr];
                        return path;
                    }
                }
            }
        }

        int szB = (int)qB.size();
        while (szB--) {
            string node = qB.front(); qB.pop();
            for (const auto &[nbr, weight] : graph[node]) {
                if (!visitedB.count(nbr)) {
                    visitedB.insert(nbr);
                    parentB[nbr] = node;
                    weightB[nbr] = weightB[node] + weight;
                    qB.push(nbr);
                    if (visitedF.count(nbr)) {
                        vector<string> path;
                        string x = nbr;
                        while (x != src) { path.push_back(x); x = parentF[x]; }
                        path.push_back(src);
                        reverse(path.begin(), path.end());
                        x = nbr;
                        while (x != dest) { x = parentB[x]; path.push_back(x); }
                        totalWeight = weightF[nbr] + weightB[nbr];
                        return path;
                    }
                }
            }
        }
    }
    totalWeight = 0.0;
    return vector<string>();
}

vector<string> bidirectional_dfs(unordered_map<string, vector<pair<string, double>>> &graph, string src, string dest, int &openSize, double &totalWeight) {
    unordered_set<string> visitedF, visitedB;
    unordered_map<string, string> parentF, parentB;
    unordered_map<string, double> weightF, weightB;
    stack<string> stF, stB;
    stF.push(src); stB.push(dest);
    visitedF.insert(src); visitedB.insert(dest);
    weightF[src] = 0.0; weightB[dest] = 0.0;

    while (!stF.empty() && !stB.empty()) {
        openSize = max(openSize, (int)(stF.size() + stB.size()));

        string nodeF = stF.top(); stF.pop();
        for (const auto &[nbr, weight] : graph[nodeF]) {
            if (!visitedF.count(nbr)) {
                visitedF.insert(nbr);
                parentF[nbr] = nodeF;
                weightF[nbr] = weightF[nodeF] + weight;
                stF.push(nbr);
                if (visitedB.count(nbr)) {
                    vector<string> path;
                    string x = nbr;
                    while (x != src) { path.push_back(x); x = parentF[x]; }
                    path.push_back(src);
                    reverse(path.begin(), path.end());
                    x = nbr;
                    while (x != dest) { x = parentB[x]; path.push_back(x); }
                    totalWeight = weightF[nbr] + weightB[nbr];
                    return path;
                }
            }
        }

        string nodeB = stB.top(); stB.pop();
        for (const auto &[nbr, weight] : graph[nodeB]) {
            if (!visitedB.count(nbr)) {
                visitedB.insert(nbr);
                parentB[nbr] = nodeB;
                weightB[nbr] = weightB[nodeB] + weight;
                stB.push(nbr);
                if (visitedF.count(nbr)) {
                    vector<string> path;
                    string x = nbr;
                    while (x != src) { path.push_back(x); x = parentF[x]; }
                    path.push_back(src);
                    reverse(path.begin(), path.end());
                    x = nbr;
                    while (x != dest) { x = parentB[x]; path.push_back(x); }
                    totalWeight = weightF[nbr] + weightB[nbr];
                    return path;
                }
            }
        }
    }
    totalWeight = 0.0;
    return vector<string>();
}

vector<string> bidirectional_beam(unordered_map<string, vector<pair<string, double>>> &graph, string src, string dest, int beamWidth, int &openSize, double &totalWeight) {
    if (src == dest) {
        totalWeight = 0.0;
        return vector<string>(1, src);
    }

    unordered_map<string, string> parentF, parentB;
    unordered_map<string, double> weightF, weightB;
    unordered_set<string> visitedF, visitedB;
    vector<pair<string, double>> qF, qB;
    qF.push_back({src, 0.0}); qB.push_back({dest, 0.0});
    visitedF.insert(src); visitedB.insert(dest);
    weightF[src] = 0.0; weightB[dest] = 0.0;

    while (!qF.empty() && !qB.empty()) {
        openSize = max(openSize, (int)(qF.size() + qB.size()));
        vector<pair<string, double>> candF, candB;

        for (const auto &[node, currWeight] : qF) {
            for (const auto &[nbr, weight] : graph[node]) {
                if (!visitedF.count(nbr)) {
                    visitedF.insert(nbr);
                    parentF[nbr] = node;
                    weightF[nbr] = currWeight + weight;
                    candF.push_back({nbr, weightF[nbr]});
                    if (visitedB.count(nbr)) {
                        vector<string> path;
                        string x = nbr;
                        while (x != src) { path.push_back(x); x = parentF[x]; }
                        path.push_back(src);
                        reverse(path.begin(), path.end());
                        x = nbr;
                        while (x != dest) { x = parentB[x]; path.push_back(x); }
                        totalWeight = weightF[nbr] + weightB[nbr];
                        return path;
                    }
                }
            }
        }
        qF.clear();
        sort(candF.begin(), candF.end(), [](const auto &a, const auto &b) {
            return a.second < b.second;
        });
        for (int i = 0; i < min(beamWidth, (int)candF.size()); i++)
            qF.push_back(candF[i]);

        for (const auto &[node, currWeight] : qB) {
            for (const auto &[nbr, weight] : graph[node]) {
                if (!visitedB.count(nbr)) {
                    visitedB.insert(nbr);
                    parentB[nbr] = node;
                    weightB[nbr] = currWeight + weight;
                    candB.push_back({nbr, weightB[nbr]});
                    if (visitedF.count(nbr)) {
                        vector<string> path;
                        string x = nbr;
                        while (x != src) { path.push_back(x); x = parentF[x]; }
                        path.push_back(src);
                        reverse(path.begin(), path.end());
                        x = nbr;
                        while (x != dest) { x = parentB[x]; path.push_back(x); }
                        totalWeight = weightF[nbr] + weightB[nbr];
                        return path;
                    }
                }
            }
        }
        qB.clear();
        sort(candB.begin(), candB.end(), [](const auto &a, const auto &b) {
            return a.second < b.second;
        });
        for (int i = 0; i < min(beamWidth, (int)candB.size()); i++)
            qB.push_back(candB[i]);
    }
    totalWeight = 0.0;
    return vector<string>();
}

vector<string> bidirectional_british(unordered_map<string, vector<pair<string, double>>> &graph, string src, string dest, int &openSize, double &totalWeight) {
    vector<string> nodes;
    for (const auto &entry : graph)
        nodes.push_back(entry.first);

    srand((unsigned)time(0));
    int trials = 0, maxTrials = 100000;
    openSize = 0;

    while (trials < maxTrials) {
        trials++;
        vector<string> pathF(1, src), pathB(1, dest);
        unordered_set<string> visitedF, visitedB;
        visitedF.insert(src); visitedB.insert(dest);
        string curF = src, curB = dest;
        double weightF = 0.0, weightB = 0.0;

        while (!graph[curF].empty() && visitedF.size() < nodes.size()) {
            openSize = max(openSize, (int)(visitedF.size() + visitedB.size()));
            const auto &adjF = graph[curF];
            int idx = rand() % adjF.size();
            const auto &[nextF, wF] = adjF[idx];
            curF = nextF;
            if (visitedF.count(curF)) break;
            visitedF.insert(curF);
            pathF.push_back(curF);
            weightF += wF;
            if (visitedB.count(curF)) {
                reverse(pathB.begin(), pathB.end());
                pathF.insert(pathF.end(), pathB.begin() + 1, pathB.end());
                totalWeight = weightF + weightB;
                return pathF;
            }
        }

        while (!graph[curB].empty() && visitedB.size() < nodes.size()) {
            openSize = max(openSize, (int)(visitedF.size() + visitedB.size()));
            const auto &adjB = graph[curB];
            int idx = rand() % adjB.size();
            const auto &[nextB, wB] = adjB[idx];
            curB = nextB;
            if (visitedB.count(curB)) break;
            visitedB.insert(curB);
            pathB.push_back(curB);
            weightB += wB;
            if (visitedF.count(curB)) {
                reverse(pathB.begin(), pathB.end());
                pathF.insert(pathF.end(), pathB.begin() + 1, pathB.end());
                totalWeight = weightF + weightB;
                return pathF;
            }
        }
    }
    totalWeight = 0.0;
    return vector<string>();
}



int main() {
    unordered_map<string, vector<pair<string, double>>> graph;
    int edges;
    cout << "Enter number of edges: ";
    cin >> edges;
    if (edges < 0) {
        cout << "Invalid number of edges\n";
        return 1;
    }

    cout << "Enter edges (u v weight):\n";
    for (int i = 0; i < edges; i++) {
        string u, v;
        double w;
        cin >> u >> v >> w;
        if (w < 0) {
            cout << "Negative weights are not allowed\n";
            return 1;
        }
        graph[u].push_back({v, w});
        graph[v].push_back({u, w}); // Undirected graph
        graph[v]; // Ensure node exists
        graph[u]; // Ensure node exists
    }

    string src, dest;
    cout << "Enter source and destination: ";
    cin >> src >> dest;
    if (graph.find(src) == graph.end() || graph.find(dest) == graph.end()) {
        cout << "Source or destination node does not exist\n";
        return 1;
    }

    vector<string> algoNames = {
        "Forward BFS", "Backward BFS", "Bidirectional BFS",
        "Forward DFS", "Backward DFS", "Bidirectional DFS",
        "Forward Beam Search", "Backward Beam Search", "Bidirectional Beam Search",
        "Forward British Museum Search", "Backward British Museum Search", "Bidirectional British Museum Search"
    };
    vector<long long> executionTimes(12, 0);
    vector<double> pathWeights(12, 0.0);
    int beamWidth = 2;

    // Complexity strings
    const string BFS_T = "O(V+E)", BFS_S = "O(V)";
    const string DFS_T = "O(V+E)", DFS_S = "O(V)";
    const string BEAM_T = "O(b^d) approx", BEAM_S = "O(bd)";
    const string BM_T = "O(∞) worst", BM_S = "O(1)";

    // 0) Forward BFS
    {
        int openSize = 0;
        double totalWeight = 0.0;
        auto start = high_resolution_clock::now();
        vector<string> path = bfs(graph, src, dest, openSize, totalWeight);
        auto stop = high_resolution_clock::now();
        long long dur = duration_cast<microseconds>(stop - start).count();
        executionTimes[0] = dur;
        pathWeights[0] = totalWeight;
        logStats(algoNames[0], path, totalWeight, openSize, BFS_T, BFS_S, dur);
    }

    // 1) Backward BFS
    {
        int openSize = 0;
        double totalWeight = 0.0;
        auto start = high_resolution_clock::now();
        vector<string> path = bfs(graph, dest, src, openSize, totalWeight);
        auto stop = high_resolution_clock::now();
        if (!path.empty()) reverse(path.begin(), path.end());
        long long dur = duration_cast<microseconds>(stop - start).count();
        executionTimes[1] = dur;
        pathWeights[1] = totalWeight;
        logStats(algoNames[1], path, totalWeight, openSize, BFS_T, BFS_S, dur);
    }

    // 2) Bidirectional BFS
    {
        int openSize = 0;
        double totalWeight = 0.0;
        auto start = high_resolution_clock::now();
        vector<string> path = bidirectional_bfs(graph, src, dest, openSize, totalWeight);
        auto stop = high_resolution_clock::now();
        long long dur = duration_cast<microseconds>(stop - start).count();
        executionTimes[2] = dur;
        pathWeights[2] = totalWeight;
        logStats(algoNames[2], path, totalWeight, openSize, BFS_T, BFS_S, dur);
    }

    // 3) Forward DFS
    {
        int openSize = 0;
        double totalWeight = 0.0;
        auto start = high_resolution_clock::now();
        vector<string> path = dfs(graph, src, dest, openSize, totalWeight);
        auto stop = high_resolution_clock::now();
        long long dur = duration_cast<microseconds>(stop - start).count();
        executionTimes[3] = dur;
        pathWeights[3] = totalWeight;
        logStats(algoNames[3], path, totalWeight, openSize, DFS_T, DFS_S, dur);
    }

    // 4) Backward DFS
    {
        int openSize = 0;
        double totalWeight = 0.0;
        auto start = high_resolution_clock::now();
        vector<string> path = dfs(graph, dest, src, openSize, totalWeight);
        auto stop = high_resolution_clock::now();
        if (!path.empty()) reverse(path.begin(), path.end());
        long long dur = duration_cast<microseconds>(stop - start).count();
        executionTimes[4] = dur;
        pathWeights[4] = totalWeight;
        logStats(algoNames[4], path, totalWeight, openSize, DFS_T, DFS_S, dur);
    }

    // 5) Bidirectional DFS
    {
        int openSize = 0;
        double totalWeight = 0.0;
        auto start = high_resolution_clock::now();
        vector<string> path = bidirectional_dfs(graph, src, dest, openSize, totalWeight);
        auto stop = high_resolution_clock::now();
        long long dur = duration_cast<microseconds>(stop - start).count();
        executionTimes[5] = dur;
        pathWeights[5] = totalWeight;
        logStats(algoNames[5], path, totalWeight, openSize, DFS_T, DFS_S, dur);
    }

    // 6) Forward Beam
    {
        int openSize = 0;
        double totalWeight = 0.0;
        auto start = high_resolution_clock::now();
        vector<string> path = beam_search(graph, src, dest, beamWidth, openSize, totalWeight);
        auto stop = high_resolution_clock::now();
        long long dur = duration_cast<microseconds>(stop - start).count();
        executionTimes[6] = dur;
        pathWeights[6] = totalWeight;
        logStats(algoNames[6], path, totalWeight, openSize, BEAM_T, BEAM_S, dur);
    }

    // 7) Backward Beam
    {
        int openSize = 0;
        double totalWeight = 0.0;
        auto start = high_resolution_clock::now();
        vector<string> path = beam_search(graph, dest, src, beamWidth, openSize, totalWeight);
        auto stop = high_resolution_clock::now();
        if (!path.empty()) reverse(path.begin(), path.end());
        long long dur = duration_cast<microseconds>(stop - start).count();
        executionTimes[7] = dur;
        pathWeights[7] = totalWeight;
        logStats(algoNames[7], path, totalWeight, openSize, BEAM_T, BEAM_S, dur);
    }

    // 8) Bidirectional Beam
    {
        int openSize = 0;
        double totalWeight = 0.0;
        auto start = high_resolution_clock::now();
        vector<string> path = bidirectional_beam(graph, src, dest, beamWidth, openSize, totalWeight);
        auto stop = high_resolution_clock::now();
        long long dur = duration_cast<microseconds>(stop - start).count();
        executionTimes[8] = dur;
        pathWeights[8] = totalWeight;
        logStats(algoNames[8], path, totalWeight, openSize, BEAM_T, BEAM_S, dur);
    }

    // 9) Forward British Museum
    {
        int openSize = 0;
        double totalWeight = 0.0;
        auto start = high_resolution_clock::now();
        vector<string> path = british_museum(graph, src, dest, openSize, totalWeight);
        auto stop = high_resolution_clock::now();
        long long dur = duration_cast<microseconds>(stop - start).count();
        executionTimes[9] = dur;
        pathWeights[9] = totalWeight;
        logStats(algoNames[9], path, totalWeight, openSize, BM_T, BM_S, dur);
    }

    // 10) Backward British Museum
    {
        int openSize = 0;
        double totalWeight = 0.0;
        auto start = high_resolution_clock::now();
        vector<string> path = british_museum(graph, dest, src, openSize, totalWeight);
        auto stop = high_resolution_clock::now();
        if (!path.empty()) reverse(path.begin(), path.end());
        long long dur = duration_cast<microseconds>(stop - start).count();
        executionTimes[10] = dur;
        pathWeights[10] = totalWeight;
        logStats(algoNames[10], path, totalWeight, openSize, BM_T, BM_S, dur);
    }

    // 11) Bidirectional British Museum
    {
        int openSize = 0;
        double totalWeight = 0.0;
        auto start = high_resolution_clock::now();
        vector<string> path = bidirectional_british(graph, src, dest, openSize, totalWeight);
        auto stop = high_resolution_clock::now();
        long long dur = duration_cast<microseconds>(stop - start).count();
        executionTimes[11] = dur;
        pathWeights[11] = totalWeight;
        logStats(algoNames[11], path, totalWeight, openSize, BM_T, BM_S, dur);
    }

    // Summary
    cout << "\n=== Execution Time and Path Weight Summary ===\n";
    for (size_t i = 0; i < algoNames.size(); ++i) {
        cout << setw(32) << left << algoNames[i] 
             << " : Time = " << executionTimes[i] << " μs, Weight = " << pathWeights[i] << "\n";
    }

    return 0;
}