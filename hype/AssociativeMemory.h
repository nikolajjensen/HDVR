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

#include "Memory.h"

namespace hype {

    template<typename T>
    class AssociativeMemory : public Memory<T> {
    public:
        void insert(const T &&data) {
            this->data.emplace_back(std::move(data));
        }

        std::size_t find(const T &query) const {
            if (this->size() == 0) {
                throw error("Failed to find query in empty associative memory.");
            }

            std::size_t index = 0;
            float min_distance = std::numeric_limits<float>::max();

            for (std::size_t i = 0; i < this->size(); ++i) {
                float tmp_distance = query.distance(this->data[i]);
                if (tmp_distance < min_distance) {
                    index = i;
                    min_distance = tmp_distance;
                }
            }

            return index;
        }
    };

} // namespace hype
