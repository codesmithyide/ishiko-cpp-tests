// SPDX-FileCopyrightText: 2000-2024 Xavier Leclercq
// SPDX-License-Identifier: BSL-1.0

#include "FileComparisonTestCheck.hpp"
#include "Test.hpp"
#include <Ishiko/BasePlatform.hpp>
#include <Ishiko/Diff.hpp>
#include <Ishiko/Errors.hpp>
#include <Ishiko/FileSystem.hpp>

using namespace Ishiko;

FileComparisonTestCheck::FileComparisonTestCheck()
{
}

FileComparisonTestCheck::FileComparisonTestCheck(boost::filesystem::path outputFilePath,
    boost::filesystem::path referenceFilePath)
    : m_outputFilePath(std::move(outputFilePath)), m_referenceFilePath(std::move(referenceFilePath))
{
}

FileComparisonTestCheck FileComparisonTestCheck::CreateFromContext(const TestContext& context,
    const boost::filesystem::path& outputAndReferenceFilePath, TestContext::PathResolution path_resolution)
{
    return FileComparisonTestCheck(context.getOutputPath(outputAndReferenceFilePath),
        context.getReferencePath(outputAndReferenceFilePath, path_resolution));
}

FileComparisonTestCheck FileComparisonTestCheck::CreateFromContext(const TestContext& context,
    const boost::filesystem::path& outputFilePath, const boost::filesystem::path& referenceFilePath,
    TestContext::PathResolution path_resolution)
{
    return FileComparisonTestCheck(context.getOutputPath(outputFilePath),
        context.getReferencePath(referenceFilePath, path_resolution));
}

void FileComparisonTestCheck::run(Test& test, const char* file, int line)
{
    // TODO: fix this file comparison code. It's hacked together at the moment

    m_result = Result::failed;

    // We first try to open the two files
#if ISHIKO_COMPILER == ISHIKO_COMPILER_GCC
    FILE* output_file = fopen(m_outputFilePath.string().c_str(), "rb");
    FILE* refFile = fopen(m_referenceFilePath.string().c_str(), "rb");
#elif ISHIKO_COMPILER == ISHIKO_COMPILER_MSVC
    FILE* output_file = nullptr;
    errno_t err = fopen_s(&output_file, m_outputFilePath.string().c_str(), "rb");
    if (err != 0)
    {
        output_file = nullptr;
    }
    FILE* refFile = nullptr;
    err = fopen_s(&refFile, m_referenceFilePath.string().c_str(), "rb");
    if (err != 0)
    {
        refFile = nullptr;
    }
#else
    #error Unsupported or unrecognized compiler
#endif

    if (output_file == nullptr)
    {
        if (refFile)
        {
            fclose(refFile);
        }
        std::string message = "failed to open output file: " + m_outputFilePath.string();
        test.fail(message, file, line);
    }
    else if (refFile == nullptr)
    {
        if (output_file)
        {
            fclose(output_file);
        }
        std::string message = "failed to open reference file: " + m_referenceFilePath.string();
        test.fail(message, file, line);
    }

    bool isFileEof = false;
    bool isRefFileEof = false;
    if (output_file && refFile)
    {
        // We managed to open both file, let's compare them

        bool isFileGood;
        bool isRefFileGood;
        char c1;
        char c2;
        int offset = 0;
        while (1)
        {
            size_t n1 = fread(&c1, 1, 1, output_file);
            size_t n2 = fread(&c2, 1, 1, refFile);
            isFileGood = (n1 == 1);
            isRefFileGood = (n2 == 1);
            if (!isFileGood || !isRefFileGood || (c1 != c2))
            {
                break;
            }
            offset++;
        }

        if (feof(output_file) != 0)
        {
            isFileEof = true;
        }
        if (feof(refFile) != 0)
        {
            isRefFileEof = true;
        }

        // The comparison is finished, close the files
        fclose(output_file);
        fclose(refFile);

        if (!(isFileEof && isRefFileEof))
        {
            test.fail(file, line);
        }
    }

    if (!(isFileEof && isRefFileEof))
    {
        Error error;
        TextPatch diff = TextDiff::LineDiffFiles(m_outputFilePath, m_referenceFilePath, error);
        if (diff.size() > 0)
        {
            m_firstDifferentLine = diff[0].text();
            test.fail(m_firstDifferentLine, file, line);   
        }

        // TODO: have a toggle on command line to explicitly enable this?

        Error persistenceError;
        boost::filesystem::path persistentStoragePath = test.context().getOutputDirectory("persistent-storage",
            persistenceError);
        // TODO: ignore error only if the error is not found
        if (!persistenceError)
        {
            boost::filesystem::path targetDirectory = persistentStoragePath / test.name();
            // TODO: CopyFile option to do that
            boost::filesystem::create_directories(targetDirectory);
            // TODO: we need to keep more than the filename, the entire relative path from output dir needs to be used
            FileSystem::CopySingleFile(m_outputFilePath, (targetDirectory / m_outputFilePath.filename()),
                persistenceError);
            FileSystem::CopySingleFile(m_referenceFilePath, (targetDirectory / m_referenceFilePath.filename()),
                persistenceError);

            // TODO: what do if we cannot copy the files?
        }
    }
    else
    {
        m_result = Result::passed;
    }
}

const boost::filesystem::path& FileComparisonTestCheck::outputFilePath() const
{
    return m_outputFilePath;
}

void FileComparisonTestCheck::setOutputFilePath(const boost::filesystem::path& path)
{
    m_outputFilePath = path;
}

const boost::filesystem::path& FileComparisonTestCheck::referenceFilePath() const
{
    return m_referenceFilePath;
}

void FileComparisonTestCheck::setReferenceFilePath(const boost::filesystem::path& path)
{
    m_referenceFilePath = path;
}

void FileComparisonTestCheck::addToJUnitXMLTestReport(JUnitXMLWriter& writer) const
{
    writer.writeText("File comparison between output ");
    writer.writeText(m_outputFilePath.string());
    writer.writeText(" and reference ");
    writer.writeText(m_referenceFilePath.string());
    writer.writeText(" failed.");
    writer.writeText(m_firstDifferentLine);
}
