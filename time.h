#pragma once

/*
#include "phnt\phnt_windows.h"
#include "phnt\phnt.h"
*/

#ifndef _out_
#define _out_
#endif

#ifndef _in_
#define _in_
#endif

#ifndef _in_out_
#define _in_out_
#endif

#include <windows.h>

typedef unsigned long long apx_uint64;
typedef unsigned int       apx_uint32;
typedef unsigned short     apx_uint16;

typedef apx_uint64 apx_time64;


typedef struct {
	apx_uint16 ver   :  2;
	apx_uint16 cal   :  7;
	apx_uint16 tz    :  7;
	apx_uint16 nano  : 10; // nano-seconds in a microsecond
	apx_uint16 sec   :  6;
	apx_uint16 us    : 10; // number of microseconds in a second
	apx_uint16 min   :  6;
	apx_uint16 year  : 13;
	apx_uint16 dow   :  3;
	apx_uint16 ms    : 10; // (1-1000)
	apx_uint16 err   :  6;
	apx_uint16 day   :  5; // (1-31)
	apx_uint16 hour  :  5; // (1-24)
	apx_uint16 month :  4; // (1-12)
	apx_uint16 pad1  :  2;
	apx_uint16 pad2;
	apx_uint16 pad3;
} *apx_datetime;

struct apx_calendar_day_s {
	char month;
	char day;
};

/*
struct apx_calendar_year_s {
};
*/

struct apx_date_s {
	short year;
	char month;
	char day;
};

struct apx_calendar_month_s {
	struct apx_date_s date[7][5];
};

void apx_get_calendar_month(int year, int month, struct apx_calendar_month_s *data);

typedef unsigned long long apx_unix_time;

typedef union {
	apx_uint64 time;
} apx_timestamp;


/* MUST be called before any other apx api */
void apx_init();


/* Optional call when done using the apx api */
void apx_term();


int unixtime_to_systime_ms(__time64_t timestamp, void *systime);

BOOL systime_to_unixtime_ms(SYSTEMTIME *systime, __time64_t *timestamp);

/* return: the current utc time */
apx_datetime apx_utc_now();

/* return: the current local time */
apx_datetime apx_local_now();

//time precision:
//a 'shake' is 10 nanoseconds


enum apx_timestamp {
	apx_timestamp_undefined,

/* The unix time in seconds */
	apx_timestamp_unix_time_sec,

/* The unix time in milliseconds */
	apx_timestamp_unix_time_msec,

/* the number of 100-nanosecond intervals since January 1, 1601 */
	apx_timestamp_winnt,

/* the uuid v1 timesstamp */
	apx_timestamp_uuid_v1,

	apx_timestamp_max
};


/* the iso 8601 / rfc 3339 date format */
#define APX_FORMAT_ISO_8601 "%Y-%m-%dT%H:%i%s%U"

/* the format used for http cookie dates */
#define APX_FORMAT_HTTP_COOKIE "%D, %d %M %Y %H:%i:%s GMT"

/* the rfc 822 */
#define APX_FORMAT_RFC_822


//returns the size of the string copied to buf

/*
 *
 *  d - The day of the month
 *  D - The 3 letter day of the week ex. Mon 
 *  M - The 3 letter month ex. Oct 
 *  d - The 2 digit day of the month
 *  m - The 2 digit numeric month
 *  Y - The 4 digit year
 *  h - The 12 hour format of an hour (01-12)
 *  H - The 24 hour format of an hour (00-23)
 *  i - The 2 digit minute with leading zeroes
 *  s - The 2 digit second with leading zeroes
 *  U - The formatted utc offset in hours and minutes ex. +00:00, -05:00
 *  F - The full textual representation of the month ex. January
 *  l - The full textual representation of the day ex. Sunday
 *
*/

/*
 * write a date string out to a buffer from a datetime object
 *
*/
int apx_format_write( _in_ apx_datetime adt,
	_in_ const char *format,
	_out_ void *buf, _in_ unsigned bufsz);

/*
 * read a date string into a datetime object
*/
apx_datetime apx_format_read( _in_ const char *format,
	_in_ const char *date, _in_ int length);

/*
 * write a datetime object out to an pre-defined timestamp
 */
apx_uint64 apx_timestamp_out(_in_ apx_datetime adt, _in_ enum apx_timestamp type);

/* 
 * read a pre-defined timestamp into a datetime object
 */
apx_datetime apx_timestamp_in( _in_ enum apx_timestamp type, _in_ apx_uint64 time);


int apx_add_hour( _in_ apx_datetime *adt, int hour);

int apx_add_minute( _in_ apx_datetime *adt, int minute);

int apx_add_second( _in_ apx_datetime *adt, int second);

int apx_add_millisecond( _in_ apx_datetime *adt, int millisecond);

#define ISO8601_MAX_STRING_LENGTH 32


/*
 * millisec    : 10 (1024)
 * seconds     : 6  (64)
 * minutes     : 6  (64)
 * hours       : 5  (32)
 * day         : 5  (32)
 *
 * year        : 13 (8192)
 * daysofweek  : 3  (8)
 * month       : 4  (16)
 * timezone    : 7  (128)
 * calendar    : 3  (8)
 * version     : 2  (4)
 */


enum apx_error {
	apx_error_none,
	Error_Success, //TODO change to NoError
	apx_error_bad_token,
	Error_BadToken,
	apx_error_too_many_tokens,
	Error_TooManyTokens,
	apx_error_dup_token,
	Error_DupToken,
	apx_error_format_mismatch,
	Error_FormatMismatch,
	apx_error_out_of_memory,
	Error_OutOfMemory,
	apx_error_bad_token_read,
	Error_BadTokenRead,
	apx_error_bad_token_write,
	Error_BadTokenWrite,
	apx_error_bad_format,
	Error_BadFormat,
};


enum Timezone {
	Timezone_LineIslandsTime   = 7,  // +14
	Timezone_ChathamIslandDT   = 8,  // +1345
	Timezone_MikeTZ            = 15, // +
	Timezone_MagadanST         = 15,
	Timezone_MagadanTime       = 19, // +11
	Timezone_LimaTZ            = 19,
	Timezone_LordHoweDT        = 19,
	Timezone_KosraeTime        = 19,
	Timezone_NewCaledoniaTime  = 19,
	Timezone_NorfolkTime       = 19,
	Timezone_NiueTime          = 19,
	Timezone_PohnpeiST         = 19,
	Timezone_SakhalinTime      = 19,
	Timezone_Srednekolymsrk    = 19,
	Timezone_Vladivostok       = 19,
	Timezone_VanuatuTime       = 19,
	Timezone_GreenwichMeanTime = 63,
	Timezone_UniversalTime     = 63,
};

enum DayOfWeek{
	Monday,
	Tuesday,
	Wednesday,
	Thursday,
	Friday,
	Saturday,
	Sunday
};

enum Month {
	January,
	February,
	March,
	April,
	May,
	June,
	July,
	August,
	September,
	October,
	November,
	December
};

enum HebrewMonth {
	HebrewMonth_Nisan,
	HebrewMonth_Iyar,
	HebrewMonth_Sivan,
	HebrewMonth_Tammuz,
	HebrewMonth_Av,
	HebrewMonth_Elul,
	HebrewMonth_Tishrei,
	HebrewMonth_Chesvan,
	HebrewMonth_Kislev,
	HebrewMonth_Tevet,
	HebrewMonth_Shevat,
	HebrewMonth_Adar,
	HebrewMonth_Adar2 //leap years only
};

enum Calendars {
	Gregorian,
	Chinese,
	Hebrew,
	Islamic,
	Persian,
	Ethiopian,
	BalinesePawukon
};
