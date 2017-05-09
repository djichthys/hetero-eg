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

#include <stdio.h>
#include <stdint.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <unistd.h>

#include "debug.h"
#include "api_err.h"
#include "datatype.h"

/*!
 * Internal Utility function declarations
 */
static api_Err_Status _read_file( uint8_t **, Vector_MetaData *,  char *, char *);
static api_Err_Status _alloc_ND_mem( void **, Vector_MetaData *, uint32_t, uint64_t );
static void  _dealloc_ND_mem( void **, Vector_MetaData *, uint32_t );
static api_Err_Status _convert_to_number( uint8_t *, Vector_MetaData *, void *, uint64_t );

static inline uint32_t _1D_sizeof_dim( Vector_MetaData *);
static inline uint32_t _2D_sizeof_dim( Vector_MetaData *, uint32_t );
static inline uint32_t _3D_sizeof_dim( Vector_MetaData *, uint32_t );

static api_Err_Status _detect_1D_items( uint8_t *, uint8_t *, Vector_MetaData *);
static api_Err_Status _detect_2D_sizes( uint8_t *, uint8_t *, Vector_MetaData *);
static api_Err_Status _detect_3D_sizes( uint8_t *, uint8_t *, Vector_MetaData *);

static api_Err_Status _parse_data_1D( void *, uint8_t *, uint8_t *, Vector_MetaData *) ;
static api_Err_Status _parse_data_2D( void **, uint8_t *, uint8_t *, Vector_MetaData *) ;
static api_Err_Status _parse_data_3D( void ***, uint8_t *, uint8_t *, Vector_MetaData *) ;



/*****************************************************************************/
/*!
 * \brief  convert list of delimiters to a string format
 *         that can readily be used for strtok for parsing. The data dimensions
 *         are considered to be uniform and the input data is not 'sparse'
 * \param  ***out - output buffer holding parsed data from file
 * \param  *meta - detected dimension. Memory should be allocated by caller.
 *                      Caller should also fill in d_type with correct entry
 *                      before this function is called
 * \param  *path - file-name to parse
 * \param  *sep -  separator between dimensions. the separator string
 * \return returns api_Success on success.
 */
/*****************************************************************************/
api_Err_Status read_data( void **out, Vector_MetaData *meta, char *path, uint8_t *sep )
{
    api_Err_Status err = api_Success ;
    uint32_t idx_i = 0, idx_j = 0 , type_size = 0 ;
    uint8_t *filebuff = NULL, *linebuff = NULL, *first_line = NULL ;
    uint8_t *l_ptr = NULL , *d_ptr = NULL , *v_ptr = NULL ;
    char *conv_err = NULL ;


    double d_temp = 0.0 ;
    float f_temp = 0.0 ;
    long double ld_temp = 0.0 ;

    /* sanity check the input parameters */
    if( out == NULL ) {
        debug("double pointer passed into funcion = NULL");
        err = api_Err_Param ;
        goto err_data_read ;
    } else if( *out != NULL ) {
        debug("Possible over-writing of valid memory - Abort");
        err = api_Err_Param ;
        goto err_data_read ;
    }

    if( meta == NULL ) {
        debug("Cannot return vector-metadata of the array space in a NULL pointer");
        err = api_Err_Param ;
        goto err_data_read ;
    }
    memset((void *)&(meta->dim), 0 , sizeof(meta->dim));


    if( path == NULL ) {
        debug("Cannot read data - invalid path = NULL") ;
        err = api_Err_Param ;
        goto err_data_read ;
    }

    if( sep == NULL ) {
        debug("Spearator string = NULL") ;
        err = api_Err_Param ;
        goto err_data_read ;
    }


    /*!
     * Read data from file into buffer
     * Also detect no of dimensions and
     * populate length in each dimension
     */
    err = _read_file( &filebuff, meta, path, sep);
    if( err != api_Success ) {
        debug("Could not read data from file[%s]", path);
        goto err_data_read ;
    }

    debug("[%d]-dimensional data within file detected", meta->no_dims);

    /* call allocation function based on number of detected dimensions */
    err = _alloc_ND_mem((void **)out, meta, 0, 1);
    if( err != api_Success ) {
        debug("Could not allocate enough memory in single dimension. err = %d", err );
        goto err_data_read_mem ;
    }
    switch( meta->no_dims )
    {
        case 1 :
            err =  _parse_data_1D( *out, filebuff , sep, meta );
            if( err != api_Success ) {
                debug("Error parsing 1D data. err = %d", err );
                goto err_data_read_mem ;
            }
            break ;
        case 2 :
            err =  _parse_data_2D((void **)(*out), filebuff , sep, meta );
            if( err != api_Success ) {
                debug("Error parsing 2D data. err = %d", err );
                goto err_data_read_mem ;
            }
            break ;
        case 3 :
            err =  _parse_data_3D((void ***)(*out), filebuff , sep, meta );
            if( err != api_Success ) {
                debug("Error parsing 3D data. err = %d", err );
                goto err_data_read_mem ;
            }
            break ;
        default :
            debug("Support only upto 3-Dimensions provided");
            err = api_Err_Failure ;
            goto err_data_read_mem ;
    }

    filebuff = (filebuff != NULL) ? free(filebuff), NULL : NULL ;
    return err ;

err_data_read_mem :
    if( out != NULL ) {
        *out = (*out != NULL) ? free(*out) , NULL : NULL ;
    }

err_data_read :
    filebuff = (filebuff != NULL) ? free(filebuff), NULL : NULL ;
    return err ;
}

/*****************************************************************************/
/*!
 * \brief  helper function to return type-size
 * \param  metadata - data type encoded
 * \return returns sizeof the data-type
 */
/*****************************************************************************/
uint32_t sizeof_datatype( Data_Type type )
{
    uint32_t type_size ;
    switch( type )
    {
        case DataType_uint8        : type_size = sizeof(uint8_t)     ;  break ;
        case DataType_uint16       : type_size = sizeof(uint16_t)    ;  break ;
        case DataType_uint32       : type_size = sizeof(uint32_t)    ;  break ;
        case DataType_uint64       : type_size = sizeof(uint64_t)    ;  break ;
        case DataType_int8         : type_size = sizeof(int8_t)      ;  break ;
        case DataType_int16        : type_size = sizeof(int16_t)     ;  break ;
        case DataType_int32        : type_size = sizeof(int32_t)     ;  break ;
        case DataType_int64        : type_size = sizeof(int64_t)     ;  break ;
        case DataType_float        : type_size = sizeof(float)       ;  break ;
        case DataType_double       : type_size = sizeof(double)      ;  break ;
        case DataType_long_double  : type_size = sizeof(long double) ;  break ;
        default :                    type_size = 0 ;
    }
    return type_size ;
}


/*****************************************************************************/
/*!
 * \brief  free memory allocated while parsing input file
 * \param  ***matrix - input buffer holding parsed data from file
 * \return returns api_Success on success.
 */
/*****************************************************************************/
api_Err_Status clean_data( void **buff, Vector_MetaData *meta )
{
    _dealloc_ND_mem( buff, meta, 0 );
    return api_Success ;
}



/*****************************************************************************/
/*!
 * \brief  Read Entire file content into a buffer,
 *         While doing this, also detect number of axes and also dimensions
 *         on each axis
 * \param[out] **buffer - output buffer into which file is read
 * \param[out] *meta - Meta-data about the file
 * \param[in]  *path - file path
 * \param[in]  *sep - List of separators (upto 3) in 'ascending' order.
 *
 * \note       *sep The data will be parsed and spatially co-located in order of
 *             separators and array subscripts are in order of separators
 *             i.e if sep = "\n|,", then data is assumed to be in the format
 *                    a0,b0,c0,....|a1,b1,c1,.....|............\n
 *                    d0,e0,f0,....|d1,e1,f1,.....|............\n
 *                            .........................        \m
 *                            .........................        \m
 *                            .........................        \m
 *                            .........................        \m
 *                    ax,bx,cx  are in contiguous memory.
 *                    buff[i][j][k] subscripting map is i->\n, j->| , k->,
 * \return returns api_Success on success.
 */
/*****************************************************************************/
static api_Err_Status _read_file( uint8_t **buff, Vector_MetaData *meta,  char *path , char *sep )
{
    api_Err_Status err = api_Success ;
    int fd = -1 ;
    uint32_t idx_i = 0, max_dim = 0 ;
    int32_t idx_k = 0 ;
    uint64_t dim_x = 0 , dim_y = 0 , dim_z = 0 ;
    size_t chunk = 4*1024 ;
    ssize_t rd , bytes ;
    struct stat sb ;
    uint8_t *file_content = NULL ;


    meta->no_dims = 0 ;
    max_dim = strlen(sep);

    /* check the file */
    fd = open( path , O_RDONLY );
    if( fd == -1 ) {
        debug("Could not open file[%s]. errno = %d", path, errno ) ;
        err = api_Err_File ;
        goto err_file_read ;
    }

    memset( &sb , 0  , sizeof(struct stat));
    if( fstat( fd , &sb) == -1) {
        debug("Could not read file meta-data. errno = %d", errno );
        err = api_Err_File ;
        goto err_file_read ;
    }

    if((sb.st_mode & S_IFMT) != S_IFREG ) {
        debug("file [%s] is not a file!", path);
        err = api_Err_File ;
        goto err_file_read ;
    }

    /* create a buffer to read the entire file into memory */
    *buff = malloc((sb.st_size * sizeof(uint8_t)) + 1);
    if( *buff == NULL ) {
        debug("Could not alloc(%u) bytes to read file", sb.st_size+1,  path);
        err = api_Err_File ;
        goto err_file_read ;
    }


    /* Read entire file into memory buffer */
    for( rd=0, bytes=0 ; rd < sb.st_size ; rd += bytes ) {
        chunk = ((sb.st_size-rd) >= LINE_SIZE) ? LINE_SIZE : (sb.st_size - rd) ;
        bytes = read(fd, *buff + rd, chunk );
        if( bytes == -1 ) {
            if( errno == EINTR ){
                bytes = 0 ;
            } else {
                debug("read() failed errno(%d).Reading file[%s], fd=(%d), read-so-far=(0x%08x), chunk-size=(%u)", errno, path, fd, rd, chunk);
                err = api_Err_File ;
                goto err_file_read_mem ;
            }
        }

        /*!
         * Figure out number of dimensions from file. The maximum number of
         * dimensions are figured from the separator list. If we reach max
         * known dimensions, then don't do the search again.
         */
        for( idx_i=0 ; (idx_i < bytes) && (meta->no_dims < max_dim); idx_i++ ) {
            for( idx_k=meta->no_dims ; idx_k < max_dim ; idx_k++ ) {
                if( (*buff)[rd + idx_i] == sep[idx_k] ) {
                    meta->no_dims++ ;
                    break ;
                }
            }
        }
    }   /* for ( read buffer from file ) */


    /*!
     * depending on how many dimensions
     * populate union with details of how many values in each dimension
     * NOTE : The code is generic enough for upto 3-dimensions. Hence why
     * dim_z is used for 1-d data , y,z for 2-d data amd x,y,z for 3-d data
     */
    switch( meta->no_dims )
    {
        case 1 :
            err = _detect_1D_items( *buff, sep, meta );
            if( err != api_Success ) {
                debug("Error detecting no.of 1D items. err = %d", err );
                goto err_file_read_mem ;
            }
            break ;
        case 2 :
            err = _detect_2D_sizes( *buff, sep, meta );
            if( err != api_Success ) {
                debug("Error detecting no.of 2D items. err = %d", err );
                goto err_file_read_mem ;
            }
            break ;
        case 3 :
            err = _detect_3D_sizes( *buff, sep,  meta );
            if( err != api_Success ) {
                debug("Error detecting no.of 3D items. err = %d", err );
                goto err_file_read_mem ;
            }
            break ;
        default :
            debug("Parser only has support for maximum 3D data");
            err = api_Err_Param ;
            goto err_file_read_mem ;
    }



    if( close(fd) ) {
        debug("Closing file[%s] (fd=%d) returned  error. errno = %d", path, fd, errno);
        err = api_Err_File ;
        goto err_file_read_mem ;
    }

    return err ;

err_file_read_mem :
    if( buff != NULL )
        *buff = (*buff != NULL) ? free(*buff) , NULL : NULL ;

err_file_read :
    if((fd != -1) && close(fd) != 0 )
        debug("close(%d)'ing  file [%s] caused error : %d", fd , path , errno );

    return err ;
}




/*****************************************************************************/
/*!
 * \brief  Dynamically create an n-dimensional array (currently set at
 *         3-dimensions)
 *
 * \param  ***out - output buffer holding parsed data from file
 * \param  *meta -  detected dimension. Memory should be allocated by caller.
 *                      Caller should also fill in type with correct entry
 *                      before this function is called
 * \param  dimension  - which axis are we currently allocating memory for
 * \param  mult_factor- to be used in recursion. Factor to multiply with
 *                      number of elements in current dimension while allocating
 *                      space. This will be the mult-accumulated value from all the
 *                      number of elements of previous axes
 * \return returns api_Success on success.
 * \note   This function will be called recursively to add memory for the next
 *         dimension until we exhaust dimensions
 */
/*****************************************************************************/
static api_Err_Status _alloc_ND_mem( void **out, Vector_MetaData *meta, uint32_t dimension , uint64_t mult_factor)
{
    api_Err_Status err = api_Success ;
    uint64_t idx_i = 0 ;
    uint32_t type_size = 0, next_dim_size_t = 0 , next_dim_elem = 0 ;
    uint64_t elem = 0 ;

    /* sanity check inputs */
    if( out == NULL ) {
        debug("Cannot alloc memory into NULL pointer");
        err = api_Err_Param ;
        goto err_NDmem_alloc ;
    } else if( *out != NULL ) {
        debug("Possible over-write of memory!");
        err = api_Err_Param ;
        goto err_NDmem_alloc ;
    }

    if( meta == NULL ) {
        debug("Meta-data about data-in-memory = NULL");
        err = api_Err_Param ;
        goto err_NDmem_alloc ;
    }


    /* We do not need to allocate for a non-existent dimension */
    if( dimension >= meta->no_dims )
        return err ;

    /*!
     * If we are at the last dimension sizeof-type should be
     * actual size of type, otherwise it should be type (void *)
     */
    type_size = ((dimension + 1) == meta->no_dims) ? sizeof_datatype( meta->type ) : sizeof(void *);


    /* Figure out how many members of each dimension we have to allocate */
    switch( meta->no_dims )
    {
        case 1 :
            elem = _1D_sizeof_dim( meta ) ;
            break ;
        case 2 :
            elem = _2D_sizeof_dim( meta, dimension ) ;
            next_dim_elem = _2D_sizeof_dim( meta, dimension+1 ) ;
            break ;
        case 3 :
            elem = _3D_sizeof_dim( meta, dimension ) ;
            next_dim_elem = _3D_sizeof_dim( meta, dimension+1 ) ;
            break ;
        default :
            debug("Currently only upto 3 dimensions supported");
            err = api_Err_Param ;
            goto err_NDmem_alloc ;
    }

    /* Allocate space to hold pointers to current stride in the matrix */
    *out = calloc( elem * mult_factor, type_size );
    if( *out == NULL ) {
        debug("Could not allocate memory space to hold %u elements . Type-size = %u", elem, type_size );
        err = api_Err_Memory ;
        goto err_NDmem_alloc ;
    }

    /* Allocate space for next dimension */
    err = _alloc_ND_mem((void **)*out, meta, dimension+1, mult_factor*elem);
    if( err != api_Success ) {
        debug("Could not allocate memory for dimension %u", dimension+1);
        goto err_NDmem_alloc_mem ;
    }

    /*!
     * Add pointers to all other rows into pointer list for access
     * We do not do this if this invocation is for the last dimension
     * as we will end up putting corrupt values (i.e addresses from NULL)
     *r into the last dimension access. If these pages are locked down,
     * then this will cause exceptions
     */
    if((dimension+1) < meta->no_dims ) {
        next_dim_size_t = ((dimension+2) == meta->no_dims) ? sizeof_datatype( meta->type ) : sizeof(void *) ;
        for( idx_i=1 ; idx_i < (elem * mult_factor) ; idx_i++ ) {
            ((void **)(*out))[idx_i] = **((void ***)out) + (idx_i * next_dim_elem * next_dim_size_t ) ;
        }
    }

    return err ;

err_NDmem_alloc_mem :
    *out = (*out != NULL) ? free(*out), NULL : NULL ;
err_NDmem_alloc :
    return err ;
}



/*****************************************************************************/
/*!
 * \brief  Dynamically create an n-dimensional array (currently set at
 *         3-dimensions)
 *
 * \param  ***out - output buffer holding parsed data from file
 * \param  *meta -  detected dimension. Memory should be allocated by caller.
 *                      Caller should also fill in type with correct entry
 *                      before this function is called
 * \param  dimension  - which axis are we currently allocating memory for
 * \return void
 * \note   This function will be called recursively to add memory for the next
 *         dimension until we exhaust dimensions
 */
/*****************************************************************************/
static void  _dealloc_ND_mem( void **out, Vector_MetaData *meta, uint32_t dimension )
{
    api_Err_Status err = api_Success ;

    if( meta == NULL )
        goto err_NDmem_dealloc ;

    /* Check if function has hit recursive limit */
    if( dimension >= meta->no_dims)
        return ;

    if( out == NULL )
        goto err_NDmem_dealloc ;

    _dealloc_ND_mem((void **)(*out), meta, (dimension+1));
    *out = (*out != NULL) ? free(*out) , NULL : NULL ;

err_NDmem_dealloc :
    return ;

}


/*****************************************************************************/
/*!
 * \brief  Function to return no of items in a dimension
 * \param  *meta -  Data structure holding global meta-data about dimensions
 * \return returns api_Success on success.
 */
/*****************************************************************************/
static inline uint32_t _1D_sizeof_dim( Vector_MetaData *meta )
{
    return meta->dim.dim_1d.items ;
}

/*****************************************************************************/
/*!
 * \brief  Function to return no of items in a dimension for the 2D structure
 * \param  *meta -  Data structure holding global meta-data about dimensions
 * \return returns api_Success on success.
 */
/*****************************************************************************/
static inline uint32_t _2D_sizeof_dim( Vector_MetaData *meta , uint32_t dim )
{
    uint64_t dim_size = 0 ;
    if((dim+1) == 1 )
        dim_size = meta->dim.dim_2d.rows ;
    else if((dim+1) == 2 )
        dim_size = meta->dim.dim_2d.cols ;

    return dim_size ;
}


/*****************************************************************************/
/*!
 * \brief  Function to return no of items in a dimension for the 3D structure
 * \param  *meta -  Data structure holding global meta-data about dimensions
 * \return returns api_Success on success.
 */
/*****************************************************************************/
static inline uint32_t _3D_sizeof_dim( Vector_MetaData *meta , uint32_t dim )
{
    uint64_t dim_size = 0 ;
    if((dim+1) == 1 )
        dim_size = meta->dim.dim_3d.dim_x ;
    else if((dim+1) == 2 )
        dim_size = meta->dim.dim_3d.dim_y ;
    else if((dim+1) == 3 )
        dim_size = meta->dim.dim_3d.dim_z ;

    return dim_size ;
}




/*****************************************************************************/
/*!
 * \brief  Function to return no of items in a dimension for the 3D structure
 * \param  *buff -  Output Buffer which will have parsed data
 * \param  *file_content -  Buffer holding content of file
 * \param  *sep_list -  List of separators - only the first is considered for 1D data
 * \param  *meta -  Data structure holding global meta-data about dimensions
 * \return returns api_Success on success.
 */
/*****************************************************************************/
static api_Err_Status _parse_data_1D( void *buff, uint8_t *file_content , uint8_t *sep_list , Vector_MetaData *meta )
{
    api_Err_Status err = api_Success ;
    uint8_t *str = NULL , *l_ptr = NULL ;
    uint8_t sep[2] = "" ;
    uint64_t idx_i = 0 ;
    uint8_t *conv_err = NULL ;
    float f_temp = 0.0f ;
    double d_temp = 0.0 ;
    long double ld_temp = 0.0 ;

    /* sanity check inputs */
    if( buff == NULL ) {
        debug("Cannot write values into buffer");
        err = api_Err_Param ;
        goto err_1D_parse ;
    }
    if( file_content == NULL ) {
        debug("Data read from file = NULL");
        err = api_Err_Param ;
        goto err_1D_parse ;
    }
    if( sep_list == NULL ) {
        debug("Separator list = NULL");
        err = api_Err_Param ;
        goto err_1D_parse ;
    }
    if( meta == NULL ) {
        debug("Vector meta-data structure = NULL. Abort");
        err = api_Err_Param ;
        goto err_1D_parse ;
    }


    sep[0] = sep_list[0] ;
    for( str = strtok_r(file_content, sep, &l_ptr) , idx_i = 0 ;
                       str != NULL ; str = strtok_r(NULL, sep, &l_ptr), idx_i++) {
        err = _convert_to_number( str, meta, buff, idx_i );
        if( err != api_Success ) {
            debug("Error converting string to value. index = %u, err = %d", idx_i, err );
            goto err_1D_parse ;
        }
    }

err_1D_parse :
    return err ;
}



/*****************************************************************************/
/*!
 * \brief  Function to return no of items in a dimension for the 3D structure
 * \param  **buff -  Output Buffer which will have parsed data
 * \param  *file_content -  Buffer holding content of file
 * \param  *sep_list -  List of separators - only the first is considered for 1D data
 * \param  *meta -  Data structure holding global meta-data about dimensions
 * \return returns api_Success on success.
 */
/*****************************************************************************/
static api_Err_Status _parse_data_2D( void **buff, uint8_t *file_content , uint8_t *sep_list , Vector_MetaData *meta )
{
    api_Err_Status err = api_Success ;
    uint8_t *linebuff = NULL , *l_ptr = NULL , *v_ptr = NULL, *d_ptr = NULL ;
    uint8_t sep_r[2] = "" , sep_c[2] = "" ;
    uint64_t idx_i = 0 , idx_j = 0 ;
    uint8_t *conv_err = NULL ;
    float f_temp = 0.0f ;
    double d_temp = 0.0 ;
    long double ld_temp = 0.0 ;

    /* sanity check inputs */
    if((buff == NULL ) || (*buff == NULL)) {
        debug("Cannot write values into NULL buffer. buff = %p", buff);
        err = api_Err_Param ;
        goto err_2D_parse ;
    }
    if( file_content == NULL ) {
        debug("Data read from file = NULL");
        err = api_Err_Param ;
        goto err_2D_parse ;
    }
    if( sep_list == NULL ) {
        debug("Separator list = NULL");
        err = api_Err_Param ;
        goto err_2D_parse ;
    }
    memset( sep_r, 0, sizeof(sep_r));
    memset( sep_c, 0, sizeof(sep_c));

    if( meta == NULL ) {
        debug("Vector meta-data structure = NULL. Abort");
        err = api_Err_Param ;
        goto err_2D_parse ;
    }

    sep_r[0] = sep_list[1];   /* Second delimiter is for rows */
    sep_c[0] = sep_list[0];    /* First delimiter is for columns */

    /* Parse files for data and convert */
    for( linebuff = strtok_r(file_content, sep_r, &l_ptr) , idx_i = 0 ;
                linebuff != NULL ; linebuff = strtok_r(NULL, sep_r, &l_ptr), idx_i++) {
        for( v_ptr = strtok_r(linebuff, sep_c, &d_ptr) , idx_j = 0 ;
                    v_ptr != NULL ; v_ptr = strtok_r(NULL, sep_c, &d_ptr) , idx_j++) {

            err = _convert_to_number( v_ptr, meta, (void *)(*buff),
                                       (idx_i * meta->dim.dim_2d.cols) + idx_j );
            if( err != api_Success ) {
                debug("Error converting string to value. index = %u, err = %d", idx_i, err );
                goto err_2D_parse ;
            }
        }    /* for each column */
    }   /* for each row */

err_2D_parse :
    return err ;
}





/*****************************************************************************/
/*!
 * \brief  Function to return no of items in a dimension for the 3D structure
 * \param  ***buff -  Output Buffer which will have parsed data
 * \param  *file_content -  Buffer holding content of file
 * \param  *sep_list -  List of separators - only the first is considered for 1D data
 * \param  *meta -  Data structure holding global meta-data about dimensions
 * \return returns api_Success on success.
 */
/*****************************************************************************/
static api_Err_Status _parse_data_3D( void ***buff, uint8_t *file_content , uint8_t *sep_list , Vector_MetaData *meta )
{
    api_Err_Status err = api_Success ;
    uint8_t *dimx_ptr = NULL , *dimy_ptr = NULL, *dimz_ptr = NULL;
    uint8_t *save_x = NULL , *save_y = NULL, *save_z = NULL;
    uint8_t sep_x[2] = "", sep_y[2] = "", sep_z[2] = "" ;
    uint64_t idx_i = 0 , idx_j = 0, idx_k = 0 ;
    uint8_t *conv_err = NULL ;
    float f_temp = 0.0f ;
    double d_temp = 0.0 ;
    long double ld_temp = 0.0 ;

    /* sanity check inputs */
    if((buff == NULL ) || (*buff == NULL) || (**buff == NULL)) {
        debug("Cannot write values into NULL buffer. buff = %p , *buff = %p"
                                               , buff , buff ? *buff : "buff = null" );
        err = api_Err_Param ;
        goto err_3D_parse ;
    }
    if( file_content == NULL ) {
        debug("Data read from file = NULL");
        err = api_Err_Param ;
        goto err_3D_parse ;
    }
    if( sep_list == NULL ) {
        debug("Separator list = NULL");
        err = api_Err_Param ;
        goto err_3D_parse ;
    }
    memset( sep_x, 0, sizeof(sep_x));
    memset( sep_y, 0, sizeof(sep_y));
    memset( sep_z, 0, sizeof(sep_z));

    if( meta == NULL ) {
        debug("Vector meta-data structure = NULL. Abort");
        err = api_Err_Param ;
        goto err_3D_parse ;
    }

    sep_x[0] = sep_list[2];   /* Third delimiter is for dimension-x */
    sep_y[0] = sep_list[1];   /* Second delimiter is for dimension-y */
    sep_z[0] = sep_list[0];   /* First delimiter is for dimension-z */

    /* Parse files for data and convert */
    for( dimx_ptr = strtok_r(file_content, sep_x, &save_x) , idx_i = 0 ;
                dimx_ptr != NULL ; dimx_ptr = strtok_r(NULL, sep_x, &save_x), idx_i++) {
        for( dimy_ptr = strtok_r(dimx_ptr, sep_y, &save_y) , idx_j = 0 ;
                    dimy_ptr != NULL ; dimy_ptr = strtok_r(NULL, sep_y, &save_y) , idx_j++) {
            for( dimz_ptr = strtok_r(dimy_ptr, sep_z, &save_z) , idx_k = 0 ;
                        dimz_ptr != NULL ; dimz_ptr = strtok_r(NULL, sep_z, &save_z) , idx_k++) {

                err = _convert_to_number( dimz_ptr, meta, (void *)(**buff),
                                          (idx_i * meta->dim.dim_3d.dim_y * meta->dim.dim_3d.dim_z) + (idx_j * meta->dim.dim_3d.dim_z) +  idx_k );
                if( err != api_Success ) {
                    debug("Error converting string to value. index = %u, err = %d", idx_i, err );
                    goto err_3D_parse ;
                }
            }   /* for each item in dim-z */
        }   /* for each item in dim-y */
    }   /* for each item in dim-x */

err_3D_parse :
    return err ;
}





/*****************************************************************************/
/*!
 * \brief  Function to convert text to a number of specified type
 * \param  *str  -  string containing text to convert to number
 * \param  *meta -  Data structure holding global meta-data about type
 * \param  *buff -  output buffer into which data is stored
 * \param  idx_i -  index at which item should be placed in buff
 * \return returns api_Success on success.
 */
/*****************************************************************************/
static api_Err_Status _convert_to_number( uint8_t *str, Vector_MetaData *meta, void *buff, uint64_t idx_i )
{
    api_Err_Status err = api_Success ;
    char *conv_err = NULL ;
    float f_temp = 0.0f ;
    double d_temp = 0.0 ;
    long double ld_temp = 0.0 ;

    if( buff == NULL ) {
        debug("Buffer to place value into = NULL");
        err = api_Err_Param ;
        goto err_convert ;
    }
    if( meta == NULL ) {
        debug("Meta data about data = NULL");
        err = api_Err_Param ;
        goto err_convert ;
    }


    errno = 0 ;
    switch( meta->type )
    {
        case DataType_uint8 :
            ((uint8_t *)buff)[idx_i] = (uint8_t)strtoul(str, NULL, 0);
            break ;
        case DataType_uint16 :
            ((uint16_t *)buff)[idx_i] = (uint16_t)strtoul(str, NULL, 0);
            break ;
        case DataType_uint32 :
            ((uint32_t *)buff)[idx_i] = (uint32_t)strtoul(str, NULL, 0);
            if(((uint32_t *)buff)[idx_i] == ULONG_MAX ) {
                debug("strtoul() returned overflow");
                err = api_Err_Failure ;
                goto err_convert ;
            }
            break ;
        case DataType_uint64 :
            ((uint64_t *)buff)[idx_i] = (uint64_t)strtoull(str, NULL, 0);
            if(((uint64_t *)buff)[idx_i] == ULLONG_MAX ) {
                debug("strtoull() returned overflow");
                err = api_Err_Failure ;
                goto err_convert ;
            }
            break ;
        case DataType_int8 :
            ((int8_t *)buff)[idx_i] = (int8_t)strtol(str, NULL, 0);
            break ;
        case DataType_int16 :
            ((int16_t *)buff)[idx_i] = (int16_t)strtol(str, NULL, 0);
            break ;
        case DataType_int32 :
            ((int32_t *)buff)[idx_i] = (int32_t)strtol(str, NULL, 0);
            if(((int32_t *)buff)[idx_i] == LONG_MIN ) {
                debug("strtol() returned underflow");
                err = api_Err_Failure ;
                goto err_convert ;
            } else if(((int32_t *)buff)[idx_i] == LONG_MAX ) {
                debug("strtol() returned overflow");
                err = api_Err_Failure ;
                goto err_convert ;
            }
            break ;
        case DataType_int64 :
            ((int64_t *)buff)[idx_i] = (int64_t)strtoll(str, NULL, 0);
            if(((int64_t *)buff)[idx_i] == LLONG_MIN ) {
                debug("strtoll() returned underflow");
                err = api_Err_Failure ;
                goto err_convert ;
            } else if(((int64_t *)buff)[idx_i] == LLONG_MAX ) {
                debug("strtoll() returned overflow");
                err = api_Err_Failure ;
                goto err_convert ;
            }
            break ;
        case DataType_float :
            f_temp = (float)strtof(str , &conv_err );
            if((f_temp == 0.0 ) && (conv_err == str)){
                debug("strtof() conversion error @ (zero-indexed) line-%llu", idx_i );
                err = api_Err_Failure ;
                goto err_convert ;
            } else if( f_temp == HUGE_VALF ) {
                debug("strtof() value saturation");
                err = api_Err_Failure ;
                goto err_convert ;
            }
            ((float *)buff)[idx_i] = f_temp ;
            break ;
        case DataType_double :
            d_temp = (double)strtod(str , &conv_err );
            if((d_temp == 0.0 ) && (conv_err == str)) {
                debug("strtod() conversion error @ (zero-indexed) line-%llu", idx_i);
                err = api_Err_Failure ;
                goto err_convert ;
            } else if( d_temp == HUGE_VAL ) {
                debug("strtod() value saturation");
                err = api_Err_Failure ;
                goto err_convert ;
            }
            ((double *)buff)[idx_i] = d_temp ;
            break ;
        case DataType_long_double  :
            ld_temp = (long double)strtold( str, &conv_err );
            if((ld_temp == 0.0 ) && (conv_err == str)) {
                debug("strtold() conversion error @ (zero-indexed) line-%llu", idx_i);
                err = api_Err_Failure ;
                goto err_convert ;
            } else if( ld_temp == HUGE_VALL ) {
                debug("strtold() value saturation");
                err = api_Err_Failure ;
                goto err_convert ;
            }
            ((long double *)buff)[idx_i] = ld_temp ;
            break ;
        default :
            debug("Unknown data-type");
            err = api_Err_Param ;
            goto err_convert ;
    }

err_convert :
    return err ;
}





/*****************************************************************************/
/*!
 * \brief  Given a long string (*buff)er and a separator string, count how many
 *         items are available in the string
 *
 * \param  *(*buff) - data (*buff)er with text to count number of items
 * \param  *sep_list -  list of separators from command-line. Only the first
 *                      character is considered as a separator.
 * \param  *meta  - meta-data structure, which contains no-of-dimensions
 *                  (caller responsibility). The number of items will be
 *                  populated in this structure
 *
 * \return returns api_Success on success.
 */
/*****************************************************************************/
static api_Err_Status _detect_1D_items( uint8_t *buff, uint8_t *sep_list, Vector_MetaData *meta )
{
    api_Err_Status err = api_Success ;
    uint8_t *content = NULL , *l_ptr=NULL, *linebuff = NULL, sep[2] = "" ;

    /* Sanity check inputs */
    if( buff == NULL ) {
        debug("Empty (*buff) data passed into function");
        err = api_Err_Param ;
        goto err_1D_item_detect ;
    }
    if( sep_list == NULL ) {
        debug("Empty separator string passed into function");
        err = api_Err_Param ;
        goto err_1D_item_detect ;
    }
    if( meta == NULL ) {
        debug("meta-data struct = NULL");
        err = api_Err_Param ;
        goto err_1D_item_detect ;
    }
    if( meta->no_dims != 1 ) {
        debug("Error : function called with dimensions [%u]", meta->no_dims);
        err = api_Err_Param ;
        goto err_1D_item_detect ;
    }

    /* obtain duplicates of data buffer */
    content = strdup(buff);
    if( content == NULL ) {
        debug("Could not duplicate data buffer in RAM to count items");
        err = api_Err_Memory ;
        goto err_1D_item_detect ;
    }

    /* obtain duplicate of separator string */
    memset(sep,0,sizeof(sep));
    sep[0] = sep_list[0];

    for( linebuff = strtok_r(content, sep, &l_ptr) ;
                       linebuff != NULL ; linebuff = strtok_r(NULL, sep, &l_ptr) )
        meta->dim.dim_1d.items++ ;


err_1D_item_detect :
    content = (content != NULL) ? free(content), NULL : NULL ;
    return err ;
}

/*****************************************************************************/
/*!
 * \brief  Given a long string buffer and a separator string, count how many
 *         items are available in the string
 *
 * \param  *buff - data buffer with text to count number of items
 * \param  *sep_list -  list of separators from command-line. Only the first
 *                      two characters are considered as a separator.
 * \param  *meta  - meta-data structure, which contains no-of_dimensions
 *                  (caller responsibility). The number of items will be
 *                  populated in this structure
 *
 * \return returns api_Success on success.
 */
/*****************************************************************************/
static api_Err_Status _detect_2D_sizes( uint8_t *buff, uint8_t *sep_list, Vector_MetaData *meta )
{
    api_Err_Status err = api_Success ;
    uint8_t *content = NULL , *row_ptr=NULL, *col_ptr = NULL, *linebuff = NULL , *ptr = NULL ;
    uint8_t sep[2] = "" ;

    /* Sanity check inputs */
    if( buff == NULL ) {
        debug("Empty buff data passed into function");
        err = api_Err_Param ;
        goto err_2D_item_detect ;
    }
    if( sep_list == NULL ) {
        debug("Empty separator string passed into function");
        err = api_Err_Param ;
        goto err_2D_item_detect ;
    }
    if( meta == NULL ) {
        debug("meta-data struct = NULL");
        err = api_Err_Param ;
        goto err_2D_item_detect ;
    }
    if( meta->no_dims != 2 ) {
        debug("Error : function called with dimensions [%u]", meta->no_dims);
        err = api_Err_Param ;
        goto err_2D_item_detect ;
    }

    memset(sep,0,sizeof(sep));

    /* obtain duplicates of data buffer */
    content = strdup(buff);
    if( content == NULL ) {
        debug("Could not duplicate data buffer in RAM to count items");
        err = api_Err_Memory ;
        goto err_2D_item_detect ;
    }

    col_ptr = strchr( content, (int)sep_list[1] );
    if( col_ptr == NULL ) {
        debug("Could not detect row-separator in 2-D data");
        err = api_Err_Memory ;
        goto err_2D_item_detect ;
    }


    linebuff = strndup( content,(col_ptr - content));
    if( linebuff == NULL ) {
        debug("Could not duplicate data buffer in RAM to count items");
        err = api_Err_Memory ;
        goto err_2D_item_detect ;
    }

    /* Count number of columns in each row */
    sep[0] = sep_list[0];    /* 1st char -denotes column delimiter */
    for( col_ptr = strtok_r(linebuff, sep, &ptr) ;
                       col_ptr != NULL ; col_ptr = strtok_r(NULL, sep, &ptr) )
        meta->dim.dim_2d.cols++ ;

    free(linebuff);
    linebuff = NULL ;


    /* Count number of rows */
    sep[0] = sep_list[1];    /* 2nd char -denotes row delimiter */
    for( linebuff = strtok_r(content, sep, &row_ptr) ;
                       linebuff != NULL ; linebuff = strtok_r(NULL, sep, &row_ptr) )
        meta->dim.dim_2d.rows++ ;


err_2D_item_detect :
    content = (content != NULL) ? free(content), NULL : NULL ;
    return err ;
}







/*****************************************************************************/
/*!
 * \brief  Given a long string buffer and a separator string, count how many
 *         items are available in the string
 *
 * \param  *buff - data buffer with text to count number of items
 * \param  *sep_list -  list of separators from command-line. Only the first
 *                      three characters are considered as a separator.
 * \param  *meta  - meta-data structure, which contains no-ofdimensions
 *                  (caller responsibility). The number of items will be
 *                  populated in this structure
 *
 * \return returns api_Success on success.
 */
/*****************************************************************************/
static api_Err_Status _detect_3D_sizes( uint8_t *buff, uint8_t *sep_list, Vector_MetaData *meta )
{
    api_Err_Status err = api_Success ;
    uint8_t *content = NULL , *dimx_ptr=NULL, *dimy_ptr = NULL, *dimz_ptr = NULL ;
    uint8_t *linebuff = NULL , *ptr = NULL ;
    uint8_t sep[2] = "" ;

    /* Sanity check inputs */
    if( buff == NULL ) {
        debug("Empty buff data passed into function");
        err = api_Err_Param ;
        goto err_3D_item_detect ;
    }
    if( sep_list == NULL ) {
        debug("Empty separator string passed into function");
        err = api_Err_Param ;
        goto err_3D_item_detect ;
    }
    if( meta == NULL ) {
        debug("meta-data struct = NULL");
        err = api_Err_Param ;
        goto err_3D_item_detect ;
    }
    if( meta->no_dims !=3 ) {
        debug("Error : function called with dimensions [%u]", meta->no_dims);
        err = api_Err_Param ;
        goto err_3D_item_detect ;
    }

    memset(sep,0,sizeof(sep));

    /* obtain duplicates of data buffer */
    content = strdup(buff);
    if( content == NULL ) {
        debug("Could not duplicate data buffer in RAM to count items");
        err = api_Err_Memory ;
        goto err_3D_item_detect ;
    }


    /*!
     * First step - calculate number of items in dimension-Z
     */
    dimy_ptr = strchr( content, (int)sep_list[1] );
    if( dimy_ptr == NULL ) {
        debug("Could not detect dimension-y-separator in 3-D data");
        err = api_Err_Memory ;
        goto err_3D_item_detect ;
    }


    linebuff = strndup( content,(dimy_ptr - content));
    if( linebuff == NULL ) {
        debug("Could not duplicate data buffer in RAM to count items");
        err = api_Err_Memory ;
        goto err_3D_item_detect ;
    }

    /* Count number of items in dimension-z */
    sep[0] = sep_list[0];    /* 1st char -denotes z axis delimiter */
    for( dimz_ptr = strtok_r(linebuff, sep, &ptr) ;
                       dimz_ptr != NULL ; dimz_ptr = strtok_r(NULL, sep, &ptr) )
        meta->dim.dim_3d.dim_z++ ;

    free(linebuff);
    linebuff = NULL ;

    /*!
     * Second step - calculate number of items in dimension-Y
     */
    dimx_ptr = strchr( content, (int)sep_list[2] );
    if( dimx_ptr == NULL ) {
        debug("Could not detect dimension-x-separator in 3-D data");
        err = api_Err_Memory ;
        goto err_3D_item_detect ;
    }


    linebuff = strndup( content,(dimx_ptr - content));
    if( linebuff == NULL ) {
        debug("Could not duplicate data buffer in RAM to count items");
        err = api_Err_Memory ;
        goto err_3D_item_detect ;
    }

    /* Count number of items in dimension-y */
    sep[0] = sep_list[1];    /* 2nd char -denotes dim-y delimiter */
    for( dimy_ptr = strtok_r(linebuff, sep, &ptr) ;
                       dimy_ptr != NULL ; dimy_ptr = strtok_r(NULL, sep, &ptr) )
        meta->dim.dim_3d.dim_y++ ;

    free(linebuff);
    linebuff = NULL ;

    /* Count number of items in dim-x  */
    sep[0] = sep_list[2];    /* 3rd char -denotes dim-x delimiter */
    for( linebuff = strtok_r(content, sep, &ptr) ;
                       linebuff != NULL ; linebuff = strtok_r(NULL, sep, &ptr) )
        meta->dim.dim_3d.dim_x++ ;


err_3D_item_detect :
    content = (content != NULL) ? free(content), NULL : NULL ;
    return err ;
}
