#include "tree.h"
#include <map>
#include <cassert>
#include <sstream>
#include <list>
#include <iomanip>
#include <random>

struct tree_pair {
  Tree tree;
  std::map<std::string, std::string> truth;
};

static inline std::string tostr(uint32_t value)
{
  std::stringstream ss;
  ss << std::setw(10) << std::setfill('0') << value;
  return ss.str();
}

static void verify_history(uint32_t coin_toss)
{
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<uint32_t> dis(0, 50000);
  std::uniform_int_distribution<uint32_t> coin(0, 100);

  // snapshot history
  std::list<tree_pair> trees;

  // on-going state
  Tree tree;
  std::map<std::string, std::string> truth;

  // build a bunch of snapshots
  for (int i = 0; i < 1000; i++) {
    for (int j = 0; j < 100; j++) {
      const std::string key = tostr(dis(gen));
      if (coin(gen) < coin_toss) {
        tree = tree.insert(key, key);
        truth.emplace(key, key);
      } else {
        tree = tree.remove(key);
        truth.erase(key);
      }
    }
    trees.emplace_back();
    trees.back().tree = tree;
    trees.back().truth = truth;
  }

  // verify all snapshots
  for (const auto& tree : trees) {
    assert(tree.tree.items() == tree.truth);
    assert(tree.tree.consistent());
  }
}

int main()
{
  verify_history(25);
  verify_history(50);
  verify_history(75);
  verify_history(100);
}
