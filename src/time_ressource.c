#include <time.h>
#include <stdlib.h>
#include <string.h>
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
	sprintf(jsonDate, "{\"time\": \"%d-%d-%dT%d:%d:%dZ\"}", 	locT.tm_year, locT.tm_mon,
																locT.tm_wday, locT.tm_hour,
																locT.tm_min, locT.tm_sec);
	return jsonDate;
}
