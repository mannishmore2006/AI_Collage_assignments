#include <bits/stdc++.h>
using namespace std;

bool consistent(char var, int val, const map<char, int>& assign){
    for(const auto& p: assign){
        if(p.second == val){
            return false;
        }
    }
    return true;
}

bool backtrack(vector<int>& domain, vector<char>& vars, map<char,int>& assign, int index){
    if(index == vars.size()){
        return true;
    }
   
    char var = vars[index];
    for(int num: domain){
        if(consistent(var, num, assign)){
            assign[var] = num;
            if(backtrack(domain, vars, assign, index+1)){
                return true;
            }
            assign.erase(var); // backtrack
        }
    }
    return false;
}

int main(){
    vector<char> vars;
    map<char, int> assign;
    vector<int> domain = {1};
    char c;
   
    cout << "Enter variables (alphabetic characters). Input stops on non-alpha:\n";
    while (cin >> c && isalpha(c)) {
        vars.push_back(c);
    }

    if(vars.empty()){
        cout << "No variables entered. No CSP to solve.\n";
        return 0;
    }
   
    bool sol = backtrack(domain, vars, assign, 0);
    if(sol){
        cout << "CSP has a solution:\n";
        for(auto& p : assign){
            cout << p.first << " = " << p.second << "\n";
        }
    }else{
        cout << "CSP has no solution\n";
    }
    return 0;
}