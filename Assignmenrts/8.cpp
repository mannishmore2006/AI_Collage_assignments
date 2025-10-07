#include <bits/stdc++.h>
using namespace std;

// A rule: IF all premises THEN conclusion
struct Rule {
    vector<string> premises;
    string conclusion;
};

// Knowledge base: rules
vector<Rule> rules = {
    {{"A", "B"}, "C"},       // A AND B => C
    {{"C", "D"}, "E"},       // C AND D => E
    {{"E"}, "F"},            // E => F
    {{"B", "D"}, "G"}        // B AND D => G
};

// Initial facts
unordered_set<string> facts = {"A", "B", "D"};

// Keep track of visited goals to avoid loops
unordered_set<string> visited;

bool backwardChain(const string& goal) {
    // If goal is already a known fact
    if (facts.find(goal) != facts.end()) return true;

    // Avoid infinite loops
    if (visited.find(goal) != visited.end()) return false;
    visited.insert(goal);

    // Find rules that conclude the goal
    for (const auto& rule : rules) {
        if (rule.conclusion == goal) {
            bool allPremisesTrue = true;
            for (const auto& premise : rule.premises) {
                if (!backwardChain(premise)) {
                    allPremisesTrue = false;
                    break;
                }
            }
            if (allPremisesTrue) {
                facts.insert(goal); // Goal can be inferred
                cout << "Inferred: " << goal << "\n";
                return true;
            }
        }
    }

    // No rule could infer the goal
    return false;
}

int main() {
    string goal = "F";

    cout << "Backward chaining for goal: " << goal << "\n\n";
    if (backwardChain(goal)) {
        cout << "Goal " << goal << " has been inferred!\n";
    } else {
        cout << "Goal " << goal << " cannot be inferred.\n";
    }

    // Optional: Print all inferred facts
    cout << "\nAll facts after backward chaining:\n";
    for (const auto& f : facts) cout << f << " ";
    cout << "\n";

    return 0;
}
