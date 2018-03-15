#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <exception>
#include <iterator>

using std::string;
using std::vector;
using std::cin;
using std::cout;
using std::endl;
using std::stringstream;
using std::ifstream;
using std::unordered_map;
using std::unordered_set;
using std::exception;

// Configuration Params.
int word_limit;
int board_rows;
int board_columns;

const string file_path = "lemma.al.txt"; // Path to the input text file.

// Randomly select k words from words file.
vector<string> words_selected;

// The CROSSWORD design board.
char** board;

void out(string s) {
    cout << s << endl;
}

vector<string> split(string str, char delimiter) {
  vector<string> internal;
  stringstream ss(str); // Turn the string into a stream.
  string tok;

  while(getline(ss, tok, delimiter)) {
    internal.push_back(tok);
  }

  return internal;
}

void print_board() {
	for (int i = 0; i < board_rows; ++i) {
		for (int j = 0; j < board_columns; ++j) {
			cout << (board[i][j]);
		}
		cout << endl;
	}
}

// Example get_word_all_positions("hello", 15);
// Output:
//  hello#####
//	#hello####
//	##hello###
//	###hello##
//	####hello#
//	#####hello
vector<string> get_word_all_positions(string word, int size) {
	vector<string> output;
	if (size < word.size()) {
		return output;
	}

	int padding_size = size - word.size();
	string padding;
	padding.insert(padding.end(), padding_size, '#');

	for (int i = 0; i <= padding_size; ++i) {
		string tmp = padding;
		output.push_back(tmp.insert(i, word));
	}

	return output;
}

// Assigns a word to a row in the board. The word is properly padded
// with whitespaces before passing it to this function.
void set_board_row(int row_number, string word) {
	for (int i = 0; i < board_columns; ++i) {
	  try{
		  board[row_number][i] = word.at(i);
	  } catch (exception& e) {
		cout << "An exception occurred." << endl;
	  }
	}
}

// Extracts words for a column upto a given row.
// Ignore words of size 1.
vector<string> get_word_for_column_ending_at_row(int column_number, int row_number) {
	string column_value_till_row;
	for (int i = 0; i <= row_number; ++i) {
		column_value_till_row += board[i][column_number];
	}

	vector<string> words;
	if (column_value_till_row.at(row_number) != '#' &&
			row_number != (board_rows -1)) {  // special handling for last row
		return words;  // empty
	}

	if (row_number > 0 && column_value_till_row.at(row_number - 1) == '#') {
		return words;  // empty
	}

	vector<string> parts = split(column_value_till_row, '#');
	string word = parts.at(parts.size() - 1);

	if (word.size() > 1) {
		words.push_back(word);
	}

	return words;
}

// Scans columns and returns the completed words found.
vector<string> get_all_column_words_upto_row(int row_number) {
	vector<string> words;
	for (int i = 0; i < board_columns; ++i) {
		for (auto word : get_word_for_column_ending_at_row(i, row_number)) {
			words.push_back(word);
		}
	}

	return words;
}

// Updates new remaining words after removing the words found in columns. If we
// find a word which is not present in the word_selected vector, we return false.
bool check_and_remove_column_words(vector<string>* new_remaining_words, int row_number) {

	vector<string> column_words = get_all_column_words_upto_row(row_number);
	for (string word : column_words) {
	    vector<string>::iterator iter = std::find(
	    		new_remaining_words->begin(), new_remaining_words->end(), word);
	    if (iter == new_remaining_words->end()) {
	    	return false;
	    } else {
	    	new_remaining_words->erase(iter);
	    }
	}

	return true;
}

// Get current word at give row
string get_row_word(int row_number) {
	string output;
	for (int i=0; i< board_columns; ++i) {
		output += board[row_number][i];
	}
	return output;
}

// BACKTRACKING IMPLEMENTATION - Recursively calls itself.
bool backtrack_design_crossword(vector<string> remaining_words, int row_number) {
	if (row_number >= board_rows) {
		if (remaining_words.size() == 0) {
			return true;
		}
		return false;
	}

	if (remaining_words.size() == 0) {
		string whitespace_row;
		whitespace_row.insert(whitespace_row.end(), board_columns, '#');
		set_board_row(row_number, whitespace_row);
		// Check columns.
		vector<string> empty;
		return check_and_remove_column_words(&empty, row_number);
	}

	int num_words_left = remaining_words.size();
	for (int i = 0; i < num_words_left; ++i) {  // try to fit each remaining word in this position.
		string word = remaining_words.at(i);
		vector<string> new_remaining_words = remaining_words;
		new_remaining_words.erase(new_remaining_words.begin() + i);


		vector<string> word_padded_all_positions = get_word_all_positions(word, board_columns);
		for (auto padded_word : word_padded_all_positions) {

			// Set the word as row
			set_board_row(row_number, padded_word);

			// Remove some more remaining words based on columns.
			if (!check_and_remove_column_words(&new_remaining_words, row_number)) {
				continue;
			}
			if (backtrack_design_crossword(new_remaining_words, (row_number + 1))) {
				return true;
			}
		}  // try the next padding combination of the word

	}  // try the next word in the row.

	// Try skipping the row - Set the row as ##########
	string whitespace_row;
	whitespace_row.insert(whitespace_row.end(), board_columns, '#');
	set_board_row(row_number, whitespace_row);

	// Check columns to remove words
	vector<string> remaining_words_with_whitespace_row = remaining_words;
	if (!check_and_remove_column_words(&remaining_words_with_whitespace_row, row_number)) {
		return false;
	}
	return (backtrack_design_crossword(remaining_words_with_whitespace_row, row_number + 1));
}

int main() {
	out("Reading the file containing english lemmas.");
	ifstream lemma_file(file_path);
	if(lemma_file.fail()) {
		out("Could not open the lemma file.");
	}

	// Process the file into an inmemory datastructure.
	// Select from the list only: adverbs (adv), verbs (v), adjectives (a) and nouns (n)
	string line;
	vector<string> words;
	while (getline(lemma_file, line)) {
		vector<string> parts = split(line, ' ');
		if (parts[3] == "adv" || parts[3] == "v" || parts[3] == "a" || parts[3] == "n") {
			words.push_back(parts[2]);
		}
	}
	out("Finished processing the file.");

	// Initialize the config params.
	cout << "Please enter the number of rows in crossword." << endl;
	int rows, columns, num_words;
	cin >> rows;
	cout << "Please enter the number of columns in crossword." << endl;
	cin >> columns;
	cout << "Please enter the number of words to pick." << endl;
	cin >> num_words;
	board_rows = rows;
	board_columns = columns;
	word_limit = num_words;
	cout  << endl << "Using board size " << board_columns << "x" << board_rows <<
			" and " << word_limit << "words out of " << words.size() << " words." << endl << endl;


	// Select k random words.
    srand (time (nullptr));
    for (int i = 0; i < word_limit; ++i) {
    	int random = rand() % words.size();
    	words_selected.push_back(words.at(random));
    }
    // Test case to make sure that the algorithm works as expected.
    //	words_selected.push_back("white");
	//	words_selected.push_back("eleven");
	//	words_selected.push_back("nineny");
	//	words_selected.push_back("hen");
	//	words_selected.push_back("ile");
	//	words_selected.push_back("ten");
	//	words_selected.push_back("evy");

    board = new char*[board_columns];
    for(int i = 0; i < board_columns; ++i) {
        board[i] = new char[board_rows];
    }

	// Map the words by their size.
	unordered_map<int, unordered_set<string> > word_size_map;
	out("The words chosen are:");
	for (string word : words_selected) {
		word_size_map[word.size()].insert(word);
		cout << word << " ";
	}
	cout << endl;

	// The first 3 constraints are satisfied by representing the board as a 2D array.
	for (int i = 0; i < board_rows; ++i) {
		for (int j = 0; j < board_columns; ++j) {
			board[i][j] = '#';
		}
	}

	// Backtracking implementation.
	// For each row, start at index 0 and try to fit next available word at 0.
	// First finish off each index to fit the word using whitespaces
	// Then try next word
	// While passing over to next row, check if column consistency exists and some words
	// can be removed.
	// End condition - no words left or
	if (backtrack_design_crossword(words_selected, 0)) {
		out("\nSuccessfully designed crossword!");
		print_board();
	} else {
		out("\nCould not design crossword!");
	}

	delete board;
	return 0;
}
