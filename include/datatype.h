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


typedef struct __Data_Dimensions_1D__
{
    uint64_t items ;     /* Number of entries in the data-set */
} Data_Dim_1D ;

typedef struct __Data_Dimensions_2D__
{
    uint64_t rows ;     /* Number of entries in the data-set */
    uint64_t cols ;     /* dimension of each data vector */
} Data_Dim_2D ;


typedef struct __Data_Dimensions_3D__
{
    uint64_t dim_x ;     /* Number of entries in the data-set */
    uint64_t dim_y ;     /* dimension of each data vector */
    uint64_t dim_z ;
} Data_Dim_3D ;



typedef union __Data_Dimensions__
{
    Data_Dim_1D dim_1d ;
    Data_Dim_2D dim_2d ;
    Data_Dim_3D dim_3d ;
} Data_Dimensions ;


typedef struct __Vector_MetaData__
{
    Data_Type type ;
    uint32_t no_dims ;
    Data_Dimensions dim ;
} Vector_MetaData ;


#define LINE_SIZE 4096

uint32_t sizeof_datatype( Data_Type type );
api_Err_Status read_data( void **, Vector_MetaData *, char *, uint8_t *);
api_Err_Status clean_data( void **, Vector_MetaData *);

