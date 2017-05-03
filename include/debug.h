This program is a set of examples about how to use openCL for use in
heterogeneous programs
Copyright (C) 2017  Dejice Jacob


This file is part of hetero-examples.

hetero-examples is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

hetero-examples is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with hetero-examples.  If not, see <http://www.gnu.org/licenses/>.



typedef enum __Debug_Levels__
{
    err_level_dbg    =  0 ,  /* Maximum debug is output */
    err_level_info        ,
    err_level_warn        ,
    err_level_error       ,
    err_level_critical    ,
    err_level_fatal       ,
    err_level_disabled    ,
    err_level_Max
} Debug_Level ;

#define DEFAULT_TRACE_LEVEL   err_level_error

#ifndef TRACE_LEVEL
  #define TRACE_LEVEL  DEFAULT_TRACE_LEVEL
#endif



#define debug( format , args... )  printf("[%s:%d] | "format"\r\n" , __FUNCTION__ , __LINE__ , ##args )
