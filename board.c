#include <stdlib.h>
#include "defines.h"		//Ginetai include ola ta prototypa twn synarthsewmn
					 //kai oi dhlwseis twn metablhtwn



/*~~~~~init_board~~~~~~
 *   Arxikopoiei to tablo toy paixnidioy.
 *
 *   parameters: ~
 *
 *   returns: ~
 *
 */

void init_board()
{
    int i;

    for (i = 0; i < 64; ++i) {
	color[i] = init_color[i];
	piece[i] = init_piece[i];
    }
    side = LIGHT;
    xside = DARK;
    castle = 15;
    ep = -1;
    fifty = 0;
    ply = 0;
    hply = 0;
    set_hash();
    first_move[0] = 0;
}


/*~~~~~~init_hash()~~~~~~~~
 *   Arxikopoiei toys tyxaioys arithmoys poy
 *   xrhsimopoioyntai pao thn set_hash
 *
 *   parameters: ~
 *
 *   returns: ~
 *
 * */

void init_hash()
{
    int i, j, k;

    srand(0);
    for (i = 0; i < 2; ++i)
	for (j = 0; j < 6; ++j)
	    for (k = 0; k < 64; ++k)
		hash_piece[i][j][k] = hash_rand();
    hash_side = hash_rand();
    for (i = 0; i < 64; ++i)
	hash_ep[i] = hash_rand();
}


/*~~~~~~~~hash_rand~~~~~~~~
 * Boithitikh synarthsh poy xrishmopoieitai gia thn kalyterh kalypsh
 * olwn twn 32 bit twn arithmwn wste na yparxei megalytero eyros apo thn
 * hash.
 *
 *	 parameters: ~
 *
 *	 returns: integer The xored number
 *
 * */

int hash_rand()
{
    int i;
    int r = 0;

    for (i = 0; i < 32; ++i)
	r ^= rand() << i;
    return r;
}


/* ~~~~~~~~~set_hash~~~~~~~~
 * Synarthsh katakermatismoy (Zobrist methodos)
 *
 *    parameters: ~
 *
 *    returns: ~
 *
 */

void set_hash()
{
    int i;

    hash = 0;
    for (i = 0; i < 64; ++i)
	if (color[i] != EMPTY)
	    hash ^= hash_piece[color[i]][piece[i]][i];
    if (side == DARK)
	hash ^= hash_side;
    if (ep != -1)
	hash ^= hash_ep[ep];
}


/*~~~~~~in_check~~~~~~
 *   Briskei ton basilia ths pleyras s kai elegxei na dei ean 
 * brisketai se epithesh.
 *
 * parameters: integer - H pleyra pros elegxo
 *
 * returns: BOOL - TRUE ean einai se epithesh alliws FALSE
 *
 */

BOOL in_check(int s)
{
    int i;

    for (i = 0; i < 64; ++i)
	if (piece[i] == KING && color[i] == s)
	    return attack(i, s ^ 1);
    return TRUE;
}


/*~~~~~~attack~~~~~~
 *  Elegxoi ean to tetragwno sq brisketai se epithesh apo thn pleyra s
 *
 *  parameters: integer sq ~ to tetragwno pros elegxo
 *  			
 *  			integer s ~ h pleyra poy epitithetai
 *
 *  returns: BOOL ~ TRUE ean to tetragwno brisketai se epithesh apo thn
 *  				pleyra s alliws FALSE
 *  				
 */

BOOL attack(int sq, int s)
{
    int i, j, n;

    for (i = 0; i < 64; ++i)
	if (color[i] == s) {
	    if (piece[i] == PAWN) {
		if (s == LIGHT) {
		    if (COL(i) != 0 && i - 9 == sq)
			return TRUE;
		    if (COL(i) != 7 && i - 7 == sq)
			return TRUE;
		} else {
		    if (COL(i) != 0 && i + 7 == sq)
			return TRUE;
		    if (COL(i) != 7 && i + 9 == sq)
			return TRUE;
		}
	    } else
		for (j = 0; j < offsets[piece[i]]; ++j)
		    for (n = i;;) {
			n = mailbox[mailbox64[n] + offset[piece[i]][j]];
			if (n == -1)
			    break;
			if (n == sq)
			    return TRUE;
			if (color[n] != EMPTY)
			    break;
			if (!slide[piece[i]])
			    break;
		    }
	}
    return FALSE;
}


/*~~~~~~~~gen()~~~~~~~~
 *   Skanarei to tablo kai vriskei filika pionia kai 
 * elegxei se poia antipala mporoyn na epitethoyn. An vrei ena 
 * tetoio syndiasmo kalei thn gen_push gia na ton balei sthn stoiba
 * ton kinhsewn.
 *
 *
 * parameters : ~
 *
 * returns: ~
 *
 */
void gen()
{
    int i, j, n;


    first_move[ply + 1] = first_move[ply];

    for (i = 0; i < 64; ++i)
	if (color[i] == side) {
	    if (piece[i] == PAWN) {
		if (side == LIGHT) {
		    if (COL(i) != 0 && color[i - 9] == DARK)
			gen_push(i, i - 9, 17);
		    if (COL(i) != 7 && color[i - 7] == DARK)
			gen_push(i, i - 7, 17);
		    if (color[i - 8] == EMPTY) {
			gen_push(i, i - 8, 16);
			if (i >= 48 && color[i - 16] == EMPTY)
			    gen_push(i, i - 16, 24);
		    }
		} else {
		    if (COL(i) != 0 && color[i + 7] == LIGHT)
			gen_push(i, i + 7, 17);
		    if (COL(i) != 7 && color[i + 9] == LIGHT)
			gen_push(i, i + 9, 17);
		    if (color[i + 8] == EMPTY) {
			gen_push(i, i + 8, 16);
			if (i <= 15 && color[i + 16] == EMPTY)
			    gen_push(i, i + 16, 24);
		    }
		}
	    } else
		for (j = 0; j < offsets[piece[i]]; ++j)
		    for (n = i;;) {
			n = mailbox[mailbox64[n] + offset[piece[i]][j]];
			if (n == -1)
			    break;
			if (color[n] != EMPTY) {
			    if (color[n] == xside)
				gen_push(i, n, 1);
			    break;
			}
			gen_push(i, n, 0);
			if (!slide[piece[i]])
			    break;
		    }
	}


    if (side == LIGHT) {
	if (castle & 1)
	    gen_push(E1, G1, 2);
	if (castle & 2)
	    gen_push(E1, C1, 2);
    } else {
	if (castle & 4)
	    gen_push(E8, G8, 2);
	if (castle & 8)
	    gen_push(E8, C8, 2);
    }


    if (ep != -1) {
	if (side == LIGHT) {
	    if (COL(ep) != 0 && color[ep + 7] == LIGHT
		&& piece[ep + 7] == PAWN)
		gen_push(ep + 7, ep, 21);
	    if (COL(ep) != 7 && color[ep + 9] == LIGHT
		&& piece[ep + 9] == PAWN)
		gen_push(ep + 9, ep, 21);
	} else {
	    if (COL(ep) != 0 && color[ep - 9] == DARK
		&& piece[ep - 9] == PAWN)
		gen_push(ep - 9, ep, 21);
	    if (COL(ep) != 7 && color[ep - 7] == DARK
		&& piece[ep - 7] == PAWN)
		gen_push(ep - 7, ep, 21);
	}
    }
}


/* ~~~~~gen_caps()~~~~~~~~
 *  Idia me thn gen() mono poy ektelei kinhseis promotion kai capture
 *
 *  parameters : ~
 *
 *  returns : ~
 *
 */

void gen_caps()
{
    int i, j, n;

    first_move[ply + 1] = first_move[ply];
    for (i = 0; i < 64; ++i)
	if (color[i] == side) {
	    if (piece[i] == PAWN) {
		if (side == LIGHT) {
		    if (COL(i) != 0 && color[i - 9] == DARK)
			gen_push(i, i - 9, 17);
		    if (COL(i) != 7 && color[i - 7] == DARK)
			gen_push(i, i - 7, 17);
		    if (i <= 15 && color[i - 8] == EMPTY)
			gen_push(i, i - 8, 16);
		}
		if (side == DARK) {
		    if (COL(i) != 0 && color[i + 7] == LIGHT)
			gen_push(i, i + 7, 17);
		    if (COL(i) != 7 && color[i + 9] == LIGHT)
			gen_push(i, i + 9, 17);
		    if (i >= 48 && color[i + 8] == EMPTY)
			gen_push(i, i + 8, 16);
		}
	    } else
		for (j = 0; j < offsets[piece[i]]; ++j)
		    for (n = i;;) {
			n = mailbox[mailbox64[n] + offset[piece[i]][j]];
			if (n == -1)
			    break;
			if (color[n] != EMPTY) {
			    if (color[n] == xside)
				gen_push(i, n, 1);
			    break;
			}
			if (!slide[piece[i]])
			    break;
		    }
	}
    if (ep != -1) {
	if (side == LIGHT) {
	    if (COL(ep) != 0 && color[ep + 7] == LIGHT
		&& piece[ep + 7] == PAWN)
		gen_push(ep + 7, ep, 21);
	    if (COL(ep) != 7 && color[ep + 9] == LIGHT
		&& piece[ep + 9] == PAWN)
		gen_push(ep + 9, ep, 21);
	} else {
	    if (COL(ep) != 0 && color[ep - 9] == DARK
		&& piece[ep - 9] == PAWN)
		gen_push(ep - 9, ep, 21);
	    if (COL(ep) != 7 && color[ep - 7] == DARK
		&& piece[ep - 7] == PAWN)
		gen_push(ep - 7, ep, 21);
	}
    }
}


/* ~~~~~~~~gen_push()~~~~~~~~~ 
 *    Bazei mia kinhsh sthn soiba twn kinhsewn. An einai metakinhsh toy
 * pionioy ginetai apo thn gen_promote. Bazei epishs ena score sthn
 * kinhsh gia voithia ston alpha-beta algorithmo.
 *
 * parameters : integer from tatragwno apo
 * 				integer to tetragwno pros
 * 				integer bits
 *
 * returns: ~
 *
 */

void gen_push(int from, int to, int bits)
{
    gen_t *g;

    if (bits & 16) {
	if (side == LIGHT) {
	    if (to <= H8) {
		gen_promote(from, to, bits);
		return;
	    }
	} else {
	    if (to >= A1) {
		gen_promote(from, to, bits);
		return;
	    }
	}
    }
    g = &gen_dat[first_move[ply + 1]++];
    g->m.b.from = (char) from;
    g->m.b.to = (char) to;
    g->m.b.promote = 0;
    g->m.b.bits = (char) bits;
    if (color[to] != EMPTY)
	g->score = 1000000 + (piece[to] * 10) - piece[from];
    else
	g->score = history[from][to];
}


/*~~~~~~~gen_promote~~~~~~~~ 
 *    Bazei sthn stoiba  kinhsewn 4 kinhseis mia gia kathe pithano pioni
 *    
 *    parameters: integer from apo tetragwno
 *    			  integer to sto tetragwno
 *    			  integer bits
 *
 *    returns: ~
 *
 */
void gen_promote(int from, int to, int bits)
{
    int i;
    gen_t *g;

    for (i = KNIGHT; i <= QUEEN; ++i) {
	g = &gen_dat[first_move[ply + 1]++];
	g->m.b.from = (char) from;
	g->m.b.to = (char) to;
	g->m.b.promote = (char) i;
	g->m.b.bits = (char) (bits | 32);
	g->score = 1000000 + (i * 10);
    }
}


/*~~~~~~~makemove~~~~~~~
 *    Ektelei mia kinhsh poy einai nomimh. Ean den einai nomimh
 * tote kanei undo oti ekanekai kanei return FALSE alliws TRUE.
 *
 *	parameters: move_bytes m
 *
 *	returns: BOOL 
 *
 */

BOOL makemove(move_bytes m)
{
    if (m.bits & 2) {
	int from, to;

	if (in_check(side))
	    return FALSE;
	switch (m.to) {
	case 62:
	    if (color[F1] != EMPTY || color[G1] != EMPTY ||
		attack(F1, xside) || attack(G1, xside))
		return FALSE;
	    from = H1;
	    to = F1;
	    break;
	case 58:
	    if (color[B1] != EMPTY || color[C1] != EMPTY
		|| color[D1] != EMPTY || attack(C1, xside)
		|| attack(D1, xside))
		return FALSE;
	    from = A1;
	    to = D1;
	    break;
	case 6:
	    if (color[F8] != EMPTY || color[G8] != EMPTY ||
		attack(F8, xside) || attack(G8, xside))
		return FALSE;
	    from = H8;
	    to = F8;
	    break;
	case 2:
	    if (color[B8] != EMPTY || color[C8] != EMPTY
		|| color[D8] != EMPTY || attack(C8, xside)
		|| attack(D8, xside))
		return FALSE;
	    from = A8;
	    to = D8;
	    break;
	default:
	    from = -1;
	    to = -1;
	    break;
	}
	color[to] = color[from];
	piece[to] = piece[from];
	color[from] = EMPTY;
	piece[from] = EMPTY;
    }

    hist_dat[hply].m.b = m;
    hist_dat[hply].capture = piece[(int) m.to];
    hist_dat[hply].castle = castle;
    hist_dat[hply].ep = ep;
    hist_dat[hply].fifty = fifty;
    hist_dat[hply].hash = hash;
    ++ply;
    ++hply;


    castle &= castle_mask[(int) m.from] & castle_mask[(int) m.to];
    if (m.bits & 8) {
	if (side == LIGHT)
	    ep = m.to + 8;
	else
	    ep = m.to - 8;
    } else
	ep = -1;
    if (m.bits & 17)
	fifty = 0;
    else
	++fifty;


    color[(int) m.to] = side;
    if (m.bits & 32)
	piece[(int) m.to] = m.promote;
    else
	piece[(int) m.to] = piece[(int) m.from];
    color[(int) m.from] = EMPTY;
    piece[(int) m.from] = EMPTY;


    if (m.bits & 4) {
	if (side == LIGHT) {
	    color[m.to + 8] = EMPTY;
	    piece[m.to + 8] = EMPTY;
	} else {
	    color[m.to - 8] = EMPTY;
	    piece[m.to - 8] = EMPTY;
	}
    }


    side ^= 1;
    xside ^= 1;
    if (in_check(xside)) {
	takeback();
	return FALSE;
    }
    set_hash();
    return TRUE;
}


/* ~~~~~~~~~takeback~~~~~~~~~
 *   Ektelei tis idies kinhseis me thn makemove alla anapoda
 *
 *   parameters : ~
 *
 *   returns : ~
 *
 */
void takeback()
{
    move_bytes m;

    side ^= 1;
    xside ^= 1;
    --ply;
    --hply;
    m = hist_dat[hply].m.b;
    castle = hist_dat[hply].castle;
    ep = hist_dat[hply].ep;
    fifty = hist_dat[hply].fifty;
    hash = hist_dat[hply].hash;
    color[(int) m.from] = side;
    if (m.bits & 32)
	piece[(int) m.from] = PAWN;
    else
	piece[(int) m.from] = piece[(int) m.to];
    if (hist_dat[hply].capture == EMPTY) {
	color[(int) m.to] = EMPTY;
	piece[(int) m.to] = EMPTY;
    } else {
	color[(int) m.to] = xside;
	piece[(int) m.to] = hist_dat[hply].capture;
    }
    if (m.bits & 2) {
	int from, to;

	switch (m.to) {
	case 62:
	    from = F1;
	    to = H1;
	    break;
	case 58:
	    from = D1;
	    to = A1;
	    break;
	case 6:
	    from = F8;
	    to = H8;
	    break;
	case 2:
	    from = D8;
	    to = A8;
	    break;
	default:
	    from = -1;
	    to = -1;
	    break;
	}
	color[to] = side;
	piece[to] = ROOK;
	color[from] = EMPTY;
	piece[from] = EMPTY;
    }
    if (m.bits & 4) {
	if (side == LIGHT) {
	    color[m.to + 8] = xside;
	    piece[m.to + 8] = PAWN;
	} else {
	    color[m.to - 8] = xside;
	    piece[m.to - 8] = PAWN;
	}
    }
}
