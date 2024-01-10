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

#include <cstddef>
#include <random>

namespace hype {
    template<std::size_t D, typename T, typename R, typename CR>
    class IVector {
    protected:
        std::mt19937 random_source;

    public:
        IVector() : random_source(std::random_device{}()) {}

        virtual std::size_t size() const = 0;

        virtual R operator[](std::size_t index) = 0;

        virtual CR operator[](std::size_t index) const = 0;

        virtual float distance(const IVector<D, T, R, CR> &other) const = 0;

        virtual IVector<D, T, R, CR> &invert() = 0;

        virtual IVector<D, T, R, CR> &invert(int start, int end) = 0;
    };
} // namespace hype