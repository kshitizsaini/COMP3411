/*********************************************************
 *  randt9.c
 *  Nine-Board Tic-Tac-Toe Random Player
 *  COMP3411/9414/9814 Artificial Intelligence
 *  modified by z5173587 based on the original code
 */
/*
	This code implements the alpha-beta search algorithm with a maximum depth
	of 9 to 17 depending on the moves.
	It used an array of size 9 to evaluate the status of each of the 9 big cells,
	which include UNDETERMINE, X_ADVANTAGE, O_ADVANTAGE, X_WIN, O_WIN, X_POS, O_POS.
	And for all of these status we could bitwise or them to determine the overall status of the boards.
	For example if status = X_ADVANTAGE | O_WIN means that this board has an
	X_EMPTY_EMPTY type shape and an O_O_EMPTY type shape.
	Then when the maximum searching depth is reached or the gameover status is reached
	simply return the heuristics evaluation based on the status of the 9 cells.
	For the evaluation:
		 9 points for the X_ADVANTAGE/O_ADVANTAGE
		 3 points for X_POS/O_POS 
		 30 points for a X_WIN/O_WIN
		 4000 points for a final win
		 0 points for a draw
	And when the maximum depth is reached the heuristic of the 9 board is the weighted
	summation of the heuristics of each 3*3 board.
	To make a feasible offensive play, the searching depth gradually increace:
		move 1-11: depth = 9
		move 12-23: depth = 11
		moves 24-29 or we have 4 subboards has X_WIN or O_WIN status: depth = 13
		moves 30-47: depth = 15
		moves 48-81: depth = 17
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "common.h"
#include "agent.h"
#include "game.h"

#define MAX_MOVE 81
/* 5 possiple state of a board 
the reason we design 0/1/2/4/8/16/32
is we can bitwise or them and when calculating the heuristic we simply use the
& operation to see if a board has this type of property*/
#define UNDETERMINE 0  // the board has no 2 x/o in a row and same position value
#define X_ADVANTAGE 1 // the board has no 2 x/o in a row and high x position value
#define O_ADVANTAGE 2 // the board has no 2 x/o in a row and high o position value
#define X_WIN 4 // the board has 2 x 
#define O_WIN 8 // the board has 2 o
#define X_POS 16 // X has some position advantage in the cell
#define O_POS 32 // O has some position advantage in the cell
/*
the board evaluation marks 
*/
#define TWOS 30.0
#define CURRENTTWOS 400.0
#define ONES 9.0  
#define POS  3.0
#define ONESL 5.0
#define POSL  2.0
typedef struct state {
	int board[10][10];  // current board filled with X/O/EMPTY
	int status[10];   // the status of the 9 ceils
	int nextboard;   // the board that should be played next
	int previous;     // the board that was played in the previous move	
} State;

typedef struct play {
	double heuristic;  // the expected heuristic of the play
	int move;          // the move
} Play;
// the 2D board that passed back to the client.c
int board[10][10];
int move[MAX_MOVE+1];
int player;
int m;

// the current board
State current;
// the function to determine the searching depth
static int searchdepth(int moves);
// the function to judge fullboard
static int fullboard(State s);
// the function to generate a play
static Play play(double value, int move);
// the belonging to X/O
static double score(int x);
// the estimate heuristic for each 3*3 small board
static int estimate(int c[]);
// the position heuristic calculation X_EMPTY_EMPTY/O_EMPTY_EMPTY type relation
static void position(int c[], double *markX, double *markO);
// judge if the game ends
static int gameover(State s);
// the function minimax
static Play Minimax(State s, int currentplayer, int depth, 
								double alpha, double beta, int maximumdepth);

// return a new play with the heuristic value and move
static Play play(double value, int move) {
	Play p;
	p.heuristic = value;
	p.move = move;
	return p;
}

// function used to judge fullboard
// just iterate through the current board and see if the 3*3 subboard is full
static int fullboard(State s) {
	int i;
	for (i = 1; i <= 9; i++) {
		if (s.board[s.nextboard][i] == EMPTY) {
			return 0;
		}
	}
	return 1;	
}

// the score of the current small 1*1 sell, 1 for X, -1 for O and 0 for EMPTY
static double score(int x) {
	if (x == 0) return 1.0;
	if (x == EMPTY) return 0;
	return -1.0;
}

static void position(int c[], double *markX, double *markO) {
	double X = 0.0, O = 0.0;
	// what this function mainly does is that it counts the number of X_EPMTY_EMPTY
	// and O_EMPTY_EMPTY and stores the result in markX, markO each such occupasion
	// would get 0.5 points
	// we simply use if-else statements to brute force all possibilities
	// only 24 possible combinations
	if (c[1] == 0 && c[2] == EMPTY && c[3] == EMPTY) {
		X += 0.5;
	} 
	
    if (c[1] == 1 && c[2] == EMPTY && c[3] == EMPTY) {
		O += 0.5;
	}
	
	if (c[1] == EMPTY && c[2] == 0 && c[3] == EMPTY) {
		X += 0.5;
	}
	
	if (c[1] == EMPTY && c[2] == 1 && c[3] == EMPTY) {
		O += 0.5;
	}
	
	if (c[1] == EMPTY && c[2] == EMPTY && c[3] == 0) {
		X += 0.5;
	} 
	
	if (c[1] == EMPTY && c[2] == EMPTY && c[3] == 1) {
		O += 0.5;
	}
	// 4,5,6
	if (c[4] == 0 && c[5] == EMPTY && c[6] == EMPTY) {
		X += 0.5;
	}
	
	if (c[4] == 1 && c[5] == EMPTY && c[6] == EMPTY) {
		O += 0.5;
	}
	
	if (c[4] == EMPTY && c[5] == 0 && c[6] == EMPTY) {
		X += 0.5;
	}
	
	if (c[4] == EMPTY && c[5] == 1 && c[6] == EMPTY) {
		O += 0.5;
	}
	
	if (c[4] == EMPTY && c[5] == EMPTY && c[6] == 0) {
		X += 0.5;
	} 
	
	if (c[4] == EMPTY && c[5] == EMPTY && c[6] == 1) {
		O += 0.5;
	}
	// 7,8,9
	if (c[7] == 0 && c[8] == EMPTY && c[9] == EMPTY) {
		X += 0.5;
	}
	
	if (c[7] == 1 && c[8] == EMPTY && c[9] == EMPTY) {
		O += 0.5;
	}
	
	if (c[7] == EMPTY && c[8] == 0 && c[9] == EMPTY) {
		X += 0.5;
	}
	
	if (c[7] == EMPTY && c[8] == 1 && c[9] == EMPTY) {
		O += 0.5;
	}
	
	if (c[7] == EMPTY && c[8] == EMPTY && c[9] == 0) {
		X += 0.5;
	}
	
	if (c[7] == EMPTY && c[8] == EMPTY && c[9] == 1) {
		O += 0.5;
	}
	// 1,4,7
	if (c[1] == 0 && c[4] == EMPTY && c[7] == EMPTY) {
		X += 0.5;
	} 
	
	if (c[1] == 1 && c[4] == EMPTY && c[7] == EMPTY) {
		O += 0.5;
	}
	
	if (c[1] == EMPTY && c[4] == 0 && c[7] == EMPTY) {
		X += 0.5;
	}
	
	if (c[1] == EMPTY && c[4] == 1 && c[7] == EMPTY) {
		O += 0.5;
	}
	
	if (c[1] == EMPTY && c[4] == EMPTY && c[7] == 0) {
		X += 0.5;
	}
	
	if (c[1] == EMPTY && c[4] == EMPTY && c[7] == 1) {
		O += 0.5;
	}
	// 2,5,8
	if (c[2] == 0 && c[5] == EMPTY && c[8] == EMPTY) {
		X += 0.5;
	} 
	
	if (c[2] == 1 && c[5] == EMPTY && c[8] == EMPTY) {
		O += 0.5;
	}
	
	if (c[2] == EMPTY && c[5] == 0 && c[8] == EMPTY) {
		X += 0.5;
	}
	
    if (c[2] == EMPTY && c[5] == 1 && c[8] == EMPTY) {
		O += 0.5;
	}
	
	if (c[2] == EMPTY && c[5] == EMPTY && c[8] == 0) {
		X += 0.5;
	}
	
	if (c[2] == EMPTY && c[5] == EMPTY && c[8] == 1) {
		O += 0.5;
	}
	// 3,6,9
	if (c[3] == 0 && c[6] == EMPTY && c[9] == EMPTY) {
		X += 0.5;
	} 
	
	if (c[3] == 1 && c[6] == EMPTY && c[9] == EMPTY) {
		O += 0.5;
	}
	
	if (c[3] == EMPTY && c[6] == 0 && c[9] == EMPTY) {
		X += 0.5;
	} 
	
	if (c[3] == EMPTY && c[6] == 1 && c[9] == EMPTY) {
		O += 0.5;
	}
	
	if (c[3] == EMPTY && c[6] == EMPTY && c[9] == 0) {
		X += 0.5;
	}
	
	if (c[3] == EMPTY && c[6] == EMPTY && c[9] == 1) {
		O += 0.5;
	}
	// 1,5,9
	if (c[1] == 0 && c[5] == EMPTY && c[9] == EMPTY) {
		X += 0.5;
	} 
	
	if (c[1] == 1 && c[5] == EMPTY && c[9] == EMPTY) {
		O += 0.5;
	}
	
	if (c[1] == EMPTY && c[5] == 0 && c[9] == EMPTY) {
		X += 0.5;
	} 
	
	if (c[1] == EMPTY && c[5] == 1 && c[9] == EMPTY) {
		O += 0.5;
	}
	
	if (c[1] == EMPTY && c[5] == EMPTY && c[9] == 0) {
		X += 0.5;
	} 
	
	if (c[1] == EMPTY && c[5] == EMPTY && c[9] == 1) {
		O += 0.5;
	}
	// 3,5,7
	if (c[3] == 0 && c[5] == EMPTY && c[7] == EMPTY) {
		X += 0.5;
	} 
	
	if (c[3] == 1 && c[5] == EMPTY && c[7] == EMPTY) {
		O += 0.5;
	}
	
	if (c[3] == EMPTY && c[5] == 0 && c[7] == EMPTY) {
		X += 0.5;
	}
	
    if (c[3] == EMPTY && c[5] == 1 && c[7] == EMPTY) {
		O += 0.5;
	}
	
	if (c[3] == EMPTY && c[5] == EMPTY && c[7] == 0) {
		X += 0.5;
	} 
	if (c[3] == EMPTY && c[5] == EMPTY &&c[7] == 1) {
		O += 0.5;
	}
	*markX = X;
	*markO = O;
}
// this function use bitwise operation to return an integer of the current
// board status let's say a board has both X win and O win potential the
// status is X_WIN | O_WIN
static int estimate(int c[]) {
	int i;
	int status = UNDETERMINE;
	double posX = 0.0, posO = 0.0;
	
	// evaluate the position marks
	position(c, &posX, &posO);
	// counting the X_X_EMPTY and O_O_EMPTY pair stores in win and lost
	int win = 0, lost = 0;
	double tmp = 0.0;
	// cols
	for (i = 1; i <= 3; i++) {
		tmp = 0.0;
		tmp = tmp + score(c[i]) + score(c[i+3]) + score(c[i+6]);
		if (tmp == 2.0 * score(0)) {
			win++;
		} else if (tmp == 2.0 * score(1)) {
			lost++;
		}
	}
	// rows
	for (i = 1; i <= 9; i+= 3) {
		tmp = 0.0;
		tmp = tmp + score(c[i]) + score(c[i+1]) + score(c[i+2]);
		if (tmp == 2.0 * score(0)) {
			win++;
		} else if (tmp == 2.0 * score(1)) {
			lost++;
		}
	}
	// two diagonals
	tmp = 0.0;
	tmp = tmp + score(c[1]) + score(c[5]) + score(c[9]);
	if (tmp == 2.0 * score(0)) {
			win++;
	} else if (tmp == 2.0 * score(1)) {
			lost++;
	}
	
	tmp = 0.0;
	tmp = tmp + score(c[3]) + score(c[5]) + score(c[7]);
	if (tmp == 2.0 * score(0)) {
			win++;
	} else if (tmp == 2.0 * score(1)) {
			lost++;
	}
	// we use bitwise operation to determine the status of the current ceil
	if (win != 0) {
		status = status | X_WIN;
	}
	
	if (lost != 0) {
		status = status | O_WIN;
	}
	
	if (posX != 0) {
		status = status | X_ADVANTAGE;
	}
	
	if (posO != 0) {
		status = status | O_ADVANTAGE;
	}
	
	if (posX != 0 && posO != 0 && posX > posO) {
		status = status | X_POS;
	}
	
	if (posX != 0 && posO != 0 && posX < posO) {
		status = status | O_POS;
	}
	
	return status;
} 

// if the game is over return 0 or -4000 or 4000 otherwise return 2
static int gameover(State s) {
	// the first move will never win
	if (s.previous == 0) {
		return 2;
	}
	// fullboard
	if (fullboard(s)) {
		return 0;
	}
	// game end by 3 in a row
	int current = 0, i;
	for (i = 1; i <= 9; i = i + 3) {
		current = 0;
		current = current + score(s.board[s.previous][i]) 
			+ score(s.board[s.previous][i+1]) + score(s.board[s.previous][i+2]);
		if (current == 3) {
			return 4000;
		} else if (current == -3) {
			return -4000;
		}
	}
	// game end by 3 in a col
	for (i = 1; i <= 3; i++) {
		current = 0;
		current = current + score(s.board[s.previous][i]) 
			+ score(s.board[s.previous][i+3]) + score(s.board[s.previous][i+6]);
		if (current == 3) {
			return 4000;
		} else if (current == -3) {
			return -4000;
		}
	}
	// game end by the 2 diagonals
	current = score(s.board[s.previous][1]) 
			+ score(s.board[s.previous][5]) + score(s.board[s.previous][9]);
	if (current == 3) {
		return 4000;
	} else if (current == -3) {
		return -4000;
	}
	
	current = score(s.board[s.previous][3]) 
			+ score(s.board[s.previous][5]) + score(s.board[s.previous][7]);
	if (current == 3) {
		return 4000;
	} else if (current == -3) {
		return -4000;
	}
	// the game is still running
	return 2;
}

double calculate(State s, int currentplayer) {
	int i = 0, j = 0;
	double posmark = ONES, advmark = POS, mark = 0.0;
	/*
	#define TWOS 30.0
	#define CURRENTTWOS 400.0
	#define ONES 7.0
	#define POS  2.0
	#define ONESL 2.0
	#define POSL  1.0
	*/
	for (i = 1; i <= 9; i++) {
		if ((s.status[i] & X_WIN) != 0 || (s.status[i] & O_WIN) != 0) {
			j++;
		}
	}
	
	if (j >= 4) {
		posmark = ONESL;
	    advmark = POSL;
	}
	// this shows the advantage of calculating the heuristic with bitwise or
	// since we can bitwise and to see if a board has a type of property without
	// many if-else statements
	for (i = 1; i <= 9; i++) {
		if (i != s.nextboard) {
			mark += ((X_WIN & s.status[i]) != 0) * TWOS * score(0);
			mark += ((O_WIN & s.status[i]) != 0) * TWOS * score(1);
		} else {
		
			if (currentplayer == 0) {
				mark += ((X_WIN & s.status[i]) != 0) * CURRENTTWOS * score(0);
				mark += ((O_WIN & s.status[i]) != 0) * TWOS * score(1);
			} else {
				mark += ((X_WIN & s.status[i]) != 0) * TWOS * score(0);
				mark += ((O_WIN & s.status[i]) != 0) * CURRENTTWOS * score(1);
			}
		}
		
		mark += ((X_ADVANTAGE & s.status[i]) != 0) * posmark * score(0);
		mark += ((X_POS & s.status[i]) != 0) * advmark * score(0);
		mark += ((O_ADVANTAGE & s.status[i]) != 0) * posmark * score(1);
		mark += ((O_POS & s.status[i]) != 0) * advmark * score(1);
	}
	return mark;
}

// the alpha-beta pruning function using the minimax strategy
static Play Minimax(State s, int currentplayer, int depth, 
								double alpha, double beta, int maximumdepth) {
	// k++;
	Play nextmove = play(0, 0);
	int value = gameover(s);
	if (value != 2) {
		return play(value * 1.0, 0);
	}
	// fix the status of the previous played board
	s.status[s.previous] = estimate(s.board[s.previous]);
	double h = -4096.0;
	int curr = 0, i;
	// when the maximum depth is breached, evaluate the heuristics and choose
	// the best move
	if (depth >= maximumdepth) {
		h = calculate(s, currentplayer);	
		return play(h, curr);
	}
	// otherwise execute the alpha-beta
	// maximizer move
	if (currentplayer == 0) {
		h = -4096.0;   // the direct killer move
		if ((s.status[s.nextboard] & X_WIN) != 0) {
			for (i = 1; i <= 9; i++) {
				if (s.board[s.nextboard][i] == EMPTY) {
					State next = s;
					next.board[s.nextboard][i] = currentplayer;
					next.previous = s.nextboard;
					next.nextboard = i;
					if (gameover(next) == 4000) {
					Play t = Minimax(next, !currentplayer, 
										depth + 1, alpha, beta, maximumdepth);
						h = t.heuristic;
						nextmove = play(h, i);
						break;
					}
				} 
			}
		} else {
			for (i = 1; i <= 9; i++) {
				if (s.board[s.nextboard][i] == EMPTY ) {
					State next = s;
					next.board[s.nextboard][i] = currentplayer;
					next.previous = s.nextboard;
					next.nextboard = i;
					Play t = Minimax(next, !currentplayer, 
										depth + 1, alpha, beta, maximumdepth);
				
					if (t.heuristic > h) {
						h = t.heuristic;
						nextmove = play(h, i);
					}
				
					if (alpha <= h) {
						alpha = h;
					}
					// cut-off
					if (alpha >= beta || alpha >= 3990) {
						return nextmove;
					}
				} 
			}
		}
	} else {  // minimizer move
		h = 4096.0;  // the direct killer move
		if ((s.status[s.nextboard] & O_WIN) != 0) {
			for (i = 1; i <= 9; i++) {
				if (s.board[s.nextboard][i] == EMPTY) {
					State next = s;
					next.board[s.nextboard][i] = currentplayer;
					next.previous = s.nextboard;
					next.nextboard = i;
					if (gameover(next) == -4000) {
						Play t = Minimax(next, !currentplayer, 
										depth + 1, alpha, beta, maximumdepth);
						h = t.heuristic;
						nextmove = play(h, i);
						break;
					}
				} 
			}
		} else {
			
			for (i = 1; i <= 9; i++) {
				if (s.board[s.nextboard][i] == EMPTY) {
					State next = s;
					next.board[s.nextboard][i] = currentplayer;
					next.previous = s.nextboard;
					next.nextboard = i;
					Play t = Minimax(next, !currentplayer, depth + 1, 
													alpha, beta, maximumdepth);
					if (t.heuristic < h) {
						h = t.heuristic;
						nextmove = play(h, i);
					}
				
					if (beta >= h) {
						beta = h;
					}
					// cut-off
					if (alpha >= beta || beta <= -3990) {
						return nextmove;
					}
				} 
			}
		}
	}
	return nextmove;
}

/*********************************************************//*
   Print usage information and exit
*/
void usage( char argv0[] )
{
  printf("Usage: %s\n",argv0);
  printf("       [-p port]\n"); // tcp port
  printf("       [-h host]\n"); // tcp host
  exit(1);
}

/*********************************************************//*
   Parse command-line arguments
*/
void agent_parse_args( int argc, char *argv[] )
{
  int i=1;
  while( i < argc ) {
    if( strcmp( argv[i], "-p" ) == 0 ) {
      if( i+1 >= argc ) {
        usage( argv[0] );
      }
      port = atoi(argv[i+1]);
      i += 2;
    }
    else if( strcmp( argv[i], "-h" ) == 0 ) {
      if( i+1 >= argc ) {
        usage( argv[0] );
      }
      host = argv[i+1];
      i += 2;
    }
    else {
      usage( argv[0] );
    }
  }
}

/*********************************************************//*
   Called at the beginning of a series of games
*/
void agent_init()
{
  struct timeval tp;

  // generate a new random seed each time
  gettimeofday( &tp, NULL );
  srandom(( unsigned int )( tp.tv_usec ));
}

/*********************************************************//*
   Called at the beginning of each game
*/
void agent_start( int this_player )
{
	  reset_board( board );
	  reset_board(current.board);
	  m = 0;
	  move[m] = 0;
	  current.nextboard = 0;
	  current.previous = 0;
	  int i;
	  for (i = 0 ; i < 10; i++) {
	  	current.status[i] = UNDETERMINE;
	  }
	  player = this_player;
}

/*********************************************************//*
   Choose second move and return it
*/
int agent_second_move( int board_num, int prev_move )
{
	  int this_move;
	  move[0] = board_num;
	  move[1] = prev_move;
	  board[board_num][prev_move] = !player;
	  current.previous = prev_move;
      current.nextboard = board_num;
      current.board[board_num][prev_move] = !player;
	  /*
	  	the board_num board heuristic was changed we need to fix it
	  */
	  
	  current.status[board_num] = estimate(current.board[board_num]);
	  
	  m = 2;
	  Play next = Minimax(current, player,0, -4096.0, 4096.0, 9);
      this_move = next.move;
      move[m] = this_move;
      board[prev_move][this_move] = player;
      current.board[prev_move][this_move] = player;
      current.previous = current.nextboard;
      current.nextboard = this_move;
      /*
      	After this move the status of the board is changed
      */
	  current.status[current.previous] = estimate(current.board[current.previous]);
	  return( this_move );
}

/*********************************************************//*
   Choose third move and return it
*/
int agent_third_move(
                     int board_num,
                     int first_move,
                     int prev_move
                    )
{
		int this_move;
		move[0] = board_num;
		move[1] = first_move;
		move[2] = prev_move;
		current.status[board_num] = estimate(current.board[board_num]);
		current.status[first_move] = estimate(current.board[first_move]);
		current.status[prev_move] = estimate(current.board[prev_move]);
		board[board_num][first_move] =  player;
		board[first_move][prev_move] = !player;
		current.board[board_num][first_move] = player;
		current.board[first_move][prev_move] = !player;
		current.nextboard = prev_move;
		m=3;
		Play next = Minimax(current, player,0, -4096.0, 4096.0, 9);
		this_move = next.move;
		move[m] = this_move;
		board[move[m-1]][this_move] = player;
		current.board[move[m-1]][this_move] = player;
		current.previous = current.nextboard;
		current.status[current.previous] 
									= estimate(current.board[current.previous]);
		current.nextboard = this_move;
		return( this_move );
}

// pass in the number of moves so far 
// and return a reasonable search depth
// remember we only want odd depth otherwise we are forcasting
// the opponent's heuristics which doesn't make sence
static int searchdepth(int moves) {
	// simple measures if the current state is simple enough which counts
	// if at least 4 X_X/O_O exists in all boards
	int simple = 0;
	int count = 0, i;
	for (i = 1; i <= 9; i++) {
		if ((current.status[i] & X_WIN) != 0) {
			count++;
		} else if ((current.status[i] & O_WIN) != 0) {
			count++;
		}
	}
	
	if (count >= 4) simple = 1;
	// printf("%d\n", simple);
	if (moves <= 11){ 
		if (simple == 1) return 11;
		return 9;
	}  // less than 12 moves
	if (moves <= 29) {
		if (simple == 1 && count == 4) return 13; // current state is very simple
		if (count >= 5) return 15;
		if (moves <= 23) return 11; // 12 - 29 moves
		return 13;
	}
	if (moves <= 43) return 15; // 30 - 47 moves
	return 17; // 48 - 81 moves
}

/*********************************************************//*
   Choose next move and return it
*/
int agent_next_move( int prev_move )
{
	  // k = 0;
	  int this_move, i;
	  m++;
	  move[m] = prev_move;
	  board[move[m-1]][move[m]] = !player;
	  current.board[move[m-1]][move[m]] = !player;
	  current.previous = current.nextboard;
	  current.nextboard = prev_move;
	  current.status[current.previous] = estimate(current.board[current.previous]);
	  m++;
	  
	  /*
	  	use minimax/alpha-beta to determine the move and if we forcast we will lose
	  	we should hold on not to lose directly unless we have no other choice
	  */
	  Play next = Minimax(current, player, 0,  -4096.0, 4096.0, searchdepth(m));
	  if (next.heuristic == 4000.0 * score(!player)) {
	  		for (i = 1; i <= 9; i++) {
	  			if (current.board[current.nextboard][i] == EMPTY) {
	  				if (player == 0) {
	  					if ((current.status[i] & O_WIN) == 0) {
	  						next.move = i;
	  						break;
	  					}
	  				} else {
	  					if ((current.status[i] & X_WIN) == 0) {
	  						next.move = i;
	  						break;
	  					}
	  				}
	  			}
	  		}
	  }
	  this_move = next.move;
	  move[m] = this_move;
	  board[move[m-1]][this_move] = player;
	  current.board[move[m-1]][this_move] = player;
	  current.previous = current.nextboard;
	  current.nextboard = this_move;
	  current.status[current.previous] = estimate(current.board[current.previous]);
	  // print_board(stdout, board, prev_move,  this_move);
	  // printf("heuristic = %lf\n", next.heuristic);
	  // printf("%d\n", k);
	  // printf("%d\n", this_move);
	  return( this_move );
}


/*********************************************************//*
   Receive last move and mark it on the board
*/
void agent_last_move( int prev_move )
{
	  m++;
	  move[m] = prev_move;
	  board[move[m-1]][move[m]] = !player;
	  current.board[move[m-1]][move[m]] = !player;
	  current.previous = current.nextboard;
	  current.status[current.previous] = estimate(current.board[current.previous]);
	  current.nextboard = move[m];
}

/*********************************************************//*
   Called after each game
*/
void agent_gameover(
                    int result,// WIN, LOSS or DRAW
                    int cause  // TRIPLE, ILLEGAL_MOVE, TIMEOUT or FULL_BOARD
                   )
{
  // nothing to do here
}

/*********************************************************//*
   Called after the series of games
*/
void agent_cleanup()
{
  // nothing to do here
}
