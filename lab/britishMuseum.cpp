#include <bits/stdc++.h>
using namespace std;

struct State {
    string location;
    set<string> cart;

    bool operator==(const State& other) const {
        return location == other.location && cart == other.cart;
    }
};

// Needed to use State in unordered_set
struct StateHash {
    size_t operator()(const State& s) const {
        size_t h1 = hash<string>()(s.location);
        size_t h2 = 0;
        for(auto& item : s.cart) {
            h2 ^= hash<string>()(item) + 0x9e3779b9 + (h2<<6) + (h2>>2);
        }
        return h1 ^ h2;
    }
};

struct Aisle {
    string name;
    vector<string> items;
    vector<string> neighbors;
};

unordered_map<string, Aisle> store;
set<string> shoppingList;
string startAisle, checkoutAisle;

bool britishMuseumSearch(State start, vector<string>& path) {
    struct Node {
        State state;
        vector<string> actions;
    };

    vector<Node> candidates;               // store all unexplored nodes
    unordered_set<State, StateHash> visited;

    candidates.push_back({start, {}});
    visited.insert(start);

    srand(time(0)); // random seed

    while(!candidates.empty()) {
        // Pick a random candidate
        int idx = rand() % candidates.size();
        Node node = candidates[idx];
        candidates.erase(candidates.begin() + idx);

        State current = node.state;
        vector<string> currentPath = node.actions;

        // Goal check
        if(current.location == checkoutAisle && current.cart == shoppingList) {
            path = currentPath;
            path.push_back("Checkout()");
            return true;
        }

        // Generate next states: pick items
        for(auto& item : store[current.location].items) {
            if(shoppingList.count(item) && !current.cart.count(item)) {
                State next = current;
                next.cart.insert(item);
                if(!visited.count(next)) {
                    vector<string> nextPath = currentPath;
                    nextPath.push_back("Pick(" + item + ")");
                    candidates.push_back({next, nextPath});
                    visited.insert(next);
                }
            }
        }

        // Generate next states: move to neighbors
        for(auto& nb : store[current.location].neighbors) {
            State next = current;
            next.location = nb;
            if(!visited.count(next)) {
                vector<string> nextPath = currentPath;
                nextPath.push_back("MoveTo(" + nb + ")");
                candidates.push_back({next, nextPath});
                visited.insert(next);
            }
        }
    }

    return false; // no path found
}

int main() {
    int n;
    cout << "Enter number of aisles: ";
    cin >> n;

    for(int i=0;i<n;i++){
        string name; int itemCount, nbCount;
        cout << "Enter aisle name: ";
        cin >> name;
        Aisle a;
        a.name = name;

        cout << "Enter number of items in " << name << ": ";
        cin >> itemCount;
        for(int j=0;j<itemCount;j++){
            string item; cin >> item;
            a.items.push_back(item);
        }

        cout << "Enter number of neighbors of " << name << ": ";
        cin >> nbCount;
        for(int j=0;j<nbCount;j++){
            string nb; cin >> nb;
            a.neighbors.push_back(nb);
        }
        store[name] = a;
    }

    cout << "Enter start aisle: ";
    cin >> startAisle;
    cout << "Enter checkout aisle: ";
    cin >> checkoutAisle;

    int m;
    cout << "Enter number of items in shopping list: ";
    cin >> m;
    for(int i=0;i<m;i++){
        string it; cin >> it;
        shoppingList.insert(it);
    }

    State start = {startAisle, {}};
    vector<string> path;

    if(britishMuseumSearch(start, path)){
        cout << "\nBritish Museum Search found a path:\n";
        for(auto& act : path) cout << act << "\n";
    } else {
        cout << "\nBritish Museum Search failed.\n";
    }
}