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



typedef struct __Option_Help_Description__
{
    char option ;
    char *option_text ;
} Option_Help ;



typedef struct __Program_Options__
{
    Data_Type type ; 
    uint32_t total_elem ; 
    void **elem ;
} Program_Options ;


api_Err_Status parse_cmdline( int , char ** , Program_Options * );
void clean_cmdline_opts( Program_Options * );


