#define CATCH_CONFIG_COLOUR_NONE
#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <date.h>
#include <tz.h>
#include <chrono>
#include <sstream>

using namespace date;
using namespace date::literals;
using namespace std::chrono;
using namespace std::chrono_literals;

using std::ratio;

TEST_CASE("construction of durations")
{
    const auto a = seconds{5};
    const auto b = milliseconds{1500};

    CHECK(a + b == milliseconds{6500});
}

TEST_CASE("chrono literals")
{
    const auto a = 1h + 30min + 20s;

    CHECK(a == 5420s);
}

TEST_CASE("no implicit loss of precision")
{
    auto a = 5s;
    const auto b = 1500ms;
    // a = b;
    a = duration_cast<seconds>(b);
    CHECK(a == 1s);

    using floating_secs = duration<double, seconds::period>;
    const floating_secs c = b;
    CHECK(c == floating_secs{1.5});
}

TEST_CASE("round durations")
{
    CHECK( 1s == ceil 	      <seconds>( 750ms));
    CHECK( 0s == floor	      <seconds>( 750ms));
    CHECK( 1s == round	      <seconds>( 750ms));
    CHECK( 0s == duration_cast<seconds>( 750ms));

    CHECK( 1s == ceil 	      <seconds>( 250ms));
    CHECK( 0s == floor	      <seconds>( 250ms));
    CHECK( 0s == round	      <seconds>( 250ms));
    CHECK( 0s == duration_cast<seconds>( 250ms));

    CHECK( 0s == ceil 	      <seconds>(-750ms));
    CHECK(-1s == floor	      <seconds>(-750ms));
    CHECK(-1s == round	      <seconds>(-750ms));
    CHECK( 0s == duration_cast<seconds>(-750ms));

    CHECK( 0s == ceil 	      <seconds>(-250ms));
    CHECK(-1s == floor	      <seconds>(-250ms));
    CHECK( 0s == round	      <seconds>(-250ms));
    CHECK( 0s == duration_cast<seconds>(-250ms));
}

TEST_CASE("defining own durations")
{
    using Tick = duration<int, ratio<1, 4>>;

    auto a = Tick{3};
    CHECK(  a == 750ms);
    CHECK(--a == 500ms);

    const auto set_timeout = [](Tick timeout)
    {
        while(--timeout != 0s) {}
        CHECK(timeout == 0s);
    };

    set_timeout(2s);
    set_timeout(ceil <Tick>(300ms));
    set_timeout(floor<Tick>(300ms));
    set_timeout(round<Tick>(300ms));
}

TEST_CASE("durations at compile time")
{
    constexpr auto a = 5s;
    constexpr auto b = 555ms;
    constexpr auto c = a + b;

    static_assert(c == 5555ms);
}

// system_clock <.......0................................*...>
//                      |<---------- duration ---------->|
//                      |
// epoch        (1970 / jan / 1) ?
//
// steady_clock <.......0............................*.......>
//                      |<-------- duration -------->|
//                      |
// epoch            boot time
TEST_CASE("time_points from clocks")
{
    const auto sysclk_tp = system_clock::now();
    const auto stdclk_tp = steady_clock::now();

    // time_points from different clocks are different types
    // so this does not compile
    // steady_clock::time_point a = system_clock::now();
    // system_clock::time_point b = steady_clock::now();

    static_cast<void>(sysclk_tp);
    static_cast<void>(stdclk_tp);
}

TEST_CASE("time_point arithmetic")
{
    auto a = steady_clock::now();
    const auto b = a + 1h;

    CHECK((b - a) == 1h);
}

TEST_CASE("date creation")
{
    const auto a = year_month_day{year{2010},   month{4},  day{12}};
    const auto b =                year{2010}  / month{4} / day{12};
    const auto c =                     2010_y /   apr    /     12_d;
    const auto d =                     2010_y /   apr    /     12;

    CHECK(a == b);
    CHECK(b == c);
    CHECK(c == d);
}

TEST_CASE("date from today")
{
  const auto now = system_clock::now();
  const auto now_days = sys_days{floor<days>(now)};
  const auto today = year_month_day{now_days};

  CHECK(today == floor<days>(system_clock::now()));
}

TEST_CASE("year-month-last")
{
    const auto a = 2000_y / feb / 29;
    const auto b = year_month_day_last{2000_y, month_day_last{feb}};
    const auto c = 2000_y / feb / last;

    CHECK(a == b);
    CHECK(a == c);
    CHECK(b == c);
}

TEST_CASE("adding months")
{
    SECTION("+ months{1}")
    {
        auto ymd = 2000_y / jan / 30 + months{1};
        CHECK(ymd == 2000_y / feb / 30);
        CHECK_FALSE(ymd.ok());
        ymd -= months{1};
        CHECK(ymd == 2000_y / jan / 30);
    }
    SECTION("+ months{1} clamp")
    {
        const auto ymd = 2000_y / jan / 30 + months{1};
        if(!ymd.ok())
        {
            const auto clamp = ymd.year() / ymd.month() / last;
            CHECK(clamp == 2000_y / feb / 29);
        }
    }
    SECTION("+ months{1} carry over")
    {
        const auto ymd = 2000_y / jan / 30 + months{1};
        if(!ymd.ok())
        {
            const auto carry_over = sys_days{ymd};
            CHECK(carry_over == 2000_y / mar / 1);
        }
    }
    SECTION("+/- months{2}")
    {
        auto ymd = 2000_y / jan / 30 + months{2};
        CHECK(ymd == 2000_y / mar / 30);
        ymd -= months{2};
        CHECK(ymd == 2000_y / jan / 30);
    }
}

TEST_CASE("date-days arithmetic")
{
    CHECK(sys_days{2010_y / feb / 28} + days(2) == 2010_y / mar / 2);
}

TEST_CASE("floor, ceil, round, cast")
{
    SECTION("system_clock::time_point")
    {
        using system_time = system_clock::time_point;

        CHECK(1970_y / jan / 2 == year_month_day{ceil 	    <days>(system_time{15h})});
        CHECK(1970_y / jan / 1 == year_month_day{floor	    <days>(system_time{15h})});
        CHECK(1970_y / jan / 2 == year_month_day{round	    <days>(system_time{15h})});
        CHECK(1970_y / jan / 1 == year_month_day{time_point_cast<days>(system_time{15h})});

        CHECK(1970_y / jan / 2 == year_month_day{ceil 	    <days>(system_time{9h})});
        CHECK(1970_y / jan / 1 == year_month_day{floor	    <days>(system_time{9h})});
        CHECK(1970_y / jan / 1 == year_month_day{round	    <days>(system_time{9h})});
        CHECK(1970_y / jan / 1 == year_month_day{time_point_cast<days>(system_time{9h})});

        CHECK(1970_y / jan /  1 == year_month_day{ceil 	     <days>(system_time{-15h})});
        CHECK(1969_y / dec / 31 == year_month_day{floor	     <days>(system_time{-15h})});
        CHECK(1969_y / dec / 31 == year_month_day{round	     <days>(system_time{-15h})});
        CHECK(1970_y / jan /  1 == year_month_day{time_point_cast<days>(system_time{-15h})});

        CHECK(1970_y / jan /  1 == year_month_day{ceil 	     <days>(system_time{-9h})});
        CHECK(1969_y / dec / 31 == year_month_day{floor	     <days>(system_time{-9h})});
        CHECK(1970_y / jan /  1 == year_month_day{round	     <days>(system_time{-9h})});
        CHECK(1970_y / jan /  1 == year_month_day{time_point_cast<days>(system_time{-9h})});
    }
}

TEST_CASE("weekday")
{
    const auto ymw = year_month_weekday{2000_y / jan / 1};
    CHECK(ymw.weekday() == sat);
    CHECK(ymw.index() == 1);
    CHECK(ymw == 2000_y / jan / sat[1]);
    CHECK(sys_days{1999_y / dec / last} == sys_days{1999_y / dec / fri[last]});
}

TEST_CASE("stream time_point")
{
    std::stringstream str;
    str << (sys_days{1986_y / sep / 30} + 19h + 53min + 2s + 457ms);
    CHECK(str.str() == "1986-09-30 19:53:02.457");
}

template<class duration_type>
static duration_type calc_age(const year_month_day& today, const year_month_day& birth)
{
    return floor<duration_type>(
               today.year() / today.month() -
               birth.year() / birth.month() -
               months{today.day() < birth.day()});
}

TEST_CASE("calc_age")
{
    using float_years = duration<double, years::period>;
    auto today = sys_days{2015_y / aug / 20};
    const auto birth = sys_days{2010_y / aug / 21};
    {
        const auto diff = today - birth;
        CHECK(years{4} == calc_age     <years>(today, birth));
        CHECK(years{4} == floor        <years>(diff));
        CHECK(years{5} == round        <years>(diff));
        CHECK(years{5} == ceil         <years>(diff));
        CHECK(years{4} == duration_cast<years>(diff));
        CHECK(float_years{4.996} .count() == Approx{float_years{diff} .count()} .epsilon(0.001));
    }
    today += days{1};
    {
        const auto diff = today - birth;
        CHECK(years{5} == calc_age     <years>(today, birth));
        CHECK(years{4} == floor        <years>(diff));
        CHECK(years{5} == round        <years>(diff));
        CHECK(years{5} == ceil         <years>(diff));
        CHECK(years{4} == duration_cast<years>(diff));
        CHECK(float_years{4.996} .count() == Approx{float_years{diff} .count()} .epsilon(0.001));
    }
    today += days{1};
    {
        const auto diff = today - birth;
        CHECK(years{5} == calc_age     <years>(today, birth));
        CHECK(years{5} == floor        <years>(diff));
        CHECK(years{5} == round        <years>(diff));
        CHECK(years{6} == ceil         <years>(diff));
        CHECK(years{5} == duration_cast<years>(diff));
        CHECK(float_years{5.002} .count() == Approx{float_years{diff} .count()} .epsilon(0.001));
    }
}

TEST_CASE("date-time")
{
    const auto today = sys_days{1970_y / jan / 1};

    SECTION("date-days")
    {
        const auto tp = today + days{2};
        CHECK(tp - today == days{2});
    }
    SECTION("date-day-hours")
    {
        const auto tp = today + days{2} + 7h;
        CHECK(tp - today == 55h);
    }
    SECTION("date-day-hours-minutes")
    {
        const auto tp = today + days{2} + 7h + 33min;
        CHECK(tp - today == 3333min);
    }
    SECTION("date-day-hours-minutes-seconds")
    {
        const auto tp = today + days{2} + 7h + 33min + 20s;
        CHECK(tp - today == 200000s);
    }
}

TEST_CASE("time of a day")
{
    const auto today = 1981_y / jan / 9;
    const auto tp = sys_days{today} + 11h + 7min + 17s + 117ms;
    auto hms = make_time(tp - sys_days{today});
    CHECK(hms.hours() == 11h);
    CHECK(hms.minutes() == 7min);
    CHECK(hms.seconds() == 17s);
    CHECK(hms.subseconds() == 117ms);
}

TEST_CASE("time of day with tick")
{
    using Tick = duration<int, ratio<1, 4>>;
    SECTION("two ticks")
    {
        const auto tick = Tick{2};
        const auto hms = make_time(tick);
        std::stringstream str;
        str << hms;
        CHECK(str.str() == "00:00:00.5");
    }
    SECTION("one tick")
    {
        const auto tick = Tick{1};
        const auto hms = make_time(tick);
        std::stringstream str;
        str << hms;
        CHECK(str.str() == "00:00:00.2");
    }
    SECTION("one tick with centi format")
    {
        const auto tick = Tick{1};
        const auto hms = make_time(duration<Tick::rep, std::centi> {tick});
        std::stringstream str;
        str << hms;
        CHECK(str.str() == "00:00:00.25");
    }
    SECTION("one tick with milli format")
    {
        const auto tick = Tick{1};
        const auto hms = make_time(duration<Tick::rep, std::milli> {tick});
        std::stringstream str;
        str << hms;
        CHECK(str.str() == "00:00:00.250");
    }
}

TEST_CASE("from serialbased to field based")
{
    const auto tp = sys_days{1976_y / jan / 11} + 7h + 33min + 20s;
    const auto date = year_month_day{floor<days>(tp)};

    CHECK(date == 1976_y / jan / 11);

    const auto since_midnight = tp - sys_days{date};
    CHECK(since_midnight == 27200s);

    const auto time = make_time(since_midnight);
    CHECK(time.hours()   ==  7h);
    CHECK(time.minutes() == 33min);
    CHECK(time.seconds() == 20s);
}

TEST_CASE("local time with time zone")
{
    const auto today = sys_days{2025_y / oct / 3} + 17h + 43min + 23s;
    SECTION("UTC")
    {
	const auto zt = make_zoned(today);
	CHECK(zt.get_info().offset == 0s);
	CHECK(zt.get_sys_time() == today);
	// local_time and sys_time are different types
	// this does not compile
	// CHECK(zt.get_local_time() == today);
	CHECK(zt.get_local_time() == local_days{2025_y / oct / 3} + 17h + 43min + 23s);
    }
}

TEST_CASE("zoned time from system time")
{
    const auto now = system_clock::now();
    const auto now_in_secs = floor<seconds>(now);
    const auto zt = zoned_seconds{now_in_secs};
    CHECK(zt.get_sys_time() == now_in_secs);
    CHECK(zt.get_local_time() == local_seconds{now_in_secs.time_since_epoch()});
}

TEST_CASE("get time zone string from zoned_time")
{
    const auto zt =
        make_zoned(
            current_zone(),
            local_days{2010_y / jan / 28} + 11h + 39min + 38s);

    CHECK(zt.get_info().abbrev == "CET");
}

TEST_CASE("local time arithmetic over daylight savings time")
{
    const auto before_ds_time =
        make_zoned(
            "Europe/Berlin",
            local_days{2016_y / mar / sat[last]} + 9h);

    CHECK(before_ds_time.get_sys_time()   == sys_days  {2016_y / mar / sat[last]} + 8h);
    CHECK(before_ds_time.get_local_time() == local_days{2016_y / mar / sat[last]} + 9h);

    const auto after_ds_time =
        make_zoned(
            "Europe/Berlin",
            before_ds_time.get_local_time() + days{1});

    CHECK(after_ds_time.get_sys_time()   == sys_days  {2016_y / mar / sun[last]} + 7h);
    CHECK(after_ds_time.get_local_time() == local_days{2016_y / mar / sun[last]} + 9h);
}

TEST_CASE("nonexistent and ambiguous local time")
{
    SECTION("summer time")
    {
        const auto never_existed =
            static_cast<local_days>(2016_y / mar / sun[last]) + 2h + 30min;
        CHECK_THROWS_AS(
            make_zoned(
                "Europe/Berlin",
                never_existed),
            nonexistent_local_time);
        CHECK(
            make_zoned(
                "Europe/Berlin",
                never_existed,
                choose::earliest).get_local_time()
            ==
            static_cast<local_days>(2016_y / mar / sun[last]) + 3h);
        CHECK(
            make_zoned(
                "Europe/Berlin",
                never_existed,
                choose::latest).get_local_time()
            ==
            static_cast<local_days>(2016_y / mar / sun[last]) + 3h);
    }
    SECTION("winter time")
    {
        const auto existed_twice =
            static_cast<local_days>(2016_y / oct / sun[last]) + 2h + 30min;
        CHECK_THROWS_AS(
            make_zoned(
                "Europe/Berlin",
                existed_twice),
            ambiguous_local_time);
        CHECK(
            make_zoned(
                "Europe/Berlin",
                existed_twice,
                choose::earliest).get_local_time()
            ==
            static_cast<local_days>(2016_y / oct / sun[last]) + 2h + 30min);
        CHECK(
            make_zoned(
                "Europe/Berlin",
                existed_twice,
                choose::latest).get_local_time()
            ==
            static_cast<local_days>(2016_y / oct / sun[last]) + 2h + 30min);
    }
}
