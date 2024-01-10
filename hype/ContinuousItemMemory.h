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
    class ContinuousItemMemory : public Memory<T> {
    public:
        using Memory<T>::Memory;

        ContinuousItemMemory(int size, int dimensions, SeedingStrategy seedingStrategy) {
            int start = 0;
            int end = start;
            int chunk = dimensions / (size - 1);

            if (chunk <= 1) {
                log_error_nl("WARNING: Continuous memory vector generation failed because the calculated chunk size (",
                             chunk, ") causes no bit-flips. Dimensions: ", dimensions, ", and size: ", size, ".");
            }

            for (std::size_t i = 0; i < size; ++i) {
                if (i == 0) {
                    this->data.emplace_back(T(seedingStrategy));
                } else {
                    T new_vector(this->data.at(i - 1));
                    end += chunk;
                    new_vector.invert(start, end);
                    start = end;
                    this->data.emplace_back(new_vector);
                }
            }
        }
    };

} // namespace hype
