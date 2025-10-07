#include <bits/stdc++.h>
using namespace std;

// Check if assigning `val` to `var` is consistent
bool consistent(char var, int val, const map<char, int>& assign) {
    for (const auto& p : assign) {
        if (p.second == val) {
            return false; // no two variables can share the same value
        }
    }
    return true;
}

// Backtracking function
bool backtrack(vector<int>& domain, vector<char>& vars, map<char,int>& assign, int index) {
    if (index == vars.size()) {
        return true; // all variables assigned successfully
    }

    char var = vars[index];
    for (int num : domain) {
        if (consistent(var, num, assign)) {
            assign[var] = num;
            if (backtrack(domain, vars, assign, index + 1)) {
                return true;
            }
            assign.erase(var); // backtrack
        }
    }
    return false; // no valid assignment found
}

int main() {
    vector<char> vars;
    map<char, int> assign;
    char c;

    cout << "Enter variables (alphabetic characters). Input stops on non-alpha:\n";
    while (cin >> c && isalpha(c)) {
        vars.push_back(c);
    }

    if (vars.empty()) {
        cout << "No variables entered. No CSP to solve.\n";
        return 0;
    }

    // Domain = [1 .. number of variables]
    vector<int> domain;
    for (int i = 1; i <= vars.size(); i++) {
        domain.push_back(i);
    }

    bool sol = backtrack(domain, vars, assign, 0);
    if (sol) {
        cout << "CSP has a solution:\n";
        for (auto& p : assign) {
            cout << p.first << " = " << p.second << "\n";
        }
    } else {
        cout << "CSP has no solution\n";
    }
    return 0;
}
