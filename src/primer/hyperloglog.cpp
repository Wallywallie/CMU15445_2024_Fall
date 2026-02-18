//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// hyperloglog.cpp
//
// Identification: src/primer/hyperloglog.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "primer/hyperloglog.h"
#include <sys/types.h>
#include <algorithm>
#include <bitset>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include "common/util/hash_util.h"

namespace bustub {

/** @brief Parameterized constructor. */
template <typename KeyType>
HyperLogLog<KeyType>::HyperLogLog(int16_t n_bits) : cardinality_(0), n_bits_(n_bits),bucket_count_(0), max_count_in_buckets_(nullptr) {
  if (n_bits < 0) {
    return;
  }
  bucket_count_ = 1 << n_bits_;
  max_count_in_buckets_ = new size_t[bucket_count_];
  std::memset(max_count_in_buckets_, 0, sizeof(size_t) * bucket_count_);
}

template <typename KeyType>
HyperLogLog<KeyType>::~HyperLogLog(){
  delete[] max_count_in_buckets_;
  max_count_in_buckets_ = nullptr;
}

/**
 * @brief Function that computes binary.
 *
 * @param[in] hash
 * @returns binary of a given hash
 */
template <typename KeyType>
auto HyperLogLog<KeyType>::ComputeBinary(const hash_t &hash) const -> std::bitset<BITSET_CAPACITY> {
  return {hash};
}

/**
 * @brief Function that computes leading zeros.
 *
 * @param[in] bset - binary values of a given bitset
 * @returns leading zeros of given binary set
 */
template <typename KeyType>
auto HyperLogLog<KeyType>::PositionOfLeftmostOne(const std::bitset<BITSET_CAPACITY> &bset) const -> uint64_t {
  for (uint64_t i = BITSET_CAPACITY - 1; i >= 0; i--) {
    if (bset.test(i)) {
      return BITSET_CAPACITY - i;
    }
  }
  return static_cast<uint64_t>(BITSET_CAPACITY);
}

/**
 * @brief Adds a value into the HyperLogLog.
 *
 * @param[in] val - value that's added into hyperloglog
 */
template <typename KeyType>
auto HyperLogLog<KeyType>::AddElem(KeyType val) -> void {
  hash_t hash = HyperLogLog<KeyType>::CalculateHash(val);
  std::bitset<BITSET_CAPACITY> bset = ComputeBinary(hash);
  std::bitset<BITSET_CAPACITY> shifted = bset >> (BITSET_CAPACITY - n_bits_);
  std::bitset<BITSET_CAPACITY> mask;
  for (int i = 0; i < n_bits_; ++i) {
    mask.set(i);
  }
  std::bitset<BITSET_CAPACITY> result = shifted & mask;
  size_t result_bukket = result.to_ulong();

  std::bitset<BITSET_CAPACITY> shifted_left = bset << n_bits_;
  std::bitset<BITSET_CAPACITY> mask_left;
  for (int i = n_bits_; i < BITSET_CAPACITY; ++i) {
    mask_left.set(i);
  }
  std::bitset<BITSET_CAPACITY> result_left = mask_left & shifted_left;
  max_count_in_buckets_[result_bukket] = std::max(PositionOfLeftmostOne(result_left), max_count_in_buckets_[result_bukket]);
}

/**
 * @brief Function that computes cardinality.
 */
template <typename KeyType>
auto HyperLogLog<KeyType>::ComputeCardinality() -> void {
  double aver = 0;
  for (size_t i = 0; i < bucket_count_; i++) {
    aver +=  1.0 / (1 << max_count_in_buckets_[i]);
  }
  if (aver == 0) {
    return;
  }
  auto res = static_cast<size_t>(CONSTANT * bucket_count_ * bucket_count_ / aver);
  cardinality_ = res;
}

template class HyperLogLog<int64_t>;
template class HyperLogLog<std::string>;

}  // namespace bustub
