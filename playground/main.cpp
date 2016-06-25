#define CATCH_CONFIG_COLOUR_NONE
#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <date.h>
#include <chrono>

using namespace date;
using namespace date::literals;
using namespace std::chrono;
using namespace std::chrono_literals;

static years calc_age(const year_month_day& today, const year_month_day& birth)
{
    return
	today.year() -
	birth.year() -
	years{today < today.year()/birth.month()/birth.day()};
}

TEST_CASE("calc_age")
{
    CHECK(years{3} == calc_age(2016_y/jul/23, 2013_y/jul/14));
}

TEST_CASE("date-days arithmetic")
{
    CHECK(sys_days{2010_y/feb/28} + days(2) == 2010_y/mar/2);
}

TEST_CASE("stream time_point")
{
    std::cout << std::chrono::system_clock::now();
}

TEST_CASE("year-month-last")
{
    CHECK((2000_y/feb/29 ==
	  year_month_day{
	      year_month_day_last{
		  2000_y, month_day_last{feb}}}));
    CHECK(2000_y/feb/29 == 2000_y/feb/last);
}

TEST_CASE("adding months")
{
    SECTION("+ months{1}")
    {
	auto ymd = 2000_y/jan/30 + months{1};
	CHECK(ymd == 2000_y/feb/30);
	CHECK_FALSE(ymd.ok());
	ymd -= months{1};
	CHECK(ymd == 2000_y/jan/30);
    }
    SECTION("+ months{1} clamp")
    {
	const auto ymd = 2000_y/jan/30 + months{1};
	if(!ymd.ok())
	{
	    const auto clamp = ymd.year()/ymd.month()/last;
	    CHECK(clamp == 2000_y/feb/29);
	}
    }
    SECTION("+ months{1} carry over")
    {
	const auto ymd = 2000_y/jan/30 + months{1};
	if(!ymd.ok())
	{
	    const auto carry_over = sys_days{ymd};
	    CHECK(carry_over == 2000_y/mar/1);
	}
    }
    SECTION("+ months{2}")
    {
	auto ymd = 2000_y/jan/30 + months{2};
	CHECK(ymd == 2000_y/mar/30);
	ymd -= months{2};
	CHECK(ymd == 2000_y/jan/30);
    }
}

TEST_CASE("floor, ceil, round, cast")
{
    SECTION("durations")
    {
	CHECK(1s == ceil <seconds>(750ms));
	CHECK(0s == floor<seconds>(750ms));
	CHECK(1s == round<seconds>(750ms));
	CHECK(0s == duration_cast<seconds>(750ms));

	CHECK(1s == ceil <seconds>(250ms));
	CHECK(0s == floor<seconds>(250ms));
	CHECK(0s == round<seconds>(250ms));
	CHECK(0s == duration_cast<seconds>(250ms));

	CHECK(0s  == ceil <seconds>(-750ms));
	CHECK(-1s == floor<seconds>(-750ms));
	CHECK(-1s == round<seconds>(-750ms));
	CHECK(0s  == duration_cast<seconds>(-750ms));

	CHECK(0s  == ceil <seconds>(-250ms));
	CHECK(-1s == floor<seconds>(-250ms));
	CHECK(0s  == round<seconds>(-250ms));
	CHECK(0s  == duration_cast<seconds>(-250ms));
    }
    SECTION("system_clock::time_point")
    {
	using system_time = system_clock::time_point;

	CHECK(1970_y/jan/2 == year_month_day{ceil <days>(system_time{15h})});
	CHECK(1970_y/jan/1 == year_month_day{floor<days>(system_time{15h})});
	CHECK(1970_y/jan/2 == year_month_day{round<days>(system_time{15h})});
	CHECK(1970_y/jan/1 == year_month_day{time_point_cast<days>(system_time{15h})});

	CHECK(1970_y/jan/2 == year_month_day{ceil <days>(system_time{9h})});
	CHECK(1970_y/jan/1 == year_month_day{floor<days>(system_time{9h})});
	CHECK(1970_y/jan/1 == year_month_day{round<days>(system_time{9h})});
	CHECK(1970_y/jan/1 == year_month_day{time_point_cast<days>(system_time{9h})});

	CHECK(1970_y/jan/1  == year_month_day{ceil <days>(system_time{-15h})});
	CHECK(1969_y/dec/31 == year_month_day{floor<days>(system_time{-15h})});
	CHECK(1969_y/dec/31 == year_month_day{round<days>(system_time{-15h})});
	CHECK(1970_y/jan/1  == year_month_day{time_point_cast<days>(system_time{-15h})});

	CHECK(1970_y/jan/1  == year_month_day{ceil <days>(system_time{-9h})});
	CHECK(1969_y/dec/31 == year_month_day{floor<days>(system_time{-9h})});
	CHECK(1970_y/jan/1  == year_month_day{round<days>(system_time{-9h})});
	CHECK(1970_y/jan/1  == year_month_day{time_point_cast<days>(system_time{-9h})});
    }
}

TEST_CASE("date-time")
{
    {
	auto tp = sys_days{1970_y/jan/3};
	CHECK(tp.time_since_epoch() == days{2});
    }
    {
	auto tp = sys_days{1970_y/jan/3} + 7h;
	CHECK(tp.time_since_epoch() == 55h);
    }
    {
	auto tp = sys_days{1970_y/jan/3} + 7h + 33min;
	CHECK(tp.time_since_epoch() == 3333min);
    }
    {
	auto tp = sys_days{1970_y/jan/3} + 7h + 33min + 20s;
	CHECK(tp.time_since_epoch() == 200000s);
    }
    {
	const auto tp = sys_days{1970_y/jan/3} + 7h + 33min + 20s;
	const auto date = floor<days>(tp);
	CHECK(date == 1970_y/jan/3);
	const auto since_midnight = tp - date;
	CHECK(since_midnight == 27200s);
	const auto time = make_time(since_midnight);
	CHECK(time.hours()   == 7h);
	CHECK(time.minutes() == 33min);
	CHECK(time.seconds() == 20s);
    }
}
