#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "time_ressource.h"


#define MAX_LEN_TIME 20
#define UTC_TIME_SIZE ((sizeof "{\"time: \" \"\"}") + (MAX_LEN_TIME + 1))

char * getUTCtimeJSON()
{
	char *jsonDate = malloc(UTC_TIME_SIZE);
    time_t timeRaw;
	time(&timeRaw);
	struct tm locT = *localtime(&timeRaw);
	/*yyyy-mm-ddThh:mm:ssZ - UTC date format*/
	sprintf(jsonDate, "{\"time\": \"%d-%d-%dT%d:%d:%dZ\"}", locT.tm_year, locT.tm_mon,
															locT.tm_wday, locT.tm_hour,
															locT.tm_min, locT.tm_sec);
	return jsonDate;
}

char * getUTCtimeHTML()
{
	char *htmlDate = malloc(UTC_TIME_SIZE);
	time_t timeRaw;
	time(&timeRaw);
	struct tm locT = *localtime(&timeRaw);
	/*yyyy-mm-ddThh:mm:ssZ - UTC date format*/
	sprintf(htmlDate, "<p>time: \"%d-%d-%dT%d:%d:%dZ</p>", 	locT.tm_year, locT.tm_mon,
															locT.tm_wday, locT.tm_hour,
															locT.tm_min, locT.tm_sec);
	return htmlDate;
}
char * getUTC_time_Text(){
	char *text_Date = malloc(UTC_TIME_SIZE);
	time_t t;
	time(&t);

	struct tm locT = *localtime(&t);
    sprintf(text_Date,"The time is :%d-%d-%dT%d:%d:%dZ", locT.tm_year,locT.tm_mon,
			                                            locT.tm_wday, locT.tm_hour,
			                                            locT.tm_min, locT.tm_sec);
    return text_Date;
}
