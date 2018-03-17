#include <benchmark/benchmark.h>
#include <iostream>
#include <random>
#include <iomanip>
#include <limits>
#include "tree.h"

static Tree<uint64_t, uint64_t> buildTree(std::size_t size)
{
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<uint64_t> dis(
      std::numeric_limits<uint64_t>::min(),
      std::numeric_limits<uint64_t>::max());

  Tree<uint64_t, uint64_t> tree;
  while (tree.size() < size) {
    const uint64_t key = dis(gen);
    tree = tree.insert(key, key);
  }

  return tree;
}

static void BM_TreeInsert(benchmark::State& state) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<uint64_t> dis(
      std::numeric_limits<uint64_t>::min(),
      std::numeric_limits<uint64_t>::max());

  const int size = state.range(0);
  for (auto _ : state) {
    state.PauseTiming();
    const auto tree = buildTree(size);
    state.ResumeTiming();
    for (int i = 0; i < size; i++) {
      const uint64_t key = dis(gen);
      benchmark::DoNotOptimize(tree.insert(key, key));
    }
  }
  state.SetItemsProcessed(state.iterations() * size);
}
BENCHMARK(BM_TreeInsert)->Range(1<<10, 1<<20);

BENCHMARK_MAIN();
