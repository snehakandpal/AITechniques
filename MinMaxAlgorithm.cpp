#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include <algorithm>    // std::max

using namespace std;

// There are 42 places to play in the board represented by input[1] ... input[42]
char global_game_board[43];

// Forward declarations.
void draw_board();
void reset();
int get_value_for_column(int column, char* current_game_board);
int get_game_status(char* current_game_board);
void end_game(int game_status);

int get_human_move();

int get_ai_move(char* current_game_board, int depth, int alpha, int beta, bool is_maximizing_player, bool is_root);

char human_move_symbol = 'X';
char ai_move_symbol = '0';

int main() {
    reset();

    // Loop which makes 1 computer move and 1 human move until game ends.
    while(true) {
    	// The computer makes the first move.
    	int ai_move_column = get_ai_move(global_game_board, 6, 999999, -999999, true, true);
    	cout << "Computer move: column " << ai_move_column << endl;
        global_game_board[get_value_for_column(ai_move_column, global_game_board)] = ai_move_symbol;

        // draw the current state of the game.
        draw_board();

        // Check victory condition.
        // 0 = in progress, 1 = human victory, 2 = computer victory, 3 = tie
        int game_status = get_game_status(global_game_board);
        if(game_status!=0) {
        	end_game(game_status);
            return 0;
        }

        // Human makes the move now.
        int human_input = get_human_move();
        global_game_board[human_input] = human_move_symbol;
        draw_board();
        // Check victory condition.
        // 0 = in progress, 1 = human victory, 2 = computer victory, 3 = tie
        game_status = get_game_status(global_game_board);
        if(game_status!=0) {
        	end_game(game_status);
            return 0;
        }
    }
}

void end_game(int game_status) {
    if(game_status == 1) {
        cout<<endl<<"Human WON !";
    } else if (game_status == 2) {
        cout<<endl<<"Computer WON ! " ;
    }  else if (game_status == 3) {
        cout<<"Game Tied ! ";
    }
    cout << "--" << endl;
}

void reset() {
    for(int i = 0 ; i<= 80 ; i++)
        global_game_board[i]=' ';
}

// Pass this function a column that you want to play in and it will return its value in input array
int get_value_for_column(int column, char* game_board) {
    if(column > 7) {
    	return 0;
    }

    int n = 0;
    for (int i = 0; i<= 6; i++) {
        if (game_board[column+7*i] == ' ') {
            n = column+7*i;
            break;
        }
    }
    return (n > 42) ? 0 : n;
}


// Draw board
void draw_board() {
    cout << endl << "    1   " << "    2   " << "    3   "
    		<< "    4   " << "    5   " << "    6   "
			<< "    7   " << endl;

    int j = 42;
    for(int i = 0 ; i<= 23 ; i++) {
        if(i % 4 == 0)
            cout<<string(57,'-');
        else {
            if( (i - 2) % 4 == 0) {
                j=42-(0.25*i+0.5)*6-((0.25*i+0.5)-1) ;
                for(int i = 0 ; i<=6 ; i++) {
                    cout<<"|"<<"   "<<global_game_board[j]<<"   ";
                    j++;
                }
                cout<<"|";
            } else {
                for(int i = 0 ; i<=6 ; i++)
                    cout<<"|"<<string(7,' ');
                cout<<"|";
            }
        }
        cout<<endl;
    }
    cout<<string(57,'-');
    cout << endl;
}


// Function that asks you to enter where you like to play
int get_human_move() {
    cout << endl << "Where would you like to play?" << endl;
    while(true) {
        int sth = 0;
        if (!(cin >> sth)) {
            cout<<"Invalid input, please enter a number."<<endl;
        	cin.clear();
        	cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        	continue;
        }

        sth=get_value_for_column(sth, global_game_board);
        if(sth != 0) {
            return sth;
        } else {
            cout<<"Please enter a number from 1 to 7." << endl;
        }
    }
}

// Returns the current status of the game
// 0 - game in progress
// 1 - Human win
// 2 - Computer win
// 3 - draw
int get_game_status(char* current_input) {
    int temp=0;
    for(int i = 1 ; i<= 42 ; i++) {
        if(current_input[i] != ' ') {
            temp++;
            if( i - int((i-1)/7) * 7 <= 4  ) {
                if( current_input[i] == current_input [i+1] && current_input[i] == current_input[i+2] && current_input[i] == current_input[i+3] ) {
                    if(current_input[i] == 'X' ) {
                        return 1 ;
                    } else {
                        return 2;
                    }
                }
            }
            if( i <= 21 ) {
                if ( current_input[i] == current_input[i+7] && current_input[i] == current_input[i+14] && current_input[i] == current_input[i+21]  ) {
                    if(current_input[i] == 'X' ) {
                        return 1 ;
                    } else {
                        return 2;
                    }
                }
            }
            if( i - int((i-1)/7) * 7 <= 4 && i<=18  ) {
                if(current_input[i] == current_input[i+8] && current_input[i] == current_input[i+16] && current_input[i]==current_input[i+24]) {
                    if(current_input[i] == 'X' ) {
                        return 1 ;
                    } else {
                        return 2;
                    }
                }
            }
            if( i - int((i-1)/7) * 7 >= 4 && i <= 21   ) {
                if(current_input[i] == current_input[i+6] && current_input[i] == current_input[i+12] && current_input[i]==current_input[i+18]) {
                    if(current_input[i] == 'X' ) {
                        return 1 ;
                    } else {
                        return 2;
                    }
                }
            }
        }

    }

    return (temp == 42) ? 3 : 0;
}

// AI Win = +1
// Human win = -1
// Draw = 0
// Game in progress = 0
int get_game_status_heuristic(int game_status) {
	if (game_status == 2) { // ai win
		return 1;
	} else if (game_status == 1) {  // human win
		return -1;
	} else { // draw/game in progress
		return 0;
	}
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////// AI ALGORITHM ////////////////////////////////////////
///////////////////////// MIN-MAX WITH ALPHA-BETA PRUNING ///////////////////////////
/////////////////////////////////////////////////////////////////////////////////////

// A min-max algorithm with alpha-beta pruning
int get_ai_move(char* current_game_board, int depth, int alpha, int beta, bool is_maximizing_player, bool is_root) {
	// If we have reached maximum depth, stop recursing and return the heuristic value.
	int current_game_status = get_game_status(current_game_board);
	if (depth == 0) {
		return get_game_status_heuristic(current_game_status);
	}

	// If the game has ended, return the heuristic value.
	if (get_game_status(current_game_board) != 0) {
		return get_game_status_heuristic(current_game_status);
	}

	if (is_maximizing_player) {  // MAX
		//    if maximizingPlayer
		//        v := -∞
		//        for each child of node
		//            v := max(v, alphabeta(child, depth - 1, α, β, FALSE))
		//            α := max(α, v)
		//            if β ≤ α
		//                break (* β cut-off *)
		//        return v
		int column_with_max_value = 0;
		int max_value_seen = -999999;

		// Loop for each possible next-move (child moves)
		for(int column = 1 ; column <= 7 ; column ++) {
			int value_for_column = get_value_for_column(column, current_game_board);
			if (value_for_column == 0) { // no more moves left in this column
				continue;
			}

			char new_game_board[43]; // create a new game board
			std::strncpy(new_game_board, current_game_board, sizeof (new_game_board)); // copy over current game
			new_game_board[value_for_column] = ai_move_symbol; // Make the new move

			// create new board
			int max_value_for_move = get_ai_move(new_game_board, depth - 1, alpha, beta, false, false);
			if (max_value_for_move > max_value_seen) {
				max_value_seen = max_value_for_move;
				column_with_max_value = column;
			}

			// Beta cutoff
//			alpha = std::max(alpha, max_value_seen);
//			if (beta <= alpha) {
//				cout << "beta cutoff" << endl;
//			}
		}

		if (is_root) {
			return column_with_max_value;  // for the first call, we return the column-number instead of the max-value
		}

		return max_value_seen;

	} else {  // MIN
		//        v := ∞
		//        for each child of node
		//            v := min(v, alphabeta(child, depth - 1, α, β, TRUE))
		//            β := min(β, v)
		//            if β ≤ α
		//                break (* α cut-off *)
		//        return v
		int min_value_seen = 999999;

		// Loop for each possible next-move (child moves)
		for(int column = 1 ; column <= 7 ; column ++) {
			int value_for_column = get_value_for_column(column, current_game_board);
			if (value_for_column == 0) {  // no more moves left in this column
				continue;
			}

			char new_game_board[43]; // create a new game board
			std::strncpy(new_game_board, current_game_board, sizeof (new_game_board)); // copy over current game
			new_game_board[value_for_column] = human_move_symbol; // Make the new human move (minimizing player)

			// create new board
			min_value_seen = std::min(min_value_seen, get_ai_move(new_game_board, depth - 1, alpha, beta, true, false));

			// Alpha cutoff
//			beta = std::min(beta, min_value_seen);
//			if (beta <= alpha) {
//				cout << "alpha cutoff" << endl;
//			}
		}

		return min_value_seen;
	}
}
