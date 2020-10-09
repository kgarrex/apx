

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


struct {
NTSTATUS (__stdcall *ZwQuerySystemTime)(PLARGE_INTEGER);
NTSTATUS (__stdcall *RtlQueryTimeZoneInformation)(PRTL_TIME_ZONE_INFORMATION);
VOID (__stdcall *RtlTimeToTimeFields)(PLARGE_INTEGER, PTIME_FIELDS);
VOID (__stdcall *RtlTimeFieldsToTime)(PTIME_FIELDS, PLARGE_INTEGER);
NTSTATUS (__stdcall *RtlSystemTimeToLocalTime)(const LARGE_INTEGER *, PLARGE_INTEGER); 
} ntdll;


void time_init()
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

void time_term()
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
		if(date->Month > daystart->Month || date->Month < stdstart->Month) return 1;
	}
	else {
		if(date->Month > daystart->Month && date->Month < stdstart->Month) return 1;
	
	}
	return 0;
}
#endif

int get_tz_offset(void *local_ptr)
{
	int tz_offset;

#if defined(_WIN32)
	NTSTATUS nterr;
	PLARGE_INTEGER timestamp = local_ptr;
	TIME_FIELDS time_fields;
	RTL_TIME_ZONE_INFORMATION tzinfo;

	nterr = ntdll.RtlQueryTimeZoneInformation(&tzinfo);
	if(nterr){
		printf("Error: %u\n", nterr);
		return 0;
	}
	ntdll.RtlTimeToTimeFields(timestamp, &time_fields);

	if(is_daylight_time(&tzinfo, &time_fields)){
		tz_offset = 0 - (tzinfo.Bias + tzinfo.DaylightBias);
	}
	else {
		tz_offset = 0 - (tzinfo.Bias + tzinfo.StandardBias);
	}
#endif

	return tz_offset;
}

LARGE_INTEGER win32_now()
{
	NTSTATUS err;
	LARGE_INTEGER utc = {0};
	err = ntdll.ZwQuerySystemTime(&utc);
	if(err){
		printf("Error: %u\n", err);
	}
	return utc;
}

LARGE_INTEGER win32_now_local()
{
	NTSTATUS err;
	LARGE_INTEGER utc, local = {0};
	err = ntdll.ZwQuerySystemTime(&utc);
	if(err){
		printf("Error: %u\n", err);
		return local;
	}

	err = ntdll.RtlSystemTimeToLocalTime(&utc, &local);
	if(err){
		printf("Error: %u\n", err);
		return local;
	}
	return local;
}

void win32_time_fields(LARGE_INTEGER *time, apx_datetime *f)
{
	TIME_FIELDS tf;
	ntdll.RtlTimeToTimeFields(time, &tf);

	f->ms = tf.Milliseconds;
	f->sec = tf.Second;
	f->min = tf.Minute;
	f->hour = tf.Hour;
	f->day = tf.Day;
	f->year = tf.Year;
	f->dow = tf.Weekday;
	f->month = tf.Month;
	f->tz = get_tz_offset(time);
}

void local_now(apx_datetime *adt)
{
	datetime_t dt;
	struct datetime_fields f;

#if defined(_WIN32)
	LARGE_INTEGER local;
	local = win32_now_local();
	win32_time_fields(&local, &f);
#endif

	dt = fields_to_datetime(f);
	return dt;
}


void now(apx_datetime *adt)
{
	datetime_t dt;
	struct datetime_fields f;

#if defined(_WIN32)
	LARGE_INTEGER utc;
	utc = win32_now();
	win32_time_fields(&utc, &f);
#endif
	
	dt = fields_to_datetime(f);
	return dt;
}


