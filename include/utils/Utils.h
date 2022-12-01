#pragma once

#include <tuple>
#include <string>
#include <vector>
#include <functional>

#define VECTOR_CONTAINS(vec, val) (std::find(vec.begin(), vec.end(), val) != vec.end())
#define SUM_VECTORS(vec0, vec1) { for (const auto& b: vec0) { vec1.push_back(b); } }
#define EXTEND_VECTOR(vec0, size) { for (size_t i = 0; i < size; i++) vec0.push_back(0); }

#define offsetof(s, v) (uintptr_t) &((((struct s*)0)->v))
#define assert(c, msg, ...) { if (!(c)) { printf("Assertion error! (%s) : " msg, #c, __VA_ARGS__); exit(1); } }
#define assert_silent(c, msg, ...) { if (!(c)) { printf(msg, __VA_ARGS__); exit(1); } }
#define assert_silent_critical(c, msg, ...) { if (!(c)) { slx::io::debug_print(io::CRITICAL, msg, __VA_ARGS__); exit(1); } }

#define REPEAT_CHAR(chr, am) ((const std::string&)[&]{ std::string s; for(size_t i=0;i<(am);i++)s+=chr; return s; }())

namespace slx::utils {
    size_t levenstein(std::string_view const & a, std::string_view const & b);

    std::tuple<size_t, size_t> findClosest(
                std::vector<std::string> const & strs, std::string const & query);

    void writeImm8(std::vector<uint8_t>& data, uint8_t val, uint64_t address);
    void writeImm16(std::vector<uint8_t>& data, uint16_t val, uint64_t address);
    void writeImm32(std::vector<uint8_t>& data, uint32_t val, uint64_t address);
    void writeImm64(std::vector<uint8_t>& data, uint64_t val, uint64_t address);
}
