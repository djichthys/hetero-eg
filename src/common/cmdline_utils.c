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
#include "program_options.h"
#include "debug.h"

char *gplv2_banner =
"  This program is a set of examples about how to use openCL for use in heterogeneous programs\n\n\
   Copyright (C) 2017  Dejice Jacob\n\n\
   This file is part of %s.\n\
   %s is free software: you can redistribute it and/or modify\n\
   it under the terms of the GNU General Public License as published by\n\
   the Free Software Foundation, either version 2 of the License, or\n\
   (at your option) any later version.\n\
   %s is distributed in the hope that it will be useful,\n\
   but WITHOUT ANY WARRANTY; without even the implied warranty of\n\
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n\
   GNU General Public License for more details.\n\
   You should have received a copy of the GNU General Public License\n\
   along with %s.  If not, see <http://www.gnu.org/licenses/>.\n" ;


/*!
 * External Variable Declarations 
 */
extern Option_Help *g_help_strings ;
extern struct option *g_option_list;

/*****************************************************************************/
/*!
 * \brief  describe command line options provided on command-line to user
 *
 * \param  *prog - Name of the program - directly taken from argv[0]
 * \param  *opt_list - option-list to determine which options are available
 * \param  *help - help strings corresponding to opt_list.
 *                 are assumed to be line-separated
 * \return None
 */
/*****************************************************************************/
void usage( char *prog , const struct option *opt_list, const Option_Help *help )
{
    unsigned int idx_i, idx_j ;
    char *_default_prog = "Hetero-eg" ;
    

    /* sanity check inputs */
    if( opt_list == NULL ) {
        debug("List of long options = NULL");
        goto err_usage ;
    }
    if( help == NULL ) {
        debug("Help documentation for options list is empty");
        goto err_usage ;
    }
    debug("=====================================================================");
    if( prog != NULL )
        printf(gplv2_banner, prog , prog, prog, prog);
    else
        printf(gplv2_banner, _default_prog , _default_prog, _default_prog , _default_prog);
    debug("=====================================================================");
    debug("=====================================================================");
    debug("%s Options description.", (prog != NULL) ? prog : _default_prog );
    debug("=====================================================================");

    for( idx_i=0 ; opt_list[idx_i].name != NULL ; idx_i++ ) {
        for( idx_j=0 ; help[idx_j].option_text != NULL ; idx_j++ ) {
            if( opt_list[idx_i].val == help[idx_j].option ) {
                debug("[%u]. %s", idx_i+1, help[idx_j].option_text);
                break ;
            }
        }
    }

err_usage :
    return ;
}


/*****************************************************************************/
/*!
 * \brief  Function to map string verions of data-types on command-line to
 *         enumerations understood by program
 * \param  *dtype - output enumeration of data-type
 * \param  *type - input string denoting type of data. Supported strings
 *                     uint8,uint16,uint32,uint64,
 *                      int8, int16, int32, int64,
 *                      float,double,longdouble
 * \return api_Success on successful conversion from string to enum Data_Type
 */
/*****************************************************************************/
api_Err_Status map_data_types( Data_Type *dtype, uint8_t *type_str ) 
{
    api_Err_Status err = api_Success ; 

    /* Sanity check inputs */
    if( dtype == NULL ) {
        debug("output variable for data-type = NULL. Abort");
        err = api_Err_Param ;
        goto err_data_type_map ;
    }
    if( type_str == NULL ) {
        debug("input type string var = NULL. Abort");
        err = api_Err_Param ;
        goto err_data_type_map ;
    }


    if( strncasecmp(type_str , "uint8" , strlen("uint8")) == 0 ) {
        *dtype = DataType_uint8 ;
    } else if( strncasecmp(type_str , "uint16" , strlen("uint16")) == 0 ) {
        *dtype = DataType_uint16 ;
    } else if( strncasecmp(type_str , "uint32" , strlen("uint32")) == 0 ) {
        *dtype = DataType_uint32 ;
    } else if( strncasecmp(type_str , "uint64" , strlen("uint64")) == 0 ) {
        *dtype = DataType_uint64 ;
    } else if( strncasecmp(type_str , "int8" , strlen("int8")) == 0 ) {
        *dtype = DataType_int8 ;
    } else if( strncasecmp(type_str , "int16" , strlen("int16")) == 0 ) {
        *dtype = DataType_int16 ;
    } else if( strncasecmp(type_str , "int32" , strlen("int32")) == 0 ) {
        *dtype = DataType_int32 ;
    } else if( strncasecmp(type_str , "int64" , strlen("int64")) == 0 ) {
        *dtype = DataType_int64 ;
    } else if( strncasecmp(type_str , "float" , strlen("float")) == 0 ) {
        *dtype = DataType_float ;
    } else if( strncasecmp(type_str , "double" , strlen("double")) == 0 ) {
        *dtype = DataType_double ;
    } else if( strncasecmp(type_str , "longdouble" , strlen("longdouble")) == 0 ) {
        *dtype = DataType_long_double ;
    } else { 
        *dtype = DataType_MaxTypes ;
        err = api_Err_Param ;
    }

err_data_type_map : 
    return err ;
}




/*****************************************************************************/
/*!
 * \brief  generate the short option string for use with getopt_long automatically
 *         This will be generated automatically from the long-option list.
 *
 * \param  *lopt - Long option list which is created for each program
 * \return on Success, returns the short-option string.Otherwise NULL
 */
/*****************************************************************************/
uint8_t *gen_opt_string( struct option *lopt )
{
    uint32_t len_optstr = 0 ;
    uint8_t *opt_str = NULL ;
    int idx_i , idx_j ;

    /* sanity check inputs */
    if( lopt == NULL ) {
        debug("Long option list param = NULL");
        goto err_optstr_gen ;
    }

    /* determine required length of the option-string */
    for( idx_i=0 ; lopt[idx_i].name != NULL ; idx_i++ ) {
        len_optstr += sizeof( lopt[idx_i].val );
        switch( lopt[idx_i].has_arg )
        {
            case required_argument :
                 len_optstr++ ;
                 break ;
            case optional_argument :
                 len_optstr += 2 ;
                 break ;
            case no_argument :
                 break ;
            default :
                 debug("Illegal value for .has_arg(%d) for option (%c)", lopt[idx_i].has_arg, lopt[idx_i].val);
                 goto err_optstr_gen ;

        }
    }

    /* allocate enough memory to hold string */
    opt_str = calloc( len_optstr+1, sizeof(uint8_t));
    if( opt_str == NULL ) {
        debug("Could not malloc(%u) bytes to hold opt-string", (len_optstr+1) * sizeof(uint8_t));
        goto err_optstr_gen ;
    }

    /* generate option string */
    for( idx_i=0, idx_j=0 ; lopt[idx_i].name != NULL ; idx_i++ ) {
        opt_str[idx_j++] = lopt[idx_i].val ;
        switch( lopt[idx_i].has_arg )
        {
            case optional_argument :
                 opt_str[idx_j++] = ':' ;   /* intentional fall-through -optional arg = '::'*/
            case required_argument :
                 opt_str[idx_j++] = ':' ;
                 break ;
            case no_argument :
                 break ;
            default :
                 debug("Illegal value for .has_arg(%d) for option (%c)", lopt[idx_i].has_arg, lopt[idx_i].val);
                 goto err_optstr_gen ;

        }
    }

    return opt_str ;

err_optstr_gen :
    opt_str = (opt_str != NULL) ? free(opt_str) , NULL : NULL ;
    return opt_str ;
}


