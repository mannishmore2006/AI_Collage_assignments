#include <bits/stdc++.h>
using namespace std;

const int N = 3; // Board size

// Print board
void printBoard(const vector<vector<char>>& board) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            cout << (board[i][j] == ' ' ? '.' : board[i][j]) << " ";
        }
        cout << "\n";
    }
    cout << "\n";
}

// Check if someone won
char checkWinner(const vector<vector<char>>& board) {
    // Rows and cols
    for (int i = 0; i < N; i++) {
        if (board[i][0] != ' ' &&
            board[i][0] == board[i][1] &&
            board[i][1] == board[i][2]) return board[i][0];
        if (board[0][i] != ' ' &&
            board[0][i] == board[1][i] &&
            board[1][i] == board[2][i]) return board[0][i];
    }
    // Diagonals
    if (board[0][0] != ' ' &&
        board[0][0] == board[1][1] &&
        board[1][1] == board[2][2]) return board[0][0];
    if (board[0][2] != ' ' &&
        board[0][2] == board[1][1] &&
        board[1][1] == board[2][0]) return board[0][2];

    return ' '; // no winner
}

// Check if board full
bool isFull(const vector<vector<char>>& board) {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            if (board[i][j] == ' ') return false;
    return true;
}

// Minimax algorithm
int minimax(vector<vector<char>>& board, bool isMaximizing) {
    char winner = checkWinner(board);
    if (winner == 'X') return +10;   // AI wins
    if (winner == 'O') return -10;   // Human wins
    if (isFull(board)) return 0;     // Draw

    if (isMaximizing) {
        int best = -1000;
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                if (board[i][j] == ' ') {
                    board[i][j] = 'X'; // AI move
                    best = max(best, minimax(board, false));
                    board[i][j] = ' ';
                }
            }
        }
        return best;
    } else {
        int best = 1000;
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                if (board[i][j] == ' ') {
                    board[i][j] = 'O'; // Human move
                    best = min(best, minimax(board, true));
                    board[i][j] = ' ';
                }
            }
        }
        return best;
    }
}

// Find best move for AI
pair<int,int> findBestMove(vector<vector<char>>& board) {
    int bestVal = -1000;
    pair<int,int> bestMove = {-1,-1};

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (board[i][j] == ' ') {
                board[i][j] = 'X'; // Try AI move
                int moveVal = minimax(board, false);
                board[i][j] = ' ';
                if (moveVal > bestVal) {
                    bestMove = {i, j};
                    bestVal = moveVal;
                }
            }
        }
    }
    return bestMove;
}

int main() {
    vector<vector<char>> board(N, vector<char>(N, ' '));

    cout << "Tic-Tac-Toe Game: You are O, AI is X\n";
    printBoard(board);

    while (true) {
        // Human move
        int r, c;
        cout << "Enter your move (row col 0-based): ";
        cin >> r >> c;
        if (r < 0 || r >= N || c < 0 || c >= N || board[r][c] != ' ') {
            cout << "Invalid move. Try again.\n";
            continue;
        }
        board[r][c] = 'O';
        printBoard(board);
        if (checkWinner(board) == 'O') { cout << "You win!\n"; break; }
        if (isFull(board)) { cout << "It's a draw!\n"; break; }

        // AI move
        auto [br, bc] = findBestMove(board);
        board[br][bc] = 'X';
        cout << "AI moves to: " << br << " " << bc << "\n";
        printBoard(board);
        if (checkWinner(board) == 'X') { cout << "AI wins!\n"; break; }
        if (isFull(board)) { cout << "It's a draw!\n"; break; }
    }

    return 0;
}
