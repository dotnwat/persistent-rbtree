#include <condition_variable>
#include <iomanip>
#include <iostream>
#include <limits>
#include <mutex>
#include <random>
#include <benchmark/benchmark.h>
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
    // thread 0 builds the tree
    if (state.thread_index == 0) {
      std::lock_guard<std::mutex> lk(lock);

      tree_size = state.range(0);
      num_inserts = state.range(1);

      tree = buildTree(r, tree_size);

      // notify other threads
      init_complete = true;
      cond.notify_all();
    }

    // all threads wait until the tree is built
    std::unique_lock<std::mutex> lk(lock);
    cond.wait(lk, [&] { return init_complete; });
    lk.unlock();
  }

  int tree_size;
  int num_inserts;

  rng r;
  Tree<uint64_t, uint64_t> tree;

  std::condition_variable cond;
  bool init_complete = false;
  std::mutex lock;
};

BENCHMARK_DEFINE_F(InsertFixture, UniformInt)(benchmark::State& state)
{
  assert(tree_size > 0);
  assert(tree.size() == tree_size);

  // generate set of keys to insert
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

BENCHMARK_REGISTER_F(InsertFixture, UniformInt)
  ->RangeMultiplier(10)
  ->Ranges({{1, 1000}, {10000, 10000}})
  ->ThreadRange(1, 2)
  ->UseRealTime();

BENCHMARK_MAIN();
