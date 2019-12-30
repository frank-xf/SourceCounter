﻿#pragma once

#include <map>
#include <mutex>

#include "ReportType.h"
#include "LangRules.h"

namespace sc
{
    // 分析线程类
    class Counter
    {
    public:

        using file_report_t = std::tuple<std::string, std::string, report_t>;

        Counter() = default;

    private:

        std::mutex m_Mutex;
        std::vector<file_report_t> m_Reports;
        pairs_t m_Files;
        LangRules m_Rules;

        // 取出一个文件
        bool _PickFile(std::pair<std::string, std::string>& file);

        // 分析线程函数
        std::vector<file_report_t> _Analyze(unsigned int mode);

    public:

        std::vector<std::string> Files() const;
        std::vector<std::string> Files(const std::string& language) const;
        const std::vector<file_report_t>& Reports() const { return m_Reports; }
        const LangRules& Rules() const { return m_Rules; }

        // 加载文件
        unsigned int Load(const std::string& input, const std::string& config, const std::string& excludes, std::vector<std::string>& includes, bool allowEmpty);

        // 启动线程
        bool Start(unsigned int nThread, unsigned int mode);

    };  // class Counter

}   // namespace sc
