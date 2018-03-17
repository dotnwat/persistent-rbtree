all: test test_cov

test: test.cc tree.h
	$(CXX) -Wall -Wextra -Werror -O3 -std=c++17 -o $@ $<

test_cov: test.cc tree.h
	$(CXX) -Wall --coverage -fno-inline -fno-exceptions \
	  -fprofile-arcs -ftest-coverage -fPIC -O0 -Wextra -g \
	  -std=c++17 -o $@ $<
