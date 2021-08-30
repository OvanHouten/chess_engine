#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include <math.h>
#include "chess.h"

int main() {
	// Allow unicode chars in program and terminal
	setlocale(LC_CTYPE, "");

	if(!read_FEN(start_fen)) {
		wprintf(L"Invalid FEN string\n");
	}
	else {
		print_FEN();
	}

	gameloop();

	return 1;
}

void gameloop() {
	wprintf(L"Type # to quit the program\n");
	move_t move_w;
	move_t move_b;

	while (run) {
		// White to move
		FEN.active_color = 'w';
		wprintf(L"%d White's turn: ", FEN.fullmove);
		do {
			move_w = get_move();
		} while(run && !is_legal_move(move_w));
		if(!run) {
			break;
		}
		
		make_move(move_w);
		print_board();

		// Black to move
		FEN.active_color = 'b';
		wprintf(L"%d Black's turn: ", FEN.fullmove);
		do {
			move_b = get_move();
		} while(run && !is_legal_move(move_b));
		
		make_move(move_b);
		print_board();

		FEN.fullmove++;
	}
}

// Quit the chess engine
void quit() {
	run = 0;
}

void make_move(move_t m) {
	piece p = board[m.from.y][m.from.x];
	// Reset en passant
	FEN.en_passant = (coords_t){0, 0};
	
	// Update halfmove counter
	FEN.halfmove++;

	// Capture or pawn move, reset halfmove counter
	if(board[m.to.y][m.to.x] != MT) {
		FEN.halfmove = 0;
	}
	else if(p == WP) {
		FEN.halfmove = 0;
		// Set en passant square
		if(m.to.y - m.from.y == 2) {
			FEN.en_passant = (coords_t){m.to.x, 2};
			
		}
	}
	else if(p == BP) {
		FEN.halfmove = 0;
		// Set en passant square
		if(m.from.y - m.to.y == 2) {
			FEN.en_passant = (coords_t){m.to.x, 5};
		}
	}

	board[m.from.y][m.from.x] = MT;
	board[m.to.y][m.to.x] = p;

}

// Check if the move is legal for the piece to be moved
int is_legal_move(move_t m) {
	if (m.from.x >= 8 && m.from.y >= 8 && m.to.x >= 8 && m.to.y >= 8) {
		return 0;
	}
	if (m.from.x == m.to.x && m.from.y == m.to.y) {
		return 0;
	}

	wchar_t c = board[m.from.y][m.from.x];
	
	// Can't move an empty spot, can you...
	if(c == MT) {
		wprintf(L"There is no piece here\n");
		return 0;
	}
	
	// Players can only move their own color
	if(FEN.active_color == 'w') {
		if(c >= BK) {
			wprintf(L"That piece belongs to black\n");
			return 0;
		}
	}
	else {
		if(c <= WP) {
			wprintf(L"That piece belongs to white\n");
			return 0;
		}
	}

	switch(c) {
		// Logic for Kings
		case WK:
		case BK:
			return is_legal_move_king(m);
			break;
		// Logic for Queens
		case WQ:
		case BQ:
			return is_legal_move_queen(m);
			break;
		// Logic for Rooks
		case WR:
		case BR:
			return is_legal_move_rook(m);
			break;
		// Logic for Bishops
		case WB:
		case BB:
			return is_legal_move_bishop(m);
			break;
		// Logic for Knights
		case WN:
		case BN:
			return is_legal_move_knight(m);
			break;
		// Logic for Pawns
		case WP:
			return is_legal_move_white_pawn(m);
			break;
		case BP:
			return is_legal_move_black_pawn(m);
			break;
		default:
			wprintf(L"Something went wrong, this piece shouldn't exist\n");
			return 0;
	}

	return 1;
}

// TODO: these legals checks should probably change :/
int is_legal_move_king(move_t m) {
	//if() {
		
	//}
	
	if(m.to.x > m.from.x + 1 && m.to.x < m.from.x - 1) {
		return 0;
	}
	if(m.to.y > m.from.y + 1 && m.to.y < m.from.y - 1) {
		return 0;
	}

	return 0;
}

int is_legal_move_queen(move_t m) {
	if(is_legal_move_rook(m) || is_legal_move_bishop(m)) {
		return 1;
	}

	return 0;
}

int is_legal_move_rook(move_t m) {
	size_t y = m.from.y;
	size_t x = m.from.x;

	if(x == m.to.x) {
		if(y > m.to.y) {
			while(--y > m.to.y && board[y][x] == MT);
		}
		else if(y < m.to.y) {
			while(++y < m.to.y && board[y][x] == MT);
		}
	}
	else if(y == m.to.y) {
		if(x > m.to.x) {
			while(--x > m.to.x && board[y][x] == MT);
		}
		else if(x < m.to.x) {
			while(++x < m.to.x && board[y][x] == MT);
		}
	}

	if(x == m.to.x && y == m.to.y) {
		if(board[y][x] == MT || is_opposite_color(m.to)) {
			return 1;
		}
	}

	return 0;
}

int is_legal_move_bishop(move_t m) {
	size_t x = m.from.x;
	size_t y = m.from.y;
	
	if(abs(x - m.to.x) != abs(y - m.to.y)) {
		return 0;
	}
	
	// TODO: This can be done better! maybe one loop?
	if(y > m.to.y) {
		y--;
		if(x > m.to.x) {
			x--;
			while(y > m.to.y && x > m.to.x && board[y][x] == MT) {
				y--;
				x--;
			}
		}
		else if(x < m.to.x) {
			x++;
			while(y > m.to.y && x < m.to.x && board[y][x] == MT) {
				y--;
				x++;
			}
		}
	}
	else if(y < m.to.y) {
		y++;
		if(x > m.to.x) {
			x--;
			while(y < m.to.y && x > m.to.x && board[y][x] == MT) {
				y++;
				x--;
			}
		}
		else if(x < m.to.x) {
			x++;
			while(y < m.to.y && x < m.to.x && board[y][x] == MT) {
				y++;
				x++;
			}
		}
	}

	if(x == m.to.x && y == m.to.y) {
		if(board[y][x] == MT || is_opposite_color(m.to)) {
			return 1;
		}
	}

	return 0;
}

int is_legal_move_knight(move_t m) {
	size_t x = m.to.x;
	size_t y = m.to.y;

	if(m.from.y == y + 2 || m.from.y == y - 2) {
		if(m.from.x == x + 1 || m.from.x == x - 1) {
			if(board[y][x] == MT || is_opposite_color(m.to)) {
				return 1;
			}
		}
	}
	else if(m.from.x == x + 2 || m.from.x == x - 2) {
		if(m.from.y == y + 1 || m.from.y == y - 1) {
			if(board[y][x] == MT || is_opposite_color(m.to)) {
				return 1;
			}
		}
	}

	return 0;
}

int is_legal_move_white_pawn(move_t m) {
	size_t x = m.to.x;
	size_t y = m.to.y;

	// Moving pawn
	if(m.from.x == m.to.x && board[m.to.y][m.to.x] == MT) {
		// Move 1 or 2 squares
		if(m.from.y == m.to.y - 1 || (m.to.y == 3 && m.from.y == 1 && board[m.to.y - 1][m.to.x] == MT)) {
			return 1;
		}
	}
	// Capturing pieces
	else if(m.from.y == m.to.y - 1 && (m.from.x == m.to.x - 1 || m.from.x == m.to.x + 1)) {
		if(is_opposite_color(m.to)) {
			return 1;
		}
		// En passant rule
		else if(board[m.to.y][m.to.x] == MT) {
			if(m.to.x == FEN.en_passant.x && m.to.y == FEN.en_passant.y) {
				return 1;
			}
		}
	}

	return 0;
}

int is_legal_move_black_pawn(move_t m) {
	// Moving pawn
	if(m.from.x == m.to.x && board[m.to.y][m.to.x] == MT) {
		// Move 1 or 2 squares
		if(m.from.y == m.to.y + 1 || (m.to.y == 4 && m.from.y == 6 && board[m.to.y + 1][m.to.x] == MT)) {
			return 1;
		}
	}
	// Capturing pieces
	else if(m.from.y == m.to.y + 1 && (m.from.x == m.to.x - 1 || m.from.x == m.to.x + 1)) {
		if(is_opposite_color(m.to)) {
			return 1;
		}
		// En passant capture
		else if(board[m.to.y][m.to.x] == MT) {
			if(m.to.x == FEN.en_passant.x && m.to.y == FEN.en_passant.y) {
				return 1;
			}
		}
	}

	return 0;
}

int is_opposite_color(coords_t c) {
	if(FEN.active_color == 'w') {
		switch(board[c.y][c.x]) {
			case BK:
			case BQ:
			case BR:
			case BB:
			case BN:
			case BP:
				return 1;
				break;
			default:
				return 0;
		}
	}
	else if(FEN.active_color == 'b') {
		switch(board[c.y][c.x]) {
			case WK:
			case WQ:
			case WR:
			case WB:
			case WN:
			case WP:
				return 1;
				break;
			default:
				return 0;
		}
	}

	return 0;
}

int is_check() {

	return 0;
}

// Get move from user input.
// Moves should be given in 4 chars in the notation from+to as:
// [a-h][1-8][a-h][1-8] -> numeric notation
move_t get_move() {
	char buffer[5]; // 4 chars + null char
	char c;
	size_t i = 0;

	// https://stackoverflow.com/questions/36715002/how-to-clear-stdin-before-getting-new-input
	while ((c = fgetc(stdin)) != EOF) {  // Read until EOF ...
		if (i + 1 < 5) {
			buffer[i++] = c;
		}
		if (c == '\n') {  // ... or end of line
			break;
		}
	}
	buffer[i] = '\0';
	
	if (i == 0) {
		return (move_t){{0, 0}, {0, 0}};
	}

	if(buffer[0] == '#') {
		quit();
		return (move_t){{0, 0}, {0, 0}};
	}

	return (move_t){{buffer[0] - 'a', buffer[1] - '1'}, {buffer[2] - 'a', buffer[3] - '1'}};
}

// Set piece and check if tile exists
int set_piece(size_t x, size_t y, piece p) {
	if (x >= BOARD_SIZE || y >= BOARD_SIZE) { 
		return 0;
	}

	board[y][x] = p;

	return 1;
}

// Read the full FEN string
int read_FEN(char* fen) {
	int v = 0;
	char* en_passant;

	// TODO: checking if substrings are valid?
	if(fen != NULL) {
		strcpy(FEN.board, strtok(fen, " "));
		FEN.active_color = *strtok(NULL, " ");
		strcpy(FEN.castling, strtok(NULL, " "));
		
		en_passant = strtok(NULL, " ");
		if(en_passant[0] == '-') {
			FEN.en_passant = (coords_t){0, 0};
		}
		else if(en_passant[0] >= 'a' && en_passant[0] <= 'h') {
			// White pawn moved 2 squares
			if(en_passant[1] == '3') {
				FEN.en_passant = (coords_t){en_passant[0] - 'a', 2};
			}
			// Black pawn moved 2 squares
			else if(en_passant[1] == '6') {
				FEN.en_passant = (coords_t){en_passant[0] - 'a', 5};
			}
			// En passant move is invalid
			else {
				return 0;
			}
		}
		// En passant move is invalid
		else {
			return 0;
		}

		FEN.halfmove = atoi(strtok(NULL, " "));
		FEN.fullmove = atoi(strtok(NULL, " "));
		
		v = 1; // String is valid
	}

	if(v) {
		v = FEN_to_board();
	}

	return v;
}

// Turns FEN into a board representation
int FEN_to_board() {	
	char c;
	size_t i = 0;
	size_t x = 0; size_t y = 7;
	size_t t;
	int v = 1; // 1 (true) as long as FEN is valid
	
	do {
		c = FEN.board[i++];

		switch(c) {
			// Newline
			case '/':
				x = 0;
				y--;
				break;
			// Whitespace
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
				for(t = atoi(&c); t > 0; t--) {
					v = set_piece(x++, y, MT);
				}
				break;
			// Pawns
			case 'p':
				v = set_piece(x++, y, BP);
				break;
			case 'P':
				v = set_piece(x++, y, WP);
				break;
			// Rooks
			case 'r':
				v = set_piece(x++, y, BR);
				break;
			case 'R':
				v = set_piece(x++, y, WR);
				break;
			// Knights
			case 'n':
				v = set_piece(x++, y, BN);
				break;
			case 'N':
				v = set_piece(x++, y, WN);
				break;
			// Bishops
			case 'b':
				v = set_piece(x++, y, BB);
				break;
			case 'B':
				v = set_piece(x++, y, WB);
				break;
			// Queens
			case 'q':
				v = set_piece(x++, y, BQ);
				break;
			case 'Q':
				v = set_piece(x++, y, WQ);
				break;
			// Kings
			case 'k':
				v = set_piece(x++, y, BK);
				break;
			case 'K':
				v = set_piece(x++, y, WK);
				break;
			case '\0':
				break;
			// Any other character is not supported
			default:
				v = 0; // Invalid FEN character
		}
	} while (v && c);

	return v;
}

// Print the parsed FEN notation
void print_FEN() {
	wprintf(L"FEN info:\n");
	wprintf(L"Board:\t\t\t%s\n",		FEN.board);
	wprintf(L"Active color:\t\t%c\n",	FEN.active_color);
	wprintf(L"Castling:\t\t%s\n",		FEN.castling);
	wprintf(L"En passant:\t\t%d%d\n",	FEN.en_passant.x, FEN.en_passant.y);
	wprintf(L"Halfmove:\t\t%d\n",		FEN.halfmove);
	wprintf(L"Fullmove:\t\t%d\n",		FEN.fullmove);
	wprintf(L"==========\n");
	print_board();
}

// Print the board in current state
int print_board(){
	wprintf(L" abcdefgh \n");
	for (size_t y = 7; y < BOARD_SIZE; y--) {
		wprintf(L"%d", y+1);
		for(size_t x = 0; x < BOARD_SIZE; x++) {
			// wprint for unicode chesspieces
			wprintf(L"%lc", board[y][x]);
		}
		wprintf(L"%d", y+1);
		wprintf(L"\n");
	}
	wprintf(L" abcdefgh \n");
	return 1;
}
