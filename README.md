# CppStreamAdapters

[![C++23](https://img.shields.io/badge/C%2B%2B-23-blue.svg)](https://en.cppreference.com/w/cpp/23)
[![CMake](https://img.shields.io/badge/build-CMake-brightgreen.svg)](https://cmake.org/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

A header-only modern C++ (C++23) stream processing and pipeline library. It enables composable, lazy data processing pipelines using pipe syntax (`|`), inspired by C++20 Ranges and Unix pipelines.

---

## Example: Word Frequency in Files

Processing all text files in a directory and computing word frequencies with constant memory overhead per stage:

```cpp
#include <processing.h>
#include <iostream>
#include <filesystem>
#include <format>

int main() {
    Dir("./documents", /*recursive=*/true)
        | Filter([](const std::filesystem::path& p) { return p.extension() == ".txt"; })
        | OpenFiles()
        | Split("\n ,.;!?")
        | Transform([](std::string token) {
              std::transform(token.begin(), token.end(), token.begin(), ::tolower);
              return token;
          })
        | AggregateByKey(
              0uz,
              [](const std::string&, size_t& count) { ++count; },
              [](const std::string& token) { return token; }
          )
        | Transform([](const auto& stat) {
              return std::format("{}: {}\n", stat.first, stat.second);
          })
        | Out(std::cout);

    return 0;
}
```

---

## Features & Available Adapters

### Stream Sources
- **`Dir(path, recursive)`**: Streams filesystem paths in a directory.
- **`OpenFiles()`**: Opens file streams for incoming paths lazily.
- **`AsDataFlow(container)`**: Converts any standard STL container into a lazy stream.

### Transformations & Filters (Lazy, $\mathcal{O}(1)$ Memory)
- **`Transform(fn)`**: Lazily applies a unary transformation function to each stream element.
- **`Filter(pred)`**: Filters elements matching a boolean predicate.
- **`Split(delimiters)`**: Tokenizes incoming string streams across custom delimiters.
- **`DropNullopt()`**: Unwraps and filters a stream of `std::optional<T>`, dropping `std::nullopt`.
- **`SplitExpected()`**: Routes `std::expected<T, E>` into success and failure streams.

### Joins & Aggregations
- **`Join(other_stream)`**: SQL-style key-value join between two data streams.
- **`AggregateByKey(init_val, aggregator_fn, key_selector)`**: Groups and accumulates values by key (hash-map based aggregation).

### Sinks & Output
- **`Out(std::ostream&)`**: Streams elements into a standard output stream.
- **`Write(std::ostream&, delimiter)`**: Writes elements with a custom separator.
- **`AsVector()`**: Materializes the stream results into `std::vector<T>`.

---

## Memory Model

All adapters—except `Join` and `AggregateByKey` which inherently require group storage—operate **strictly in $\mathcal{O}(1)$ auxiliary memory**. Computation is evaluated lazily as elements are pulled through iterators.

---

## Build Instructions

### Prerequisites
- Modern C++ compiler with C++23 support (`gcc >= 13` or `clang >= 17`)
- CMake `>= 3.12`

### Building

```bash
# Clone the repository
git clone https://github.com/NikitaKkv/CppStreamAdapters.git
cd CppStreamAdapters

# Build demo and tests
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)

# Run demo
./build/bin/main
```

### Running Tests

```bash
ctest --test-dir build --output-on-failure
```

---

## License

This project is licensed under the MIT License.
