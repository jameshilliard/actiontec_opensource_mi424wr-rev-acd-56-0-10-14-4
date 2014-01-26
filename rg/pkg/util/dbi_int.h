/*
 * libdbi - database independent abstraction layer for C.
 * Copyright (C) 2001-2003, David Parker and Mark Tobenkin.
 * http://libdbi.sourceforge.net
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 */

#ifndef __DBI_INT_H__
#define __DBI_INT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "dbi.h"

/*********************
 * SQL RELATED TYPES *
 *********************/

typedef union dbi_data_t {
    char d_char;
    short d_short;
    long d_long;
    long long d_longlong;
    float d_float;
    double d_double;
    char *d_string;
    time_t d_datetime;
} dbi_data_t;

typedef struct dbi_row_t {
    dbi_data_t *field_values;
    unsigned long long *field_sizes; /* NULL field = 0, string field = len, anything else = -1 */
								     /* XXX TODO: above is false as of 8/6/02. no -1 */
} dbi_row_t;

typedef struct field_binding_t field_binding_t;

struct dbi_result_t {
    dbi_conn_t *conn;
    void *result_handle; /* will be typecast into conn-specific type */
    unsigned long long numrows_matched; /* set immediately after query */
    unsigned long long numrows_affected;
    field_binding_t *field_bindings;
    
    unsigned short numfields; /* can be zero or NULL until first fetchrow */
    char **field_names;
    unsigned short *field_types;
    unsigned long *field_attribs;

    enum { NOTHING_RETURNED, ROWS_RETURNED } result_state;
    dbi_row_t **rows; /* array of filled rows, elements set to NULL if not fetched yet */
    unsigned long long currowidx;
};

struct field_binding_t {
    struct field_binding_t *next;
    void (*helper_function)(field_binding_t *);
    dbi_result_t *result;
    void *bindto;
    short idx;
};

/***************************************
 * DRIVER INFRASTRUCTURE RELATED TYPES *
 ***************************************/

typedef struct dbi_info_t {
    char *name; /* all lowercase letters and numbers, no spaces */
    char *description; /* one or two short sentences, no newlines */
    char *maintainer; /* Full Name <fname@fooblah.com> */
    char *url; /* where this driver came from (if maintained by a third party) */
    char *version;
} dbi_info_t;

typedef struct capability_t {
    struct capability_t *next;
    char *name;
    int value;
} capability_t;

typedef struct dbi_option_t {
    struct dbi_option_t *next;
    char *key;
    char *string_value;
    int numeric_value; /* use this for port and other numeric settings */
} dbi_option_t;

typedef struct dbi_functions_t {
    int (*initialize)(dbi_driver_t *);
    int (*connect)(dbi_conn_t *);
    int (*disconnect)(dbi_conn_t *);
    int (*fetch_row)(dbi_result_t *, unsigned long long);
    int (*free_query)(dbi_result_t *);
    int (*goto_row)(dbi_result_t *, unsigned long long);
    int (*get_socket)(dbi_conn_t *);
    dbi_result_t *(*list_dbs)(dbi_conn_t *, char *);
    dbi_result_t *(*list_tables)(dbi_conn_t *, char *, char *);
    dbi_result_t *(*query)(dbi_conn_t *, char *);
    dbi_result_t *(*query_null)(dbi_conn_t *, unsigned char *, unsigned long);
    int (*quote_string)(dbi_driver_t *, char *, char *);
    char *(*select_db)(dbi_conn_t *, char *);
    int (*geterror)(dbi_conn_t *, int *, char **);
    unsigned long long (*get_seq_last)(dbi_conn_t *, char *);
    unsigned long long (*get_seq_next)(dbi_conn_t *, char *);
    int (*ping)(dbi_conn_t *);
} dbi_functions_t;

typedef struct dbi_custom_function_t {
    struct dbi_custom_function_t *next;
    char *name;
    void *function_pointer;
} dbi_custom_function_t;

struct dbi_driver_t {
    struct dbi_driver_t *next;
    dbi_info_t *info;
    dbi_functions_t *functions;
    dbi_custom_function_t *custom_functions;
    char **reserved_words;
    capability_t *caps;
};
    
struct dbi_conn_t {
    struct dbi_conn_t *next; /* so libdbi can unload all conns at exit */
    dbi_driver_t *driver; /* generic unchanging attributes shared by all instances of this conn */
    dbi_option_t *options;
    capability_t *caps;
    void *connection; /* will be typecast into conn-specific type */
    char *current_db;
    dbi_error_flag error_flag;
    int error_number; /*XXX*/
    char *error_message; /*XXX*/
    dbi_conn_error_handler_func error_handler;
    void *error_handler_argument;
    dbi_result_t **results; /* for garbage-collector-mandated result disjoins */
    int results_used;
    int results_size;
};

unsigned long _isolate_attrib(unsigned long attribs, unsigned long rangemin, unsigned long rangemax);
void _error_handler(dbi_conn_t *conn, dbi_error_flag errflag);
int _disjoin_from_conn(dbi_result_t *result);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

