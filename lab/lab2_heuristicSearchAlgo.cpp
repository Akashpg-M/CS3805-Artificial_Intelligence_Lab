#include <bits/stdc++.h>
using namespace std;

struct State {
    vector<vector<int>> towers;
    int h; // heuristic value
    vector<string> moves; // sequence of moves to reach this state
};

// Print current state
void printState(const State& s) {
    cout << "State:\n";
    for (int i = 0; i < s.towers.size(); i++) {
        cout << "Tower " << i << ": ";
        for (int r : s.towers[i]) cout << r << " ";
        cout << "\n";
    }
    cout << "Heuristic = " << s.h << "\n\n";
}

// Heuristic: count rings not in goal tower
int heuristic(const vector<vector<int>>& cur, const vector<int>& goal, int targetTower) {
    int mis = 0;
    const vector<int>& t = cur[targetTower];
    set<int> inTarget(t.begin(), t.end());
    for (int x : goal) {
        if (inTarget.find(x) == inTarget.end()) mis++;
    }
    for (int i = 0; i < cur.size(); i++) {
        if (i == targetTower) continue;
        mis += cur[i].size();
    }
    return mis;
}

// Generate next states
vector<State> generateMoves(const State& s, int targetTower) {
    vector<State> next;
    int n = s.towers.size();
    for (int i = 0; i < n; i++) {
        if (s.towers[i].empty()) continue;
        for (int j = 0; j < n; j++) {
            if (i == j) continue;
            State ns = s;
            int ring = ns.towers[i].back();
            ns.towers[i].pop_back();
            ns.towers[j].push_back(ring);
            ns.moves.push_back("Move ring " + to_string(ring) + " from Tower " + to_string(i) + " -> Tower " + to_string(j));
            ns.h = 0; // heuristic will be computed later
            next.push_back(ns);
        }
    }
    return next;
}

// Hill Climbing
void hillClimbing(State initial, const vector<int>& goal, int targetTower) {
    initial.h = heuristic(initial.towers, goal, targetTower);
    cout << "=== Hill Climbing ===\n";
    printState(initial);

    State current = initial;

    while (current.h > 0) {
        auto neighbors = generateMoves(current, targetTower);
        for (auto& n : neighbors) n.h = heuristic(n.towers, goal, targetTower);

        auto best = min_element(neighbors.begin(), neighbors.end(), [](auto& a, auto& b){ return a.h < b.h; });

        if (best->h >= current.h) {  //lower heuristic is better
            cout << "Stuck in local maxima!\n";
            break;
        }

        // print move and state
        cout << best->moves.back() << "\n";
        printState(*best);

        current = *best;
    }

    if (current.h == 0) cout << "Goal reached!\n";
}

// Beam Search
void beamSearch(State initial, const vector<int>& goal, int targetTower, int k) {
    initial.h = heuristic(initial.towers, goal, targetTower); //calculate the heuristic of the current state, this is not calculated in the generateMoves function
    cout << "=== Beam Search ===\n";

    vector<State> beam = {initial};
    while (!beam.empty()) {
        vector<State> candidates;
        for (auto& s : beam) {
            if (s.h == 0) {
                cout << "Goal reached!\n";
                return;
            }
            auto next = generateMoves(s, targetTower);
            for (auto& ns : next) ns.h = heuristic(ns.towers, goal, targetTower);
            candidates.insert(candidates.end(), next.begin(), next.end());
        }

        if (candidates.empty()) break;

        sort(candidates.begin(), candidates.end(), [](auto& a, auto& b){ return a.h < b.h; });
        beam.assign(candidates.begin(), candidates.begin() + min(k, (int)candidates.size()));

        cout << "--- Beam Step ---\n";
        for (auto& s : beam) printState(s);
    }

    cout << "Beam search ended without reaching goal.\n";
}

int main() {
    int n;
    cout << "Enter number of rings in initial tower: ";
    cin >> n;

    vector<int> initRings(n), goalRings(n);
    cout << "Enter initial tower (top to bottom): ";
    for (int i = 0; i < n; i++) cin >> initRings[i];

    cout << "Enter goal tower (top to bottom): ";
    for (int i = 0; i < n; i++) cin >> goalRings[i];

    int totalTowers = n;
    vector<vector<int>> towers(totalTowers);
    towers[0] = initRings;

    State initial{towers, 0, {}};
    int targetTower = 1; // tower position of goal

    hillClimbing(initial, goalRings, targetTower);
    cout << "\n";
    beamSearch(initial, goalRings, targetTower, 2); // beam width = 2

    return 0;
}
