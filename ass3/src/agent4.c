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
static int gameover(State s);
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

static int estimate(int c[], double *mark) {
	int i;
	double  poscost = 0.0;
	int win = 0, lost = 0;
	// 1.5 points for the centre of the board;
	poscost = poscost + 1.5 * score(c[5]);
	// 0.5 points for the corner of the board;
	poscost = poscost 
		+ 0.5 * (score(c[1]) + score(c[3]) + score(c[7]) + score(c[9]));
	// getting 12.0 points for each 2 consecutive non-blocked occupation
	// equals the current player
	double tmp = 0.0;
	for (i = 1; i <= 3; i++) {
		tmp = 0.0;
		tmp = tmp + score(c[i]) + score(c[i+3]) + score(c[i+6]);
		if (tmp == 2.0 * score(0)) {
			win++;
		} else if (tmp == 2.0 * score(1)) {
			lost++;
		}
	}
	
	for (i = 1; i <= 9; i+= 3) {
		tmp = 0.0;
		tmp = tmp + score(c[i]) + score(c[i+1]) + score(c[i+2]);
		if (tmp == 2.0 * score(0)) {
			win++;
		} else if (tmp == 2.0 * score(1)) {
			lost++;
		}
	}
	
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
	
	
	if (win == 0 && lost == 0) {
		*mark = poscost;
		return -1;
	} else if (win != 0 && lost == 0) {
		*mark =  score(0) * 15.0;
		return 0;
	} else if (win == 0 && lost != 0) {
		*mark =  score(1) * 15.0;
		return 1;
	}
	*mark = 0.0;
	return 2;
} 

static double calculate(State s, int pos) {
	int i, j;
	// we let type represents the status if each board
	// -1 represents the board is undetermined
	// 2 represents both X and O could win
	// 1 represents the O player could win
	// 0 represents the X player could win
	int type[10] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
	double mark[10] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
	// how many boards could currently get to this board
	int indegreeX[10] = {0,0,0,0,0,0,0,0,0,0};
	int indegreeO[10] = {0,0,0,0,0,0,0,0,0,0};
	s.previous = pos;
	int value = gameover(s);
	if (value != 2) {
		return value * 1.0;
	}
	
	for (i = 1; i <= 9; i++) {
		type[i] = estimate(s.board[i], &mark[i]);
	}
	
	for (i = 1; i <= 9; i++) {
		for (j = 1; j <= 9; j++) {
			if (s.board[i][j] != EMPTY) {
				if (type[i] == -1) {
					indegreeX[j]++;
					indegreeO[j]++;
				} else if (type[i] == 0) {
					indegreeX[j]++;
				} else if (type[i] == 1) {
					indegreeO[j]++;
				}
			} 
		}
	}
	double hcost = 0.0;
	for (i = 1; i <= 9; i++) {
		if (type[i] == 2) {
			hcost = hcost + score(0) * indegreeX[i] + score(1) * indegreeO[i];
		} else if (type[i] == 1) {
			hcost = hcost + mark[i] * indegreeO[i];
		} else if (type[i] == 0) {
			hcost = hcost + mark[i] * indegreeX[i];
		} else {
			hcost = hcost + mark[i] * (indegreeX[i] + indegreeO[i]) / 2.0;
		}
	}
	
	return hcost;
}
// if the game is over return 0 or -400 or 400 otherwise return 2
static int gameover(State s) {
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
			return 400;
		} else if (current == -3) {
			return -400;
		}
	}
	
	for (i = 1; i <= 3; i++) {
		current = 0;
		current = current + score(s.board[s.previous][i]) 
			+ score(s.board[s.previous][i+3]) + score(s.board[s.previous][i+6]);
		if (current == 3) {
			return 400;
		} else if (current == -3) {
			return -400;
		}
	}
	
	current = score(s.board[s.previous][1]) 
			+ score(s.board[s.previous][5]) + score(s.board[s.previous][9]);
	if (current == 3) {
		return 400;
	} else if (current == -3) {
		return -400;
	}
	
	current = score(s.board[s.previous][3]) 
			+ score(s.board[s.previous][5]) + score(s.board[s.previous][7]);
	if (current == 3) {
		return 400;
	} else if (current == -3) {
		return -400;
	}
	return 2;
}

static Play Minimax(State s, int currentplayer, int depth, double alpha, double beta) {
	Play nextmove = play(0, 0);
	int value = gameover(s);
	if (value != 2) {
		return play(value * 1.0, 0);
	}
	double h = -1024.0, tmp;
	int curr = 0, i,limit;
	// make an estimate of the current situation
	if (m >= 40) {
		limit = 10;
	} else if (m >= 15) {
		limit = 9;
	} else {
		limit = 8;
	}
	if (depth >= limit) {
		for (i = 1; i <= 9; i++) {
			if (s.board[s.nextboard][i] == EMPTY) {
				s.board[s.nextboard][i] = currentplayer;
				if (h == -1024.0) {
					h = calculate(s, i);
					curr = i;
				} else {
					tmp = calculate(s, i);
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
				
				if (alpha >= beta || alpha >= 390) {
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
				
				if (alpha >= beta || beta <= -390) {
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
  // if we are going to lose anyway play randomly such that 
  // if our opponent make mistakes we still have chance
  if (next.heuristic == score(!player) * 400) {
  	printf("random move\n");
  	do {
    	this_move = 1 + random()% 9;
  	} while( board[prev_move][this_move] != EMPTY );
  }
  move[m] = this_move;
  board[move[m-1]][this_move] = player;
  current.board[move[m-1]][this_move] = player;
  current.previous = current.nextboard;
  current.nextboard = this_move;

  // print_board(stdout, board, prev_move,  this_move);
  //printf("heuristic = %lf\n", next.heuristic);
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
