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
 *   | Vin
 *   |
 *  | |
 *  | | R1
 *  | |
 *   | 
 *   | Vout
 *   |
 *  | |
 *  | | R2
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

double
calc_Vout(double Vin, double R1, double R2)
{
    return Vin * R2 / (R1 + R2);
}


int
main(int argc, char *argv[])
{
    char read_opt;
    int r1, r2;
    double Vin, Vout, Verror;
    double calculated_Vout;
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

    if (argc - optind != 3) {
	fprintf(stderr, "%s [-s <res series>] <Vin> <Vout> <Verror_max>\n", 
		argv[0]);
	fprintf(stderr, " where <res series> is one off 24(default), 12, 6 or 3\n");
	exit(0);
    }

    Vin    = strtod(argv[optind], NULL);
    Vout   = strtod(argv[optind + 1], NULL);
    Verror = strtod(argv[optind + 2], NULL);

    /* Sanity check */
    if (Vout > Vin) {
	fprintf(stderr, "Vout must be bigger than Vin\n");
	exit(0);
    }	

    /*
     * If Vout >> Vin we must go up in size of resistor.
     */
    if ((Vout / Vin) < 0.1) {
	multiplier  = 10.0;
    } else if ((Vout / Vin) < 0.01) {
	multiplier  = 100.0;
    } else {
	multiplier  = 1.0;
    }

    for (r1 = 0; r1 < sizeof(e24_list)/sizeof(e24_list[0]); r1 += sstep) {
	for (r2 = 0; r2 < sizeof(e24_list)/sizeof(e24_list[0]); r2 +=sstep) {
#if 0
	    printf("%f\n",calc_Vout(Vin, e24_list[r1] * multiplier, 
				    e24_list[r2]));
#endif
	    if (fabs(calc_Vout(Vin, e24_list[r1] * multiplier, e24_list[r2]) - Vout) < Verror) {
		calculated_Vout = calc_Vout(Vin, e24_list[r1] * multiplier, 
					    e24_list[r2]);
		printf("r1: %2.2f  r2: %2.2f Vout(diff): %f(%f%%)\n", 
		       e24_list[r1] * multiplier, e24_list[r2],
		       calculated_Vout - Vout,
		       (calculated_Vout / Vout - 1.0) / 100.0);
	    }

	}
    }

    return 0;
}
