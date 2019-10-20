﻿#include <vector>
#include <string>
#include <map>
#include <filesystem>

// #include "resources/help_chinese.h"

#include "third/xf_log_console.h"
#include "third/xf_cmd_parser.h"

#include "config/Option.h"
#include "config/LangRules.h"

#include "counter/FileReport.h"
#include "counter/Rapporteur.h"

namespace sc
{

    unsigned int _parser_detail(const std::string& detail)
    {
        if (detail.empty())
            return 0;

        std::map<std::string, unsigned int> order_map{
            { "files",      order_t::by_files },
            { "lines",      order_t::by_lines },
            { "codes",      order_t::by_codes },
            { "blanks",     order_t::by_blanks },
            { "comments",   order_t::by_comments },
            { "asc",        order_t::ascending },
            { "des",        order_t::descending },
            { "ascending",  order_t::ascending },
            { "descending", order_t::descending }
        };

        auto pos = detail.find('|', 3);
        if (std::string::npos == pos)
        {
            auto iter = order_map.find(detail);
            if (iter != order_map.end())
            {
                if (0 < (order_t::order_mask & iter->second))
                    return (iter->second | order_t::descending);
            }
        }
        else
        {
            auto a = detail.substr(0, pos);
            auto b = detail.substr(pos);
            if (a != b)
            {
                auto iter1 = order_map.find(a);
                if (iter1 == order_map.end())
                    return 0;

                auto iter2 = order_map.find(b);
                if (iter2 == order_map.end())
                    return 0;

                if (0 < (order_t::order_mask & iter1->second) && 0 < (order_t::order_mask & iter2->second))
                    return 0;

                unsigned int order = (iter1->second | iter2->second);
                if (0 < (order_t::order_mask & order))
                    return order;
            }
        }

        return 0;
    }

    inline size_t _split_string(std::vector<std::string>& strs, const std::string& str, char symbol)
    {
        size_t n = 0, start = 0;
        for (size_t pos = str.find(symbol, start); std::string::npos != pos; pos = str.find(symbol, start))
        {
            if (start < pos)
            {
                strs.emplace_back(str, start, pos - start);
                ++n;
            }

            start = pos + 1;
        }

        if (start < str.size())
        {
            strs.emplace_back(str, start);
            ++n;
        }

        return n;
    }

    inline const char* version() { return "1.0.0-snapshot"; }
    inline const char* app_name() { return "SourceCounter"; }

    bool Option::InitOption(const xf::cmd::result_t& result)
    {
        if (!result.is_valid())
        {
            std::cout << result.info() << std::endl;
            return false;
        }

        if (result.is_existing("--version"))
        {
            std::cout << app_name() << " " << version() << "  2019-10 by FrankXiong" << std::endl;
            return false;
        }

        if (result.is_existing("--help"))
        {
            // ...
            return false;
        }

        Instance().input = result.get<std::string>("--input");

        if (result.is_existing("--output"))
            Instance().output = result.get<std::string>("--output");

        if (result.is_existing("--config"))
            Instance().configFile = result.get<std::string>("--config");

        if (result.is_existing("--exclude"))
            Instance().exclusion = result.get<std::string>("--exclude");

        if (result.is_existing("--mode"))
            Instance().mode = result.get<unsigned int>("--mode");

        if (result.is_existing("--empty"))
            Instance().empty = result.get<bool>("--empty");

        if (result.is_existing("--languages"))
            _split_string(Instance().languages, result.get<std::string>("--languages"), ',');

        if (result.is_existing("--detail"))
        {
            if (result.has_value("--detail"))
                Instance().detail = _parser_detail(result.get<std::string>("--detail"));
            else
                Instance().detail = order_t::by_nothing;
        }

        _sc_lrs.Load(Instance().ConfigFile());
        _sc_lrs.BuildInRules();

        _sc_rapporteur.Load(Instance().InputPath(), Instance().Languages(), Instance().Exclusion());

        if (result.is_existing("--thread"))
            Instance().nThread = result.get<unsigned int>("--thread");
        else
            Instance().nThread = (_sc_rapporteur.Files().size() < 0x0010 ? 0x01
                                  : _sc_rapporteur.Files().size() < 0x0020 ? 0x02
                                  : _sc_rapporteur.Files().size() < 0x0040 ? 0x04
                                  : _sc_rapporteur.Files().size() < 0x0080 ? 0x08
                                  : _sc_rapporteur.Files().size() < 0x0200 ? 0x10 : 0x20);

        if (result.is_existing("--explain"))
        {
            Instance().Explain();
            return false;
        }

        return true;
    }

    void Option::Explain() const
    {
        // ...
    }

}