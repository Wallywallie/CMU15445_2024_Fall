//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// lru_k_replacer.cpp
//
// Identification: src/buffer/lru_k_replacer.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "buffer/lru_k_replacer.h"
#include <algorithm>
#include <cstddef>
#include <memory>
#include <stdexcept>
#include "common/config.h"
#include "common/exception.h"
#include "common/macros.h"
#include <chrono>
#include <string>
#include <utility>

namespace bustub {

void LRUKNode::SetEvictable(bool is_evictable){
    auto msg = "Frame " + std::to_string(fid_) + " " + (is_evictable ? "true" : "false");
    BUSTUB_LOG(msg);
    is_evictable_ = is_evictable;
}

auto LRUKNode::GetEvictable() const -> bool {
    return is_evictable_;
}

void LRUKNode::Visit(size_t curr_time){
    if (history_.size() == k_) {
        history_.pop_front();
    }
    history_.emplace_back(curr_time);
    auto msg = "Frame " + std::to_string(fid_) + " Visited, First TimeStamp: "  + std::to_string(history_.front()) + " History_count: " + std::to_string(history_.size());
    //BUSTUB_LOG(msg);
}

auto LRUKNode::GetHistoryCount() const -> size_t {
    return history_.size();
}

LRUKNode::LRUKNode(frame_id_t frame_id, size_t k) : k_(k), fid_(frame_id) 
{
    auto msg = "LRUKNode: frame_id_t " + std::to_string(frame_id) + " k_ " + std::to_string(k_);
    BUSTUB_LOG(msg);
}

/**
 *
 * TODO(P1): Add implementation
 *
 * @brief a new LRUKReplacer.
 * @param num_frames the maximum number of frames the LRUReplacer will be required to store
 */
LRUKReplacer::LRUKReplacer(size_t num_frames, size_t k) : replacer_size_(num_frames), k_(k) {}

/**
 * TODO(P1): Add implementation
 *
 * @brief Find the frame with largest backward k-distance and evict that frame. Only frames
 * that are marked as 'evictable' are candidates for eviction.
 *
 * A frame with less than k historical references is given +inf as its backward k-distance.
 * If multiple frames have inf backward k-distance, then evict frame whose oldest timestamp
 * is furthest in the past.
 *
 * Successful eviction of a frame should decrement the size of replacer and remove the frame's
 * access history.
 *
 * @return the frame ID if a frame is successfully evicted, or `std::nullopt` if no frames can be evicted.
 */
auto LRUKReplacer::Evict() -> std::optional<frame_id_t> { 
    LRUKNode* evict_node = nullptr;
    size_t max_time_gap = 0;
    if (node_store_.size() == 2) 
    {

    }
    for (auto& i : node_store_) {
        if (!i.second.GetEvictable()) {
            continue;
        }
        if (i.second.GetHistoryCount() < k_)
        {
            max_time_gap = inf_;
            if (evict_node == nullptr || i.second.history_.front() < evict_node->history_.front()) {
                auto msg = "Evict_node has been set to " + std::to_string(i.second.fid_);
                //BUSTUB_LOG(msg);
                evict_node = &i.second;
            }
        } else {
            size_t curr_time_gap = current_timestamp_ - i.second.history_.front();
            if (curr_time_gap > max_time_gap) {
                auto msg = "Evict_node has been set to " + std::to_string(i.second.fid_);
                //BUSTUB_LOG(msg);
                evict_node = &i.second;
                max_time_gap = curr_time_gap;
            }
        }
    }
    if (evict_node != nullptr) {
        frame_id_t frame_id = evict_node -> fid_;
        Remove(frame_id);
        auto msg = "Frame " + std::to_string(frame_id) + " has been Evicted";
        //BUSTUB_LOG(msg);
        return frame_id;
    }
    return std::nullopt; 
}

/**
 * TODO(P1): Add implementation
 *
 * @brief Record the event that the given frame id is accessed at current timestamp.
 * Create a new entry for access history if frame id has not been seen before.
 *
 * If frame id is invalid (ie. larger than replacer_size_), throw an exception. You can
 * also use BUSTUB_ASSERT to abort the process if frame id is invalid.
 *
 * @param frame_id id of frame that received a new access.
 * @param access_type type of access that was received. This parameter is only needed for
 * leaderboard tests.
 */
void LRUKReplacer::RecordAccess(frame_id_t frame_id, [[maybe_unused]] AccessType access_type) {
    BUSTUB_ENSURE(frame_id > 0, "Error: frame_id <= 0");
    BUSTUB_ENSURE(static_cast<size_t>(frame_id) <= replacer_size_, "Error: key does not exist");
    
    auto iter = node_store_.find(frame_id);
    if (iter == node_store_.end()) {
        node_store_.emplace(frame_id, LRUKNode(frame_id, k_));
    } 
    node_store_[frame_id].Visit(current_timestamp_);
    ++current_timestamp_;
}

/**
 * TODO(P1): Add implementation
 *
 * @brief Toggle whether a frame is evictable or non-evictable. This function also
 * controls replacer's size. Note that size is equal to number of evictable entries.
 *
 * If a frame was previously evictable and is to be set to non-evictable, then size should
 * decrement. If a frame was previously non-evictable and is to be set to evictable,
 * then size should increment.
 *
 * If frame id is invalid, throw an exception or abort the process.
 *
 * For other scenarios, this function should terminate without modifying anything.
 *
 * @param frame_id id of frame whose 'evictable' status will be modified
 * @param set_evictable whether the given frame is evictable or not
 */
void LRUKReplacer::SetEvictable(frame_id_t frame_id, bool set_evictable) {
    auto iter = node_store_.find(frame_id);
    if (iter == node_store_.end()) {
        //throw std::runtime_error("Error: key = " + std::to_string(frame_id) + " does not exist in unode_store_");
        return;
    }
    BUSTUB_ENSURE(iter != node_store_.end(), "key does not exist in unode_store_");
    LRUKNode& node = iter -> second;
    if (!node.GetEvictable() && set_evictable) {
        curr_size_++;
    }else if (node.GetEvictable() && !set_evictable) {
        curr_size_--;
    }
    node.SetEvictable(set_evictable);
}

/**
 * TODO(P1): Add implementation
 *
 * @brief Remove an evictable frame from replacer, along with its access history.
 * This function should also decrement replacer's size if removal is successful.
 *
 * Note that this is different from evicting a frame, which always remove the frame
 * with largest backward k-distance. This function removes specified frame id,
 * no matter what its backward k-distance is.
 *
 * If Remove is called on a non-evictable frame, throw an exception or abort the
 * process.
 *
 * If specified frame is not found, directly return from this function.
 *
 * @param frame_id id of frame to be removed
 */
void LRUKReplacer::Remove(frame_id_t frame_id) {
    auto iter = node_store_.find(frame_id);
    if (iter == node_store_.end()) {
        return;
    }
    const LRUKNode& node = iter -> second;
    BUSTUB_ENSURE(node.GetEvictable(), "Error: Remove is called on a non-evictable frame");
    node_store_.erase(frame_id);
    curr_size_--;
    auto msg = "Remove " + std::to_string(frame_id) + " Count: " + std::to_string(node_store_.size()); 
    BUSTUB_LOG(msg);
}

/**
 * TODO(P1): Add implementation
 *
 * @brief Return replacer's size, which tracks the number of evictable frames.
 *
 * @return size_t
 */
auto LRUKReplacer::Size() -> size_t {
     return curr_size_; 
}

}  // namespace bustub
