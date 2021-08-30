#define BOARD_SIZE 8

int run = 1;
char start_fen[] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

// enum of pieces, MT = empty (M-T)
typedef enum {MT = 0x00B7,
	WK = 0x2654, WQ = 0x2655, WR = 0x2656, WB = 0x2657, WN = 0x2658, WP = 0x2659,
	BK = 0x265A, BQ = 0x265B, BR = 0x265C, BB = 0x265D, BN = 0x265E, BP = 0x265F} piece;

// The board the game is played on TODO: piece instead of int
piece board[BOARD_SIZE][BOARD_SIZE];

typedef struct coords_t {
	size_t x; // file on the board
	size_t y; // rank on the board
} coords_t;

typedef struct move_t {
	coords_t from;
	coords_t to;
} move_t;

struct {
	char board[72]; // if every tile has a piece + some extra to account for slashes
	char active_color;
	char castling[5]; // Need 5 chars to include '\0'. Otherwise en_passant gets included in print function
	coords_t en_passant;
	size_t halfmove;
	size_t fullmove;
} FEN;

void quit();
int set_piece(size_t x, size_t y, piece p);
int is_opposite_color(coords_t c);

int print_board();
int read_FEN(char* fen);
void print_FEN();
int FEN_to_board();
int board_to_FEN();

move_t get_move();
void make_move(move_t m);

int is_legal_move(move_t m);
int is_legal_move_king(move_t m);
int is_legal_move_queen(move_t m);
int is_legal_move_rook(move_t m);
int is_legal_move_bishop(move_t m);
int is_legal_move_knight(move_t m);
int is_legal_move_white_pawn(move_t m);
int is_legal_move_black_pawn(move_t m);

void gameloop();
