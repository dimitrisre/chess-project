#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include "defines.h"



jmp_buf env;			//metablhth gia setjmp kai longjmp
BOOL stop_search;


/*~~~~~~~~~think~~~~~~~~
 *  Synarthsh poy ylopoiei thn skepsh toy ypologisth gia thn kalyterh
 *  kinhsh poy mporei na ektelesei.
 *
 *	parameters: ~
 *
 *	returns: ~ 
 *
 * */

void think()
{
    int i, x;

    /* Edw tha gyrisei otan an o xronos toy ypologisth gia na paiksei
     * exei teliwsei. */
    stop_search = FALSE;
    setjmp(env);
    if (stop_search) {


	while (ply)
	    takeback();
	return;
    }

    start_time = get_ms();
    stop_time = start_time + max_time;

    ply = 0;
    nodes = 0;

    memset(pv, 0, sizeof(pv));
    memset(history, 0, sizeof(history));
    for (i = 1; i <= max_depth; ++i) {
	follow_pv = TRUE;
	x = search(-10000, 10000, i);

	if (x > 9000 || x < -9000)
	    break;
    }
}


/*~~~~~~~search()~~~~~~~
 *   H synarthsh poy psaxnei gia thn kalyterh kinhsh toy ypologisth
 *
 *   parameters: integer alpha
 *   			 integer beta
 *   			 integer depth
 *
 *   returns : integer
 *
 *   
 * */
int search(int alpha, int beta, int depth)
{
    int i, j, x;
    BOOL c, f;

    if (!depth)
	return minimax(alpha, beta);
    ++nodes;


    if ((nodes & 1023) == 0)
	checkup();

    pv_length[ply] = ply;

    if (ply && reps())
	return 0;


    if (ply >= MAX_PLY - 1)
	return eval();
    if (hply >= HIST_STACK - 1)
	return eval();


    c = in_check(side);
    if (c)
	++depth;
    gen();
    if (follow_pv)
	sort_pv();
    f = FALSE;


    for (i = first_move[ply]; i < first_move[ply + 1]; ++i) {
	sort(i);
	if (!makemove(gen_dat[i].m.b))
	    continue;
	f = TRUE;
	x = -search(-beta, -alpha, depth - 1);
	takeback();
	if (x > alpha) {


	    history[(int) gen_dat[i].m.b.from][(int) gen_dat[i].m.b.to] +=
		depth;
	    if (x >= beta)
		return beta;
	    alpha = x;


	    pv[ply][ply] = gen_dat[i].m;
	    for (j = ply + 1; j < pv_length[ply + 1]; ++j)
		pv[ply][j] = pv[ply + 1][j];
	    pv_length[ply] = pv_length[ply + 1];
	}
    }


    if (!f) {
	if (c)
	    return -10000 + ply;
	else
	    return 0;
    }


    if (fifty >= 100)
	return 0;
    return alpha;
}


/* H sunarthsh poy ylopoiei ton minmax algorithmo
 *
 * parameters : integer alpha
 * 				integer beta
 *
 * retruns: integer 
 */
int minimax(int alpha, int beta)
{
    int i, j, x;

    ++nodes;


    if ((nodes & 1023) == 0)
	checkup();

    pv_length[ply] = ply;


    if (ply >= MAX_PLY - 1)
	return eval();
    if (hply >= HIST_STACK - 1)
	return eval();


    x = eval();
    if (x >= beta)
	return beta;
    if (x > alpha)
	alpha = x;

    gen_caps();
    if (follow_pv)
	sort_pv();


    for (i = first_move[ply]; i < first_move[ply + 1]; ++i) {
	sort(i);
	if (!makemove(gen_dat[i].m.b))
	    continue;
	x = -minimax(-beta, -alpha);
	takeback();
	if (x > alpha) {
	    if (x >= beta)
		return beta;
	    alpha = x;


	    pv[ply][ply] = gen_dat[i].m;
	    for (j = ply + 1; j < pv_length[ply + 1]; ++j)
		pv[ply][j] = pv[ply + 1][j];
	    pv_length[ply] = pv_length[ply + 1];
	}
    }
    return alpha;
}


/*~~~~~~ reps() ~~~~~~~~ 
 * Epistrefei ton arithmo ton forwn poy h twrinh thesh exei epanalhfthei 
 * */
int reps()
{
    int i;
    int r = 0;

    for (i = hply - fifty; i < hply; ++i)
	if (hist_dat[i].hash == hash)
	    ++r;
    return r;
}


/*~~~~~~~sort_pv()~~~~~~~~
 *
 * parameters : ~
 *
 * returns : ~
 *
 * */

void sort_pv()
{
    int i;

    follow_pv = FALSE;
    for (i = first_move[ply]; i < first_move[ply + 1]; ++i)
	if (gen_dat[i].m.u == pv[0][ply].u) {
	    follow_pv = TRUE;
	    gen_dat[i].score += 10000000;
	    return;
	}
}


/* ~~~~~~~sort()~~~~~~
 *   Taksnomei th lista me tis kinhseis wste h kinhsh me to kalytero
 * score na einai ayth poy tha elegxei apo thn search prwth. 
 *
 * parameters : integer Apo poio shmeio na ksekinhsei h sort
 *
 * returns: ~ 
 *
 */
void sort(int from)
{
    int i;
    int bs;
    int bi;
    gen_t g;

    bs = -1;
    bi = from;
    for (i = from; i < first_move[ply + 1]; ++i)
	if (gen_dat[i].score > bs) {
	    bs = gen_dat[i].score;
	    bi = i;
	}
    g = gen_dat[from];
    gen_dat[from] = gen_dat[bi];
    gen_dat[bi] = g;
}


/*~~~~~~ checkup() ~~~~~~~
 *   Kaleitai kata thn diarkeia ths search.
 *
 *   parameters : ~
 *
 *   returns : ~
 *
 */
void checkup()
{
    if (get_ms() >= stop_time) {
	stop_search = TRUE;
	longjmp(env, 0);
    }
}
