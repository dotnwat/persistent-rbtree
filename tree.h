/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#pragma once
#include <map>
#include <cassert>
#include <memory>
#include <stack>
#include <string>
#include <utility>
#include <cstddef>

struct Entry {
  Entry(const std::string& key, const std::string& value) :
    key(key),
    value(value)
  {}

  const std::string key;
  const std::string value;
};

// A node in the red-black tree that contains a key-value pair, and two child
// pointers. Every reference to a node is an immutable shared pointer. Changes
// are made to a node (e.g. changing its color) by modifying a copy of the node.
struct Node : std::enable_shared_from_this<const Node> {
 public:
  // Build a node
  Node(const bool red, const std::shared_ptr<const Entry>& entry,
      const std::shared_ptr<const Node>& left,
      const std::shared_ptr<const Node>& right) :
    red(red),
    entry(entry),
    left(left),
    right(right)
  {}

  // Build a node with no children
  Node(const bool red, const std::string& key, const std::string& value) :
    red(red),
    entry(std::make_shared<const Entry>(key, value))
  {}

  // Build copies of this node
 public:
  inline auto copyWithEntry(const std::string& key,
      const std::string& value) const {
    const auto new_entry = std::make_shared<const Entry>(key, value);
    return std::make_shared<const Node>(red, new_entry, left, right);
  }

  inline auto copyWithLeft(const std::shared_ptr<const Node> left) const {
    return std::make_shared<const Node>(red, entry, std::move(left), right);
  }

  inline auto copyWithRight(const std::shared_ptr<const Node> right) const {
    return std::make_shared<const Node>(red, entry, left, std::move(right));
  }

  inline auto copyAsBlack() const {
    return std::make_shared<const Node>(false, entry, left, right);
  }

  inline auto copyAsRed() const {
    return std::make_shared<const Node>(true, entry, left, right);
  }

 public:
  static std::pair<std::shared_ptr<const Node>, bool> insert(
      const std::shared_ptr<const Node>& node, const std::string& key,
      const std::string& value);

  std::shared_ptr<const Node> balance() const;

  static std::size_t checkConsistency(
      const std::shared_ptr<const Node>& node);

  // remove
 public:
  static std::shared_ptr<const Node> fuse(
      const std::shared_ptr<const Node>& left,
      const std::shared_ptr<const Node>& right);

  static std::shared_ptr<const Node> balance(
      const std::shared_ptr<const Node>& node);

  static std::shared_ptr<const Node> balance_left(
      const std::shared_ptr<const Node>& node);

  static std::shared_ptr<const Node> balance_right(
      const std::shared_ptr<const Node>& node);

  static std::pair<std::shared_ptr<const Node>, bool> remove_left(
      const std::shared_ptr<const Node>& node, const std::string& key);

  static std::pair<std::shared_ptr<const Node>, bool> remove_right(
      const std::shared_ptr<const Node>& node, const std::string& key);

  static std::pair<std::shared_ptr<const Node>, bool> remove(
      const std::shared_ptr<const Node>& node,
      const std::string& key);

 public:
  const bool red;
  const std::shared_ptr<const Entry> entry;
  const std::shared_ptr<const Node> left;
  const std::shared_ptr<const Node> right;
};

std::shared_ptr<const Node> Node::fuse(
    const std::shared_ptr<const Node>& left,
    const std::shared_ptr<const Node>& right)
{
  // match: (left, right)
  // case: (None, r)
  if (!left) {
    return right;
  // case: (l, None)
  } else if (!right) {
    return left;
  }
  // case: (Some(l), Some(r))
  // fall through
  assert(left && right);

  // match: (left.color, right.color)
  // case: (B, R)
  if (!left->red && right->red) {
    return std::make_shared<const Node>(
        true,
        right->entry,
        fuse(left, right->left),
        right->right);

  // case: (R, B)
  } else if (left->red && !right->red) {
    return std::make_shared<const Node>(
        true,
        left->entry,
        left->left,
        fuse(left->right, right));

  // case: (R, R)
  } else if (left->red && right->red) {
    const auto fused = fuse(left->right, right->left);
    if (fused && fused->red) {
      const auto new_left = std::make_shared<const Node>(
          true,
          left->entry,
          left->left,
          fused->left);

      const auto new_right = std::make_shared<const Node>(
          true,
          right->entry,
          fused->right,
          right->right);

      return std::make_shared<const Node>(
          true,
          fused->entry,
          new_left,
          new_right);
    }

    const auto new_right = std::make_shared<const Node>(
        true,
        right->entry,
        fused,
        right->right);

    return std::make_shared<const Node>(
        true,
        left->entry,
        left->left,
        new_right);

  // case: (B, B)
  } else if (!left->red && !right->red) {
    const auto fused = fuse(left->right, right->left);
    if (fused && fused->red) {
      const auto new_left = std::make_shared<const Node>(
          false,
          left->entry,
          left->left,
          fused->left);

      const auto new_right = std::make_shared<const Node>(
          false,
          right->entry,
          fused->right,
          right->right);

      return std::make_shared<const Node>(
          true,
          fused->entry,
          new_left,
          new_right);
    }

    const auto new_right = std::make_shared<const Node>(
        false,
        right->entry,
        fused,
        right->right);

    const auto new_node = std::make_shared<const Node>(
        true,
        left->entry,
        left->left,
        new_right);

    return balance_left(new_node);
  }

  assert(0); // LCOV_EXCL_LINE
}

std::shared_ptr<const Node> Node::balance(
    const std::shared_ptr<const Node>& node)
{
  if (node->left && node->left->red &&
      node->right && node->right->red) {

    const auto new_left = node->left ?
      node->left->copyAsBlack() : node->left;

    const auto new_right = node->right ?
      node->right->copyAsBlack() : node->right;

    return std::make_shared<const Node>(
      true,
      node->entry,
      new_left,
      new_right);
  }

  assert(!node->red);
  return node->balance();
}

std::shared_ptr<const Node> Node::balance_left(
    const std::shared_ptr<const Node>& node)
{
  // match: (color_l, color_r, color_r_l)
  // case: (Some(R), ..)
  if (node->left && node->left->red) {
    const auto new_left = std::make_shared<const Node>(
        false,
        node->left->entry,
        node->left->left,
        node->left->right);

    return std::make_shared<const Node>(
        true,
        node->entry,
        new_left,
        node->right);

  // case: (_, Some(B), _)
  } else if (node->right && !node->right->red) {
    const auto new_right = std::make_shared<const Node>(
        true,
        node->right->entry,
        node->right->left,
        node->right->right);

    const auto new_node = std::make_shared<const Node>(
        false,
        node->entry,
        node->left,
        new_right);

    return balance(new_node);

  // case: (_, Some(R), Some(B))
  } else if (node->right && node->right->red &&
      node->right->left && !node->right->left->red) {

    const auto unbalanced_new_right = std::make_shared<const Node>(
        false,
        node->right->entry,
        node->right->left->right,
        node->right->right->copyAsRed());

    const auto new_right = balance(unbalanced_new_right);

    const auto new_left = std::make_shared<const Node>(
        false,
        node->entry,
        node->left,
        node->right->left->left);

    return std::make_shared<const Node>(
        true,
        node->right->left->entry,
        new_left,
        new_right);
  }

  assert(0); // LCOV_EXCL_LINE
}

// node guaranteed non-null?
std::shared_ptr<const Node> Node::balance_right(
    const std::shared_ptr<const Node>& node)
{
  // match: (color_l, color_l_r, color_r)
  // case: (.., Some(R))
  if (node->right && node->right->red) {
    const auto new_right = std::make_shared<const Node>(
        false,
        node->right->entry,
        node->right->left,
        node->right->right);

    return std::make_shared<const Node>(
        true,
        node->entry,
        node->left,
        new_right);

  // case: (Some(B), ..)
  } else if (node->left && !node->left->red) {
    const auto new_left = std::make_shared<const Node>(
        true,
        node->left->entry,
        node->left->left,
        node->left->right);

    const auto new_node = std::make_shared<const Node>(
        false,
        node->entry,
        new_left,
        node->right);

    return balance(new_node);

  // case: (Some(R), Some(B), _)
  } else if (node->left && node->left->red &&
      node->left->right && !node->left->right->red) {

    const auto unbalanced_new_left = std::make_shared<const Node>(
        false,
        node->left->entry,
        node->left->left->copyAsRed(),
        node->left->right->left);

    const auto new_left = balance(unbalanced_new_left);

    const auto new_right = std::make_shared<const Node>(
        false,
        node->entry,
        node->left->right->right,
        node->right);

    return std::make_shared<const Node>(
        true,
        node->left->right->entry,
        new_left,
        new_right);
  }

  assert(0); // LCOV_EXCL_LINE
}

std::pair<std::shared_ptr<const Node>, bool> Node::remove_left(
    const std::shared_ptr<const Node>& node, const std::string& key)
{
  const auto [new_left, removed] = remove(node->left, key);

  const auto new_node = std::make_shared<const Node>(
    true, // In case of rebalance the color does not matter
    node->entry,
    new_left,
    node->right);

  const bool left_black = node->left && !node->left->red;
  const auto balanced_new_node = left_black ?
    balance_left(new_node) : new_node;

  return std::pair(balanced_new_node, removed);
}

std::pair<std::shared_ptr<const Node>, bool> Node::remove_right(
    const std::shared_ptr<const Node>& node, const std::string& key)
{
  const auto [new_right, removed] = remove(node->right, key);

  const auto new_node = std::make_shared<const Node>(
    true, // In case of rebalance the color does not matter
    node->entry,
    node->left,
    new_right);

  const bool right_black = node->right && !node->right->red;
  const auto bal_new_node = right_black ?
    balance_right(new_node) : new_node;

  return std::pair(bal_new_node, removed);
}

std::pair<std::shared_ptr<const Node>, bool> Node::remove(
    const std::shared_ptr<const Node>& node, const std::string& key)
{
  if (node) {
    const auto cmp = key.compare(node->entry->key);
    if (cmp < 0) {
      return remove_left(node, key);
    } else if (cmp == 0) {
      const auto new_node = fuse(node->left, node->right);
      return std::pair(new_node, true);
    } else {
      return remove_right(node, key);
    }
  } else {
    return std::pair(nullptr, false);
  }
}

// credit eternally confused
std::size_t Node::checkConsistency(const std::shared_ptr<const Node>& node)
{
  if (!node) {
    return 1;
  }

  const auto left = node->left;
  const auto right = node->right;

  if (node->red && ((left && left->red) || (right && right->red))) {
    return 0; // LCOV_EXCL_LINE
  }

  if ((left && left->entry->key.compare(node->entry->key) >= 0) ||
      (right && right->entry->key.compare(node->entry->key) <= 0)) {
    return 0; // LCOV_EXCL_LINE
  }

  const auto lh = checkConsistency(left);
  const auto rh = checkConsistency(right);

  if (lh != 0 && rh != 0 && lh != rh) {
    return 0; // LCOV_EXCL_LINE
  }

  if (lh != 0 && rh != 0) {
    return node->red ? lh : lh + 1;
  }

  return 0; // LCOV_EXCL_LINE
}

std::pair<std::shared_ptr<const Node>, bool>Node::insert(
    const std::shared_ptr<const Node>& node, const std::string& key,
    const std::string& value)
{
  if (node) {
    const auto cmp = key.compare(node->entry->key);
    if (cmp < 0) {
      const auto [new_left, is_new_key] = insert(node->left, key, value);
      const auto new_node = node->copyWithLeft(new_left);
      if (is_new_key) {
        return std::pair(new_node->balance(), is_new_key);
      } else {
        return std::pair(new_node, is_new_key);
      }

    } else if (cmp == 0) {
      const auto new_node = node->copyWithEntry(key, value);
      return std::pair(new_node, false);

    } else {
      const auto [new_right, is_new_key] = insert(node->right, key, value);
      const auto new_node = node->copyWithRight(new_right);
      if (is_new_key) {
        return std::pair(new_node->balance(), is_new_key);
      } else {
        return std::pair(new_node, is_new_key);
      }
    }
  } else {
    const auto new_node = std::make_shared<const Node>(true, key, value);
    return std::pair(new_node, true);
  }
}

std::shared_ptr<const Node> Node::balance() const
{
  if (!red) {
    // match: (color_l, color_l_l, color_l_r, color_r, color_r_l, color_r_r)
    if (left && left->red) {
      // case: (Some(R), Some(R), ..)
      if (left->left && left->left->red) {
        const auto new_left = std::make_shared<Node>(
            false,
            left->left->entry,
            left->left->left,
            left->left->right);

        const auto new_right = std::make_shared<Node>(
            false,
            entry,
            left->right,
            right);

        return std::make_shared<const Node>(
            true,
            left->entry,
            new_left,
            new_right);

      // case: (Some(R), _, Some(R), ..)
      } else if (left->right && left->right->red) {
        const auto new_left = std::make_shared<Node>(
            false,
            left->entry,
            left->left,
            left->right->left);

        const auto new_right = std::make_shared<Node>(
            false,
            entry,
            left->right->right,
            right);

        return std::make_shared<const Node>(
            true,
            left->right->entry,
            new_left,
            new_right);
      }
    }

    // case: (.., Some(R), Some(R), _)
    if (right && right->red) {
      if (right->left && right->left->red) {
        const auto new_left = std::make_shared<Node>(
            false,
            entry,
            left,
            right->left->left);

        const auto new_right = std::make_shared<Node>(
            false,
            right->entry,
            right->left->right,
            right->right);

        return std::make_shared<const Node>(
            true,
            right->left->entry,
            new_left,
            new_right);

      // case: (.., Some(R), _, Some(R))
      } else if (right->right && right->right->red) {
        const auto new_left = std::make_shared<Node>(
            false,
            entry,
            left,
            right->left);

        const auto new_right = std::make_shared<Node>(
            false,
            right->right->entry,
            right->right->left,
            right->right->right);

        return std::make_shared<const Node>(
            true,
            right->entry,
            new_left,
            new_right);
      }
    }
  }

  // red, or no matching case above
  return shared_from_this();
}

class Tree {
 public:
  Tree() :
    root_(nullptr),
    size_(0)
  {}

  Tree(std::shared_ptr<const Node> root, std::size_t size) :
    root_(root), size_(size)
  {}

  Tree insert(const std::string& key, const std::string& value) const;
  Tree remove(const std::string& key) const;

  // replace with iterator interface
  std::map<std::string, std::string> items() const;

  auto size() const {
    return size_;
  }

  bool consistent() const {
    if (root_) {
      return Node::checkConsistency(root_) != 0;
    } else {
      return true;
    }
  }

 private:
  std::shared_ptr<const Node> root_;
  std::size_t size_;
};

std::map<std::string, std::string> Tree::items() const
{
  std::map<std::string, std::string> out;

  auto node = root_;
  auto s = std::stack<std::shared_ptr<const Node>>();
  while (!s.empty() || node) {
    if (node) {
      s.push(node);
      node = node->left;
    } else {
      node = s.top();
      s.pop();
      out.emplace(node->entry->key, node->entry->value);
      node = node->right;
    }
  }
  return out;
}

Tree Tree::insert(const std::string& key, const std::string& value) const
{
  const auto [mb_new_root, is_new_key] = Node::insert(root_, key, value);
  const auto new_root = mb_new_root->copyAsBlack(); // mb = maybe black

  const auto new_size = size_ + (is_new_key ? 1 : 0);
  return Tree(new_root, new_size);
}

Tree Tree::remove(const std::string& key) const
{
  const auto [mb_new_root, removed] = Node::remove(root_, key);
  if (removed) {
    const auto new_root = mb_new_root ?
      mb_new_root->copyAsBlack() : mb_new_root;
    return Tree(new_root, size_ - 1);
  } else {
    return *this;
  }
}
