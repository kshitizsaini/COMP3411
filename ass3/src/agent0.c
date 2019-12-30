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

#include "common.h"
#include "agent.h"
#include "game.h"

#define MAX_MOVE 81

typedef struct state {
	int board[10][10];
	int nextboard;
	int previous;
} State;

typedef struct play {
	double heuristic;
	int move;
} Play;

int board[10][10];
int move[MAX_MOVE+1];
int player;
int m;

State current;

Play play(double value, int move) {
	Play p;
	p.heuristic = value;
	p.move = move;
	return p;
}

static int fullboard(State s) {
	int i;
	for (i = 1; i <= 9; i++) {
		if (s.board[s.nextboard][i] == EMPTY) {
			return 0;
		}
	}
	return 1;	
}

static double score(int x) {
	if (x == 0) return 1.0;
	if (x == EMPTY) return 0;
	return -1.0;
}

static double estimate(int c[], int currentplayer) {
	int i;
	int cost = 0, current = 0, product = 1;
	// calculate the cost of each row
	for (i = 1; i <= 9; i = i + 3) {
		current = 0;
		product = 1;
		current = current + score(c[i]) 
			+ score(c[i+1]) + score(c[i+2]);
		product = product * score(c[i]) 
			* score(c[i+1]) * score(c[i+2]);
		if ((current != -1 && current != 1) || product == 0) {
			cost = cost + current;
		}  
	}	
	
	// calculate the cost of each col
	for (i = 1; i <= 3; i++) {
		current = 0;
		product = 1;
		current = current + score(c[i]) 
			+ score(c[i+3]) + score(c[i+6]);
		product = product * score(c[i]) 
			* score(c[i+3]) * score(c[i+6]);
		if ((current != -1 && current != 1) || product == 0) {
			cost = cost + current;
		}  
	}	
	
	current = 0;
	product = 1;
	current = current + score(c[1]) 
			+ score(c[5]) + score(c[9]);
	product = product * score(c[1]) 
			* score(c[5]) * score(c[9]);
	if ((current != -1 && current != 1) || product == 0) {
		cost = cost + current;
	} 
	
	current = 0;
	product = 1;
	current = current + score(c[3]) 
			+ score(c[5]) + score(c[7]);
	product = product * score(c[3]) 
			* score(c[5]) * score(c[7]);
	if ((current != -1 && current != 1) || product == 0) {
		cost = cost + current;
	}  
	return cost * 1.0;
} 

double calculate(State s, int currentplayer) {
	int i;
	double cost = 0, pcost = 0;
	int total = 0;
	for (i = 1; i <= 9; i++) {
		if (s.board[s.nextboard][i] != EMPTY) {
			total++;
			cost = cost + estimate(s.board[i], !currentplayer);
		} else {
			pcost = pcost + estimate(s.board[i], !currentplayer);
		}
	}
	if (s.board[s.nextboard][s.nextboard] != EMPTY) {
		cost = cost + estimate(s.board[s.nextboard], currentplayer);
	}
	return cost + total / 2.0;
}
// if the game is over return 0 or -12 or 12 otherwise return 2
int gameover(State s) {
	// the first move will never win
	if (s.previous == 0) {
		return 2;
	}
	if (fullboard(s)) {
		return 0;
	}
	
	int current = 0, i;
	for (i = 1; i <= 9; i = i + 3) {
		current = 0;
		current = current + score(s.board[s.previous][i]) 
			+ score(s.board[s.previous][i+1]) + score(s.board[s.previous][i+2]);
		if (current == 3) {
			return 128;
		} else if (current == -3) {
			return -128;
		}
	}
	
	for (i = 1; i <= 3; i++) {
		current = 0;
		current = current + score(s.board[s.previous][i]) 
			+ score(s.board[s.previous][i+3]) + score(s.board[s.previous][i+6]);
		if (current == 3) {
			return 128;
		} else if (current == -3) {
			return -128;
		}
	}
	
	current = score(s.board[s.previous][1]) 
			+ score(s.board[s.previous][5]) + score(s.board[s.previous][9]);
	if (current == 3) {
		return 128;
	} else if (current == -3) {
		return -128;
	}
	
	current = score(s.board[s.previous][3]) 
			+ score(s.board[s.previous][5]) + score(s.board[s.previous][7]);
	if (current == 3) {
		return 128;
	} else if (current == -3) {
		return -128;
	}
	return 2;
}

static Play Minimax(State s, int currentplayer, int depth, double alpha, double beta) {
	Play nextmove = play(0, 0);
	int value = gameover(s);
	if (value != 2) {
		return play(value, 0);
	}
	double h = -1024.0, tmp;
	int curr = 0, i;
	// make an estimate of the current situation
	if (depth >= 9) {
		for (i = 1; i <= 9; i++) {
			if (s.board[s.nextboard][i] == EMPTY) {
				s.board[s.nextboard][i] = currentplayer;
				if (h == -1024.0) {
					h = calculate(s, currentplayer);
					curr = i;
				} else {
					tmp = calculate(s, currentplayer);
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
	
	if (currentplayer == 0) {
		h = -1024.0;
		for (i = 1; i <= 9; i++) {
			if (s.board[s.nextboard][i] == EMPTY) {
				State next = s;
				next.board[s.nextboard][i] = currentplayer;
				next.previous = s.nextboard;
				next.nextboard = i;
				Play t = Minimax(next, !currentplayer, depth + 1, alpha, beta);
				if (t.heuristic > h) {
					h = t.heuristic;
					nextmove = play(h, i);
				}
				
				if (alpha <= h) {
					alpha = h;
				}
				
				if (alpha >= beta || alpha >= 127) {
					return nextmove;
				}
			} 
		}
	} else {
		h = 1024.0;
		for (i = 1; i <= 9; i++) {
			if (s.board[s.nextboard][i] == EMPTY) {
				State next = s;
				next.board[s.nextboard][i] = currentplayer;
				next.previous = s.nextboard;
				next.nextboard = i;
				Play t = Minimax(next, !currentplayer, depth + 1, alpha, beta);
				if (t.heuristic < h) {
					h = t.heuristic;
					nextmove = play(h, i);
				}
				
				if (beta >= h) {
					beta = h;
				}
				
				if (alpha >= beta || beta <= -127) {
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
  printf("%d\n", player);
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
  Play next = Minimax(current, player,0, -1024.0, 1024.0);
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
  Play next = Minimax(current, player,0, -1024.0, 1024.0);
  this_move = next.move;
  move[m] = this_move;
  board[move[m-1]][this_move] = player;
  current.board[move[m-1]][this_move] = player;
  current.previous = current.nextboard;
  current.nextboard = this_move;
  return( this_move );
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
  Play next = Minimax(current, player, 0,  -1024.0, 1024.0);
  this_move = next.move;
  move[m] = this_move;
  board[move[m-1]][this_move] = player;
  current.board[move[m-1]][this_move] = player;
  current.previous = current.nextboard;
  current.nextboard = current.previous;
  print_board(stdout, board, prev_move,  this_move);
  printf("\n");
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
