/*!
 * This program is a set of examples about how to use openCL for use in
 * heterogeneous programs
 * Copyright (C) 2017  Dejice Jacob
 * 
 * 
 * This file is part of hetero-examples.
 * 
 * hetero-examples is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 * 
 * hetero-examples is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with hetero-examples.  If not, see <http://www.gnu.org/licenses/>.
 */


/*!
 * Macros to evaluate time performance
 * Calls clock() - defined in <time.h>
 */
#define start_timer(x)  \
do { \
    x = clock(); \
} while(0)

/* to distinguish code easier */
#define stop_timer(x)  start_timer(x)

#define time_taken(begin,end)  ( ((double)((end) - (begin)))/ CLOCKS_PER_SEC )


