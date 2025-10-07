#include <bits/stdc++.h>  // for std::optional
using namespace std;

// Hash for pair<int,int> so we can use unordered_map/set with coordinates
struct PairHash {
    size_t operator()(const pair<int,int>& p) const noexcept {
        return (uint64_t(p.first) << 32) ^ uint64_t((unsigned)p.second);
    }
};

struct Node {
    pair<int,int> pos;
    double g; // cost from start
    double h; // heuristic to goal
    double f; // g + h
    Node(pair<int,int> p = {0,0}, double gg = 0, double hh = 0) : pos(p), g(gg), h(hh), f(gg+hh) {}
};

// For priority queue: smallest f on top; if tie, smaller h
struct PQItem {
    Node node;
    bool operator<(PQItem const& other) const {
        if (node.f == other.node.f) return node.h > other.node.h; // tie-break
        return node.f > other.node.f;
    }
};

// Manhattan heuristic
double manhattan(const pair<int,int>& a, const pair<int,int>& b) {
    return double(abs(a.first - b.first) + abs(a.second - b.second));
}

// Euclidean heuristic
double euclidean(const pair<int,int>& a, const pair<int,int>& b) {
    double dx = double(a.first - b.first);
    double dy = double(a.second - b.second);
    return sqrt(dx*dx + dy*dy);
}

// Reconstruct path using parent map
vector<pair<int,int>> reconstructPath(
    unordered_map<pair<int,int>, pair<int,int>, PairHash>& parent,
    pair<int,int> current)
{
    vector<pair<int,int>> path;
    while (true) {
        path.push_back(current);
        auto it = parent.find(current);
        if (it == parent.end()) break;
        current = it->second;
    }
    reverse(path.begin(), path.end());
    return path;
}

// A* on a 2D grid
// grid: 0 = free, 1 = obstacle
// start, goal: coordinate pairs (row, col). 0-based.
// allowDiagonals: if true uses 8-neighbors; else 4-neighbors.
// useEuclidean: choose heuristic (Euclidean vs Manhattan)
std::optional<vector<pair<int,int>>> AStarGrid(
    const vector<vector<int>>& grid,
    pair<int,int> start,
    pair<int,int> goal,
    bool allowDiagonals = false,
    bool useEuclidean = false)
{
    int R = (int)grid.size();
    if (R == 0) return std::nullopt;
    int C = (int)grid[0].size();

    auto inBounds = [&](const pair<int,int>& p){
        return p.first >= 0 && p.first < R && p.second >= 0 && p.second < C;
    };
    if (!inBounds(start) || !inBounds(goal)) return std::nullopt;
    if (grid[start.first][start.second] != 0 || grid[goal.first][goal.second] != 0) return std::nullopt;

    vector<pair<int,int>> dirs4 = {{-1,0},{1,0},{0,-1},{0,1}};
    vector<pair<int,int>> dirs8 = {{-1,0},{1,0},{0,-1},{0,1},{-1,-1},{-1,1},{1,-1},{1,1}};
    const auto& dirs = allowDiagonals ? dirs8 : dirs4;

    unordered_map<pair<int,int>, double, PairHash> gScore;
    unordered_map<pair<int,int>, pair<int,int>, PairHash> parent;
    unordered_set<pair<int,int>, PairHash> closed;

    priority_queue<PQItem> open;
    double h0 = useEuclidean ? euclidean(start, goal) : manhattan(start, goal);
    gScore[start] = 0.0;
    open.push(PQItem{ Node(start, 0.0, h0) });

    while (!open.empty()) {
        Node cur = open.top().node;
        open.pop();

        if (closed.count(cur.pos)) continue;
        if (cur.pos == goal) {
            return reconstructPath(parent, cur.pos);
        }

        closed.insert(cur.pos);

        for (auto d : dirs) {
            pair<int,int> nb = {cur.pos.first + d.first, cur.pos.second + d.second};
            if (!inBounds(nb)) continue;
            if (grid[nb.first][nb.second] != 0) continue;

            double moveCost = (d.first != 0 && d.second != 0) ? sqrt(2.0) : 1.0;
            double tentative_g = cur.g + moveCost;

            auto gIt = gScore.find(nb);
            if (gIt == gScore.end() || tentative_g < gIt->second) {
                gScore[nb] = tentative_g;
                double h = useEuclidean ? euclidean(nb, goal) : manhattan(nb, goal);
                parent[nb] = cur.pos;
                open.push(PQItem{ Node(nb, tentative_g, h) });
            }
        }
    }

    return std::nullopt; // no path
}

// Example usage
int main() {
    // 0 = free, 1 = obstacle
    vector<vector<int>> grid = {
        {0,0,0,0,0,0},
        {0,1,1,0,1,0},
        {0,0,0,0,1,0},
        {0,1,0,0,0,0},
        {0,0,0,1,1,0},
        {0,0,0,0,0,0}
    };

    pair<int,int> start = {0,0};
    pair<int,int> goal  = {5,5};

    cout << "Running A* (4-neighbors, Manhattan heuristic)...\n";
    auto res = AStarGrid(grid, start, goal, false, false);
    if (!res) {
        cout << "No path found.\n";
    } else {
        auto path = *res;
        cout << "Path length (steps): " << (path.size()-1) << "\n";

        // Compute actual path cost
        double totalCost = 0.0;
        for (size_t i=1;i<path.size();i++) {
            int dr = abs(path[i].first - path[i-1].first);
            int dc = abs(path[i].second - path[i-1].second);
            totalCost += (dr==1 && dc==1) ? sqrt(2.0) : 1.0;
        }
        cout << "Total path cost: " << totalCost << "\n";

        cout << "Path: ";
        for (auto &p : path) cout << "(" << p.first << "," << p.second << ") ";
        cout << "\n";

        // Print grid with path
        vector<string> out(grid.size(), string(grid[0].size(), '.'));
        for (int r=0;r<(int)grid.size();++r)
            for (int c=0;c<(int)grid[0].size();++c)
                if (grid[r][c] == 1) out[r][c] = '#';
        for (auto &p: path) out[p.first][p.second] = 'o';
        out[start.first][start.second] = 'S';
        out[goal.first][goal.second] = 'G';

        cout << "Grid with path (S=start, G=goal, o=path, #=obstacle):\n";
        for (auto &row : out) cout << row << "\n";
    }

    return 0;
}
