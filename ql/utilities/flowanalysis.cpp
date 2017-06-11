/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2017 Michael von den Driesch

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

#include <ql/cashflows/capflooredcoupon.hpp>
#include <ql/cashflows/digitalcoupon.hpp>
#include <ql/indexes/interestrateindex.hpp>
#include <ql/utilities/flowanalysis.hpp>

using QuantLib::Visitor;
using std::vector;

namespace {

class AnalysisGenerator : public QuantLib::AcyclicVisitor,
                          public Visitor<QuantLib::CashFlow>,
                          public Visitor<QuantLib::Coupon>,
                          public Visitor<QuantLib::FloatingRateCoupon>,
                          public Visitor<QuantLib::CappedFlooredCoupon>,
                          public Visitor<QuantLib::DigitalCoupon> {
   private:
    stringVector flows_;
    anyVector fullFlows_;
    static const QuantLib::Size numberOfColumns_ = 20;
    std::stringstream& stream_;
    std::string dateFormat_;
    std::string str(double d);
    std::string str(QuantLib::Date d);

   public:
    AnalysisGenerator(std::stringstream& str, const std::string& dateFormat);
    void reset();
    void visit(QuantLib::CashFlow& c);
    void visit(QuantLib::Coupon& c);
    void visit(QuantLib::FloatingRateCoupon& c);
    void visit(QuantLib::CappedFlooredCoupon& c);
    void visit(QuantLib::DigitalCoupon& c);
    const stringVector& analysis() const;
    anyVector analysisAny() const;
};

#define PAYMENT_DATE 0
#define AMOUNT 1
#define NOMINAL 2
#define ACCRUAL_START_DATE 3
#define ACCRUAL_END_DATE 4
#define ACCRUAL_DAYS 5
#define INDEX 6
#define FIXING_DAYS 7
#define FIXING_DATES 8
#define DAY_COUNTER 9
#define ACCRUAL_PERIOD 10
#define EFFECTIVE_RATE 11
#define FLOOR 12
#define GEARING 13
#define INDEX_FIXING 14
#define CONV_ADJ 15
#define SPREAD 16
#define CAP 17
#define CALLDIGITALRATE 18
#define PUTDIGITALRATE 19

AnalysisGenerator::AnalysisGenerator(std::stringstream& str,
                                     const std::string& dateFormat)
    : dateFormat_(dateFormat), stream_(str) {
    reset();
}

void AnalysisGenerator::reset() {
    flows_.clear();
    fullFlows_.clear();

    vector<std::string> headings(numberOfColumns_);
    headings[PAYMENT_DATE] = std::string("Payment Date");
    headings[AMOUNT] = std::string("Amount");

    headings[NOMINAL] = std::string("Nominal");
    headings[ACCRUAL_START_DATE] = std::string("Accrual Start Date");
    headings[ACCRUAL_END_DATE] = std::string("Accrual End Date");
    headings[ACCRUAL_DAYS] = std::string("Accrual Days");
    headings[DAY_COUNTER] = std::string("Day Counter");
    headings[ACCRUAL_PERIOD] = std::string("Accrual Period");
    headings[EFFECTIVE_RATE] = std::string("Effective Rate");

    headings[FIXING_DAYS] = std::string("Fixing Days");
    headings[FIXING_DATES] = std::string("Fixing Dates");
    headings[INDEX] = std::string("Index");
    headings[FLOOR] = std::string("Floor");
    headings[GEARING] = std::string("Gearing");
    headings[INDEX_FIXING] = std::string("Index Fixing");
    headings[CONV_ADJ] = std::string("Conv. Adj.");
    headings[SPREAD] = std::string("Spread");
    headings[CAP] = std::string("Cap");
    headings[PUTDIGITALRATE] = std::string("Put Digital Payoff");
    headings[CALLDIGITALRATE] = std::string("Call Digital Payoff");

    flows_.push_back(headings);
    std::vector<boost::any> tmp;
    for (auto i = tmp.begin(); i != tmp.end(); ++i) tmp.push_back(*i);
    fullFlows_.push_back(tmp);
}

inline std::string AnalysisGenerator::str(double d) {
    stream_.str(std::string());
    stream_ << d;
    return stream_.str();
}

inline std::string AnalysisGenerator::str(QuantLib::Date d) {
    stream_.str(std::string());
    stream_ << QuantLib::io::formatted_date(d, dateFormat_);
    return stream_.str();
}

void AnalysisGenerator::visit(QuantLib::CashFlow& c) {
    vector<std::string> cf(numberOfColumns_, std::string("#N/A"));
    vector<boost::any> cfAny(numberOfColumns_, std::string("#N/A"));
    cf[PAYMENT_DATE] = str(c.date());
    cfAny[PAYMENT_DATE] = c.date();
    try {
        cf[AMOUNT] = str(c.amount());
        cfAny[AMOUNT] = c.amount(); 
    } catch (...) {
    }
    flows_.push_back(cf);
    fullFlows_.push_back(cfAny);
}

void AnalysisGenerator::visit(QuantLib::Coupon& c) {
    visit(static_cast<QuantLib::CashFlow&>(c));
    flows_.back()[NOMINAL] = str(c.nominal());
    flows_.back()[ACCRUAL_START_DATE] = str(c.accrualStartDate());
    flows_.back()[ACCRUAL_END_DATE] = str(c.accrualEndDate());
    flows_.back()[ACCRUAL_DAYS] = str((long)c.accrualDays());
    flows_.back()[DAY_COUNTER] = c.dayCounter().name();
    flows_.back()[ACCRUAL_PERIOD] = str(c.accrualPeriod());
    try {
        flows_.back()[EFFECTIVE_RATE] = str(c.rate());
    } catch (...) {
    }
};

void AnalysisGenerator::visit(QuantLib::FloatingRateCoupon& c) {
    visit(static_cast<QuantLib::Coupon&>(c));
    flows_.back()[FIXING_DAYS] = str((long)c.fixingDays());
    flows_.back()[FIXING_DATES] = str(c.fixingDate());
    flows_.back()[INDEX] = c.index()->name();
    flows_.back()[FLOOR] = std::string("#N/A");
    flows_.back()[GEARING] = str(c.gearing());
    try {
        flows_.back()[INDEX_FIXING] = str(c.indexFixing());
    } catch (...) {
    }
    try {
        flows_.back()[CONV_ADJ] = str(c.convexityAdjustment());
    } catch (...) {
    }
    flows_.back()[SPREAD] = str(c.spread());
    flows_.back()[CAP] = std::string("#N/A");
}

void AnalysisGenerator::visit(QuantLib::CappedFlooredCoupon& c) {
    visit(static_cast<QuantLib::Coupon&>(c));
    flows_.back()[FIXING_DAYS] = str((long)c.fixingDays());
    flows_.back()[FIXING_DATES] = str(c.fixingDate());
    flows_.back()[INDEX] = c.index()->name();
    if (c.floor() != QuantLib::Null<QuantLib::Rate>())
        flows_.back()[FLOOR] = str(c.floor());
    else
        flows_.back()[FLOOR] = std::string("#N/A");
    flows_.back()[GEARING] = str(c.gearing());
    try {
        flows_.back()[INDEX_FIXING] = str(c.indexFixing());
    } catch (...) {
    }
    try {
        flows_.back()[CONV_ADJ] = str(c.convexityAdjustment());
    } catch (...) {
    }
    flows_.back()[SPREAD] = str(c.spread());
    if (c.cap() != QuantLib::Null<QuantLib::Rate>())
        flows_.back()[CAP] = str(c.cap());
    else
        flows_.back()[CAP] = std::string("#N/A");
}

void AnalysisGenerator::visit(QuantLib::DigitalCoupon& c) {
    visit(static_cast<QuantLib::Coupon&>(c));
    flows_.back()[FIXING_DAYS] = str((long)c.fixingDays());
    flows_.back()[FIXING_DATES] = str(c.fixingDate());
    flows_.back()[INDEX] = c.index()->name();
    if (c.hasPut())
        flows_.back()[FLOOR] = str(c.putStrike());
    else
        flows_.back()[FLOOR] = std::string("#N/A");
    flows_.back()[GEARING] = str(c.gearing());
    try {
        flows_.back()[INDEX_FIXING] = str(c.indexFixing());
    } catch (...) {
    }
    try {
        flows_.back()[CONV_ADJ] = str(c.underlying()->convexityAdjustment());
    } catch (...) {
    }
    flows_.back()[SPREAD] = str(c.spread());
    if (c.hasCall())
        flows_.back()[CAP] = str(c.callStrike());
    else
        flows_.back()[CAP] = std::string("#N/A");
    if (c.putDigitalPayoff() != QuantLib::Null<QuantLib::Rate>())
        flows_.back()[PUTDIGITALRATE] = str(c.putDigitalPayoff());
    else
        flows_.back()[PUTDIGITALRATE] = std::string("#N/A");
    if (c.callDigitalPayoff() != QuantLib::Null<QuantLib::Rate>())
        flows_.back()[CALLDIGITALRATE] = str(c.callDigitalPayoff());
    else
        flows_.back()[CALLDIGITALRATE] = std::string("#N/A");
}

const stringVector& AnalysisGenerator::analysis() const { return flows_; }
}

namespace QuantLib {

stringVector flowAnalysis(const QuantLib::Leg& leg, const QuantLib::Date& d,
                          const int& precision, const int& width,
                          const std::ios_base::fmtflags& fmt,
                          const std::string& dateFormat) {
    std::stringstream ss;
    ss.flags(fmt);
    ss.precision(precision);
    if (width > 0) ss.width(width);
    AnalysisGenerator generator(ss, dateFormat);
    for (QuantLib::Size i = 0; i < leg.size(); ++i) {
        if (d == QuantLib::Null<QuantLib::Date>() || leg[i]->date() > d)
            leg[i]->accept(generator);
    }
    return generator.analysis();
}

std::string flowAnalysisPrint(stringVector s) {
    std::stringstream ss;
    Size rows = s.size();
    QL_ASSERT(rows > 0, "String matrix is empty.");
    Size columns = s[0].size();

    for (Size row = 0; row < rows; ++row) {
        for (Size col = 0; col < columns; ++col) {
            ss << s[row][col] << ",";
        }
        ss.seekp(-1, std::ios_base::end);  // remove last comma
        ss << std::endl;
    }
    return ss.str();
}
}
