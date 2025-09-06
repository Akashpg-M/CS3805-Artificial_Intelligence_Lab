#include <bits/stdc++.h>
using namespace std;

// Structures
struct State {
    string location;
    set<string> cart;

    bool operator==(const State& other) const {
        return location == other.location && cart == other.cart;
    }
};

struct Aisle {
    string name;
    vector<string> items;
    vector<string> neighbors;
};

unordered_map<string, Aisle> store;   // aisles and connections
set<string> shoppingList;             // target items
string startAisle, checkoutAisle;

bool dfs(State current, vector<string>& path, set<pair<string,set<string>>>& visited) {
    // Check goal
    if(current.location == checkoutAisle && current.cart == shoppingList) {
        path.push_back("Checkout()");
        return true;
    }

    // Mark current state as visited
    visited.insert({current.location, current.cart});

    // Option 1: Pick items at current location
    for(auto& item : store[current.location].items) {
        if(shoppingList.count(item) && !current.cart.count(item)) {
            State next = current;
            next.cart.insert(item);
            if(!visited.count({next.location, next.cart})) {
                path.push_back("Pick(" + item + ")");
                if(dfs(next, path, visited)) return true;
                path.pop_back(); // backtrack
            }
        }
    }

    // Option 2: Move to neighbors
    for(auto& nb : store[current.location].neighbors) {
        State next = current;
        next.location = nb;
        if(!visited.count({next.location, next.cart})) {
            path.push_back("MoveTo(" + nb + ")");
            if(dfs(next, path, visited)) return true;
            path.pop_back(); // backtrack
        }
    }

    return false;
}

int main() {
    int n;
    cout << "Enter number of aisles: ";
    cin >> n;

    // Input aisles
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
    set<pair<string,set<string>>> visited;

    if(dfs(start, path, visited)){
        cout << "\nDFS found a path:\n";
        for(auto& act : path) cout << act << "\n";
    } else {
        cout << "\nDFS failed.\n";
    }
}
