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

#ifndef _DBI_H_
#define _DBI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdarg.h>
#include <time.h>

/* opaque type definitions */
typedef struct dbi_driver_t dbi_driver_t;
typedef struct dbi_conn_t dbi_conn_t;
typedef struct dbi_result_t dbi_result_t;

/* other type definitions */
typedef enum {
    DBI_ERROR_USER = -1,
    DBI_ERROR_NONE = 0,
    DBI_ERROR_DBD,
    DBI_ERROR_BADOBJECT,
    DBI_ERROR_BADTYPE,
    DBI_ERROR_BADIDX,
    DBI_ERROR_BADNAME,
    DBI_ERROR_UNSUPPORTED,
    DBI_ERROR_NOCONN,
    DBI_ERROR_NOMEM,
} dbi_error_flag;

typedef struct {
    unsigned char month;
    unsigned char day;
    signed short year; // may be negative (B.C.)
} dbi_date;

typedef struct {
    // when used as an interval value, at most one of these values may be negative.
    // when used as a counter, the hour may be greater than 23.
    // when used as a time of day, everything is as you would expect.

    signed long hour;
    signed char minute;
    signed char second;
    signed short millisecond;
    signed long utc_offset; // seconds east of UTC
} dbi_time;

typedef struct {
    dbi_date date;
    dbi_time time;
} dbi_datetime;


/* function callback definitions */
typedef void (*dbi_conn_error_handler_func)(dbi_conn_t *, void *);

/* values for the int in field_types[] */
#define DBI_TYPE_INTEGER 1
#define DBI_TYPE_DECIMAL 2
#define DBI_TYPE_STRING 3
#define DBI_TYPE_BINARY 4
#define DBI_TYPE_DATETIME 5

/* values for the bitmask in field_type_attributes[] */
#define DBI_INTEGER_UNSIGNED	(1 << 0)
#define DBI_INTEGER_SIZE1		(1 << 1)
#define DBI_INTEGER_SIZE2		(1 << 2)
#define DBI_INTEGER_SIZE3		(1 << 3)
#define DBI_INTEGER_SIZE4		(1 << 4)
#define DBI_INTEGER_SIZE8		(1 << 5)

#define DBI_DECIMAL_UNSIGNED	(1 << 0)
#define DBI_DECIMAL_SIZE4		(1 << 1)
#define DBI_DECIMAL_SIZE8		(1 << 2)

#define DBI_STRING_FIXEDSIZE	(1 << 0) /* XXX unused as of now */

#define DBI_DATETIME_DATE		(1 << 0)
#define DBI_DATETIME_TIME		(1 << 1)

int dbi_initialize(char *driverdir);
void dbi_shutdown(void);
char *dbi_version(void);
int dbi_set_verbosity(int verbosity);

dbi_driver_t *dbi_driver_list(dbi_driver_t *current); /* returns next driver. if current is NULL, return first driver. */
dbi_driver_t *dbi_driver_open(char *name); /* goes thru linked list until it finds the right one */
int dbi_driver_is_reserved_word(dbi_driver_t *driver, char *word);
void *dbi_driver_specific_function(dbi_driver_t *driver, char *name);
int dbi_driver_quote_string(dbi_driver_t *driver, char **orig);
int dbi_driver_cap_get(dbi_driver_t *driver, char *capname);

char *dbi_driver_get_name(dbi_driver_t *driver);
char *dbi_driver_get_filename(dbi_driver_t *driver);
char *dbi_driver_get_description(dbi_driver_t *driver);
char *dbi_driver_get_maintainer(dbi_driver_t *driver);
char *dbi_driver_get_url(dbi_driver_t *driver);
char *dbi_driver_get_version(dbi_driver_t *driver);
char *dbi_driver_get_date_compiled(dbi_driver_t *driver);

dbi_conn_t *dbi_conn_new(char *name); /* shortcut for dbi_conn_open(dbi_driver_open("foo")) */
dbi_conn_t *dbi_conn_open(dbi_driver_t *driver); /* returns an actual instance of the conn */
dbi_driver_t *dbi_conn_get_driver(dbi_conn_t *conn);
int dbi_conn_set_option(dbi_conn_t *conn, char *key, char *value); /* if value is NULL, remove option from list */
int dbi_conn_set_option_numeric(dbi_conn_t *conn, char *key, int value);
char *dbi_conn_get_option(dbi_conn_t *conn, char *key);
int dbi_conn_get_option_numeric(dbi_conn_t *conn, char *key);
char *dbi_conn_require_option(dbi_conn_t *conn, char *key); /* like get, but generate an error if key isn't found */
int dbi_conn_require_option_numeric(dbi_conn_t *conn, char *key); /* ditto */
char *dbi_conn_get_option_list(dbi_conn_t *conn, char *current); /* returns key of next option, or the first option key if current is NULL */
void dbi_conn_clear_option(dbi_conn_t *conn, char *key);
void dbi_conn_clear_options(dbi_conn_t *conn);
int dbi_conn_cap_get(dbi_conn_t *conn, char *capname);
int dbi_conn_disjoin_results(dbi_conn_t *conn);
void dbi_conn_close(dbi_conn_t *conn);

int dbi_conn_error(dbi_conn_t *conn, char **errmsg_dest);
void dbi_conn_error_handler(dbi_conn_t *conn, dbi_conn_error_handler_func function, void *user_argument);
dbi_error_flag dbi_conn_error_flag(dbi_conn_t *conn);
int dbi_conn_set_error(dbi_conn_t *conn, int errnum, char *formatstr, ...) __attribute__ ((format (printf, 3, 4)));

int dbi_conn_connect(dbi_conn_t *conn);
int dbi_conn_get_socket(dbi_conn_t *conn);
dbi_result_t *dbi_conn_get_db_list(dbi_conn_t *conn, char *pattern);
dbi_result_t *dbi_conn_get_table_list(dbi_conn_t *conn, char *db, char *pattern);
dbi_result_t *dbi_conn_query(dbi_conn_t *conn, char *statement); 
dbi_result_t *dbi_conn_queryf(dbi_conn_t *conn, char *formatstr, ...) __attribute__ ((format (printf, 2, 3)));
dbi_result_t *dbi_conn_query_null(dbi_conn_t *conn, unsigned char *statement, unsigned long st_length); 
int dbi_conn_select_db(dbi_conn_t *conn, char *db);
unsigned long long dbi_conn_sequence_last(dbi_conn_t *conn, char *name); /* name of the sequence or table */
unsigned long long dbi_conn_sequence_next(dbi_conn_t *conn, char *name);
int dbi_conn_ping(dbi_conn_t *conn);

dbi_conn_t *dbi_result_get_conn(dbi_result_t *result);
int dbi_result_free(dbi_result_t *result);
int dbi_result_seek_row(dbi_result_t *result, unsigned long long row);
int dbi_result_first_row(dbi_result_t *result);
int dbi_result_last_row(dbi_result_t *result);
int dbi_result_prev_row(dbi_result_t *result);
int dbi_result_next_row(dbi_result_t *result);
unsigned long long dbi_result_get_currow(dbi_result_t *result);
unsigned long long dbi_result_get_numrows(dbi_result_t *result);
unsigned long long dbi_result_get_numrows_affected(dbi_result_t *result);
unsigned long long dbi_result_get_fieldsize(dbi_result_t *result, char *fieldname);
unsigned long long dbi_result_get_fieldsize_idx(dbi_result_t *result, unsigned short idx);
unsigned short dbi_result_get_field_idx(dbi_result_t *result, char *fieldname);
char *dbi_result_get_field_name(dbi_result_t *result, unsigned short idx);
unsigned short dbi_result_get_numfields(dbi_result_t *result);
unsigned short dbi_result_get_field_type(dbi_result_t *result, char *fieldname);
unsigned short dbi_result_get_field_type_idx(dbi_result_t *result, unsigned short idx);
unsigned long dbi_result_get_field_attribs(dbi_result_t *result, char *fieldname);
unsigned long dbi_result_get_field_attribs_idx(dbi_result_t *result, unsigned short idx);
int dbi_result_disjoin(dbi_result_t *result);

int dbi_result_get_fields(dbi_result_t *result, char *format, ...);
int dbi_result_get_fields_ap(dbi_result_t *result, char *format, va_list ap);
int dbi_result_bind_fields(dbi_result_t *result, char *format, ...);
int dbi_result_bind_fields_ap(dbi_result_t *result, char *format, va_list ap);

signed char dbi_result_get_char(dbi_result_t *result, char *fieldname);
unsigned char dbi_result_get_uchar(dbi_result_t *result, char *fieldname);
short dbi_result_get_short(dbi_result_t *result, char *fieldname);
unsigned short dbi_result_get_ushort(dbi_result_t *result, char *fieldname);
long dbi_result_get_long(dbi_result_t *result, char *fieldname);
unsigned long dbi_result_get_ulong(dbi_result_t *result, char *fieldname);
long long dbi_result_get_longlong(dbi_result_t *result, char *fieldname);
unsigned long long dbi_result_get_ulonglong(dbi_result_t *result, char *fieldname);

float dbi_result_get_float(dbi_result_t *result, char *fieldname);
double dbi_result_get_double(dbi_result_t *result, char *fieldname);

char *dbi_result_get_string(dbi_result_t *result, char *fieldname);
unsigned char *dbi_result_get_binary(dbi_result_t *result, char *fieldname);

time_t dbi_result_get_datetime(dbi_result_t *result, char *fieldname);

int dbi_result_bind_char(dbi_result_t *result, char *fieldname, char *bindto);
int dbi_result_bind_uchar(dbi_result_t *result, char *fieldname, unsigned char *bindto);
int dbi_result_bind_short(dbi_result_t *result, char *fieldname, short *bindto);
int dbi_result_bind_ushort(dbi_result_t *result, char *fieldname, unsigned short *bindto);
int dbi_result_bind_long(dbi_result_t *result, char *fieldname, long *bindto);
int dbi_result_bind_ulong(dbi_result_t *result, char *fieldname, unsigned long *bindto);
int dbi_result_bind_longlong(dbi_result_t *result, char *fieldname, long long *bindto);
int dbi_result_bind_ulonglong(dbi_result_t *result, char *fieldname, unsigned long long *bindto);

int dbi_result_bind_float(dbi_result_t *result, char *fieldname, float *bindto);
int dbi_result_bind_double(dbi_result_t *result, char *fieldname, double *bindto);

int dbi_result_bind_string(dbi_result_t *result, char *fieldname, char **bindto);
int dbi_result_bind_binary(dbi_result_t *result, char *fieldname, unsigned char **bindto);

int dbi_result_bind_datetime(dbi_result_t *result, char *fieldname, time_t *bindto);
int dbi_result_bind_fieldsize(dbi_result_t *result, char *fieldname, unsigned int *bindto);

/* and now for the same exact thing in index form: */

signed char dbi_result_get_char_idx(dbi_result_t *result, unsigned short idx);
unsigned char dbi_result_get_uchar_idx(dbi_result_t *result, unsigned short idx);
short dbi_result_get_short_idx(dbi_result_t *result, unsigned short idx);
unsigned short dbi_result_get_ushort_idx(dbi_result_t *result, unsigned short idx);
long dbi_result_get_long_idx(dbi_result_t *result, unsigned short idx);
unsigned long dbi_result_get_ulong_idx(dbi_result_t *result, unsigned short idx);
long long dbi_result_get_longlong_idx(dbi_result_t *result, unsigned short idx);
unsigned long long dbi_result_get_ulonglong_idx(dbi_result_t *result, unsigned short idx);

float dbi_result_get_float_idx(dbi_result_t *result, unsigned short idx);
double dbi_result_get_double_idx(dbi_result_t *result, unsigned short idx);

char *dbi_result_get_string_idx(dbi_result_t *result, unsigned short idx);
unsigned char *dbi_result_get_binary_idx(dbi_result_t *result, unsigned short idx);

time_t dbi_result_get_datetime_idx(dbi_result_t *result, unsigned short idx);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	/* _DBI_H_ */
