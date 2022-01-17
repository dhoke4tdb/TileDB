/**
 * @file   writer_tile.cc
 *
 * @section LICENSE
 *
 * The MIT License
 *
 * @copyright Copyright (c) 2017-2021 TileDB, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * @section DESCRIPTION
 *
 * This file implements class WriterTile.
 */

#include "tiledb/sm/tile/writer_tile.h"

using namespace tiledb::common;

namespace tiledb {
namespace sm {

/* ****************************** */
/*           STATIC API           */
/* ****************************** */

template <class T>
inline void* WriterTile::min(void* current, void* val, bool non_null) {
  return (non_null && *(T*)current > *(T*)val) ? val : current;
}

template <class T>
inline void* WriterTile::max(void* current, void* val, bool non_null) {
  return (non_null && *(T*)current < *(T*)val) ? val : current;
}

template <class T>
inline void WriterTile::sum_signed(uint64_t* sum, void* val, bool non_null) {
  if (non_null) {
    *((int64_t*)sum) += (int64_t) * (T*)val;
  }
}

template <class T>
inline void WriterTile::sum_unsigned(uint64_t* sum, void* val, bool non_null) {
  if (non_null) {
    *sum += (uint64_t) * (T*)val;
  }
}

template <class T>
inline void WriterTile::sum_double(uint64_t* sum, void* val, bool non_null) {
  if (non_null) {
    *((double*)sum) += (double)*(T*)val;
  }
}

/* ****************************** */
/*   CONSTRUCTORS & DESTRUCTORS   */
/* ****************************** */

WriterTile::WriterTile()
    : Tile()
    , pre_filtered_size_(0)
    , min_(0)
    , max_(0)
    , sum_(0)
    , non_null_count_(0) {
}

/* ****************************** */
/*               API              */
/* ****************************** */

uint64_t WriterTile::pre_filtered_size() const {
  return pre_filtered_size_;
}

void WriterTile::set_pre_filtered_size(uint64_t pre_filtered_size) {
  pre_filtered_size_ = pre_filtered_size;
}

void WriterTile::set_metadata(
    const std::tuple<void*, uint64_t, void*, uint64_t, uint64_t, uint64_t>&
        md) {
  min_ = std::get<0>(md);
  min_size_ = std::get<1>(md);
  max_ = std::get<2>(md);
  max_size_ = std::get<3>(md);
  sum_ = std::get<4>(md);
  non_null_count_ = std::get<5>(md);
}

WriterTile WriterTile::clone(bool deep_copy) const {
  WriterTile clone;
  clone.pre_filtered_size_ = pre_filtered_size_;
  clone.min_ = min_;
  clone.max_ = max_;
  clone.sum_ = sum_;
  clone.non_null_count_ = non_null_count_;
  clone.cell_size_ = cell_size_;
  clone.dim_num_ = dim_num_;
  clone.format_version_ = format_version_;
  clone.type_ = type_;
  clone.filtered_buffer_ = filtered_buffer_;

  if (deep_copy) {
    clone.owns_buffer_ = owns_buffer_;
    if (owns_buffer_ && buffer_ != nullptr) {
      clone.buffer_ = tdb_new(Buffer);
      // Calls Buffer copy-assign, which performs a deep copy.
      *clone.buffer_ = *buffer_;
    } else {
      clone.buffer_ = buffer_;
    }
  } else {
    clone.owns_buffer_ = false;
    clone.buffer_ = buffer_;
  }

  return clone;
}

}  // namespace sm
}  // namespace tiledb
