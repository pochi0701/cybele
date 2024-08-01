//
// time.c
//
// Time routines
//
// Copyright (C) 2002 Michael Ringgaard. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. Neither the name of the project nor the names of its contributors
//    may be used to endorse or promote products derived from this software
//    without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
// OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
// SUCH DAMAGE.
//
#include "stdafx.h"
#ifdef linux
#include <unistd.h>
#include <sys/time.h>
#include <sys/times.h>
#endif
#include <time.h>
#include "cbl_String.h"

#define YEAR0                   1900
#define EPOCH_YR                1970
#define SECS_DAY                (24L * 60L * 60L)
#define LEAPYEAR(year)          (!((year) % 4) && (((year) % 100) || !((year) % 400)))
#define YEARSIZE(year)          (LEAPYEAR(year) ? 366 : 365)
#define FIRSTSUNDAY(timp)       (((timp)->tm_yday - (timp)->tm_wday + 420) % 7)
#define FIRSTDAYOF(timp)        (((timp)->tm_wday - (timp)->tm_yday + 420) % 7)

#define TIME_MAX                2147483647L
#define ASCBUFSIZE              (26 + 2)

#ifdef linux
int _daylight = 0;                  // Non-zero if daylight savings time is used
#endif
//long _dstbias = 0;                  // Offset for Daylight Saving Time
#ifdef linux
long _timezone = 0;                 // Difference in seconds between GMT and local time
char* _tzname[2] = { (char*)"GMT", (char*)"GMT" };  // Standard/daylight savings time zone names
#endif

/// <summary>
/// 曜日の表現
/// </summary>
//const char* _days[] = {
//  "Sunday", "Monday", "Tuesday", "Wednesday",
//  "Thursday", "Friday", "Saturday"
//};

/// <summary>
/// 曜日の省略表現
/// </summary>
//const char* _days_abbrev[] = {
//  "Sun", "Mon", "Tue", "Wed",
//  "Thu", "Fri", "Sat"
//};

/// <summary>
/// 月の表現
/// </summary>
//const char* _months[] = {
//  "January", "February", "March",
//  "April", "May", "June",
//  "July", "August", "September",
//  "October", "November", "December"
//};

/// <summary>
/// 月の省略表現
/// </summary>
//const char* _months_abbrev[] = {
//  "Jan", "Feb", "Mar",
//  "Apr", "May", "Jun",
//  "Jul", "Aug", "Sep",
//  "Oct", "Nov", "Dec"
//};

/// <summary>
/// 月ごとの日数
/// </summary>
const int _ytab[2][12] = {
  {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
  {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
};


/// <summary>
/// 格納されている時刻へのポインター。
/// 時刻は、世界協定時刻 (UTC: Coordinated Universal Time) の 1970 年 1 月 1 日の深夜 00:00:00 から経過した時間 (秒単位) を表します。
/// </summary>
/// <param name="timer"></param>
/// <param name="tmbuf"></param>
/// <returns>tm 型の構造体へのポインター。 返された構造体の各フィールドには、sourceTime 引数を現地時刻ではなく UTC で評価した値が格納されています。
/// </returns>
struct tm* gmtime_r(const time_t* timer, struct tm* tmbuf) {
	time_t time = *timer;
	int year = EPOCH_YR;

	auto dayclock = (unsigned long)time % SECS_DAY;
	auto dayno = (unsigned long)time / SECS_DAY;

	tmbuf->tm_sec = dayclock % 60;
	tmbuf->tm_min = (dayclock % 3600) / 60;
	tmbuf->tm_hour = dayclock / 3600;
	tmbuf->tm_wday = (dayno + 4) % 7; // Day 0 was a thursday
	while (dayno >= (unsigned long)YEARSIZE(year)) {
		dayno -= YEARSIZE(year);
		year++;
	}
	tmbuf->tm_year = year - YEAR0;
	tmbuf->tm_yday = dayno;
	tmbuf->tm_mon = 0;
	while (dayno >= (unsigned long)_ytab[LEAPYEAR(year)][tmbuf->tm_mon]) {
		dayno -= _ytab[LEAPYEAR(year)][tmbuf->tm_mon];
		tmbuf->tm_mon++;
	}
	tmbuf->tm_mday = dayno + 1;
	tmbuf->tm_isdst = 0;
#ifdef linux
	tmbuf->tm_gmtoff = 0;
	tmbuf->tm_zone = "UTC";
#endif
	return tmbuf;
}

/// <summary>
/// timer に格納されているデータを 現地時間（日本時間）に変換し、tm構造体に格納します。
/// timerに格納されているデータは一般にtime関数を使って取得します。
/// </summary>
/// <param name="timer">時間データの格納元</param>
/// <param name="tmbuf"></param>
/// <returns></returns>
struct tm* localtime_r(const time_t* timer, struct tm* tmbuf) {
	time_t t;

	t = *timer - _timezone;
	return gmtime_r(&t, tmbuf);
}

/// <summary>
/// asctime() 関数は、time により指される構造体として格納された時間を、文字ストリングに変換します。
/// </summary>
/// <param name="tm">変換する時間を表す構造体</param>
/// <returns>変換した文字列</returns>
wString* asctimew(const struct tm* tm) {
	wString* buf = new wString();
	buf->resize(ASCBUFSIZE);
	strftime(buf->c_str(), ASCBUFSIZE, "%c\n", tm);
	return buf;
}

/// <summary>
/// 時間から文字ストリングへの変換
/// </summary>
/// <param name="timer">変換する時間</param>
/// <returns>変換した文字列</returns>
wString* ctimew(const time_t* timer) {
	struct tm tmbuf = {};
	localtime_r(timer, &tmbuf);
	wString* buf = asctimew(&tmbuf);
	return buf;
}

//char *_strdate(char *s) {
//  time_t now;
//
//  time(&now);
//  strftime(s, 9, "%D", localtime(&now));
//  return s;
//}

//char *_strtime(char *s) {
//  time_t now;
//
//  time(&now);
//  strftime(s, 9, "%T", localtime(&now));
//  return s;
//}



