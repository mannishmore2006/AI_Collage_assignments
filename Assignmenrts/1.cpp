#include <bits/stdc++.h>
using namespace std;

struct Puzzle {
    vector<vector<int>> state;
    string path; // store moves taken to reach this state

    Puzzle(vector<vector<int>> s, string p = "") : state(s), path(p) {}
};

// Goal state
vector<vector<int>> goal = {{1,2,3}, {4,5,6}, {7,8,0}};

// Directions: Up, Down, Left, Right
vector<int> dx = {-1, 1, 0, 0};
vector<int> dy = {0, 0, -1, 1};
vector<char> moveChar = {'U', 'D', 'L', 'R'};

// Check if two states are equal
bool isGoal(const vector<vector<int>>& a, const vector<vector<int>>& b) {
    return a == b;
}

// Convert board to string (for visited check)
string stateToString(const vector<vector<int>>& state) {
    string s = "";
    for (auto &row : state)
        for (int val : row) s += to_string(val);
    return s;
}

// Get neighbors (possible moves)
vector<Puzzle> getNeighbors(Puzzle current) {
    vector<Puzzle> neighbors;
    int n = current.state.size();
    int x, y;

    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            if (current.state[i][j] == 0) { x = i; y = j; }

    for (int d = 0; d < 4; d++) {
        int nx = x + dx[d], ny = y + dy[d];
        if (nx >= 0 && ny >= 0 && nx < n && ny < n) {
            auto newState = current.state;
            swap(newState[x][y], newState[nx][ny]);
            neighbors.push_back(Puzzle(newState, current.path + moveChar[d]));
        }
    }
    return neighbors;
}

// BFS Search
void BFS(vector<vector<int>> start) {
    queue<Puzzle> q;
    unordered_set<string> visited;

    q.push(Puzzle(start));
    visited.insert(stateToString(start));

    while (!q.empty()) {
        Puzzle current = q.front(); q.pop();

        if (isGoal(current.state, goal)) {
            cout << "BFS Solution found! Path: " << current.path << "\n";
            return;
        }

        for (auto neighbor : getNeighbors(current)) {
            string key = stateToString(neighbor.state);
            if (visited.find(key) == visited.end()) {
                visited.insert(key);
                q.push(neighbor);
            }
        }
    }
    cout << "No solution found with BFS.\n";
}

// Depth-Limited Search (recursive helper for IDDFS)
bool DLS(Puzzle current, int limit, unordered_set<string>& visited, string& result) {
    if (isGoal(current.state, goal)) {
        result = current.path;
        return true;
    }
    if ((int)current.path.size() >= limit) return false;

    visited.insert(stateToString(current.state));
    for (auto neighbor : getNeighbors(current)) {
        string key = stateToString(neighbor.state);
        if (visited.find(key) == visited.end()) {
            if (DLS(neighbor, limit, visited, result)) return true;
        }
    }
    visited.erase(stateToString(current.state)); // allow revisits on different paths
    return false;
}

// Iterative Deepening DFS
void IDDFS(vector<vector<int>> start, int maxDepth = 50) {
    for (int depth = 0; depth <= maxDepth; depth++) {
        unordered_set<string> visited;
        string result = "";
        if (DLS(Puzzle(start), depth, visited, result)) {
            cout << "IDDFS Solution found at depth " << depth << "! Path: " << result << "\n";
            return;
        }
    }
    cout << "No solution found with IDDFS (depth limit exceeded).\n";
}

int main() {
    vector<vector<int>> start = {
        {1, 2, 3},
        {4, 0, 6},
        {7, 5, 8}
    };

    cout << "Running BFS...\n";
    BFS(start);

    cout << "Running IDDFS...\n";
    IDDFS(start);

    return 0;
}
