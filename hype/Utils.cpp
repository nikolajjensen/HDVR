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

#include "Utils.h"
#include <fstream>

namespace hype {
    bool is_file(const std::string &path) {
        return std::filesystem::exists(path);
    }

    std::string read_file_directly(const std::string &path) {
        if (!is_file(path)) {
            throw error("The path '", path, "' does not lead to a file.");
        }

        std::ifstream file(path);
        if (!file.is_open()) {
            throw error("Could not open file at path: ", path);
        } else {
            std::stringstream ss;
            ss << file.rdbuf();
            file.close();

            return ss.str();
        }
    }

    void save_file_directly(const std::string &path, const std::string &data) {
        std::ofstream file(path);
        if (!file.is_open()) {
            throw error("Could not open file at path: ", path);
        } else {
            file << data;
            file.close();
        }
    }

    std::ostream &operator<<(std::ostream &os, BundlingAction action) {
        switch (action) {
            case DEFAULT:
                os << "DEFAULT";
                break;
            case MAJORITY:
                os << "MAJORITY";
                break;
        }
        return os;
    }

    std::ostream &operator<<(std::ostream &os, SeedingStrategy seedingStrategy) {
        switch (seedingStrategy) {
            case NONE:
                os << "NONE";
                break;
            case POLAR:
                os << "POLAR";
                break;
            case BINARY:
                os << "BINARY";
                break;
            case RANDOM:
                os << "RANDOM";
                break;
        }
        return os;
    }
} // namespace hype