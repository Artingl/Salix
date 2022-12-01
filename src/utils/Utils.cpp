#include "utils/Utils.h"

namespace slx::utils {

    // https://stackoverflow.com/questions/70236962/how-do-i-find-the-closest-match-to-a-string-key-within-a-c-map

    size_t levenstein(std::string_view const & a, std::string_view const & b) {
        // https://en.wikipedia.org/wiki/Levenshtein_distance
        std::vector<size_t> d_((a.size() + 1) * (b.size() + 1), size_t(-1));
        auto d = [&](size_t ia, size_t ib) -> size_t & {
            return d_[ia * (b.size() + 1) + ib];
        };
        std::function<size_t(size_t, size_t)> LevensteinInt =
                [&](size_t ia, size_t ib) -> size_t {
                    if (d(ia, ib) != size_t(-1))
                        return d(ia, ib);
                    size_t dist = 0;
                    if (ib >= b.size())
                        dist = a.size() - ia;
                    else if (ia >= a.size())
                        dist = b.size() - ib;
                    else if (a[ia] == b[ib])
                        dist = LevensteinInt(ia + 1, ib + 1);
                    else
                        dist = 1 + std::min(
                                std::min(
                                        LevensteinInt(ia,     ib + 1),
                                        LevensteinInt(ia + 1, ib    )
                                ),  LevensteinInt(ia + 1, ib + 1)
                        );
                    d(ia, ib) = dist;
                    return dist;
                };
        return LevensteinInt(0, 0);
    }

    std::tuple<size_t, size_t> findClosest(
            std::vector<std::string> const & strs, std::string const & query) {
        size_t minv = size_t(-1), mini = size_t(-1);
        for (size_t i = 0; i < strs.size(); ++i) {
            size_t const dist = levenstein(strs[i], query);
            if (dist < minv) {
                minv = dist;
                mini = i;
            }
        }
        return std::make_tuple(mini, minv);
    }

    void writeImm8(std::vector<uint8_t>& data, uint8_t val, uint64_t address) {
        data[address] = val;
    }

    void writeImm16(std::vector<uint8_t>& data, uint16_t val, uint64_t address) {
        data[address + 0] = ((uint16_t)val >> 8) & 0xFF;
        data[address + 1] = ((uint16_t)val >> 0) & 0xFF;
    }

    void writeImm32(std::vector<uint8_t>& data, uint32_t val, uint64_t address) {
        data[address + 0] = ((uint32_t)val >> 24) & 0xFF;
        data[address + 1] = ((uint32_t)val >> 16) & 0xFF;
        data[address + 2] = ((uint32_t)val >> 8) & 0xFF;
        data[address + 3] = ((uint32_t)val >> 0) & 0xFF;
    }

    void writeImm64(std::vector<uint8_t>& data, uint64_t val, uint64_t address) {
        data[address + 0] = ((uint64_t)val >> 56) & 0xFF;
        data[address + 1] = ((uint64_t)val >> 48) & 0xFF;
        data[address + 2] = ((uint64_t)val >> 40) & 0xFF;
        data[address + 3] = ((uint64_t)val >> 32) & 0xFF;
        data[address + 4] = ((uint64_t)val >> 24) & 0xFF;
        data[address + 5] = ((uint64_t)val >> 16) & 0xFF;
        data[address + 6] = ((uint64_t)val >> 8) & 0xFF;
        data[address + 7] = ((uint64_t)val >> 0) & 0xFF;
    }
}
