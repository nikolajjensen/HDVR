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

#include "Model.h"
#include "Dataset.h"
#include "Types.h"
#include "Metrics.h"

#include <chrono>

namespace hdvr {

#define MAX_FREQUENCY ((data_t)1.0)
#define MIN_FREQUENCY ((data_t)-1.0)
#define PROGRESS_UPDATES 10


    template<std::size_t L, std::size_t D, std::size_t F, hype::SeedingStrategy S>
    class HDVR {
    private:
        int frequency_bin(const data_t &frequency, int bin_levels) {
            if (frequency < MIN_FREQUENCY || frequency > MAX_FREQUENCY) {
                throw hype::error("Frequency of ", frequency, " is outside expected range of [", MIN_FREQUENCY, ", ",
                                  MAX_FREQUENCY, "]");
            }

            data_t range = MAX_FREQUENCY - MIN_FREQUENCY;
            data_t step = range / bin_levels;

            for (int i = 0; i < bin_levels; ++i) {
                data_t threshold = MIN_FREQUENCY + (step * (i + 1));
                if (frequency <= threshold) {
                    return i;
                }
            }

            return bin_levels - 1;
        }

        Vect<D> encode(const hype::Vector<F, data_t> &data_point) {
            std::vector<Vect<D>> temp;
            temp.reserve(data_point.size());

            int bin;
            for (std::size_t i = 0; i < data_point.size(); ++i) {
                bin = frequency_bin(data_point[i], model.continuousItemMemory.size());
                auto bin_vec = model.frequencyChannelMemory[i];
                auto item_vec = model.continuousItemMemory[bin];
                auto result = mul(bin_vec, item_vec);
                temp.emplace_back(std::move(result));
            }

            return add(std::move(temp));
        }

        Dataset<Vect<D>, int> encode(const Dataset<hype::Vector<F, data_t>, int> &dataset) {
            Dataset<Vect<D>, int> result;
            int chunk_size = dataset.size() / PROGRESS_UPDATES;

            int i = 0;
            for (const auto &data_point: dataset) {
                ++i;
                if (i % chunk_size == 0) {
                    hype::log_info(" ", static_cast<int>(static_cast<float>(i) / dataset.size() * 100), "% ");
                }
                result.add({encode(data_point.first), data_point.second});
            }
            return result;
        }

        std::vector<std::vector<Vect<D>>> get_class_vectors(const Dataset<Vect<D>, int> &dataset) {
            std::vector<std::vector<Vect<D>>> class_vectors(dataset.class_set().size());
            for (const auto &data_point: dataset) {
                class_vectors.at(data_point.second).emplace_back(data_point.first);
            }
            return class_vectors;
        }

        void configure_memory(const Dataset<Vect<D>, int> &dataset, float dataset_fraction = 1.0) {
            model.associativeMemory.clear();
            auto class_vectors = get_class_vectors(dataset);
            for (const auto &vectors: class_vectors) {
                model.associativeMemory.insert(add(std::vector<Vect<D>>(vectors.begin(), vectors.begin() + (vectors.size() * dataset_fraction))));
            }
        }

        float train_one_epoch(const Dataset<Vect<D>, int> &dataset) {
            int wrongs = 0;
            int chunk_size = dataset.size() / PROGRESS_UPDATES;

            for (std::size_t i = 0; i < dataset.size(); ++i) {
                int prediction = predict(dataset[i].first);
                if (prediction != dataset[i].second) {
                    ++wrongs;
                    model.associativeMemory[prediction] = sub(model.associativeMemory[prediction], dataset[i].first);
                    model.associativeMemory[dataset[i].second] = add(model.associativeMemory[dataset[i].second],
                                                                     dataset[i].first);
                }

                if (i % chunk_size == 0) {
                    //hype::log_info_nl("Processed ", i + 1, " samples.");
                }
            }
            return static_cast<float>(wrongs) / static_cast<float>(dataset.size()) * 100.0;
        }

        float test(const Dataset<Vect<D>, int> &dataset) {
            int correct = 0;
            for (const auto &data_point: dataset) {
                int prediction = predict(data_point.first);
                if (prediction == data_point.second) {
                    ++correct;
                }
            }
            return static_cast<float>(correct) / static_cast<float>(dataset.size()) * 100.0;
        }

        int predict(const Vect<D> &input) {
            return model.associativeMemory.find(input);
        }

        bool trainable() {
            return train_dataset.size() > 0 && test_dataset.size() > 0;
        }

        std::pair<std::string, std::string>
        construct_dataset_paths(const std::string &stub, const std::string &prefix, const std::string &extension) {
            return {stub + "/./" + prefix + extension, stub + "/./" + prefix + "_labels" + extension};
        }

        std::pair<std::string, std::string>
        construct_valid_dataset_paths(const std::string &stub, const std::string &prefix,
                                      const std::string &extension) {
            auto paths = construct_dataset_paths(stub, prefix, extension);

            if (hype::is_file(paths.first) && hype::is_file(paths.second)) {
                return paths;
            }

            throw hype::error("Could not find a dataset in ", stub);
        }

        bool load_datasets(const std::string &dataset_path, const std::string &extension) {
            try {
                auto train_paths = construct_valid_dataset_paths(dataset_path, "train", extension);
                auto test_paths = construct_valid_dataset_paths(dataset_path, "test", extension);

                try {
                    if (extension == ".datmem") {
                        hype::log_info("Loading encoded datasets... ");
                        train_dataset.load(train_paths.first, train_paths.second);
                        test_dataset.load(test_paths.first, test_paths.second);
                        hype::log_info_nl("DONE");
                    } else {
                        hype::log_info("Loading raw datasets... ");
                        auto train_tmp = Dataset<hype::Vector<F, data_t>, int>(train_paths.first, train_paths.second);
                        auto test_tmp = Dataset<hype::Vector<F, data_t>, int>(test_paths.first, test_paths.second);
                        hype::log_info_nl("DONE");

                        hype::log_info("Encoding training data... ");
                        train_dataset = encode(train_tmp);
                        hype::log_info_nl("DONE");
                        hype::log_info("Encoding testing data... ");
                        test_dataset = encode(test_tmp);
                        hype::log_info_nl("DONE");
                    }
                    return true;
                } catch (std::runtime_error &e) {
                    hype::log_error_nl(e.what());
                    return false;
                }
            } catch (std::runtime_error &e) {
                return false;
            }
        }

    public:

        HDVR(Model<L, D, F, S> &model_) : model(model_) {}

        bool load_datasets(const std::string &dataset_path, float dataset_fraction = 1.0) {
            std::array<std::string, 2> extensions{".datmem", ".csv"};
            for (const auto &extension: extensions) {
                if (load_datasets(dataset_path, extension)) {
                    hype::log_info_nl("Loaded ", train_dataset.size(), " training samples, and ", test_dataset.size(), " testing samples.");
                    configure_memory(train_dataset, dataset_fraction);
                    return true;
                }
            }
            return false;
        }

        bool save_datasets(const std::string &dataset_path) {
            auto train_paths = construct_dataset_paths(dataset_path, "train", ".datmem");
            auto test_paths = construct_dataset_paths(dataset_path, "test", ".datmem");

            try {
                train_dataset.save(train_paths.first, train_paths.second);
                test_dataset.save(test_paths.first, test_paths.second);
                return true;
            } catch (std::runtime_error &e) {
                hype::log_error_nl("Failed to save dataset: ", e.what());
                return false;
            }
        }

        Metrics train(int epochs) {
            if (!trainable()) {
                throw hype::error("Could not train model. Did you forget to setup datasets?");
            }

            std::stringstream ss;
            ss << "\"" << "Training: epochs: " << epochs << ", levels: " << L << ", dimensions: " << D << ", frequency points: "
               << F << "\"";
            Metrics metrics(ss.str());

            hype::log_info_nl("Running test... ");
            float accuracy = test(test_dataset);
            hype::log_info_nl("Accuracy before training: ", accuracy, "%");
            metrics.log(0, 0, accuracy);

            for (int i = 0; i < epochs; ++i) {
                hype::log_info("[Epoch: ", i + 1, "]: ");
                float error = train_one_epoch(train_dataset);
                accuracy = test(test_dataset);
                hype::log_info_nl("error: ", error, "% – accuracy: ", accuracy, "%");
                metrics.log(i + 1, error, accuracy);
            }

            return metrics;
        }

    private:
        Model<L, D, F, S> &model;
        Dataset<Vect<D>, int> train_dataset;
        Dataset<Vect<D>, int> test_dataset;
    };

} // namespace hdvr