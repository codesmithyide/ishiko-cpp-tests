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

#ifndef _ISHIKO_TESTFRAMEWORK_CORE_TEST_H_
#define _ISHIKO_TESTFRAMEWORK_CORE_TEST_H_

#include "TestNumber.h"
#include "TestResult.h"
#include "TestEnvironment.h"
#include "TestSetupAction.h"
#include "TestTeardownAction.h"
#include <functional>
#include <string>
#include <vector>
#include <memory>

namespace Ishiko
{
namespace Tests
{

/// This class represents a test.

/// It is the abstract base class for the various test classes.
class Test
{
public:
    class Observer
    {
    public:
        enum EEventType
        {
            eTestStart,
            eTestEnd
        };

        virtual ~Observer() noexcept = default;

        virtual void onEvent(const Test& source, EEventType type);
    };

    class Observers final
    {
    public:
        void add(std::shared_ptr<Observer> observer);
        void remove(std::shared_ptr<Observer> observer);

        void notifyEvent(const Test& source, Observer::EEventType type);

    private:
        void removeDeletedObservers();

    private:
        std::vector<std::pair<std::weak_ptr<Observer>, size_t>> m_observers;
    };

    /// Constructor.
    /// @param number The number of the test.
    /// @param name The name of the test.
    Test(const TestNumber& number, const std::string& name);
    Test(const TestNumber& number, const std::string& name, const TestEnvironment& environment);
    Test(const TestNumber& number, const std::string& name, TestResult result);
    Test(const TestNumber& number, const std::string& name, TestResult result, const TestEnvironment& environment);
    Test(const TestNumber& number, const std::string& name, std::function<void(Test& test)> runFct);
    Test(const TestNumber& number, const std::string& name, std::function<void(Test& test)> runFct,
        const TestEnvironment& environment);
    virtual ~Test() noexcept = default;

    const TestNumber& number() const;
    virtual void setNumber(const TestNumber& number);
    const std::string& name() const;

    TestResult result() const;
    void setResult(TestResult result);
    bool passed() const;
    virtual void getPassRate(size_t& unknown, size_t& passed, size_t& passedButMemoryLeaks, size_t& exception,
        size_t& failed, size_t& total) const;
    void abort(const char* file, int line);
    void abortIf(bool condition, const char* file, int line);
    void fail(const char* file, int line);
    void failIf(bool condition, const char* file, int line);
    void pass();

    const TestEnvironment& environment() const;
    
    virtual void run();

    virtual void addSetupAction(std::shared_ptr<TestSetupAction> action);
    virtual void addTeardownAction(std::shared_ptr<TestTeardownAction> action);

    virtual void traverse(std::function<void(const Test& test)> function) const;

    Observers& observers();

protected:
    virtual void setup();
    virtual void doRun();
    virtual void teardown();
    virtual void notify(Observer::EEventType type);
    
private:
    class AbortException
    {
    };

    TestNumber m_number;
    std::string m_name;
    TestResult m_result;
    const TestEnvironment& m_environment;
    bool m_memoryLeakCheck;
    std::vector<std::shared_ptr<TestSetupAction>> m_setupActions;
    std::vector<std::shared_ptr<TestTeardownAction>> m_teardownActions;
    Observers m_observers;
    std::function<void(Test& test)> m_runFct;
};

}
}

#define ISHTF_ABORT() test.abort(__FILE__, __LINE__)
// The double negation is needed to cope with classes that have an explicit operator bool
#define ISHTF_ABORT_IF(condition) test.abortIf(!!(condition), __FILE__, __LINE__)
#define ISHTF_ABORT_UNLESS(condition) test.abortIf(!(condition), __FILE__, __LINE__)
#define ISHTF_FAIL() test.fail(__FILE__, __LINE__)
// The double negation is needed to cope with classes that have an explicit operator bool
#define ISHTF_FAIL_IF(condition) test.failIf(!!(condition), __FILE__, __LINE__)
#define ISHTF_FAIL_UNLESS(condition) test.failIf(!(condition), __FILE__, __LINE__)
#define ISHTF_PASS() test.pass()

#endif
