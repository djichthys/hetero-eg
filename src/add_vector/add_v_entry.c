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

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

#include "api_err.h"
#include "debug.h"
#include "datatype.h"
#include "add_v_options.h"
#include "program_options.h"

int main( int argc , char *argv[] )
{
    api_Err_Status err = api_Success ;
    uint16_t  *buff1D = NULL , **buff2D = NULL , ***buff3D = NULL ;
    uint32_t idx_i, idx_j, idx_k ;

    Program_Options p_opt ;
    Vector_MetaData meta ;

    memset(&p_opt, 0, sizeof(Program_Options));
    memset(&meta, 0, sizeof(Vector_MetaData));

    err = parse_cmdline( argc, argv, &p_opt);
    if( err != api_Success ) {
        debug("Error parsing cmdline options. err=%d", err );
        goto err_main ;
    }

    debug("===============================================");
    debug("Command-line Options :");
    debug("File-name : [%s]", p_opt.file);
    debug("Separator String : [%s]", p_opt.sep);
    debug("DataType-value: [%u]", p_opt.type);
    debug("===============================================");

    meta.type = p_opt.type ;
#if 0
    err = read_data((void **)&buff1D, &meta, p_opt.file, p_opt.sep );
    if( err != api_Success ) {
        debug("Could not read data from file[%s]. separator-list[%s]. Error = %d"
                                                         , p_opt.file, p_opt.sep, err);
        goto err_main ;
    }


    /* Display data */
    debug("Data :") ;
    for(idx_i=0 ; idx_i < meta.dim.dim_1d.items ; idx_i++ ) {
        if((idx_i % 16) == 0 )
            printf("\n [%04x] ", idx_i);
        printf("% 4d ", buff1D[idx_i]);
    }
    printf("\n");
    debug("===============================================");
#endif
#if 0
    err = read_data((void **)&buff2D, &meta, p_opt.file, p_opt.sep );
    if( err != api_Success ) {
        debug("Could not read data from file[%s]. separator-list[%s]. Error = %d"
                                                         , p_opt.file, p_opt.sep, err);
        goto err_main ;
    }

    /* Display data */
    debug("Data :") ;
    for(idx_i=0 ; idx_i < meta.dim.dim_2d.rows ; idx_i++ ) {
        printf("\n[%u] ", idx_i);
        for(idx_j=0 ; idx_j < meta.dim.dim_2d.cols ; idx_j++ )
            printf("% 4d ", buff2D[idx_i][idx_j] );
    }
    printf("\n");
    debug("===============================================");
#endif
//#if 0
    err = read_data((void **)&buff3D, &meta, p_opt.file, p_opt.sep );
    if( err != api_Success ) {
        debug("Could not read data from file[%s]. separator-list[%s]. Error = %d"
                                                         , p_opt.file, p_opt.sep, err);
        goto err_main ;
    }

    /* Display data */
    debug("Data :") ;
    for(idx_i=0 ; idx_i < meta.dim.dim_3d.dim_x ; idx_i++ ) {
        printf("\n[x-%u] ", idx_i);
        for(idx_j=0 ; idx_j < meta.dim.dim_3d.dim_y ; idx_j++ ) {
            printf("| <y-%u> ", idx_j);
            for(idx_k=0 ; idx_k < meta.dim.dim_3d.dim_z ; idx_k++ )
                printf("%d,", buff3D[idx_i][idx_j][idx_k] );
        }
    }
    printf("\n");
    debug("===============================================");
//#endif


err_main :
    clean_data((void **)&buff1D, &meta );
    clean_data((void **)&buff2D, &meta );
    clean_data((void **)&buff3D, &meta );
    clean_cmdline_opts( &p_opt );
    return err ;
}
