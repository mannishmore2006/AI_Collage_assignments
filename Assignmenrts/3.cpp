#include <bits/stdc++.h>
using namespace std;

// Simple KB for family facts and inference
struct KB {
    unordered_map<string, vector<string>> childrenOf; // parent -> list of children
    unordered_map<string, vector<string>> parentsOf;  // child -> list of parents
    unordered_map<string, string> spouse;             // person -> spouse (one-to-one assumed)
    unordered_set<string> maleSet, femaleSet;
    unordered_set<string> people;

    void addParentChild(const string &p, const string &c) {
        childrenOf[p].push_back(c);
        parentsOf[c].push_back(p);
        people.insert(p); people.insert(c);
    }
    void addSpouse(const string &a, const string &b) {
        spouse[a] = b; spouse[b] = a;
        people.insert(a); people.insert(b);
    }
    void addGender(const string &name, const string &g) {
        if (g == "male") { maleSet.insert(name); femaleSet.erase(name); }
        else if (g == "female") { femaleSet.insert(name); maleSet.erase(name); }
        people.insert(name);
    }

    // utility
    bool isParent(const string &a, const string &b) {
        auto it = childrenOf.find(a);
        if (it == childrenOf.end()) return false;
        for (auto &c : it->second) if (c == b) return true;
        return false;
    }
    bool isChild(const string &a, const string &b) { // a child of b?
        return isParent(b,a);
    }
    bool isSibling(const string &a, const string &b) {
        if (a == b) return false;
        auto pa = parentsOf.find(a), pb = parentsOf.find(b);
        if (pa == parentsOf.end() || pb == parentsOf.end()) return false;
        // share at least one parent
        for (auto &p : pa->second)
            for (auto &q : pb->second)
                if (p == q) return true;
        return false;
    }
    bool isAncestor(const string &a, const string &b) {
        if (a == b) return false;
        // DFS from a to find b
        unordered_set<string> vis;
        stack<string> st;
        st.push(a); vis.insert(a);
        while (!st.empty()) {
            string cur = st.top(); st.pop();
            for (auto &ch : childrenOf[cur]) {
                if (ch == b) return true;
                if (!vis.count(ch)) { vis.insert(ch); st.push(ch); }
            }
        }
        return false;
    }
    bool isDescendant(const string &a, const string &b) { // a descendant of b?
        return isAncestor(b,a);
    }
    bool isGrandparent(const string &a, const string &b) {
        // a parent of some x who is parent of b
        auto it = childrenOf.find(a);
        if (it == childrenOf.end()) return false;
        for (auto &x : it->second) if (isParent(x,b)) return true;
        return false;
    }
    bool isUncleOrAunt(const string &a, const string &b) {
        // a is sibling of one of b's parents OR spouse of such sibling
        if (parentsOf.find(b) == parentsOf.end()) return false;
        for (auto &p : parentsOf[b]) {
            if (isSibling(a,p)) return true;
            // spouse of sibling
            auto it = spouse.find(a);
            if (it != spouse.end()) {
                if (isSibling(it->second, p)) return true;
            }
            // or spouse of parent who is sibling? spouse of a sibling already covered
        }
        return false;
    }
    bool isCousin(const string &a, const string &b) {
        // if their parents are siblings
        if (parentsOf.find(a) == parentsOf.end() || parentsOf.find(b) == parentsOf.end()) return false;
        for (auto &pa : parentsOf[a])
            for (auto &pb : parentsOf[b])
                if (isSibling(pa,pb)) return true;
        return false;
    }

    vector<string> whoParentsOf(const string &x) {
        vector<string> r;
        auto it = childrenOf.find(x);
        if (it != childrenOf.end()) r = it->second;
        sort(r.begin(), r.end());
        return r;
    }
    vector<string> whoChildrenOf(const string &x) {
        vector<string> r;
        auto it = parentsOf.find(x);
        if (it != parentsOf.end()) r = it->second;
        sort(r.begin(), r.end());
        return r;
    }

    string genderOf(const string &x) {
        if (maleSet.count(x)) return "male";
        if (femaleSet.count(x)) return "female";
        return "unknown";
    }
};

// -------- parsing helpers ----------
static string trim(const string &s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b-a+1);
}

bool parseFact(const string &line, KB &kb) {
    // accept: parent(A,B) or spouse(A,B) or male(A) female(A)
    string s = trim(line);
    if (s.empty()) return false;
    // remove trailing semicolon if present
    if (s.back() == ';') s.pop_back();
    // find '('
    auto pos = s.find('(');
    if (pos == string::npos) return false;
    string pred = trim(s.substr(0,pos));
    string inside = s.substr(pos+1);
    if (inside.size()==0) return false;
    if (inside.back() == ')') inside.pop_back();
    // split args by comma
    vector<string> args;
    string tmp;
    for (char c : inside) {
        if (c == ',') { args.push_back(trim(tmp)); tmp.clear(); }
        else tmp.push_back(c);
    }
    if (!tmp.empty()) args.push_back(trim(tmp));
    for (auto &a: args) {
        // remove optional surrounding quotes
        if (a.size() >= 2 && ((a.front() == '\'' && a.back() == '\'') || (a.front() == '"' && a.back() == '"')))
            a = a.substr(1, a.size()-2);
    }

    if (pred == "parent" && args.size() == 2) {
        kb.addParentChild(args[0], args[1]);
        return true;
    } else if (pred == "spouse" && args.size() == 2) {
        kb.addSpouse(args[0], args[1]);
        return true;
    } else if ((pred == "male" || pred == "female") && args.size() == 1) {
        kb.addGender(args[0], pred);
        return true;
    }
    return false;
}

// parse queries of simple forms (see README below)
string runQuery(const string &q, KB &kb) {
    string s = trim(q);
    if (s.empty()) return "";

    // remove trailing semicolon
    if (s.back() == ';') s.pop_back();

    // direct relation queries like parent(A,B)
    auto pos = s.find('(');
    if (pos != string::npos) {
        string pred = trim(s.substr(0,pos));
        string inside = s.substr(pos+1);
        if (inside.back() == ')') inside.pop_back();
        // split args
        vector<string> args;
        string tmp;
        for (char c : inside) {
            if (c == ',') { args.push_back(trim(tmp)); tmp.clear(); }
            else tmp.push_back(c);
        }
        if (!tmp.empty()) args.push_back(trim(tmp));
        for (auto &a: args) {
            if (a.size() >= 2 && ((a.front() == '\'' && a.back() == '\'') || (a.front() == '"' && a.back() == '"')))
                a = a.substr(1,a.size()-2);
        }

        if (pred == "parent" && args.size() == 2) {
            bool val = kb.isParent(args[0], args[1]);
            return string(pred) + "(" + args[0] + "," + args[1] + ") = " + (val? "true":"false");
        } else if (pred == "child" && args.size() == 2) {
            bool val = kb.isChild(args[0], args[1]);
            return string(pred) + "(" + args[0] + "," + args[1] + ") = " + (val? "true":"false");
        } else if (pred == "sibling" && args.size() == 2) {
            bool val = kb.isSibling(args[0], args[1]);
            return string(pred) + "(" + args[0] + "," + args[1] + ") = " + (val? "true":"false");
        } else if (pred == "ancestor" && args.size() == 2) {
            bool val = kb.isAncestor(args[0], args[1]);
            return string(pred) + "(" + args[0] + "," + args[1] + ") = " + (val? "true":"false");
        } else if (pred == "descendant" && args.size() == 2) {
            bool val = kb.isDescendant(args[0], args[1]);
            return string(pred) + "(" + args[0] + "," + args[1] + ") = " + (val? "true":"false");
        } else if (pred == "grandparent" && args.size() == 2) {
            bool val = kb.isGrandparent(args[0], args[1]);
            return string(pred) + "(" + args[0] + "," + args[1] + ") = " + (val? "true":"false");
        } else if (pred == "uncle_or_aunt" && args.size() == 2) {
            bool val = kb.isUncleOrAunt(args[0], args[1]);
            return string(pred) + "(" + args[0] + "," + args[1] + ") = " + (val? "true":"false");
        } else if (pred == "cousin" && args.size() == 2) {
            bool val = kb.isCousin(args[0], args[1]);
            return string(pred) + "(" + args[0] + "," + args[1] + ") = " + (val? "true":"false");
        } else if (pred == "gender" && args.size() == 1) {
            return "gender(" + args[0] + ") = " + kb.genderOf(args[0]);
        } else if (pred == "who_parent_of" && args.size() == 1) {
            auto v = kb.whoParentsOf(args[0]);
            string out = "who_parent_of(" + args[0] + ") = ";
            if (v.empty()) out += "none";
            else {
                for (size_t i=0;i<v.size();++i) {
                    if (i) out += ", ";
                    out += v[i];
                }
            }
            return out;
        } else if (pred == "who_children_of" && args.size() == 1) {
            auto v = kb.whoChildrenOf(args[0]);
            string out = "who_children_of(" + args[0] + ") = ";
            if (v.empty()) out += "none";
            else {
                for (size_t i=0;i<v.size();++i) {
                    if (i) out += ", ";
                    out += v[i];
                }
            }
            return out;
        } else {
            return "Unsupported query or wrong arity: " + s;
        }
    } else {
        return "Unsupported query format: " + s;
    }
}

// ----------------- main -------------------
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    cout << "Enter facts (one per line). Empty line to stop facts input.\n";
    cout << "Supported facts: parent(A,B)  spouse(A,B)  male(A)  female(A)\n";
    KB kb;
    string line;
    while (true) {
        if (!std::getline(cin, line)) break;
        line = trim(line);
        if (line.empty()) break;
        bool ok = parseFact(line, kb);
        if (!ok) {
            cout << "Warning: could not parse fact: " << line << "\n";
        }
    }

    cout << "\nEnter queries (one per line). Empty line to exit.\n";
    cout << "Supported queries examples:\n";
    cout << " parent(John,Mary)\n sibling(A,B)\n ancestor(Ann,Bob)\n grandparent(G,C)\n uncle_or_aunt(U,C)\n cousin(X,Y)\n gender(Name)\n who_parent_of(Name)  who_children_of(Name)\n\n";

    while (true) {
        if (!std::getline(cin, line)) break;
        line = trim(line);
        if (line.empty()) break;
        cout << runQuery(line, kb) << "\n";
    }

    cout << "Bye.\n";
    return 0;
}
