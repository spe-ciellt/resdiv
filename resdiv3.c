/*
 * Resistive divider calculation tool
 *
 * Copyright (c) 2010-2012, Ciellt/Stefan Petersen (spe@ciellt.se)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the author nor the names of any contributors
 *    may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *   | VinX
 *   |
 *  | |
 *  | | R1
 *  | |
 *   | 
 *   | Vout1 @ Vin1
 *   |
 *  | |
 *  | | R2
 *  | |
 *   |
 *   | Vout2 @ Vin2
 *   |
 *  | |
 *  | | R3
 *  | |
 *   |
 *  GND
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>

const double e24_list[] = { 10.0, 11.0, 12.0, 13.0, 15.0, 16.0, 18.0,
			    20.0, 22.0, 24.0, 27.0, 30.0, 33.0, 36.0,
			    39.0, 43.0, 47.0, 51.0, 56.0, 62.0, 68.0,
			    75.0, 82.0, 91.0};
const int E24 = 1;
const int E12 = 2;
const int E6 = 4;
const int E3 = 8;

#define NUF_TESTS 2

void
print_help(char *argument)
{
    fprintf(stderr, "\n");
    fprintf(stderr, "   | VinX\n");
    fprintf(stderr, "   |\n");
    fprintf(stderr, "  | |\n");
    fprintf(stderr, "  | | R1\n");
    fprintf(stderr, "  | |\n");
    fprintf(stderr, "   | \n");
    fprintf(stderr, "   | Vout1 @ Vin1\n");
    fprintf(stderr, "   |\n");
    fprintf(stderr, "  | |\n");
    fprintf(stderr, "  | | R2\n");
    fprintf(stderr, "  | |\n");
    fprintf(stderr, "   |\n");
    fprintf(stderr, "   | Vout2 @ Vin2\n");
    fprintf(stderr, "   |\n");
    fprintf(stderr, "  | |\n");
    fprintf(stderr, "  | | R3\n");
    fprintf(stderr, "  | |\n");
    fprintf(stderr, "   |\n");
    fprintf(stderr, "  GND\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "%s [-s <res series>] <Vin1> <Vin2> <Vout1> <Vout2> <Verror_max>\n", 
		argument);
    fprintf(stderr, " where <res series> is one off 24(default), 12, 6 or 3\n");
} /* print_help */


double
calc_Vout(double Vin, double R1, double R2)
{
    return Vin * R2 / (R1 + R2);
} /* calc_Vout */


int
main(int argc, char *argv[])
{
    char read_opt;
    int r1, r2, r3;
    int i, idx;
    double Vin[NUF_TESTS], Vout[NUF_TESTS], Verror;
    double calculated_Vout[NUF_TESTS];
    double multiplier = 1.0;
    int sstep = 1;

    while ((read_opt = getopt(argc, argv, "s:")) != -1) {

	switch (read_opt) {
	case 's':
	    switch (strtol(optarg, NULL, 10)) {
	    case 24:
		sstep = E24;
		break;
	    case 12:
		sstep = E12;
		break;
	    case 6:
		sstep = E6;
		break;
	    case 3:
		sstep = E3;
		break;
	    default:
		fprintf(stderr, "Unknown series E%s\n", optarg);
		fprintf(stderr, "Known series are E24, E12, E6 and E3\n");
		fprintf(stderr, "Use just the number of the serie\n");
		exit(0);
	    }
	    break;
	default:
	    fprintf(stderr, "Not handled option [%d=%c]\n", read_opt, 
		    read_opt);
	}
    }

    if (argc - optind != 5) {
	print_help(argv[0]);
	exit(0);
    }

    idx = optind;
    for (i = 0; i < NUF_TESTS; i++) {
	Vin[i] = strtod(argv[idx++], NULL);
    }
    for (i = 0; i < NUF_TESTS; i++) {
	Vout[i] = strtod(argv[idx++], NULL);
    }
    Verror = strtod(argv[idx++], NULL);

    /* Sanity check */
    for (i = 0; i < NUF_TESTS; i++) {
	if (Vout[i] > Vin[i]) {
	    fprintf(stderr, "Vout[%d] must be bigger than Vin [%d]\n", i, i);
	    exit(0);
	}
    }

    /*
     * If Vout >> Vin we must go up in size of resistor.
     */
    if (((Vout[0] / Vin[0]) < 0.1) || ((Vout[1] / Vin[1]) < 0.1)) {
	multiplier  = 10.0;
    } else if (((Vout[0] / Vin[0]) < 0.01) ||((Vout[1] / Vin[1]) < 0.1)) {
	multiplier  = 100.0;
    } else {
	multiplier  = 1.0;
    }

    for (r1 = 0; r1 < sizeof(e24_list)/sizeof(e24_list[0]); r1 += sstep) {
	for (r2 = 0; r2 < sizeof(e24_list)/sizeof(e24_list[0]); r2 +=sstep) {
	    for (r3 = 0; r3 < sizeof(e24_list)/sizeof(e24_list[0]); r3 +=sstep) {
		
		calculated_Vout[0] = calc_Vout(Vin[0],
					       e24_list[r1] * multiplier +
					       e24_list[r2], e24_list[r3]);
		calculated_Vout[1] = calc_Vout(Vin[1],
					       e24_list[r1] * multiplier,
					       e24_list[r2] + e24_list[r3]);

		if ((fabs(calculated_Vout[0] - Vout[0]) < Verror) &&
		    (fabs(calculated_Vout[1] - Vout[1]) < Verror)) {
		    printf("r1: %2.2f  r2: %2.2f  r3: %2.2f\n",
			   e24_list[r1] * multiplier, e24_list[r2], 
			   e24_list[r3]);
		    printf("  Vout1(diff): %f(%f%%) Vout2(diff): %f(%f%%)\n",
			   calculated_Vout[0] - Vout[0],
			   (calculated_Vout[0] / Vout[0] - 1.0) / 100.0,
			   calculated_Vout[1] - Vout[1],
			   (calculated_Vout[1] / Vout[1] - 1.0) / 100.0);
		}
	    }
	}
    }
    
    return 0;
}
