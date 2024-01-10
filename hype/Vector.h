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

#include "IVector.h"
#include "Utils.h"

#include <random>

namespace hype {

    template<std::size_t D, typename T>
    class Vector : public IVector<D, T, T &, const T &> {
    private:
        using Vector_t = IVector<D, T, T &, const T &>;

        void seed(SeedingStrategy seedingStrategy) {
            std::function<T()> generator = nullptr;

            switch (seedingStrategy) {
                case RANDOM:
                    generator = [&]() {
                        std::uniform_int_distribution<> distribution(std::numeric_limits<T>::min(),
                                                                     std::numeric_limits<T>::max());
                        return distribution(this->random_source);
                    };
                    break;
                case POLAR:
                    generator = [&]() {
                        std::uniform_int_distribution<> distribution(0, 1);
                        return distribution(this->random_source) ? 1 : -1;
                    };
                    break;
                case BINARY:
                    generator = [&]() {
                        std::uniform_int_distribution<> distribution(0, 1);
                        return distribution(this->random_source);
                    };
                    break;
                default:
                    break;
            }

            if (generator != nullptr) {
                std::generate(data.begin(), data.end(), generator);
            }
        }

        static std::array<T, D> decode(const std::string &str) {
            std::stringstream stream(str);
            std::string tmp;
            std::array<T, D> result;
            std::size_t i = 0;
            while (std::getline(stream, tmp, ',')) {
                if constexpr (std::is_same<int, T>::value) {
                    result[i] = std::stoi(tmp);
                } else if constexpr (std::is_same<float, T>::value) {
                    result[i] = std::stof(tmp);
                } else {
                    result[i] = T{tmp};
                }
                ++i;
            }

            if (i != D) {
                log_info_nl("Length mismatch. Decoding string '", str, "' which contains ", i, " elements where ", D,
                            " was expected.");
            }

            return result;
        }

    public:
        explicit Vector(SeedingStrategy seedingStrategy = NONE) {
            seed(seedingStrategy);
        }

        explicit Vector(std::string _data) : data(decode(_data)) {}

        explicit Vector(const std::vector<T> &_data) : data(_data) {}

        Vector(const std::initializer_list<T> &_data) : data(_data) {}

        Vector(const Vector<D, T> &other) : data(other.data) {}

        [[nodiscard]]
        std::size_t size() const override {
            return data.size();
        }

        T &operator[](std::size_t index) override {
            return data[index];
        }

        const T &operator[](std::size_t index) const override {
            return data[index];
        }

        float distance(const Vector_t &other) const override {
            // Inspired by: https://www.simonwenkel.com/notes/ai/metrics/cosine_distance.html
            double a_dot_b = 0.0;
            double a_mag = 0;
            double b_mag = 0;
            for (size_t i = 0; i < other.size(); ++i) {
                a_dot_b += (data[i] * other[i]);
                a_mag += (data[i] * data[i]);
                b_mag += (other[i] * other[i]);
            }
            return 1.0 - (a_dot_b / (std::sqrt(a_mag) * std::sqrt(b_mag)));
        }

        Vector<D, T> &invert() override {
            return invert(0, size());
        }

        Vector<D, T> &invert(int start, int end) override {
            if (start >= end) {
                throw error("Inverting vector failed because start >= end (", start, " >= ", end, ").");
            }
            for (int i = start; i < end; ++i) {
                data[i] = -1 * data[i];
            }
            return *this;
        }

        friend std::ostream &operator<<(std::ostream &os, const Vector<D, T> &v) {
            for (std::size_t i = 0; i < v.data.size(); ++i) {
                os << v.data[i];
                if (i != v.data.size() - 1) {
                    os << ',';
                }
            }
            return os;
        }

        friend Vector<D, T> invert(const Vector<D, T> &vector) {
            return Vector<D, T>(vector).invert();
        }

        friend Vector<D, T> add(const std::vector<Vector<D, T>> &vectors, float dropout) {
            Vector<D, T> result(vectors[0]);
            std::bernoulli_distribution distribution(dropout);

            for (int i = 0; i < result.size(); ++i) {
                if (distribution(result.random_source)) {
                    continue;
                }
                for (int vi = 1; vi < vectors.size(); ++vi) {
                    result.data[i] += vectors[vi].data[i];
                }
            }
            return result;
        }

        friend Vector<D, T> add(const std::vector<Vector<D, T>> &vectors) {
            Vector<D, T> result(vectors[0]);
            for (int i = 0; i < result.size(); ++i) {
                for (int vi = 1; vi < vectors.size(); ++vi) {
                    result.data[i] += vectors[vi].data[i];
                }
            }
            return result;
        }

        friend Vector<D, T> add(const Vector<D, T> &one, const Vector<D, T> &two, float dropout) {
            Vector<D, T> result(one);
            std::bernoulli_distribution distribution(dropout);

            for (int i = 0; i < result.size(); ++i) {
                if (distribution(result.random_source)) {
                    continue;
                }
                result.data[i] = result.data[i] + two.data[i];
            }
            return result;
        }

        friend Vector<D, T> add(const Vector<D, T> &one, const Vector<D, T> &two) {
            Vector<D, T> result(one);
            for (int i = 0; i < result.size(); ++i) {
                result.data[i] = result.data[i] + two.data[i];
            }
            return result;
        }

        friend Vector<D, T> sub(const std::vector<Vector<D, T>> &vectors, float dropout) {
            Vector<D, T> result(vectors[0]);
            std::bernoulli_distribution distribution(dropout);

            for (int i = 0; i < result.size(); ++i) {
                if (distribution(result.random_source)) {
                    continue;
                }
                for (int vi = 1; vi < vectors.size(); ++vi) {
                    result.data[i] -= vectors[vi].data[i];
                }
            }
            return result;
        }

        friend Vector<D, T> sub(const std::vector<Vector<D, T>> &vectors) {
            Vector<D, T> result(vectors[0]);

            for (int i = 0; i < result.size(); ++i) {
                for (int vi = 1; vi < vectors.size(); ++vi) {
                    result.data[i] -= vectors[vi].data[i];
                }
            }
            return result;
        }

        friend Vector<D, T> sub(const Vector<D, T> &one, const Vector<D, T> &two, float dropout) {
            Vector<D, T> result(one);
            std::bernoulli_distribution distribution(dropout);

            for (int i = 0; i < result.size(); ++i) {
                if (distribution(result.random_source)) {
                    continue;
                }
                result.data[i] = result.data[i] - two.data[i];
            }
            return result;
        }

        friend Vector<D, T> sub(const Vector<D, T> &one, const Vector<D, T> &two) {
            Vector<D, T> result(one);
            for (int i = 0; i < result.size(); ++i) {
                result.data[i] = result.data[i] - two.data[i];
            }
            return result;
        }

        friend Vector<D, T> mul(const std::vector<Vector<D, T>> &vectors) {
            Vector<D, T> result(vectors[0]);
            for (int i = 0; i < result.size(); ++i) {
                for (int vi = 1; vi < vectors.size(); ++vi) {
                    result.data[i] *= vectors[vi].data[i];
                }
            }
            return result;
        }

        friend Vector<D, T> mul(const Vector<D, T> &one, const Vector<D, T> &two) {
            Vector<D, T> result(one);
            for (std::size_t i = 0; i < result.size(); ++i) {
                result.data[i] = result.data[i] * two.data[i];
            }
            return result;
        }

    private:
        std::array<T, D> data;
    };

    template<std::size_t D>
    class BinaryVector
            : public IVector<D, bool, typename std::bitset<D>::reference, typename std::bitset<D>::const_reference> {
    private:
        using BinaryVector_t = IVector<D, bool, typename std::bitset<D>::reference, typename std::bitset<D>::const_reference>;

        void seed(SeedingStrategy seedingStrategy) {
            if (seedingStrategy == NONE) return;
            if (seedingStrategy != BINARY) {
                throw error("Cannot seed BinaryVector with strategy ", seedingStrategy);
            }

            std::bernoulli_distribution distribution(0.5);
            for (std::size_t i = 0; i < D; ++i) {
                data[i] = distribution(this->random_source);
            }
        }

        // https://stackoverflow.com/a/18311086/15372019
        static std::string hex_char_to_bin(char c) {
            // TODO handle default / error
            switch (toupper(c)) {
                case '0':
                    return "0000";
                case '1':
                    return "0001";
                case '2':
                    return "0010";
                case '3':
                    return "0011";
                case '4':
                    return "0100";
                case '5':
                    return "0101";
                case '6':
                    return "0110";
                case '7':
                    return "0111";
                case '8':
                    return "1000";
                case '9':
                    return "1001";
                case 'A':
                    return "1010";
                case 'B':
                    return "1011";
                case 'C':
                    return "1100";
                case 'D':
                    return "1101";
                case 'E':
                    return "1110";
                case 'F':
                    return "1111";
                default:
                    throw error("Unexpected character encountered which is not a recognized hex character: ", c);
            }
        }

        static std::bitset<D> decode(const std::string &str) {
            if (str.size() * 4 != D) {
                throw error("Error converting vector to bitset: string is of length ", str.size(), " in hex (",
                            str.size() * 4, " in binary) but expected ", D);
            }

            std::bitset<D> result;
            std::string reversed_str(str.rbegin(), str.rend());
            int offset = 0;
            for (const auto &c: reversed_str) {
                std::string tmp = hex_char_to_bin(c);
                for (std::size_t i = 0; i < tmp.size(); ++i) {
                    result[offset + i] = tmp[(tmp.size() - 1) - i] == '1';
                }
                offset += tmp.size();
            }
            return result;
        }

        static std::bitset<D> to_bitset(const std::vector<bool> &src) {
            if (src.size() != D) {
                throw error("Error converting vector to bitset: vector is of length ", src.size(), " but expected ", D);
            }

            std::bitset<D> result;
            for (std::size_t i = 0; i < src.size(); ++i) {
                result[i] = src[i];
            }
            return result;
        }

    public:
        explicit BinaryVector(SeedingStrategy seedingStrategy = NONE) {
            seed(seedingStrategy);
        }

        explicit BinaryVector(const std::string &_data) : data(decode(_data)) {}

        explicit BinaryVector(const char *_data) : data(decode(std::string(_data))) {}

        explicit BinaryVector(const std::vector<bool> &_data) : data(to_bitset(_data)) {}

        BinaryVector(const std::initializer_list<bool> &_data) : data(to_bitset(_data)) {}

        [[nodiscard]]
        std::size_t size() const override {
            return data.size();
        }

        typename std::bitset<D>::reference operator[](std::size_t index) override {
            return data[index];
        }

        typename std::bitset<D>::const_reference operator[](std::size_t index) const override {
            return data[index];
        }

        float distance(const BinaryVector_t &other) const override {
            int diffs = 0;
            for (std::size_t i = 0; i < size(); ++i) {
                diffs += (data[i] != other[i]);
            }
            return static_cast<float>(diffs) / size();
        }

        BinaryVector &invert() override {
            return invert(0, size());
        }

        BinaryVector &invert(int start, int end) override {
            if (start >= end) {
                throw error("Inverting vector failed because start >= end (", start, " >= ", end, ").");
            }
            for (int i = start; i < end; ++i) {
                data.flip(i);
            }
            return *this;
        }

        friend std::ostream &operator<<(std::ostream &os, const BinaryVector &v) {
            // Inspired from: https://stackoverflow.com/a/29217939/15372019
            static const char rets[] = "0123456789abcdef";
            std::string str = v.data.to_string();

            if (str.length() % 4 != 0) {
                throw error("Length must be a multiple of 4");
            }

            auto b = str.begin();
            auto e = str.end();
            while (b != e) {
                int index =
                        ((*(b + 0) - '0') << 3) |
                        ((*(b + 1) - '0') << 2) |
                        ((*(b + 2) - '0') << 1) |
                        ((*(b + 3) - '0') << 0);
                os << rets[index];
                b += 4;
            }

            return os;
        }

        friend BinaryVector<D> invert(const BinaryVector<D> &vector) {
            return BinaryVector<D>(vector).invert();
        }

        friend BinaryVector<D> add(const std::vector<BinaryVector<D>> &vectors) {
            BinaryVector<D> result(NONE);
            int cutoff = vectors.size() / 2;

            for (int i = 0; i < result.size(); ++i) {
                int count = 0;
                for (int vi = 0; vi < vectors.size(); ++vi) {
                    count += vectors[vi].data[i];
                }
                result.data[i] = count > cutoff;
            }

            return result;
        }

        friend BinaryVector<D> add(const BinaryVector<D> &one, const BinaryVector<D> &two) {
            BinaryVector<D> result(NONE);
            result.data = one.data | two.data;
            return result;
        }

        BinaryVector<D> sub(const std::vector<BinaryVector<D>> &vectors) {
            BinaryVector<D> result(NONE);
            for (auto const &vector: vectors) {
                result.data = result.data & ~vector.data;
            }
            return result;
        }

        friend BinaryVector<D> sub(const BinaryVector<D> &one, const BinaryVector<D> &two) {
            BinaryVector<D> result(NONE);
            result.data = one.data & ~two.data;
            return result;
        }

        friend BinaryVector<D> mul(const std::vector<BinaryVector<D>> &vectors) {
            BinaryVector<D> result(NONE);
            for (auto const &vector: vectors) {
                result.data = result.data ^ vector.data;
            }
            return result;
        }

        friend BinaryVector<D> mul(const BinaryVector<D> &one, const BinaryVector<D> &two) {
            BinaryVector<D> result(NONE);
            result.data = one.data ^ two.data;
            return result;
        }

    private:
        std::bitset<D> data;
    };
} // namespace hype