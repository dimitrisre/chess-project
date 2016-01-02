#include "defines.h"



int color[64];			/* ASPRA, MAYRA, KENO. */
int piece[64];			/* PIONI, ALOGO, STRATHGOS, VASILISSA, VASILIAS, PYRGOS, KENO. */
int side;			/* H seira tou paikth pou paizei. */
int xside;			/* Aytos pou den paizei. */
int castle;			/* bitfield pou mas leei an einai epitrepto na kanoume roke. */
int ep;				/* the en passant square. if white moves e2e4, the en passant
				   square is set to e3, because that's where a pawn would move
				   in an en passant capture */
int fifty;			/* O aritmos twn kinhsewn apo thn teleytaia aixmalwsia pioniou. */
int hash;			/* Monadikos arithmos pou antiproswpei mia thesei. */
int ply;			/* Arithmos pou auksanetai me thn kinhsh kathe paikth. */
int hply;			/* O arithmos twn ply apo thn enarksh tou paixnidiou. */

/* gen_dat einai mnhmh gia lista apo kinhseis pou geniountai apo tous
 * move generators.H lista kinhsewn gia n ply arxizei apo first_move[n]
 * kai teleiwnei sto first_move[n+1].	
 */
gen_t gen_dat[GEN_STACK];
int first_move[MAX_PLY];

/* Pinakas pou krataei to istoriko tou paixnidiou. 
 */
int history[64][64];


hist_t hist_dat[HIST_STACK];	// Pinakas pou xrishmopoieitai gia move back.

/* Tha psaksoume eite gia mx_time ms eite ews otou teleiwsoume thn
 * anazhthsh se max_depth iso me ply.
 */
int max_time;
int max_depth;

int start_time;			// o xronos ston opoio arxizoume thn anazhthsh.
int stop_time;			// o xronos ston opoio teleiwnoume thn anazhthsh.

int nodes;			/* O arithmos twn komvwn pou psaksame. */


move pv[MAX_PLY][MAX_PLY];
int pv_length[MAX_PLY];
BOOL follow_pv;

/* Tyxaioi arithmoi gia na ypologysoume to hash. 
*/
int hash_piece[2][6][64];
int hash_side;
int hash_ep[64];


/* Pinakas pou krataei tis epitrepomenes theseis gia kathe pioni.
 * Theseis me -1 einai mh epitreptes.
 */
int mailbox[120] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, 0, 1, 2, 3, 4, 5, 6, 7, -1,
    -1, 8, 9, 10, 11, 12, 13, 14, 15, -1,
    -1, 16, 17, 18, 19, 20, 21, 22, 23, -1,
    -1, 24, 25, 26, 27, 28, 29, 30, 31, -1,
    -1, 32, 33, 34, 35, 36, 37, 38, 39, -1,
    -1, 40, 41, 42, 43, 44, 45, 46, 47, -1,
    -1, 48, 49, 50, 51, 52, 53, 54, 55, -1,
    -1, 56, 57, 58, 59, 60, 61, 62, 63, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

int mailbox64[64] = {
    21, 22, 23, 24, 25, 26, 27, 28,
    31, 32, 33, 34, 35, 36, 37, 38,
    41, 42, 43, 44, 45, 46, 47, 48,
    51, 52, 53, 54, 55, 56, 57, 58,
    61, 62, 63, 64, 65, 66, 67, 68,
    71, 72, 73, 74, 75, 76, 77, 78,
    81, 82, 83, 84, 85, 86, 87, 88,
    91, 92, 93, 94, 95, 96, 97, 98
};


/* Pinakas pou deixnei pros ta pou mporei na kinhthei to kathe pioni.
 * An einai FALSE tote mporei na kinhthei ena tetragwno se opoiadhpote
 * kateuthinsh.
 */
BOOL slide[6] = {
    FALSE, FALSE, TRUE, TRUE, TRUE, FALSE
};


int offsets[6] = {		// Arithmos twn kateythinsewn stis opoies mporei na kinhthei.
    0, 8, 4, 4, 8, 8
};

int offset[6][8] = {		// Pinakas me tis pragmatikes kateuthinseis.
    {0, 0, 0, 0, 0, 0, 0, 0},
    {-21, -19, -12, -8, 8, 12, 19, 21},
    {-11, -9, 9, 11, 0, 0, 0, 0},
    {-10, -1, 1, 10, 0, 0, 0, 0},
    {-11, -10, -9, -1, 1, 9, 10, 11},
    {-11, -10, -9, -1, 1, 9, 10, 11}
};



/* O pinakas castle_mask kathorizei an mporoume na kanoume
 * roke meta apo mia kinhsh .Auto ginetai pairnontas to logiko AND
 * ths metavlhths castle me to castle_mask[i], opou to i einai analogo
 * tou pyrgou.
 */
int castle_mask[64] = {
    7, 15, 15, 15, 3, 15, 15, 11,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    13, 15, 15, 15, 12, 15, 15, 14
};


/* Ta grammata pou antiproswpeyoun to kathe pioni. 
*/
char piece_char[6] = {
    'P', 'N', 'B', 'R', 'Q', 'K'
};


/* Arxikh katastash tou tablo 
 */

int init_color[64] = {
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};

int init_piece[64] = {
    3, 1, 2, 4, 5, 2, 1, 3,
    0, 0, 0, 0, 0, 0, 0, 0,
    6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6,
    0, 0, 0, 0, 0, 0, 0, 0,
    3, 1, 2, 4, 5, 2, 1, 3
};
