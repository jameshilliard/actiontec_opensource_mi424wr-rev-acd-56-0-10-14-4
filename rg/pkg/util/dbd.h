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

#ifndef _DBD_H_
#define _DBD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "dbi.h"
#include "dbi_int.h"

/* FUNCTIONS EXPORTED BY EACH DRIVER */
#if 0
/* those function are static in each driver */
void dbd_register_driver(dbi_info_t **_driver_info,
    char ***_custom_functions, char ***_reserved_words);
int dbd_initialize(dbi_driver_t *driver);
int dbd_connect(dbi_conn_t *conn);
int dbd_disconnect(dbi_conn_t *conn);
int dbd_fetch_row(dbi_result_t *result, unsigned long long rownum);
int dbd_free_query(dbi_result_t *result);
int dbd_goto_row(dbi_result_t *result, unsigned long long row);
dbi_result_t *dbd_list_dbs(dbi_conn_t *conn, char *pattern);
dbi_result_t *dbd_list_tables(dbi_conn_t *conn, char *db, char *pattern);
dbi_result_t *dbd_query(dbi_conn_t *conn, char *statement);
dbi_result_t *dbd_query_null(dbi_conn_t *conn, unsigned char *statement,
    unsigned long st_length);
int dbd_quote_string(dbi_driver_t *driver, char *orig, char *dest);
char *dbd_select_db(dbi_conn_t *conn, char *db);
int dbd_geterror(dbi_conn_t *conn, int *errno, char **errstr);
unsigned long long dbd_get_seq_last(dbi_conn_t *conn, char *sequence);
unsigned long long dbd_get_seq_next(dbi_conn_t *conn, char *sequence);
int dbd_ping(dbi_conn_t *conn);
#endif

/* _DBD_* DRIVER AUTHORS HELPER FUNCTIONS */
dbi_result_t *_dbd_result_create(dbi_conn_t *conn, void *handle,
    unsigned long long numrows_matched, unsigned long long numrows_affected);
void _dbd_result_set_numfields(dbi_result_t *result, unsigned short numfields);
void _dbd_result_add_field(dbi_result_t *result, unsigned short idx,
    char *name, unsigned short type, unsigned int attribs);
dbi_row_t *_dbd_row_allocate(unsigned short numfields);
void _dbd_row_finalize(dbi_result_t *result, dbi_row_t *row,
    unsigned long long idx);
void _dbd_internal_error_handler(dbi_conn_t *conn, char *errmsg, int errno);
dbi_result_t *_dbd_result_create_from_stringarray(dbi_conn_t *conn,
    unsigned long long numrows_matched, char **stringarray);
void _dbd_register_driver_cap(dbi_driver_t *driver, char *capname, int value);
void _dbd_register_conn_cap(dbi_conn_t *conn, char *capname, int value);
int _dbd_result_add_to_conn(dbi_result_t *result);
time_t _dbd_parse_datetime(char *raw, unsigned long attribs);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	/* _DBD_H_ */
