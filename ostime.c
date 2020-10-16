

typedef short CSHORT;
typedef struct _TIME_FIELDS
{
    CSHORT Year; // 1601...
    CSHORT Month; // 1..12
    CSHORT Day; // 1..31
    CSHORT Hour; // 0..23
    CSHORT Minute; // 0..59
    CSHORT Second; // 0..59
    CSHORT Milliseconds; // 0..999
    CSHORT Weekday; // 0..6 = Sunday..Saturday
} TIME_FIELDS, *PTIME_FIELDS;


typedef struct _RTL_TIME_ZONE_INFORMATION
{
    LONG Bias;
    WCHAR StandardName[32];
    TIME_FIELDS StandardStart;
    LONG StandardBias;
    WCHAR DaylightName[32];
    TIME_FIELDS DaylightStart;
    LONG DaylightBias;
} RTL_TIME_ZONE_INFORMATION, *PRTL_TIME_ZONE_INFORMATION;

/*
NTSYSAPI
NTSTATUS
NTAPI
RtlQueryTimeZoneInformation(
    _Out_ PRTL_TIME_ZONE_INFORMATION TimeZoneInformation
    );
*/


#if defined(_WIN32)
struct {
NTSTATUS (__stdcall *ZwQuerySystemTime)(PLARGE_INTEGER);
NTSTATUS (__stdcall *RtlQueryTimeZoneInformation)(PRTL_TIME_ZONE_INFORMATION);
VOID (__stdcall *RtlTimeToTimeFields)(PLARGE_INTEGER, PTIME_FIELDS);
VOID (__stdcall *RtlTimeFieldsToTime)(PTIME_FIELDS, PLARGE_INTEGER);
NTSTATUS (__stdcall *RtlSystemTimeToLocalTime)(const LARGE_INTEGER *, PLARGE_INTEGER); 
} ntdll;
#endif


void apx_init()
{
#if defined(_WIN32)
	HMODULE hmod;
	const char *ntdll_path = "C:\\Windows\\System32\\ntdll.dll";

	if( !(hmod = GetModuleHandle(ntdll_path)) ){
		if( !(hmod = LoadLibrary(ntdll_path)) ){
			printf("Error: Could not load Ntdll\n");
			return;
		}
	}
	ntdll.ZwQuerySystemTime = (void*)GetProcAddress(hmod, "ZwQuerySystemTime");
	if(!ntdll.ZwQuerySystemTime){
		printf("Error: Could not get NtQuerySystemTime\n");	
		return;
	}

	ntdll.RtlQueryTimeZoneInformation = (void*)GetProcAddress(hmod, "RtlQueryTimeZoneInformation");
	if(!ntdll.RtlQueryTimeZoneInformation){
		printf("Error: Could not get RtlQueryTimeZoneInformation\n");	
		return;
	}

	ntdll.RtlTimeToTimeFields = (void*)GetProcAddress(hmod, "RtlTimeToTimeFields");
	if(!ntdll.RtlTimeToTimeFields){
		printf("Error: Could not get RtlTimeToTimeFields\n");	
		return;
	}

	ntdll.RtlTimeFieldsToTime = (void*)GetProcAddress(hmod, "RtlTimeFieldsToTime");
	if(!ntdll.RtlTimeFieldsToTime){
		printf("Error: Could not get RtlTimeFieldsToTime\n");	
		return;
	}

	ntdll.RtlSystemTimeToLocalTime = (void*)GetProcAddress(hmod, "RtlSystemTimeToLocalTime");
	if(!ntdll.RtlSystemTimeToLocalTime){
		printf("Error: Could not get RtlSystemTimeToLocalTime\n");	
		return;
	}
#endif

}

void apx_term()
{
	//FreeLibrary(
}

void print_date(PTIME_FIELDS time){
	printf("%s %s %u, %u %u:%u:%u.%u", dow[time->Weekday], month[time->Month-1],
		time->Day, time->Year, time->Hour, time->Minute,
		time->Second, time->Milliseconds
	);
	printf("\n");
}

#if defined(_WIN32)
int is_daylight_time(PRTL_TIME_ZONE_INFORMATION tz, PTIME_FIELDS date)
{
	
	PTIME_FIELDS stdstart;
	PTIME_FIELDS daystart;

	if(!tz->DaylightBias) return 0;
	
	stdstart = &tz->StandardStart;
	daystart = &tz->DaylightStart;

	if(date->Month == daystart->Month){
		if(date->Day == daystart->Day){
			//check time	
		}
		else if(date->Day > daystart->Day) return 1;
		else return 0;
	}
	
	if(date->Month == stdstart->Month){
		if(date->Day == stdstart->Day){
			//check time	
		}
		else if(date->Day < stdstart->Day) return 1;
		else return 0;
	}

	if(daystart->Month > stdstart->Month){
		if(date->Month > daystart->Month ||
		date->Month < stdstart->Month)
			return 1;
	}
	else {
		if(date->Month > daystart->Month &&
		date->Month < stdstart->Month)
			return 1;
	
	}
	return 0;
}
#endif


#if defined(_WIN32)
int get_tz_offset(TIME_FIELDS *tf)
{
	int tz_offset;

	NTSTATUS nterr;
	RTL_TIME_ZONE_INFORMATION tzinfo;

	nterr = ntdll.RtlQueryTimeZoneInformation(&tzinfo);
	if(nterr){
		printf("Error: %u\n", nterr);
		return 0;
	}

	if(is_daylight_time(&tzinfo, tf)){
		tz_offset = 0 - (tzinfo.Bias + tzinfo.DaylightBias);
	}
	else {
		tz_offset = 0 - (tzinfo.Bias + tzinfo.StandardBias);
	}

	return tz_offset;
}
#endif

	

void local_now(apx_datetime dt)
{
#if defined(_WIN32)
	NTSTATUS err;
	TIME_FIELDS tf;
	LARGE_INTEGER utc, local; 

	err = ntdll.ZwQuerySystemTime(&utc);
	if(err){
		printf("Error: %u\n", err);
	}

	err = ntdll.RtlSystemTimeToLocalTime(&utc, &local);
	if(err){
		printf("Error: %u\n", err);
	}

	ntdll.RtlTimeToTimeFields(&local, &tf);

	dt->nano = 0;
	dt->us = 0;
	dt->ms = tf.Milliseconds;
	dt->sec = tf.Second;
	dt->min = tf.Minute;
	dt->hour = tf.Hour;
	dt->day = tf.Day;
	dt->year = tf.Year;
	dt->dow = tf.Weekday;
	dt->month = tf.Month;
	dt->tz = get_tz_offset(&tf);

#endif
}


void utc_now(apx_datetime dt)
{
#if defined(_WIN32)
	NTSTATUS err;
	TIME_FIELDS tf;
	LARGE_INTEGER utc = {0};

	err = ntdll.ZwQuerySystemTime(&utc);
	if(err){
		printf("Error: %u\n", err);
	}

	ntdll.RtlTimeToTimeFields(&utc, &tf);

	dt->nano = 0;
	dt->us = 0;
	dt->ms = tf.Milliseconds;
	dt->sec = tf.Second;
	dt->min = tf.Minute;
	dt->hour = tf.Hour;
	dt->day = tf.Day;
	dt->year = tf.Year;
	dt->dow = tf.Weekday;
	dt->month = tf.Month;
	dt->tz = get_tz_offset(&tf);

#endif
}


