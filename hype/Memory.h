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

#include "Utils.h"

#include <vector>

namespace hype {

    template<typename T>
    class Memory {
    public:
        Memory() = default;

        explicit Memory(const std::string &path) {
            load(path);
        }

        explicit Memory(const std::vector<T> &data_) : data(data_) {}

        void load(const std::string &path) {
            data = read_file<T>(path);
        }

        void save(const std::string &path) {
            save_file<T>(path, data);
        }

        T &operator[](std::size_t i) {
            return data.at(i);
        }

        const T &operator[](std::size_t i) const {
            return data.at(i);
        }

        [[nodiscard]]
        std::size_t size() const {
            return data.size();
        }

        auto begin() const {
            return data.begin();
        }

        auto end() const {
            return data.end();
        }

        void clear() {
            data.clear();
        }

    protected:
        std::vector<T> data;
    };

} // namespace hype
