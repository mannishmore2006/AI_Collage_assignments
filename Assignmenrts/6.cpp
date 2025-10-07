#include <bits/stdc++.h>
using namespace std;

const int N = 8;

// Check if placing queen at (row, col) is safe
bool isSafe(vector<int>& board, int row, int col) {
    for (int i = 0; i < row; i++) {
        int qCol = board[i];
        if (qCol == col || abs(qCol - col) == abs(i - row)) {
            return false; // Same column or diagonal
        }
    }
    return true;
}

// Recursive function to place queens
void solveNQueens(vector<int>& board, int row, vector<vector<int>>& solutions) {
    if (row == N) {
        solutions.push_back(board); // Found a solution
        return;
    }

    for (int col = 0; col < N; col++) {
        if (isSafe(board, row, col)) {
            board[row] = col; // Place queen
            solveNQueens(board, row + 1, solutions);
            // Backtrack
            board[row] = -1;
        }
    }
}

// Print board solution
void printBoard(const vector<int>& board) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (board[i] == j) cout << "Q ";
            else cout << ". ";
        }
        cout << "\n";
    }
    cout << "\n";
}

int main() {
    vector<int> board(N, -1); // board[i] = column of queen in row i
    vector<vector<int>> solutions;

    solveNQueens(board, 0, solutions);

    cout << "Total solutions: " << solutions.size() << "\n\n";

    // Print first 3 solutions
    for (int i = 0; i < min(3, (int)solutions.size()); i++) {
        cout << "Solution " << i+1 << ":\n";
        printBoard(solutions[i]);
    }

    return 0;
}
