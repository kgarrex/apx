
#include "time.h"

#define MAX_MILLISECONDS 0x3e7
#define MAX_SECONDS      0x3b
#define MAX_MINUTES      0x3b

#define DAYS_PER_YEAR     365
#define DAYS_PER_LEAPYEAR 366
#define MS_PER_SECOND     1000
#define MS_PER_MINUTE     60000
#define MS_PER_HOUR       3600000
#define MS_PER_DAY        86400000
#define MS_PER_YEAR       31536000000
#define MS_PER_LEAPYEAR   31622400000

#define SEC_PER_MINUTE   60
#define SEC_PER_HOUR     3600
#define SEC_PER_DAY      86400
#define SEC_PER_YEAR     31536000
#define SEC_PER_LEAPYEAR 31622400

#define US_PER_DAY      86400000000
#define NANO100_PER_DAY 864000000000
#define NANO_PER_DAY    86400000000000

#define SEC_TO_NANO100(s) (s*10000000)
#define MS_TO_NANO100(ms) (ms*10000)
#define US_TO_NANO100(us) (us*10)

#define MIN_PER_HOUR     60
#define HOUR_PER_DAY     24

#define MONTH_PER_YEAR   12

#define is_leap_year(y) (y % 4 != 0 ? 0 : (y % 100 != 0 ? 1 : (y % 400 == 0 ? 1 : 0)))

/*
inline int is_leap_year(int y)
{
	return y % 4 != 0 ? 0 : (y % 100 != 0 ? 1 : (y % 400 == 0 ? 1 : 0));
}
*/

typedef union {
#if defined(_WIN32)

	struct {	
		unsigned int low_time;
		unsigned int high_time;
	};
	unsigned long long time;

#elif defined(_LINUX)	


#endif
} systime;

unsigned atouint(const char *str, unsigned len)
{
	unsigned c;
	unsigned n = 0;

	for(c = *str; len--; c = *++str)
		n = ((n<<3) + (n<<1)) + (c - '0'); //n*10 + c-32

	return n;
}


unsigned atoint(const char *str, unsigned len)
{
	unsigned c;
	unsigned n = 0;
	char neg = 0;

	if(*str == '+') { str++; len--; }
	else if(*str == '-'){
		neg = 1;
	}
	for(c = *str; len--; c = *++str)
		n = ((n<<3) + (n<<1)) + (c - '0'); //n*10 + c-32

	return neg ? 0-n : n;
}

/**
 * @return full length of number in buffer
**/
static int __cdecl itoa_zt (int n, char* buf, unsigned char padding, unsigned char base)
{
//check that the base is valid
char *ptr = buf,
     *ptr1 = buf,
     tmp_char,
     _buf[16];
int nn, i = 0;

    if(!buf) return 0;
    if(base < 2 || base > 36) return 0;

    do {
        nn = n;
        n /= base;

        *ptr++ = "ZYXWVUTSRQPONMLKJIHGFEDCBA9876543210" //71 chars
                 "123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        [35 + (nn-n*base)];
        i++;
    } while(n);

    while(i < padding) {
        *ptr++ = '0';
        i++;
    }

    //Apply negative sign
    if(nn < 0) {
        *ptr++ = '-'; /* todo: check size left */
        i++;
    }
    *ptr-- = '\0';

    while(ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr-- = *ptr1;
        *ptr1++ = tmp_char;
    }
    return i;
}


struct text_s {const char *text; const char length; };

const struct text_s dow[] = {
	{"Sunday",   6},
	{"Monday",   6},
	{"Tuesday",  7},
	{"Wednesday",9},
	{"Thursday", 8},
	{"Friday",   6},
	{"Saturday", 8}
};

const struct text_s month[] = {
	{"January",   7},
	{"February",  8},
	{"March",     5},
	{"April",     5},
	{"May",       3},
	{"June",      4},
	{"July",      4},
	{"August",    6},
	{"September", 9},
	{"October",   7},
	{"November",  8},
	{"December",  8}
};

//char tz_abbr
/*
 * ACDT
 * ACST
 * ACT
 * ACT
 * ACWST
 * ADT
 * AEDT
 * AEST
 * AET
 * AFT
 * AKDT
 * AKST
 * ALMT
 * AMST
 * AMT
 * AMT
 * ANAT
 * AQTT
 * ART
 * AST
 * AST
 * AWST
 * AZOST
 * AZOT
 * AZT
 * BDT
 * BIOT
 * BIT
 * BOT
 * BRST
 * BRT
 * BST
 * BST
 * BST
 * BTT
 * CAT
 * CCT
 * CDT
 * CDT
 * CEST
 * CET
 * CHADT
 * CHAST
 * CHOT
 * CHOST
 * CHST
 * CHUT
 * CIST
 * WITA
 * CKT
 * CLST
 * CLT
 * COST
 * COT
 * CST
 * CST
 * CST
 * CT
 * CVT
 * CWST
 * CXT
 * DAVT
 * DDUT
 * DFT
 * EASST
 * EAST
 * EAT
 * ECT
 * ECT
 * EDT
 * EEST
 * EET
 * EGST
 * EGT
 * WIT
 * EST
 * FET
 * FJT
 * FKST
 */

/*
J 3
M 2
A 2
O 1
N 1
D 1
S 1
F 1
switch(*ptr){
case 'J':
case 'M':
case 'A':
case 'O':
case 'N':
case 'D':
case 'S':
case 'F':
}
*/

int is_weekday(const char *str)
{
	char c;

	c = *str;

	switch(c)
	{
	case 'T': case 't':
		switch(*++str){
		case 'u': case 'U': //Tuesday	
			break;
		case 'h': case 'H': //Thursday
			break;
		default:
			return -1;
		}
		break;
	case 'S': case 's':
		switch(*++str){
		case 'a': case 'A': //Saturday
			break;
		case 'u': case 'U': //Sunday
		default:
			return -1;
		}
		break;
	case 'M': case 'm': //Monday
		break;
	case 'W': case 'w': //Wednesday
		break;
	case 'F': case 'f': //Friday
		break;
	}
	return -1;
}

int is_month(const char *str)
{
	char c;
	c = *str;
	switch(*str)
	{
		case 'J': case 'j':
			c = *++str;
			switch(c){
				case 'a': case 'A': // January	
					break;
				case 'u': case 'U':
					c = *++str;
					switch(c){
						case 'n': case 'N': // June	
							break;
						case 'l': case 'L': // July
							break;
						default:
							return -1;
					}
					break;
				default:
					return -1;
			}
			break;
		case 'M': case 'm': //March, May
			c = *++str;
			if(c != 'a' && c != 'A') return -1;
			c = *++str;

			break;
		case 'A': case 'a': //April, August
			break;
		case 'O': case 'o': //October
			break;
		case 'N': case 'n': //November
			break;
		case 'D': case 'd': //December
			break;
		case 'S': case 's': //September
			break;
		case 'F': case 'f': //February
			break;
	}
	return -1;
}

#define TT_UNDEF 0x001
#define TT_MONTH 0x002
#define TT_DAY   0x004
#define TT_YEAR  0x008
#define TT_HOUR  0x010
#define TT_MIN   0x020
#define TT_SEC   0x040
#define TT_MILLI 0x080
#define TT_TZ    0x100
#define TT_DOW   0x200
#define TT_WEEKS (TT_MONTH|TT_DAY)
//#define TT_COUNT 0x400


typedef struct {
	int type;
	int (*writer)(void*, void*);
	int (*reader)(void*, void*);
} token_info_t;

typedef struct {
	const token_info_t *info;
	int pos;
	char index;
	void *user_data;
} token_t;


/*
void current_time(SYSTIME *systime)
{
	GetSystemTime(systime);
}
*/

#define get_value(dt, mask, off) ((dt >> off) & mask)
#define set_value(cal, val, mask, off) (dt |= (((unsigned long long)val & mask) << off))

#define get_ms(dt)    get_value(dt, 0x3ff, 0)
#define get_sec(dt)   get_value(dt, 0x3f, 10)
#define get_min(dt)   get_value(dt, 0x3f, 16)
#define get_day(dt)   get_value(dt, 0x1f, 22)
#define get_hour(dt)  get_value(dt, 0x1f, 27)
#define get_year(dt)  get_value(dt, 0x1fff, 32)
#define get_dow(dt)   get_value(dt, 0x7, 45)
#define get_month(dt) get_value(dt, 0xf, 48)
#define get_tz(dt)    get_value(dt, 0x7f, 52)
#define get_cal(dt)   get_value(dt, 0x7, 59)
#define get_ver(dt)   get_value(dt, 0x3, 62)

#define set_ms(dt, ms)       set_value(dt, ms, 0x3ff, 0)
#define set_sec(dt, sec)     set_value(dt, sec, 0x3f, 10)
#define set_min(dt, min)     set_value(dt, min, 0x3f, 16)
#define set_day(dt, day)     set_value(dt, day, 0x1f, 22)
#define set_hour(dt, hour)   set_value(dt, hour, 0x1f, 27)
#define set_year(dt, year)   set_value(dt, year, 0x1fff, 32)
#define set_dow(dt, dow)     set_value(dt, dow, 0x7, 45)
#define set_month(dt, month) set_value(dt, month, 0xf, 48)
#define set_tz(dt, tz)       set_value(dt, tz, 0x7f, 52)
#define set_cal(dt, cal)     set_value(dt, cal, 0x7, 59)
#define set_ver(dt, ver)     set_value(dt, ver, 0x3, 62)



/*
inline datetime_t fields_to_datetime(struct datetime_fields f)
{
	datetime_t dt = 0;
	if((f.year < 1582) || (f.year > 9774)){
		f.err = 1;
	}
	set_ms(dt,  f.ms);
	set_sec(dt, f.sec);
	set_min(dt, f.min);
	set_day(dt, f.day);
	set_hour(dt, f.hour);
	f.year -= 1582; //1st year of gregorian calendar
	set_year(dt, f.year);
	set_dow(dt, f.dow);
	f.month--; //for windows
	set_month(dt, f.month);
	set_tz( dt, abs((f.tz/15)-63) );
	set_cal(dt, f.cal);
	set_ver(dt, f.ver);

	return dt;
}


inline struct datetime_fields datetime_to_fields(datetime_t dt)
{
	static short utc_offset[] = {
	 945,  930,  915,  900,  885,  870,  855,  840,
	 825,  810,  795,  780,  765,  750,  735,  720,
	 705,  690,  675,  660,  645,  630,  615,  600,
	 585,  570,  555,  540,  525,  510,  495,  480,
	 465,  450,  435,  420,  405,  390,  375,  360,
	 345,  330,  315,  300,  285,  270,  255,  240,
	 225,  210,  195,  180,  165,  150,  135,  120,
	 105,   90,   75,   60,   45,   30,   15,    0,
	 -15,  -30,  -45,  -60,  -75,  -90, -105, -120,
	-135, -150, -165, -180, -195, -210, -225, -240,
	-255, -270, -285, -300, -315, -330, -345, -360,
	-375, -390, -405, -420, -435, -450, -465, -480,
	-495, -510, -525, -540, -555, -570, -585, -600,
	-615, -630, -645, -660, -675, -690, -705, -720,
	-735, -750, -765, -780, -795, -810, -825, -840,
	-855, -870, -885, -900, -915, -930, -945, -960
	};

	struct datetime_fields f;
	f.ms = get_ms(dt);
	f.sec = get_sec(dt);
	f.min = get_min(dt);
	f.day = get_day(dt);
	f.hour = get_hour(dt);
	f.year = get_year(dt)+1582;
	f.dow = get_dow(dt);
	f.month = get_month(dt);
	f.tz = utc_offset[get_tz(dt)];
	f.cal = get_cal(dt);
	f.ver = get_ver(dt);
	return f;
}
*/

#include "ostime.c"

#define MAX_FORMAT_TOKENS 16 
#define MAX_FORMAT_LENGTH 64

struct tokenizer {
	token_t tok[MAX_FORMAT_TOKENS];
	int tokcount; //number of tokens in a formatted string
	int tokmask; //a bit mask of token types in a formatted string
	char delim;  //token character delimiter
	char numChars; //number of characters that make up a token
};

struct date_parser {
	struct tokenizer tokenizer;	
	const char *format;
	const char *date;
	int error;
	struct datetime_fields *fields;
};


#include "time2.c"

/*
1. There should only be as many tokens available as there are token types
2. Tokens should be initialized to zero
3. Once a token has been parsed, set the tokens offset as the location in the string
*/

static inline const token_info_t *
token_lookup(char tok)
{
	tok -= 48;
	if(tok > 74) return 0; //error! bad token

	static const token_info_t db[] = {
	{0,w_0,r_0}, {0,w_1,r_1},
	{0,w_2,r_2}, {0,w_3,r_3},
	{0,w_4,r_4}, {0,w_5,r_5},
	{0,w_6,r_6}, {0,w_7,r_7}, //7
	{0,w_8,r_8}, {0,w_9,r_9},
	{0,0,0},     {0,0,0},
	{0,0,0},     {0,0,0},
	{0,0,0},     {0,0,0},     //15
	{0,0,0},     {0,w_A,r_A},
	{0,w_B,r_B}, {0,w_C,r_C},
	{TT_DOW,w_D,r_D}, {0,w_E,r_E},
	{TT_MONTH,w_F,r_F}, {0,w_G,r_G}, //23
	{TT_HOUR,w_H,r_H}, {0,w_I,r_I},
	{0,w_J,r_J}, {0,w_K,r_K},
	{0,w_L,r_L}, {TT_MONTH,w_M,r_M},
	{0,w_N,r_N}, {0,w_O,r_O}, //31
	{0,w_P,r_P}, {0,w_Q,r_Q},
	{0,w_R,r_R}, {0,w_S,r_S},
	{0,w_T,r_T}, {TT_TZ,w_U,r_U},
	{0,w_V,r_V}, {0,w_W,r_W},  //39
	{0,w_X,r_X}, {TT_YEAR,w_Y,r_Y},
	{0,w_Z,r_Z}, {0,0,0},
	{0,0,0}, {0,0,0},
	{0,0,0}, {0,0,0},          //47
	{0,0,0}, {0,w_a,r_a},
	{0,w_b,r_b}, {0,w_c,r_c},
	{TT_DAY,w_d,r_d}, {0,w_e,r_e},
	{0,w_f,r_f}, {0,w_g,r_g}, //55
	{TT_HOUR,w_h,r_h}, {TT_MIN,w_i,r_i},
	{0,w_j,r_j}, {0,w_k,r_k},
	{TT_DOW,w_l,r_l}, {TT_MONTH,w_m,r_m}, //63
	{0,w_n,r_n}, {0,w_o,r_o},
	{0,w_p,r_p}, {0,w_q,r_q},
	{0,w_r,r_r}, {TT_SEC,w_s,r_s},
	{0,w_t,r_t}, {0,w_u,r_u}, //71
	{0,w_v,r_v}, {0,w_w,r_w},
	{0,w_x,r_x}, {0,w_y,r_y},
	{0,w_z,r_z}
	};
	return db[tok].type ? &db[tok] : 0;
}


// tokenize a formated date string
static int tokenizer_format(struct tokenizer *tokenizer, const char *format)
{
	int i = 0;
	int pos = 0;
	const token_info_t *info;
	const char *ptr = format;
	int tokmask = 0;

	while(*ptr){
		if(*ptr++ != '%'){
			pos++;
			continue;
		}
		info = token_lookup(*ptr);
		if(!info) return Error_BadToken;

		// Check to make sure we have no duplicate tokens
		if(info->type & tokmask) return Error_DupToken;
		tokmask |= info->type;

		// TODO Need to determine a legit max token count
		if(i == MAX_FORMAT_TOKENS) return Error_TooManyTokens;

		tokenizer->tok[i].info = info; 
		tokenizer->tok[i].pos = pos++;
		i++;
	}

	if( !(tokmask & (TT_YEAR | TT_MONTH | TT_DAY)) )
		return Error_BadFormat;

	tokenizer->tokmask = tokmask;
	tokenizer->tokcount = i;
	return Error_Success;
}

void fill_datetime(int tokmask, struct datetime_fields *f)
{
	if( !(tokmask & TT_DOW) ){
		//f->year * 365;
	}
}

int tokenizer_writer(struct tokenizer *tokenizer,
const char *buf, const char *format, void *user_data)
{
	int i = 0;
	int pos = 0;
	int n;
	int diff;
	const char *fmtptr = format;
	const char *bufptr = buf;
	token_t *tok;
	const token_info_t *tokinfo;
	int tokcount;
	int error;

	error = tokenizer_format(tokenizer, format);
	if(error != Error_Success)
	{
		return error;
	}


preloop:
	tokcount = tokenizer->tokcount;
	if(i >= tokcount) goto exit;

	tok = &tokenizer->tok[i];
	tok->user_data = user_data;

	if(tok->pos == 0)
	{
		fmtptr += 2;	
		goto callback;
	}

	diff = tok->pos - pos;
	strncpy(bufptr, fmtptr, diff);
	bufptr += tok->pos;
	fmtptr += tok->pos+2;
	goto callback;

loop:
	if(i >= tokcount) goto postloop;

	tok = &tokenizer->tok[i];
	tok->user_data = user_data;
	diff = tok->pos - pos - 2;
	strncpy(bufptr, fmtptr, diff);
	bufptr += diff;
	fmtptr += diff + 2;

callback:
	//write out the date format
	tokinfo = tok->info;
	n = tokinfo->writer(tok, bufptr);
	if(!n)
	{
		return Error_BadTokenWrite;
	}
	bufptr += n;
	i++;
	pos = tok->pos;
	goto loop;

postloop:
	// copy the rest of the format string to the buffer

exit:
	return Error_Success;
	
}

int tokenizer_reader(struct tokenizer *tokenizer,
	const char *str, const char *format, void *user_data)
{
	int i = 0;
	int pos = 0;
	int n;
	int diff; //difference between two token positions
	const char *fmtptr = format;
	const char *strptr = str;
	token_t *tok;
	const token_info_t *tokinfo;
	int tokcount;
	int error;


	error = tokenizer_format(tokenizer, format);
	if(error != Error_Success)
	{
		return error;
	}

preloop:
	tokcount = tokenizer->tokcount;
	if(i >= tokcount) goto exit;

	tok = &tokenizer->tok[i];
	tok->user_data = user_data;

	if(tok->pos == 0)
	{
		fmtptr += 2;
		goto callback;
	}

	diff = tok->pos - pos;
	if(strncmp(strptr, fmtptr, diff))
	{
		return Error_FormatMismatch;
	}
	strptr += tok->pos;
	fmtptr += tok->pos+2;
	goto callback;

loop:
	if(i >= tokcount) goto postloop;

	tok = &tokenizer->tok[i];
	tok->user_data = user_data;
	diff = tok->pos - pos - 2;
	if(strncmp(strptr, fmtptr, diff))
	{
		return Error_FormatMismatch;
	}
	strptr += diff;
	fmtptr += diff + 2;

callback:
	tokinfo = tok->info;
	n = tokinfo->reader(tok, strptr);
	if(!n)
	{
		return Error_BadTokenRead;
	}
	strptr += n;

	i++;
	pos = tok->pos;
	goto loop;

postloop:
	//verify the rest of the string after the last token

exit:
	return Error_Success;
}

#define US_PER_MS 1000

inline int days_since_epoch(unsigned long long n)
{
	return (n % 365) - (n / 1460) + 1;
}

// return the day of the week from total days since epoch
// days: total days since the Gregorian epoch
inline char day_of_week(unsigned long long days)
{
	//Gregorian epoch starts on Friday
	const char dow[] = {5, 6, 0, 1, 2, 3, 4};
	return dow[days % 7];
}


//returns the month and current day from the total number of days expired in a year
short get_date_from_days(int days, short year)
{
	char dom[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	char i;

	if (days > 366) return 0;
	dom[1] += is_leap_year(year);
	for (i = 0; i <= 11; i++)
	{
		if (days <= dom[i]) break;
		days -= dom[i];
	}
	return ((i << 8) | days);
}

// return: the number of days that have PASSED in a year
inline short num_year_days(register year, register month, register day)
{
	char dom[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	short days = 0;

	dom[2] += is_leap_year(year);
	for(int i = 1; i < month; i++) days += dom[i];
	return days += day-1;
}


// return: the number of leap years that have passed in a year
inline int num_leap_years_from_year(register year)
{
	if(year < 1601) return (year - 1582) / 4;
	year -= 1601;
	return ((year/400)*97) + (((year%400)/100)*24) + ((year%100)/4) + 5;

	//leap_years = ((years/100)*24) + ((years%100)/4);
	//leap_years = (year - 1582) / 4 - (year % 100);
}

//365.2425
//36524.25

inline int num_leap_days_from_days(register days)
{
	//register double d = (double)days;
	if(days < 6347) return  days / 1461 ;
	days -= 6347;
	return ((days/146097)*97) + (((days%146097)/36524)*24) + ((days%36524)/1461) + 5;
	//return ((d/146097.0)*97.0) + (((d%146097.0)/36524.25)*24.0) + ((d%36524.25)/1460.97) + 1.0;
	//return (days/1461) - ((days/146097)*3) - ((days%146097)/36525) + 5;
}

// return: the number days that have PASSED since the Gregorian epoch (1582-10-15)
// TODO MUST handle dates within the year 1582
inline int num_epoch_days(register year, register month, register day)
{
	return 78 + ((year - 1583) * 365) +
		num_leap_years_from_year(year) + num_year_days(year, month, day);
}

struct date_s {
	char month;
	char day;
	short year;
};

inline struct date_s date_from_days(register int days)
{
	struct date_s date;

	struct {
		short month : 4;
		short day   : 5;	
		short pad   : 7;
	} date_table[] = {
	{10,15}, {10,16}, {10,17}, {10,18}, {10,19}, {10,20}, /* 5 */
	{10,21}, {10,22}, {10,23}, {10,24}, {10,25}, {10,26},
	{10,27}, {10,28}, {10,29}, {10,30}, {10,31}, {11, 1},
	{11, 2}, {11, 3}, {11, 4}, {11, 5}, {11, 6}, {11, 7},
	{11, 8}, {11, 9}, {11,10}, {11,11}, {11,12}, {11,13},
	{11,14}, {11,15}, {11,16}, {11,17}, {11,18}, {11,19}, /* 35 */
	{11,20}, {11,21}, {11,22}, {11,23}, {11,24}, {11,25},
	{11,26}, {11,27}, {11,28}, {11,29}, {11,30}, {12, 1},
	{12, 2}, {12, 3}, {12, 4}, {12, 5}, {12, 6}, {12, 7},
	{12, 8}, {12, 9}, {12,10}, {12,11}, {12,12}, {12,13},
	{12,14}, {12,15}, {12,16}, {12,17}, {12,18}, {12,19}, /* 65 */
	{12,20}, {12,21}, {12,22}, {12,23}, {12,24}, {12,25},
	{12,26}, {12,27}, {12,28}, {12,29}, {12,30}, {12,31},
	{ 1, 1}, { 1, 2}, { 1, 3}, { 1, 4}, { 1, 5}, { 1, 6},
	{ 1, 7}, { 1, 8}, { 1, 9}, { 1,10}, { 1,11}, { 1,12},
	{ 1,13}, { 1,14}, { 1,15}, { 1,16}, { 1,17}, { 1,18}, /* 95 */
	{ 1,19}, { 1,20}, { 1,21}, { 1,22}, { 1,23}, { 1,24},
	{ 1,25}, { 1,26}, { 1,27}, { 1,28}, { 1,29}, { 1,30},
	{ 1,31}, { 2, 1}, { 2, 2}, { 2, 3}, { 2, 4}, { 2, 5},
	{ 2, 6}, { 2, 7}, { 2, 8}, { 2, 9}, { 2,10}, { 2,11},
	{ 2,12}, { 2,13}, { 2,14}, { 2,15}, { 2,16}, { 2,17}, /* 125 */
	{ 2,18}, { 2,19}, { 2,20}, { 2,21}, { 2,22}, { 2,23},
	{ 2,24}, { 2,25}, { 2,26}, { 2,27}, { 2,28}, { 2,29},
	{ 3, 1}, { 3, 2}, { 3, 3}, { 3, 4}, { 3, 5}, { 3, 6},
	{ 3, 7}, { 3, 8}, { 3, 9}, { 3,10}, { 3,11}, { 3,12},
	{ 3,13}, { 3,14}, { 3,15}, { 3,16}, { 3,17}, { 3,18}, /* 155 */
	{ 3,19}, { 3,20}, { 3,21}, { 3,22}, { 3,23}, { 3,24},
	{ 3,25}, { 3,26}, { 3,27}, { 3,28}, { 3,29}, { 3,30},
	{ 3,31}, { 4, 1}, { 4, 2}, { 4, 3}, { 4, 4}, { 4, 5},
	{ 4, 6}, { 4, 7}, { 4, 8}, { 4, 9}, { 4,10}, { 4,11},
	{ 4,12}, { 4,13}, { 4,14}, { 4,15}, { 4,16}, { 4,17}, /* 185 */
	{ 4,18}, { 4,19}, { 4,20}, { 4,21}, { 4,22}, { 4,23},
	{ 4,24}, { 4,25}, { 4,26}, { 4,27}, { 4,28}, { 4,29},
	{ 4,30}, { 5, 1}, { 5, 2}, { 5, 3}, { 5, 4}, { 5, 5},
	{ 5, 6}, { 5, 7}, { 5, 8}, { 5, 9}, { 5,10}, { 5,11},
	{ 5,12}, { 5,13}, { 5,14}, { 5,15}, { 5,16}, { 5,17}, /* 215 */
	{ 5,18}, { 5,19}, { 5,20}, { 5,21}, { 5,22}, { 5,23},
	{ 5,24}, { 5,25}, { 5,26}, { 5,27}, { 5,28}, { 5,29},
	{ 5,30}, { 5,31}, { 6, 1}, { 6, 2}, { 6, 3}, { 6, 4},
	{ 6, 5}, { 6, 6}, { 6, 7}, { 6, 8}, { 6, 9}, { 6,10},
	{ 6,11}, { 6,12}, { 6,13}, { 6,14}, { 6,15}, { 6,16}, /* 245 */
	{ 6,17}, { 6,18}, { 6,19}, { 6,20}, { 6,21}, { 6,22},
	{ 6,23}, { 6,24}, { 6,25}, { 6,26}, { 6,27}, { 6,28},
	{ 6,29}, { 6,30}, { 7, 1}, { 7, 2}, { 7, 3}, { 7, 4},
	{ 7, 5}, { 7, 6}, { 7, 7}, { 7, 8}, { 7, 9}, { 7,10},
	{ 7,11}, { 7,12}, { 7,13}, { 7,14}, { 7,15}, { 7,16}, /* 275 */
	{ 7,17}, { 7,18}, { 7,19}, { 7,20}, { 7,21}, { 7,22},
	{ 7,23}, { 7,24}, { 7,25}, { 7,26}, { 7,27}, { 7,28},
	{ 7,29}, { 7,30}, { 7,31}, { 8, 1}, { 8, 2}, { 8, 3},
	{ 8, 4}, { 8, 5}, { 8, 6}, { 8, 7}, { 8, 8}, { 8, 9},
	{ 8,10}, { 8,11}, { 8,12}, { 8,13}, { 8,14}, { 8,15}, /* 305 */
	{ 8,16}, { 8,17}, { 8,18}, { 8,19}, { 8,20}, { 8,21},
	{ 8,22}, { 8,23}, { 8,24}, { 8,25}, { 8,26}, { 8,27},
	{ 8,28}, { 8,29}, { 8,30}, { 8,31}, { 9, 1}, { 9, 2},
	{ 9, 3}, { 9, 4}, { 9, 5}, { 9, 6}, { 9, 7}, { 9, 8},
	{ 9, 9}, { 9,10}, { 9,11}, { 9,12}, { 9,13}, { 9,14}, /* 335 */
	{ 9,15}, { 9,16}, { 9,17}, { 9,18}, { 9,19}, { 9,20},
	{ 9,21}, { 9,22}, { 9,23}, { 9,24}, { 9,25}, { 9,26},
	{ 9,27}, { 9,28}, { 9,29}, { 9,30}, {10, 1}, {10, 2},
	{10, 3}, {10, 4}, {10, 5}, {10, 6}, {10, 7}, {10, 8},
	{10, 9}, {10,10}, {10,11}, {10,12}, {10,13}, {10,14} /* 365 */
	};

	register int leap_days;
	register int n, i;

	leap_days = num_leap_days_from_days(days);

	//n = ((days % 366) * n) % 366;
	n = (days%365)-leap_days;
	//printf("365: %u | %u\n", days%365, n < 0 ? 365-(-n) : n); //n < 0 ? 365-(-n) : n );
	//printf("366: %u | %u\n", days%366, ((days%366) + ((days/365)-leap_days)) % 366 );
	
	//n = ((days % 366) + ((days/365)-leap_days)) % 366;
	n = ((days%365)-leap_days) % 365;
	n = n < 0 ? 365-(-n) : n;

	/*
	printf("N = %u\n", n);
	i = (days % 365) - n;
	//n = n + ((days % 365) - leap_days);
	n = (n + i) - leap_days;
	return n < 0 ? 366-(-n) : n;
	*/

	date.month = date_table[n].month;
	date.day = date_table[n].day & 0x1f;
	date.year = (days/365) + 1582;

	return date;
}

void from_timestamp(apx_datetime *dt, register unsigned long long t)
{
	struct datetime_fields *f = dt;
	register int n;
	/*
	union {
		unsigned long long nanoseconds;	
		unsigned long long microseconds;
		unsigned long long milliseconds;

	} date;
	*/

	struct date_s date;

	/* Gregorian epoch starts on Friday */
	const char dow[] = {5, 6, 0, 1, 2, 3, 4};
	//char dow[] = { 6, 0, 1, 2, 3, 4, 5 }; //windows epoch is on Monday
	
	// get the number of nanoseconds
	f->nano = (t % 10) * 100;
	t /= 10; //num microseconds
	
	// get the number of microseconds
	f->us = t % 1000; 
	t /= 1000; //num milliseconds 
	
	// get the number of milliseconds
	f->ms = t % 1000;
	t /= 1000; //num seconds
	
	// get the number of seconds
	f->sec = t % 60;
	t /= 60; //num minutes
	
	// get the number of minutes
	f->min = t % 60;
	t /= 60; //num hours

	f->hour = t % 24;
	t /= 24; //num days since epoch
	
	printf("days: %u\n", t);
	f->dow = dow[t % 7];

	date = date_from_days(t);

	f->month = date.month;
	f->day = date.day;
	f->year = date.year;

	printf("Year: %u | Month: %u | date: %u\n", f->year, f->month, f->day);

}

//For every 4 years, add 3 years


// test dates
//  2001-05-12

void test_win()
{
	NTSTATUS r = 0xffff;
	LARGE_INTEGER t = {0};
	TIME_FIELDS f = {0};
	struct datetime_fields dt;

	f.Year = 1601;
	f.Month = 2;
	f.Day = 28;

	ntdll.RtlTimeFieldsToTime(&f, &t);

	//122238720000000000
	t.QuadPart += 5748192000000000;

	printf("Time: %llu\n", t.QuadPart);

	printf("num_leap_years: %u\n", num_leap_years_from_year(f.Year));

	from_timestamp(&dt, t.QuadPart);
}

void to_timestamp(apx_datetime *dt, systime *st)
{

}


// Parse and compute the binary representation of a formatted time string
int apx_format_read(APX_DATETIME *dt,
	const char *format, const char *date, int size)
{
	struct datetime_fields *fields = (struct datetime_fields*)dt;
	struct date_parser parser;
	const char *ptr;
	systime *st;

	parser.format = format;
	parser.date = date;

	if((enum apx_format)format < apx_format_max){

		switch((enum apx_format)format){
		case apx_format_iso_8601:
			parser.format = "%Y-%m-%dT%H:%i:%s%U";
			break;

		case apx_format_http_cookie:
			parser.format = "%D, %d %M %Y %H:%i:%s GMT";
			break;

		// format is a 64 bit Windows FileTime struct
		case apx_format_winnt:
			st = (systime*)format;

		}
	}
	else {
	}

	tokenizer_reader(&parser.tokenizer, date, parser.format, dt);
	printf("Year: %d | Tz: %d\n", fields->year, fields->tz);

	return 0;
}

#define GREGORIAN_EPOCH_TO_UNIX_EPOCH_DAYS    141427
#define GREGORIAN_EPOCH_TO_UNIX_EPOCH_HOURS   3394248
#define GREGORIAN_EPOCH_TO_UNIX_EPOCH_MINUTES 203654880
#define GREGORIAN_EPOCH_TO_UNIX_EPOCH_SECONDS 12219292800 
#define GREGORIAN_EPOCH_TO_UNIX_EPOCH_MILLISE 12219292800000
#define GREGORIAN_EPOCH_TO_UNIX_EPOCH_MICROSE 12219292800000000
#define GREGORIAN_EPOCH_TO_UNIX_EPOCH_NANO100 122192928000000000
#define GREGORIAN_EPOCH_TO_UNIX_EPOCH_NANOSEC 12219292800000000000


int apx_format_write(apx_datetime *adt,
	void *buf, unsigned bufsz, const char *format)
{
	int n;
	struct date_parser parser;
	const char *str = 0;
	enum DateFormat constant;
	struct datetime_fields *f = (struct datetime_fields*)dt;

	struct tokenizer tokenizer;

	if((enum apx_format)format < apx_format_max){
		printf("Printing pre-defined format...\n");
		constant = (enum apx_format)format;
		switch(constant)
		{
		case apx_format_iso_8601:
			format = "%Y-%m-%dT%H:%i%s%U";
			break;

		case apx_format_http_cookie:
			format = "%D, %d %M %Y %H:%i:%s GMT";
			break;

		case apx_format_winnt:
			// convert the systime to a datetime struct
			break;
		}
	}
	else {
		printf("Printing custom format...\n");
		str = format;

		/*
		switch(c){
		case 'd':
		}
		*/
	}

	test_win();

	tokenizer_writer(&tokenizer, buf, format, dt);
	return 0;
}

int apx_timestamp_write(apx_datetime *adt,
	_in_ enum apx_timestamp type, _out_ apx_timestamp *ts)
{
	switch(type){
	case apx_timestamp_winnt:	
		winnt_timestamp(adt);
		break;

	case apx_timestamp_unix_time_sec:
		break;

	case apx_timestamp_unix_time_msec:
		break;

	}
}


int apx_timestamp_read()
{
}


int unixtime_to_systime_ms(__time64_t timestamp, struct datetime_fields *f)
{
	__int64 n;
	__int64 days;
	short val;

	char dow[] = { 3, 4, 5, 6, 0, 1, 2 }; //unix epoch is on a Thursday so we must normalize the weekday

	f->ms = timestamp % MS_PER_SECOND;

	n = timestamp / MS_PER_SECOND; //seconds since epoch
	f->sec = n % SEC_PER_MINUTE;

	n /= SEC_PER_MINUTE; //minutes since epoch
	f->min = n % MIN_PER_HOUR;

	n /= MIN_PER_HOUR; //hours since epoch
	f->hour = n % HOUR_PER_DAY;

	n /= HOUR_PER_DAY; //days since epoch
	days = (n % DAYS_PER_YEAR) - (n / 1460) + 1;

	//f->dow = day_of_week(n, dow);

	n /= DAYS_PER_YEAR; //years since epoch
	val = 1970 + (short)n;
	f->year = (WORD)val;

	val = get_date_from_days((int)days, val);
	if (!val) return 0;
	f->month = (val >> 8) + 1; //windows systemtime starts at 1
	f->day = 0x1f & val;

	return 1;
}

BOOL systime_to_unixtime_ms(SYSTEMTIME *systime, __time64_t *timestamp)
{
	__time64_t ts = 0;
	short days;

	ts += (systime->wYear - 1970) * MS_PER_YEAR;
	days = num_year_days(systime->wMonth-1, systime->wDay, systime->wYear);
	ts += days * MS_PER_DAY;
	ts += systime->wHour * MS_PER_HOUR;
	ts += systime->wMinute * MS_PER_MINUTE;
	ts += systime->wSecond * MS_PER_SECOND;
	ts += systime->wMilliseconds;
	*timestamp = ts;
	
	return TRUE;
}

int test_time()
{
	__time64_t ts = 1600303652534;

	SYSTEMTIME st;
	st.wYear = 2020;
	st.wMonth = 9;
	st.wDayOfWeek = 3;
	st.wDay = 17;
	st.wHour = 0;
	st.wMinute = 47;
	st.wSecond = 32;
	st.wMilliseconds = 0;
	return 1;
}
