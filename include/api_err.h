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
 * Enumeration types for all APIs to talk to
 * each other at the application layer
 */
typedef enum __api_Err_Status_Types__
{
    api_Err_Param            = -256 ,
    api_Err_Init                    ,
    api_Err_Hardware                ,
    api_Err_Failure                 ,
    api_Err_Memory                  ,
    api_Err_File                    ,
    api_Success              = 0    ,
    api_Stat_InProgress             ,
    api_Stat_Complete               ,
    api_Err_Max_Type
} api_Err_Status;

