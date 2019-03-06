/*
    Copyright (c) 2000-2019 Xavier Leclercq

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

#include "HeapAllocationErrorsTest.h"
#include "FunctionBasedTest.h"

namespace Ishiko
{
namespace Tests
{

HeapAllocationErrorsTest::HeapAllocationErrorsTest(const TestNumber& number, const std::string& name,
    TestResult (*runFct)())
    : Test(number, name)
{
    m_test = std::make_shared<FunctionBasedTest>(TestNumber(), name, runFct);
}

HeapAllocationErrorsTest::HeapAllocationErrorsTest(const TestNumber& number, const std::string& name,
    TestResult (*runFct)(), const TestEnvironment& environment)
    : Test(number, name, environment)
{
    m_test = std::make_shared<FunctionBasedTest>(TestNumber(), name, runFct, environment);
}

HeapAllocationErrorsTest::HeapAllocationErrorsTest(const TestNumber& number, const std::string& name,
    TestResult (*runFct)(Test& test))
    : Test(TestNumber(), name)
{
    m_test = std::make_shared<FunctionBasedTest>(TestNumber(), name, runFct);
}

HeapAllocationErrorsTest::HeapAllocationErrorsTest(const TestNumber& number, const std::string& name,
    TestResult (*runFct)(Test& test), const TestEnvironment& environment)
    : Test(TestNumber(), name, environment)
{
    m_test = std::make_shared<FunctionBasedTest>(TestNumber(), name, runFct, environment);
}

TestResult HeapAllocationErrorsTest::doRun(Observer& observer)
{
    // We do not pass in the observer as m_test wasn't 
    // created by the user.
    m_test->run();

    return m_test->result();
}

}
}
