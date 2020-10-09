#include <stdio.h>

//num of leap years: (cur_year - 1970) >> 2
#define SECONDS_IN_MINUTE        60
#define SECONDS_IN_HOUR          3600
#define SECONDS_IN_DAY           86400
#define SECONDS_IN_ASTRO_YEAR    31557600
#define SECONDS_IN_COMMON_YEAR   31536000
#define SECONDS_IN_LEAP_YEAR     31622400

#define SECONDS_IN_MONTH         2592000

#define SECONDS_IN_JAN 2678400
#define SECONDS_IN_FEB
#define SECONDS_IN_MAR 2678400
#define SECONDS_IN_APR 2592000
#define SECONDS_IN_MAY 2678400


enum dayofweek{
SUNDAY,
MONDAY,
TUESDAY,
WEDNESDAY,
THURSDAY,
FRIDAY,
SATURDAY
};

enum months{
JANUARY,
FEBRUARY,
MARCH,
APRIL,
MAY,
JUNE,
JULY,
AUGUST,
SEPTEMBER,
OCTOBER,
NOVEMBER,
DECEMBER
};

// feb is 29 on leap year, add 1 if leap year
//char daysinmonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

/* days in month
 * jan: 31
 * feb: common = 28, leap = 29
 * mar: 31
 * apr: 30
 * may: 31
 * jun: 30
 * jul: 31
 * aug: 31
 * sep: 30
 * oct: 31
 * nov: 30
 * dec: 31
*/

#define SEC_YEAR_SINCE_EPOCH(y)\
    ((((y-1970) - ((y-1970) >> 2)) * SECONDS_IN_COMMON_YEAR) + \
    (((y-1970) >> 2) * SECONDS_IN_LEAP_YEAR)) 

#define IS_LEAP_YEAR(y) ((y % 4) ? 0 : (y % 100 ? 1 : (y % 400 ? 0 : 1)))


void calc_time()
{
unsigned i, n;
char daysinmonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    for(i=0, n=0; i < OCTOBER; n += daysinmonth[i++]);
    if(IS_LEAP_YEAR(2018)) n++;

    printf("posix time %u\n", time(0));
    printf("seconds in year since epoch: %u\n",
	SEC_YEAR_SINCE_EPOCH(2018) + (n * SECONDS_IN_DAY) + (26 * SECONDS_IN_DAY) + (22 * SECONDS_IN_HOUR) + (25 * SECONDS_IN_MINUTE));
}
