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

#include <string>
#include <utility>
#include <set>

#include "hype/Utils.h"
#include "Types.h"

namespace hdvr {

    template<typename X, typename Y>
    class Dataset {
    private:

        template<typename F, typename S>
        class Iterator {
        public:
            Iterator(F it1_, S it2_) : it1(it1_), it2(it2_) {}

            std::pair<X, Y> operator*() const {
                return {*it1, *it2};
            }

            Iterator &operator++() {
                ++it1;
                ++it2;
                return *this;
            }

            bool operator!=(const Iterator &other) const {
                return it1 != other.it1 || it2 != other.it2;
            }

        private:
            F it1;
            S it2;
        };

        using iterator = Iterator<typename std::vector<X>::iterator, typename std::vector<Y>::iterator>;
        using const_iterator = Iterator<typename std::vector<X>::const_iterator, typename std::vector<Y>::const_iterator>;

        static std::vector<X> decode(const std::vector<std::string> &source) {
            std::vector<X> result;

            for (const auto &str: source) {
                result.emplace_back(X(str));
            }

            return result;
        }

    public:
        Dataset() = default;

        Dataset(const std::string &data_path, const std::string &labels_path) {
            load(data_path, labels_path);
        }

        void load(const std::string &data_path, const std::string &labels_path) {
            data = hype::read_file<X>(data_path);
            labels = hype::read_file<Y>(labels_path);

            if (data.size() != labels.size()) {
                throw hype::error("Mismatching amount of data (", data.size(), ") and labels (", labels.size(), ").");
            } else if (data.empty()) {
                throw hype::error("Loaded dataset at ", data_path, " but it is empty.");
            }
        }

        void save(const std::string &data_path, const std::string &labels_path) {
            hype::save_file<X>(data_path, data);
            hype::save_file<Y>(labels_path, labels);
        }

        iterator begin() {
            return {data.begin(), labels.begin()};
        }

        iterator end() {
            return {data.end(), labels.end()};
        }

        const_iterator begin() const {
            return {data.begin(), labels.begin()};
        }

        const_iterator end() const {
            return {data.end(), labels.end()};
        }

        void add(const std::pair<X, Y> &&to_add) {
            data.emplace_back(to_add.first);
            labels.emplace_back(to_add.second);
        }

        const std::pair<X, Y> operator[](std::size_t i) const {
            return {data[i], labels[i]};
        }

        std::size_t size() const {
            return data.size();
        }

        std::set<Y> class_set() const {
            return std::set<Y>{labels.begin(), labels.end()};
        }

    private:
        std::vector<X> data;
        std::vector<Y> labels;
    };

} // namespace hdvr