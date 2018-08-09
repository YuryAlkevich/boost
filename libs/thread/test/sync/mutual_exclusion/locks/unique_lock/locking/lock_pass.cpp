//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// Copyright (C) 2011 Vicente J. Botet Escriba
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// <boost/thread/locks.hpp>

// template <class Mutex> class unique_lock;

// void lock();

#include <boost/thread/lock_types.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <boost/detail/lightweight_test.hpp>
#include <iostream>
#include "../../../../../timming.hpp"

boost::mutex m;

#if defined BOOST_THREAD_USES_CHRONO
typedef boost::chrono::system_clock Clock;
typedef Clock::time_point time_point;
typedef Clock::duration duration;
typedef boost::chrono::milliseconds ms;
typedef boost::chrono::nanoseconds ns;
#else
#endif

const ms max_diff(BOOST_THREAD_TEST_TIME_MS);

void f()
{
#if defined BOOST_THREAD_USES_CHRONO
  boost::unique_lock < boost::mutex > lk(m, boost::defer_lock);
  time_point t0 = Clock::now();
  lk.lock();
  time_point t1 = Clock::now();
  BOOST_TEST(lk.owns_lock() == true);
  ns d = t1 - t0 - ms(250);
  BOOST_THREAD_TEST_IT(d, ns(max_diff));
  try
  {
    lk.lock();
    BOOST_TEST(false);
  }
  catch (boost::system::system_error& e)
  {
    BOOST_TEST(e.code().value() == boost::system::errc::resource_deadlock_would_occur);
  }
  lk.unlock();
  lk.release();
  try
  {
    lk.lock();
    BOOST_TEST(false);
  }
  catch (boost::system::system_error& e)
  {
    BOOST_TEST(e.code().value() == boost::system::errc::operation_not_permitted);
  }
#else
  boost::unique_lock < boost::mutex > lk(m, boost::defer_lock);
  //time_point t0 = Clock::now();
  lk.lock();
  //time_point t1 = Clock::now();
  BOOST_TEST(lk.owns_lock() == true);
  //ns d = t1 - t0 - ms(250);
  //BOOST_TEST(d < max_diff);
  try
  {
    lk.lock();
    BOOST_TEST(false);
  }
  catch (boost::system::system_error& e)
  {
    BOOST_TEST(e.code().value() == boost::system::errc::resource_deadlock_would_occur);
  }
  lk.unlock();
  lk.release();
  try
  {
    lk.lock();
    BOOST_TEST(false);
  }
  catch (boost::system::system_error& e)
  {
    BOOST_TEST(e.code().value() == boost::system::errc::operation_not_permitted);
  }
#endif
}

int main()
{
  m.lock();
  boost::thread t(f);
#if defined BOOST_THREAD_USES_CHRONO
  boost::this_thread::sleep_for(ms(250));
#else
#endif
  m.unlock();
  t.join();

  return boost::report_errors();
}


