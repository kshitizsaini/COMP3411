/*********************************************************
 *  agent.c
 *  Nine-Board Tic-Tac-Toe Agent
 *  COMP3411/9414/9814 Artificial Intelligence
 *  Alan Blair, CSE, UNSW
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include "common.h"
#include "agent.h"
#include "game.h"

#define MAX_MOVE 81
// state represents the current board
typedef struct state {
	int board[10][10];  // current board filled with X/O/EMPTY
	int nextboard;     // next playing board
	int previous;     // the board that was played in the previous move
} State;

typedef struct play {
	double heuristic;  // the expected heuristic of the play
	int move;          // the move
} Play;
// the chessboard that is given back to server
int board[10][10];
// the movements
int move[MAX_MOVE+1];
// the current player X or O
int player;
// the number of moves so far
int m;
// the current state
State current;

static int gameover(State s);

static Play play(double value, int move);

static int fullboard(State s);

static double score(int x);

static double estimate(int c[], int control);

static void position(int c[], double *markX, double *markO);

static double calculate(State s, int currentboard, int pos, int currentplayer);

static Play Minimax(State s, int currentplayer, int depth, 
					double alpha, double beta, int maximumdepth);

static int searchdepth(int moves);

// return a new play with the heuristic value and move
static Play play(double value, int move) {
	Play p;
	p.heuristic = value;
	p.move = move;
	return p;
}
// function used to judge fullboard
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
// the estimate function of one 3*3 ceil, considering who's controling the ceil
static double estimate(int c[], int control) {
	int i;
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
	
	// control refers to the board is controled by the x player or the o player
	// control equals to score(0) means x; score(1) means o otherwise, both
	// has no winning or losing shape return 0
	if (win == lost  && lost == 0) return 0;
	// has a x-winning shape
	if (win != 0 && lost == 0) {
		// controled by the x player  
		if (control == score(0)) return 1;
		// controled by the o player
		if (control == score(1)) return 0.2;
		// otherwise
		return 0.5;
	} else if (win == 0 && lost != 0) {
		// has a o-winning shape
		// control by the x player
		if (control == score(0)) return -0.2;
		// controlled by the o player
		if (control == score(1)) return -1;
		// otherwise
		return -0.5;
	}  
	// has an x-winning shape and an o-winning shape
	if (control == score(0)) return 1;
	if (control == score(1)) return -1;
	// if it is controled by neither player randomly decide which one possesed it
	return (rand() % 2 != 0 ? 0.5 : -0.5);
} 
// the position mark for each 3 * 3 ceil
static void position(int c[], double *markX, double *markO) {
	double X = 0.0, O = 0.0;
	// what this function mainly does is that it counts the number of X_EPMTY_EMPTY
	// and O_EMPTY_EMPTY and stores the result in markX, markO each such occupasion
	// would get 0.5 points
	if (c[1] == 0 && c[2] == EMPTY && c[3] == EMPTY) {
		X += 0.5;
	} else if (c[1] == 1 && c[2] == EMPTY && c[3] == EMPTY) {
		O += 0.5;
	}
	
	if (c[1] == EMPTY && c[2] == 0 && c[3] == EMPTY) {
		X += 0.5;
	} else if (c[1] == EMPTY && c[2] == 1 && c[3] == EMPTY) {
		O += 0.5;
	}
	
	if (c[1] == EMPTY && c[2] == EMPTY && c[3] == 0) {
		X += 0.5;
	} else if (c[1] == EMPTY && c[2] == EMPTY && c[3] == 1) {
		O += 0.5;
	}
	
	if (c[4] == 0 && c[5] == EMPTY && c[6] == EMPTY) {
		X += 0.5;
	} else if (c[4] == 1 && c[5] == EMPTY && c[6] == EMPTY) {
		O += 0.5;
	}
	
	if (c[4] == EMPTY && c[5] == 0 && c[6] == EMPTY) {
		X += 0.5;
	} else if (c[4] == EMPTY && c[5] == 1 && c[6] == EMPTY) {
		O += 0.5;
	}
	
	if (c[4] == EMPTY && c[5] == EMPTY && c[6] == 0) {
		X += 0.5;
	} else if (c[4] == EMPTY && c[5] == EMPTY && c[6] == 1) {
		O += 0.5;
	}
	
	if (c[1] == 0 && c[2] == EMPTY && c[3] == EMPTY) {
		X += 0.5;
	} else if (c[1] == 1 && c[2] == EMPTY && c[3] == EMPTY) {
		O += 0.5;
	}
	
	if (c[7] == EMPTY && c[8] == 0 && c[9] == EMPTY) {
		X += 0.5;
	} else if (c[7] == EMPTY && c[8] == 1 && c[9] == EMPTY) {
		O += 0.5;
	}
	
	if (c[7] == EMPTY && c[8] == EMPTY && c[9] == 0) {
		X += 0.5;
	} else if (c[7] == EMPTY && c[8] == EMPTY && c[9] == 1) {
		O += 0.5;
	}
	
	if (c[1] == 0 && c[4] == EMPTY && c[7] == EMPTY) {
		X += 0.5;
	} else if (c[1] == 1 && c[4] == EMPTY && c[7] == EMPTY) {
		O += 0.5;
	}
	
	if (c[1] == EMPTY && c[4] == 0 && c[7] == EMPTY) {
		X += 0.5;
	} else if (c[1] == EMPTY && c[4] == 1 && c[7] == EMPTY) {
		O += 0.5;
	}
	
	if (c[1] == EMPTY && c[4] == EMPTY && c[7] == 0) {
		X += 0.5;
	} else if (c[1] == EMPTY && c[4] == EMPTY && c[7] == 1) {
		O += 0.5;
	}
	
	if (c[2] == 0 && c[5] == EMPTY && c[8] == EMPTY) {
		X += 0.5;
	} else if (c[2] == 1 && c[5] == EMPTY && c[8] == EMPTY) {
		O += 0.5;
	}
	
	if (c[2] == EMPTY && c[5] == 0 && c[8] == EMPTY) {
		X += 0.5;
	} else if (c[2] == EMPTY && c[5] == 1 && c[8] == EMPTY) {
		O += 0.5;
	}
	
	if (c[2] == EMPTY && c[5] == EMPTY && c[8] == 0) {
		X += 0.5;
	} else if (c[2] == EMPTY && c[5] == EMPTY && c[8] == 1) {
		O += 0.5;
	}
	
	if (c[3] == 0 && c[6] == EMPTY && c[9] == EMPTY) {
		X += 0.5;
	} else if (c[3] == 1 && c[6] == EMPTY && c[9] == EMPTY) {
		O += 0.5;
	}
	
	if (c[3] == EMPTY && c[6] == 0 && c[9] == EMPTY) {
		X += 0.5;
	} else if (c[3] == EMPTY && c[6] == 1 && c[9] == EMPTY) {
		O += 0.5;
	}
	
	if (c[3] == EMPTY && c[6] == EMPTY && c[9] == 0) {
		X += 0.5;
	} else if (c[3] == EMPTY && c[6] == EMPTY && c[9] == 1) {
		O += 0.5;
	}
	
	if (c[1] == 0 && c[5] == EMPTY && c[9] == EMPTY) {
		X += 0.5;
	} else if (c[1] == 1 && c[5] == EMPTY && c[9] == EMPTY) {
		O += 0.5;
	}
	
	if (c[1] == EMPTY && c[5] == 0 && c[9] == EMPTY) {
		X += 0.5;
	} else if (c[1] == EMPTY && c[5] == 1 && c[9] == EMPTY) {
		O += 0.5;
	}
	
	if (c[1] == EMPTY && c[5] == EMPTY && c[9] == 0) {
		X += 0.5;
	} else if (c[1] == EMPTY && c[5] == EMPTY && c[9] == 1) {
		O += 0.5;
	}
	
	if (c[3] == 0 && c[5] == EMPTY && c[7] == EMPTY) {
		X += 0.5;
	} else if (c[3] == 1 && c[5] == EMPTY && c[7] == EMPTY) {
		O += 0.5;
	}
	
	if (c[3] == EMPTY && c[5] == 0 && c[7] == EMPTY) {
		X += 0.5;
	} else if (c[3] == EMPTY && c[5] == 1 && c[7] == EMPTY) {
		O += 0.5;
	}
	
	if (c[3] == EMPTY && c[5] == EMPTY && c[7] == 0) {
		X += 0.5;
	} else if (c[3] == EMPTY && c[5] == EMPTY &&c[7] == 1) {
		O += 0.5;
	}
	*markX = *markX + X;
	*markO = *markO + O;
}
// calculate the heuristic
static double calculate(State s, int currentboard, int pos, int currentplayer) {
	int i;
	double hcost = 0.0, singleX = 0.0, singleO = 0.0;
	s.previous = currentboard;
	i = gameover(s);
	if (i != 2) {
		return i * 1.0;
	}
	
	// the position is owned by the !currentplayer
	// the other position is owned by both
	for (i = 1; i <= 9; i++) {
		singleX = 0.0;
		singleO = 0.0;
		position(s.board[i], &singleX, &singleO);
		if (i == pos) {
			// the position that !currentplayer plays next has the most weight
			// 40 marks for the X_X_EMPTY/O_O_EMPTY
			hcost += 40.0 * estimate(s.board[i], score(!currentplayer)); 
			hcost += (singleX + singleO);
		} else {
			// for the other 8 ceils will becomes an estimate
			// 40 marks for the X_X_EMPTY/O_O_EMPTY score and also counts
			// the position scores
			hcost += 20.0 * estimate(s.board[i], score(EMPTY));
			hcost += ((singleX + singleO) * 0.4);
		}
	}
	
	return hcost;
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
// the alpha-beta pruning function using the minimax strategy
static Play Minimax(State s, int currentplayer, int depth, double alpha, double beta, int maximumdepth) {
	Play nextmove = play(0, 0);
	int value = gameover(s);
	if (value != 2) {
		return play(value * 1.0, 0);
	}
	double h = -4096.0, tmp;
	int curr = 0, i;
	// when the maximum depth is breached, evaluate the heuristics and choose
	// the best move
	if (depth >= maximumdepth) {
		for (i = 1; i <= 9; i++) {
			if (s.board[s.nextboard][i] == EMPTY) {
				s.board[s.nextboard][i] = currentplayer;
				if (h == -4096.0) {
					h = calculate(s, s.nextboard, i, currentplayer);
					curr = i;
				} else {
					tmp = calculate(s, s.nextboard, i, currentplayer);
					if (currentplayer == 0 && tmp > h) {
						h = tmp;
						curr = i;
					} else if (currentplayer != 0 && tmp < h){
						h = tmp;
						curr = i;
					}
				}
				s.board[s.nextboard][i] = EMPTY;
			}
		}
		return play(h, curr);
	}
	// otherwise execute the alpha-beta
	if (currentplayer == 0) {
		h = -4096.0;
		for (i = 1; i <= 9; i++) {
			if (s.board[s.nextboard][i] == EMPTY) {
				State next = s;
				next.board[s.nextboard][i] = currentplayer;
				next.previous = s.nextboard;
				next.nextboard = i;
				Play t = Minimax(next, !currentplayer, depth + 1, alpha, beta, maximumdepth);
				
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
	} else {
		h = 4096.0;
		for (i = 1; i <= 9; i++) {
			if (s.board[s.nextboard][i] == EMPTY) {
				State next = s;
				next.board[s.nextboard][i] = currentplayer;
				next.previous = s.nextboard;
				next.nextboard = i;
				Play t = Minimax(next, !currentplayer, depth + 1, alpha, beta, maximumdepth);
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
 	m = 0;
	move[m] = 0;
    reset_board(current.board);
    current.nextboard = 0;
    current.previous = 0;
    player = this_player;
    // printf("%d\n", player);
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
    m = 2;
    /*
    do {
      this_move = 1 + random()% 9;
    } while( board[prev_move][this_move] != EMPTY );*/
    Play next = Minimax(current, player,0, -4096.0, 4096.0, 8);
    this_move = next.move;
    move[m] = this_move;
    board[prev_move][this_move] = player;
    current.board[prev_move][this_move] = player;
    current.previous = current.nextboard;
    current.nextboard = this_move;
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
    board[board_num][first_move] =  player;
    board[first_move][prev_move] = !player;
    current.board[board_num][first_move] = player;
    current.board[first_move][prev_move] = !player;
    current.nextboard = prev_move;
    m=3;/*
    do {
      this_move = 1 + random()% 9;
    } while( board[prev_move][this_move] != EMPTY );*/
    Play next = Minimax(current, player,0, -4096.0, 4096.0, 8);
    this_move = next.move;
    move[m] = this_move;
    board[move[m-1]][this_move] = player;
    current.board[move[m-1]][this_move] = player;
    current.previous = current.nextboard;
    current.nextboard = this_move;
    return( this_move );
}
// pass in the number of moves so far 
// and return a reasonable search depth
static int searchdepth(int moves) {
	if (moves <= 14) return 8; // less than 14 moves
	if (moves <= 27) return 9;
	if (moves <= 36) return 10;
	if (moves <= 45) return 11;
	if (moves <= 54) return 13;
	return 17;
}

/*********************************************************//*
   Choose next move and return it
*/
int agent_next_move( int prev_move )
{
	  int this_move;
	  m++;
	  move[m] = prev_move;
	  board[move[m-1]][move[m]] = !player;
	  current.board[move[m-1]][move[m]] = !player;
	  current.previous = current.nextboard;
	  current.nextboard = prev_move;
	  m++;/*
	  do {
		this_move = 1 + random()% 9;
	  } while( board[prev_move][this_move] != EMPTY );*/
	  Play next = Minimax(current, player, 0,  -4096.0, 4096.0, searchdepth(m));
	  this_move = next.move;
	  // if we are going to lose anyway play a relative safe move such that 
	  // we will not lose directly
	  // if our opponent make mistakes we still have chance
	  if (next.heuristic == score(!player) * 4000) {
	  	// printf("random move\n");
	  	next = Minimax(current, player, 0,  -4096.0, 4096.0, 2);
	  	this_move = next.move;
	  }
	  move[m] = this_move;
	  board[move[m-1]][this_move] = player;
	  current.board[move[m-1]][this_move] = player;
	  current.previous = current.nextboard;
	  current.nextboard = this_move;

	  // print_board(stdout, board, prev_move,  this_move);
	  // printf("%lf\n", calculate(current, this_move));
	  // printf("heuristic = %lf\n", next.heuristic);
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
