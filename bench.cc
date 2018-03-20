#include <benchmark/benchmark.h>
#include <iostream>
#include <random>
#include <iomanip>
#include <limits>
#include <mutex>
#include <condition_variable>
#include "tree.h"

struct rng {
  rng() :
    gen(rd()),
    dis(std::numeric_limits<uint64_t>::min(),
        std::numeric_limits<uint64_t>::max())
  {}

  inline auto next() {
    std::lock_guard<std::mutex> lk(lock);
    return dis(gen);
  }

  std::random_device rd;
  std::mt19937 gen;
  std::uniform_int_distribution<uint64_t> dis;
  std::mutex lock;
};

static auto buildTree(rng& r, std::size_t size)
{
  Tree<uint64_t, uint64_t> tree;
  while (tree.size() < size) {
    const uint64_t key = r.next();
    tree = tree.insert(key, key);
  }
  return tree;
}

class InsertFixture : public ::benchmark::Fixture {
 public:
  void SetUp(const ::benchmark::State& state) {
    if (state.thread_index == 0) {
      const int tree_size = state.range(0);
      tree = buildTree(r, tree_size);

      std::lock_guard<std::mutex> lk(lock);
      init_complete = true;
      cond.notify_all();
    }
  }

  rng r;
  Tree<uint64_t, uint64_t> tree;

  std::condition_variable cond;
  bool init_complete = false;
  std::mutex lock;
};

BENCHMARK_DEFINE_F(InsertFixture, Test)(benchmark::State& state)
{
  // wait for the tree to be built
  {
    std::unique_lock<std::mutex> lk(lock);
    cond.wait(lk, [&] { return init_complete; });
    lk.unlock();

    const int tree_size = state.range(0);
    assert(tree.size() == tree_size);
  }

  // generate set of keys to insert
  const int num_inserts = state.range(1);
  std::vector<uint64_t> keys;
  keys.reserve(num_inserts);
  while (keys.size() < num_inserts) {
    const auto key = r.next();
    if (!tree.get(key)) {
      keys.emplace_back(key);
    }
  }

  for (auto _ : state) {
    for (const auto& key : keys) {
      benchmark::DoNotOptimize(tree.insert(key, key));
    }
  }

  state.SetItemsProcessed(state.iterations() * keys.size());
}

BENCHMARK_REGISTER_F(InsertFixture, Test)
  ->RangeMultiplier(2)
  ->Ranges({{1, 128}, {512, 512}});
  //->ThreadRange(1, 2);

BENCHMARK_MAIN();
