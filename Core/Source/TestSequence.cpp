/*
    Copyright (c) 2005-2019 Xavier Leclercq

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
    IN THE SOFTWARE.
*/

#include "TestSequence.h"

namespace Ishiko
{
namespace Tests
{

TestSequence::TestSequence(const TestNumber& number, const std::string& name)
    : Test(number, name)
{
}

TestSequence::TestSequence(const TestNumber& number, const std::string& name, const TestEnvironment& environment)
    : Test(number, name, environment)
{
}

TestSequence::~TestSequence() noexcept
{
}

const Test& TestSequence::operator[](size_t pos) const
{
    return *(m_tests[pos]);
}

size_t TestSequence::size() const noexcept
{
    return m_tests.size();
}

void TestSequence::append(std::shared_ptr<Test> test)
{
    // We need to update the number of the test
    if (m_tests.size() == 0)
    {
        TestNumber newNumber = number();
        test->setNumber(newNumber.deeperNumber());
    }
    else
    {
        TestNumber newNumber = m_tests.back()->number();
        test->setNumber(++newNumber);
    }

    m_tests.push_back(test);
}

TestResult TestSequence::doRun(Observer::ptr& observer)
{
    // By default the outcome is unknown
    TestResult result = TestResult::eUnknown;

    for (size_t i = 0; i < m_tests.size(); ++i)
    {
        Test& test = *m_tests[i];
        
        test.run(observer);

        // Update the result
        TestResult newResult = test.result();
        if (i == 0)
        {
            // The first test determines the initial value of the result
            result = newResult;
        }
        else if (result == TestResult::eUnknown)
        {
            // If the current sequence outcome is unknown it can only get worse and be set
            // to exception or failed (if the outcome we are adding is exception or 
            // failed)
            if ((newResult == TestResult::eFailed) || (newResult == TestResult::eException))
            {
                result = newResult;
            }
        }
        else if (result == TestResult::ePassed)
        {
            // If the current sequence outcome is passed it stays at this state only if the
            // result we are adding is passed, else it will be 'unknown', 
            // 'passedButMemoryLeaks', 'exception' or 'failed'.
            // depending on the outcome of the result we are adding.
            result = newResult;
        }
        else if (result == TestResult::ePassedButMemoryLeaks)
        {
            // It can only stay at this state if the test is passed or ePassedButMemoryLeaks.
            if ((newResult == TestResult::eFailed) ||
                (newResult == TestResult::eException) ||
                (newResult == TestResult::eUnknown))
            {
                result = newResult;
            }
        }
        else if (result == TestResult::eException)
        {
            // It can only get worse. This happens only if the outcome is 'failed'
            if (newResult == TestResult::eFailed)
            {
                result = newResult;
            }
        }
    }

    return result;
}

}
}
