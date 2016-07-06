#define CATCH_CONFIG_COLOUR_NONE
#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <date.h>
#include <chrono>

using namespace date;
using namespace date::literals;
using namespace std::chrono;
using namespace std::chrono_literals;

template<class duration_type>
static duration_type calc_age(const year_month_day& today, const year_month_day& birth)
{
    return floor<duration_type>(
	today.year()/today.month() -
	birth.year()/birth.month() -
	months{today.day() < birth.day()});
}

TEST_CASE("calc_age")
{
    using float_years = duration<double, years::period>;
    auto today = sys_days{2015_y/aug/20};
    const auto birth = sys_days{2010_y/aug/21};
    {
	const auto diff = today - birth;
	CHECK(years{4} == calc_age     <years>(today, birth));
	CHECK(years{4} == floor        <years>(diff));
	CHECK(years{5} == round        <years>(diff));
	CHECK(years{5} == ceil         <years>(diff));
	CHECK(years{4} == duration_cast<years>(diff));
	CHECK(float_years{4.996}.count() == Approx{float_years{diff}.count()}.epsilon(0.001));
    }
    today += days{1};
    {
	const auto diff = today - birth;
	CHECK(years{5} == calc_age     <years>(today, birth));
	CHECK(years{4} == floor        <years>(diff));
	CHECK(years{5} == round        <years>(diff));
	CHECK(years{5} == ceil         <years>(diff));
	CHECK(years{4} == duration_cast<years>(diff));
	CHECK(float_years{4.996}.count() == Approx{float_years{diff}.count()}.epsilon(0.001));
    }
    today += days{1};
    {
	const auto diff = today - birth;
	CHECK(years{5} == calc_age     <years>(today, birth));
	CHECK(years{5} == floor        <years>(diff));
	CHECK(years{5} == round        <years>(diff));
	CHECK(years{6} == ceil         <years>(diff));
	CHECK(years{5} == duration_cast<years>(diff));
	CHECK(float_years{5.002}.count() == Approx{float_years{diff}.count()}.epsilon(0.001));
    }
}

TEST_CASE("date-days arithmetic")
{
    CHECK(sys_days{2010_y/feb/28} + days(2) == 2010_y/mar/2);
}

TEST_CASE("stream time_point")
{
    std::stringstream str;
    str << (sys_days{1986_y/sep/30} + 19h + 53min + 2s + 457ms);
    CHECK(str.str() == "1986-09-30 19:53:02.457");
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
	CHECK(1s == ceil 	 <seconds>(750ms));
	CHECK(0s == floor	 <seconds>(750ms));
	CHECK(1s == round	 <seconds>(750ms));
	CHECK(0s == duration_cast<seconds>(750ms));

	CHECK(1s == ceil 	 <seconds>(250ms));
	CHECK(0s == floor	 <seconds>(250ms));
	CHECK(0s == round	 <seconds>(250ms));
	CHECK(0s == duration_cast<seconds>(250ms));

	CHECK( 0s == ceil 	  <seconds>(-750ms));
	CHECK(-1s == floor	  <seconds>(-750ms));
	CHECK(-1s == round	  <seconds>(-750ms));
	CHECK( 0s == duration_cast<seconds>(-750ms));

	CHECK( 0s == ceil 	  <seconds>(-250ms));
	CHECK(-1s == floor	  <seconds>(-250ms));
	CHECK( 0s == round	  <seconds>(-250ms));
	CHECK( 0s == duration_cast<seconds>(-250ms));
    }
    SECTION("system_clock::time_point")
    {
	using system_time = system_clock::time_point;

	CHECK(1970_y/jan/2 == year_month_day{ceil 	    <days>(system_time{15h})});
	CHECK(1970_y/jan/1 == year_month_day{floor	    <days>(system_time{15h})});
	CHECK(1970_y/jan/2 == year_month_day{round	    <days>(system_time{15h})});
	CHECK(1970_y/jan/1 == year_month_day{time_point_cast<days>(system_time{15h})});

	CHECK(1970_y/jan/2 == year_month_day{ceil 	    <days>(system_time{9h})});
	CHECK(1970_y/jan/1 == year_month_day{floor	    <days>(system_time{9h})});
	CHECK(1970_y/jan/1 == year_month_day{round	    <days>(system_time{9h})});
	CHECK(1970_y/jan/1 == year_month_day{time_point_cast<days>(system_time{9h})});

	CHECK(1970_y/jan/ 1 == year_month_day{ceil 	     <days>(system_time{-15h})});
	CHECK(1969_y/dec/31 == year_month_day{floor	     <days>(system_time{-15h})});
	CHECK(1969_y/dec/31 == year_month_day{round	     <days>(system_time{-15h})});
	CHECK(1970_y/jan/ 1 == year_month_day{time_point_cast<days>(system_time{-15h})});

	CHECK(1970_y/jan/ 1 == year_month_day{ceil 	     <days>(system_time{-9h})});
	CHECK(1969_y/dec/31 == year_month_day{floor	     <days>(system_time{-9h})});
	CHECK(1970_y/jan/ 1 == year_month_day{round	     <days>(system_time{-9h})});
	CHECK(1970_y/jan/ 1 == year_month_day{time_point_cast<days>(system_time{-9h})});
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
	CHECK(time.hours()   ==  7h);
	CHECK(time.minutes() == 33min);
	CHECK(time.seconds() == 20s);
    }
}

TEST_CASE("weekday")
{
    const auto ymw = year_month_weekday{2000_y/jan/1};
    CHECK(ymw.weekday() == sat);
    CHECK(ymw.index() == 1);
    CHECK(ymw == 2000_y/jan/sat[1]);
    CHECK(sys_days{1999_y/dec/last} == sys_days{1999_y/dec/fri[last]});
}

TEST_CASE("time of a day")
{
    const auto today = 1981_y/jan/9;
    const auto tp = sys_days{today} + 11h + 7min + 17s + 117ms;
    auto hms = make_time(tp - sys_days{today});
    CHECK(hms.hours() == 11h);
    CHECK(hms.minutes() == 7min);
    CHECK(hms.seconds() == 17s);
    CHECK(hms.subseconds() == 117ms);
}
