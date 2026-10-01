#include <iostream>
#include <format>

#include <processing.h>



int main(int argc, char **argv) {
    std::vector<std::string> input = {"name4", "name0", "name1", "name0", "name2", "name0", "name1"};

    auto result =
        AsDataFlow(input)
            | AggregateByKey(
                std::size_t{0},
                [](const std::string&, std::size_t& accumulated) { ++accumulated; },
                [](const std::string& token) { return token; }
            );

	for (auto i : result) {
		std::cout << i.first << " " << i.second << "\n";
	}	
  
  return 0;
}
