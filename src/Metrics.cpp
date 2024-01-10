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

#include "Metrics.h"
#include "hype/Utils.h"

namespace hdvr {

    Metrics::Metrics(const std::string &header_) : header(header_) {}

    std::string Metrics::form_path(const std::string &stub, std::size_t it) {
        if (it == 0) {
            return stub;
        } else {
            return stub + std::to_string(it);
        }
    }

    void Metrics::save(const std::string &path, const std::string &name) {
        std::vector<std::string> result;

        std::stringstream ss;
        ss << "epoch,error,accuracy";
        result.emplace_back(ss.str());
        ss.str("");

        for (const auto &dp: data) {
            ss << dp.epoch << "," << dp.error << "," << dp.accuracy;
            result.emplace_back(ss.str());
            ss.str("");
        }

        result.emplace_back(header);

        std::size_t i = 0;
        std::string path_stub = path + "/./" + name;
        while (i < 100 && hype::is_file(form_path(path_stub, i) + ".csv")) {
            ++i;
        }

        hype::save_file(form_path(path_stub, i) + ".csv", result);
    }

    void Metrics::log(std::size_t epoch, float error, float accuracy) {
        data.emplace_back(Data{epoch, error, accuracy});
    }
} // namespace hdvr