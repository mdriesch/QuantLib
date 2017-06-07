/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2017 Michael von den Driesch (based on the flowanalysis from
 the quantlibaddin - see e.g. here
 https://github.com/lballabio/quantlib-old/blob/master/QuantLibAddin/qlo/flowanalysis.hpp)

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <http://quantlib.org/license.shtml>.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

#ifndef ql_analysis_hpp
#define ql_analysis_hpp

#include <boost/shared_ptr.hpp>
#include <iostream>
#include <ql/cashflow.hpp>
#include <ql/utilities/null.hpp>
#include <string>
#include <vector>

namespace QuantLib {
// class Date;

std::vector<std::vector<std::string> >
flowAnalysis(
    const Leg& leg, const Date& d = Null<Date>(),
    const std::ios_base::fmtflags& fmt = std::ios_base::fmtflags(),
    const std::string& dateFormat = "%Y-%m-%d");
}

#endif
