//
// Copyright 2024 Nikolaj Banke Jensen.
//
// This file is part of HDVR.
// 
// HDVR is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// HDVR is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License 
// along with HDVR. If not, see <https://www.gnu.org/licenses/>.
//

#pragma once

#include <iostream>
#include <string>
#include <filesystem>
#include <sstream>
#include <exception>
#include <random>

#include "Types.h"

namespace hype {
    template<typename... Args>
    std::string concat(Args... args) {
        std::stringstream stream;
        (stream << ... << std::forward<Args>(args));
        return stream.str();
    }

    template<typename T, typename... Args>
    void log_error_nl(T arg, Args... args) {
        std::cerr << concat(arg, args...) << std::endl;
    }

    template<typename T, typename... Args>
    void log_info_nl(T arg, Args... args) {
        std::cout << concat(arg, args...) << std::endl;
    }

    template<typename T, typename... Args>
    std::ostream &log_error(T arg, Args... args) {
        std::cerr << concat(arg, args...);
        return std::cerr;
    }

    template<typename T, typename... Args>
    std::ostream &log_info(T arg, Args... args) {
        std::cout << concat(arg, args...) << std::flush;
        return std::cout;
    }

    template<typename T, typename... Args>
    std::runtime_error error(T arg, Args... args) {
        return std::runtime_error(concat(arg, args...));
    }

    bool is_file(const std::string &path);

    std::string read_file_directly(const std::string &path);

    void save_file_directly(const std::string &path, const std::string &data);


    template<typename T>
    std::vector<T> read_file(const std::string &path) {
        std::stringstream stream(hype::read_file_directly(path));
        std::string tmp;
        std::vector<T> result;
        while (std::getline(stream, tmp, '\n')) {
            if constexpr (std::is_same_v<int, T>) {
                result.emplace_back(std::stoi(tmp));
            } else {
                result.emplace_back(T{tmp});
            }
        }
        return result;
    }

    template<typename T>
    void save_file(const std::string &path, const std::vector<T> &data) {
        std::stringstream ss;
        for (const auto &element: data) {
            ss << element << "\n";
        }
        save_file_directly(path, ss.str());
    }

    std::ostream &operator<<(std::ostream &os, BundlingAction action);

    std::ostream &operator<<(std::ostream &os, SeedingStrategy action);
} // namespace hype
