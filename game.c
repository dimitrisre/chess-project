#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/timeb.h>
#include "defines.h"



BOOL ftime_ok = FALSE;
int get_ms()
{
    struct timeb timebuffer;
    ftime(&timebuffer);
    if (timebuffer.millitm != 0)
	ftime_ok = TRUE;
    return (timebuffer.time * 1000) + timebuffer.millitm;
}


/* 	main
 * H main einai h kentrikh synarthsh tou paixnidiou kai sthn
 * ousia einai ena infinite loop, sto opoio kaleitai eite h
 * think otan paizei to computer, eite parakineitai o paikths
 * na kanei mia kinhsh.
 * To paixnidi teleiwnei otan kapoios apo tous 2 paiktes kanei 
 * mat h' otan o paikths plhktrologhsei to 'q'.
 *
 */

int main()
{
    int computer_side;
    char s[256];
    int m;


    init_hash();
    init_board();
    gen();
    computer_side = EMPTY;
    max_time = 1 << 25;
    max_depth = 4;
    printf("Enter 'q' to quit game.\n");
    print_board();
    for (;;) {
	if (computer_side != EMPTY) {	/* computer's turn */
	    /* think about the move and make it */
	    think(1);
	    if (!pv[0][0].u) {
		printf("(no legal moves)\n");
		computer_side = EMPTY;
		continue;
	    }
	    printf("Computer's move: %s\n", move_str(pv[0][0].b));
	    makemove(pv[0][0].b);
	    ply = 0;
	    gen();
	    print_result();
	    computer_side = EMPTY;
	} else {		/* player's turn */
	    /* get player's input */
	  L1:printf("Your move: ");
	    if (scanf("%s", s) == EOF)
		return 0;
	    if (!strcmp(s, "q")) {
		printf("Exit game\n");
		break;
	    }
	    m = parse_move(s);
	    if (m == -1 || !makemove(gen_dat[m].m.b)) {
		printf("Illegal move.\n");
		print_board();
		goto L1;
	    } else {
		ply = 0;
		gen();
		print_result();
	    }
	    computer_side = 0;
	}
	print_board();
    }

    return 0;
}


/*	parse_move
 * H synarthsh ayth epeksergazetai thn kinhsh tou paikth se syntetagmenes
 * panw sto tablo kai epistrefei to index ts kinhshs se gen_dat h' -1 an
 * h kinhsh einai paranomh.
 * 
 */

int parse_move(char *s)
{
    int from, to, i;

    /* elegxei an to string moiazei me kinhsh */
    if (s[0] < 'a' || s[0] > 'h' ||
	s[1] < '0' || s[1] > '9' ||
	s[2] < 'a' || s[2] > 'h' || s[3] < '0' || s[3] > '9')
	return -1;

    from = s[0] - 'a';
    from += 8 * (8 - (s[1] - '0'));
    to = s[2] - 'a';
    to += 8 * (8 - (s[3] - '0'));

    for (i = 0; i < first_move[1]; ++i)
	if (gen_dat[i].m.b.from == from && gen_dat[i].m.b.to == to) {

	    /* Se periptwsh opou h kinhsh einai prowthhsh elegxetai to teleytaio gramma ths eisodou
	     * kai antikathistatai to pioni me to pioni pou epelexe o paikths alliws an den dothike
	     * pioni ,dinetai h vasilissa
	     */
	    if (gen_dat[i].m.b.bits & 32)
		switch (s[4]) {
		case 'N':
		    return i;
		case 'B':
		    return i + 1;
		case 'R':
		    return i + 2;
		default:	/* assume it's a queen */
		    return i + 3;
		}
	    return i;
	}

    /* H kinhsh den vrethike */
    return -1;
}


/*	move_str
 * Epistrefei enaa string me thn kinhsh tou paikth (m) se syntetagmenes
 * panw sto tablo.
 * 
 */

char *move_str(move_bytes m)
{
    static char str[6];

    char c;

    if (m.bits & 32) {
	switch (m.promote) {
	case KNIGHT:
	    c = 'n';
	    break;
	case BISHOP:
	    c = 'b';
	    break;
	case ROOK:
	    c = 'r';
	    break;
	default:
	    c = 'q';
	    break;
	}
	sprintf(str, "%c%d%c%d%c",
		COL(m.from) + 'a',
		8 - ROW(m.from), COL(m.to) + 'a', 8 - ROW(m.to), c);
    } else
	sprintf(str, "%c%d%c%d",
		COL(m.from) + 'a',
		8 - ROW(m.from), COL(m.to) + 'a', 8 - ROW(m.to));
    return str;
}


/*	print_board
 * Ektypwnei to tablo tou paixnidiou meta apo kathe kinhsh.	
 * 
 */

void print_board()
{
    int i;

    printf("\n8 ");
    for (i = 0; i < 64; ++i) {
	switch (color[i]) {
	case EMPTY:
	    printf(" .");
	    break;
	case LIGHT:
	    printf(" %c", piece_char[piece[i]]);
	    break;
	case DARK:
	    printf(" %c", piece_char[piece[i]] + ('a' - 'A'));
	    break;
	}
	if ((i + 1) % 8 == 0 && i != 63)
	    printf("\n%d ", 7 - ROW(i));
    }
    printf("\n\n   a b c d e f g h\n\n");
}


/*	print_result
 * Elegxei se kathe gyro tou paixnidiou an ayto teleiwse,
 * kai an auto isxyei ektypwnei to apotelesma.
 * 
 */

void print_result()
{
    int i;

    /* is there a legal move? */
    for (i = 0; i < first_move[1]; ++i)
	if (makemove(gen_dat[i].m.b)) {
	    takeback();
	    break;
	}
    if (i == first_move[1]) {
	if (in_check(side)) {
	    if (side == LIGHT) {
		printf("Black mates\n");
		printf("You lose!\n");
		exit(0);
	    } else {
		printf("White mates\n");
		printf("You win!\n");
		exit(0);
	    }
	}
    }
}
