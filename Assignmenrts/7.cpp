#include <bits/stdc++.h>
using namespace std;

// A rule: if all conditions in "premises" are true, then we can infer "conclusion"
struct Rule {
    vector<string> premises;
    string conclusion;
};

// Check if all premises are satisfied
bool allPremisesSatisfied(const vector<string>& premises, const unordered_set<string>& facts) {
    for (const auto& p : premises) {
        if (facts.find(p) == facts.end()) return false;
    }
    return true;
}

int main() {
    // Knowledge base: rules
    vector<Rule> rules = {
        {{"A", "B"}, "C"},       // A AND B => C
        {{"C", "D"}, "E"},       // C AND D => E
        {{"E"}, "F"},            // E => F
        {{"B", "D"}, "G"}        // B AND D => G
    };

    // Initial facts
    unordered_set<string> facts = {"A", "B", "D"};

    // Goal we want to derive
    string goal = "F";

    bool newFactAdded;
    do {
        newFactAdded = false;
        for (const auto& rule : rules) {
            if (facts.find(rule.conclusion) == facts.end() &&
                allPremisesSatisfied(rule.premises, facts)) {
                facts.insert(rule.conclusion);
                cout << "Inferred: " << rule.conclusion << "\n";
                newFactAdded = true;
            }
        }
    } while (newFactAdded);

    if (facts.find(goal) != facts.end()) {
        cout << "Goal " << goal << " has been inferred!\n";
    } else {
        cout << "Goal " << goal << " cannot be inferred.\n";
    }

    // Optional: Print all inferred facts
    cout << "\nAll facts after forward chaining:\n";
    for (const auto& f : facts) cout << f << " ";
    cout << "\n";

    return 0;
}
