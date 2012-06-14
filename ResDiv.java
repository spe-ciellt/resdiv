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


class ResDivCalc {
    private final double e24_list[] = {10.0, 11.0, 12.0, 13.0, 15.0, 16.0, 18.0,
				       20.0, 22.0, 24.0, 27.0, 30.0, 33.0, 36.0,
				       39.0, 43.0, 47.0, 51.0, 56.0, 62.0, 68.0,
				       75.0, 82.0, 91.0};
    public final int E24 = 1;
    public final int E12 = 2;
    public final int E6 = 4;
    public final int E3 = 8;

    public double VoutCalc(double Vin, double R1, double R2) {
	return Vin * R2 / (R1 + R2);
    }

    public int FindSolution(double Vin, double Vout, double Verror, int series) {
	int r1idx, r2idx;
	double calculated_vout;
	double multiplier;

	/* 
	 * Sanity check
	 */

	if (Vout > Vin) {
	    // Throw exception
	    return 0;
	    
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

	for (r1idx = 0; r1idx < e24_list.length; r1idx += series) {
	    for (r2idx = 0; r2idx < e24_list.length; r2idx += series) {
		calculated_vout = VoutCalc(Vin, e24_list[r1idx] * multiplier, 
					   e24_list[r2idx]);
		if (Math.abs(calculated_vout - Vout) < Verror) {
		    System.out.println("r1: " + e24_list[r1idx] * multiplier + 
				       "  r2: " + e24_list[r2idx] +
				       " " + (calculated_vout - Vout) + 
				       "(" + (calculated_vout / Vout - 1.0)/100.0 + "%)");
		}
	    }
	}

	return 0;
    }
}

public class ResDiv {
    public static void main(String[] args) {
	ResDivCalc rd;
	int i;

	rd = new ResDivCalc();
	
	for (i = 0; i < args.length; i++) {
	    System.out.println(i + ":" + args[i]);
	}
	rd.FindSolution(12.5, 0.8, 0.01, rd.E12);

    }
}
