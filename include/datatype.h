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


typedef enum __Supported_Data_Type__
{
    DataType_uint8      =  0 ,
    DataType_uint16          ,
    DataType_uint32          ,
    DataType_uint64          ,
    DataType_int8            ,
    DataType_int16           ,
    DataType_int32           ,
    DataType_int64           ,
    DataType_float           ,
    DataType_double          ,
    DataType_long_double     ,
    DataType_MaxTypes          /* Sentinel value for error checking */
} Data_Type ;


uint32_t sizeof_datatype( Data_Type type );

api_Err_Status read_data( void ***, Data_Dimension *, char *, uint8_t *);
api_Err_Status clean_data( void ***);

