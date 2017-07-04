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
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

#include "api_err.h"
#include "datatype.h"
#include "add_v_options.h"
#include "program_options.h"
#include "debug.h"

Option_Help g_help_strings[] =
{
    { .option = 'f', .option_text = "-f,--file...input data file. Each data point is line-separated"                                 },
    { .option = 'd', .option_text = "-d,--dtype..data-types. Use (u)int8,(u)int16,(u)int32,(u)int64,float,double,longdouble"         },
    { .option = 's', .option_text = "-s,--sep....Separator list. Separators should be in reverse order. [^:|] implies x->^,y->:,z->|"},
    { .option = 'h', .option_text = "-h,--help...this help menu"                                                                     },
    { .option =  0 , .option_text = NULL                                                                                             },
};


struct option g_option_list[] = {
    {.name = "file" , .has_arg = required_argument, .flag = NULL, .val = 'f'},
    {.name = "dtype", .has_arg = required_argument, .flag = NULL, .val = 'd'},
    {.name = "sep"  , .has_arg = required_argument, .flag = NULL, .val = 's'},
    {.name = "help" , .has_arg = no_argument      , .flag = NULL, .val = 'h'},
    {.name = NULL,    .has_arg = 0                , .flag = NULL, .val =  0 }
};




/*****************************************************************************/
/*!
 * \brief  parse command-line options
 *
 * \param  *argc - number of params from cmdline inclusive of program name
 * \param  *argv[] - list of commandline options
 * \param  *p_opt - structure to store options provided on cmdline in a
 *                  program friendly format
 * \return api_Success on successful conversion from string to enum Data_Type
 */
/*****************************************************************************/
api_Err_Status parse_cmdline( int argc , char **argv , Program_Options *p_opt)
{
    api_Err_Status err = api_Success ;
    char *short_opt = NULL ;
    int opt = 0 ;

    if( argv == NULL ) {
        debug("No options to parse from the command-line");
        err = api_Err_Param ;
        goto err_cmdline_parse ;
    }


    if( argc < 2 ) {
        debug("number of cmd-line options < 2");
        err = api_Err_Param ;
        goto err_cmdline_parse ;
    }

    if( p_opt == NULL ) {
        debug("variable to store cmdline-options = NULL");
        err = api_Err_Param ;
        goto err_cmdline_parse ;
    }


    /* initialise with default values */
    p_opt->type = DataType_MaxTypes ;
    p_opt->file = NULL ;
    p_opt->sep = NULL ;

    short_opt = gen_opt_string( g_option_list );
    if( short_opt == NULL ) {
        debug("Could not gen short opt-string for parsing cmdline opts");
        err = api_Err_Failure ;
        goto err_cmdline_parse ;
    }

    optind = 1 ;
    for( opt = getopt_long(argc, argv, short_opt, g_option_list, NULL) ;
         opt != -1 ; opt = getopt_long(argc, argv, short_opt, g_option_list, NULL)) {
        switch( opt )
        {
            case '?' :   /* intentional fall-through */
            case 'h' :   /* intentional fall-through */
            case ':' :
                usage( argv[0] , g_option_list , g_help_strings);
                err = api_Stat_Complete ;
                goto err_cmdline_parse ;
            case 'f' :
                p_opt->file = strdup(optarg);
                if( p_opt->file == NULL ) {
                    debug("Could not alloc memory to hold file-name [%s]", optarg);
                    err = api_Err_Memory ;
                    goto err_cmdline_parse ;
                }
                break ;
            case 's' :
                p_opt->sep = strdup(optarg);
                if( p_opt->sep == NULL ) {
                    debug("Could not alloc memory to hold separators [%s]", optarg);
                    err = api_Err_Memory ;
                    goto err_cmdline_parse ;
                }
                break ;
            case 'd' :
                err = map_data_types( &(p_opt->type), optarg);
                if( err != api_Success ) {
                    debug("Error mapping cmdline data-type [%s] to known type. err = %d", optarg, err );
                    goto err_cmdline_parse ;
                }
                break ;
        }
    }

    short_opt = (short_opt != NULL) ? free(short_opt), NULL : NULL ;
    return err ;

err_cmdline_parse :
    clean_cmdline_opts( p_opt ) ;
    short_opt = (short_opt != NULL) ? free(short_opt), NULL : NULL ;
    return err ;

}






/*****************************************************************************/
/*!
 * \brief  Clean up data structure holding command-line params. To be used
 *         either for error-handling or when termintating program
 *
 * \param  *p_opt - structure to store options provided on cmdline in a
 *                  program friendly format
 * \return api_Success on successful conversion from string to enum Data_Type
 */
/*****************************************************************************/
void clean_cmdline_opts( Program_Options *p_opt )
{
    if( p_opt == NULL )
        return ;

    p_opt->file = (p_opt->file != NULL) ? free(p_opt->file), NULL : NULL ;
    p_opt->sep = (p_opt->sep != NULL) ? free(p_opt->sep), NULL : NULL ;
    p_opt->type = DataType_MaxTypes ;
    return ;
}

