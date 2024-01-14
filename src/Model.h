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

#include "Types.h"

#include "hype/ContinuousItemMemory.h"
#include "hype/AssociativeMemory.h"
#include "hype/FrequencyChannelMemory.h"


namespace hdvr {

    template<std::size_t L, std::size_t D, std::size_t F, hype::SeedingStrategy S>
    class Model {
    public:
        Model() : continuousItemMemory(L, D, S), frequencyChannelMemory(F, D, S) {}

        bool load(const std::string &path) {
            try {
                associativeMemory.load(path + "/./associative_memory.mem");
                continuousItemMemory.load(path + "/./continuous_memory.mem");
                frequencyChannelMemory.load(path + "/./level_memory.mem");
                return true;
            } catch (std::runtime_error e) {
                *this = Model<L, D, F, S>();
                return false;
            }
        }

        bool save(const std::string &path) {
            try {
                associativeMemory.save(path + "/./associative_memory.mem");
                continuousItemMemory.save(path + "/./continuous_memory.mem");
                frequencyChannelMemory.save(path + "/./level_memory.mem");
                return true;
            } catch (std::runtime_error &e) {
                hype::log_error_nl("Failed to save model: ", e.what());
                return false;
            }
        }

        bool blank() const {
            return associativeMemory.size() == 0 && continuousItemMemory.size() == 0 && frequencyChannelMemory.size() == 0;
        }

    public:
        hype::AssociativeMemory<Vect<D>> associativeMemory;
        hype::ContinuousItemMemory<Vect<D>> continuousItemMemory;
        hype::FrequencyChannelMemory<Vect<D>> frequencyChannelMemory;
    };

} // namespace hdvr