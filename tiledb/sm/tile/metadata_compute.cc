/**
 * @file   metadata_compute.cc
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
 * This file implements class MetadataCompute.
 */

#include "tiledb/sm/tile/metadata_compute.h"

using namespace tiledb::common;

namespace tiledb {
namespace sm {

/* ****************************** */
/*           STATIC API           */
/* ****************************** */

template <class T>
inline void* MetadataCompute::min(void* current, void* val) {
  return *(T*)current < *(T*)val ? current : val;
}

template <class T>
inline void* MetadataCompute::max(void* current, void* val) {
  return *(T*)current > *(T*)val ? current : val;
}

// TODO detect overflow.
template <class T>
inline void MetadataCompute::sum_signed(uint64_t* sum, void* val) {
  *((int64_t*)sum) += (int64_t) * (T*)val;
}

template <class T>
inline void MetadataCompute::sum_unsigned(uint64_t* sum, void* val) {
  *sum += (uint64_t) * (T*)val;
}

template <class T>
inline void MetadataCompute::sum_double(uint64_t* sum, void* val) {
  *((double*)sum) += (double)*(T*)val;
}

template <class T>
inline void* MetadataCompute::min_nullable(
    void* current, void* val, bool non_null) {
  return (non_null && *(T*)current > *(T*)val) ? val : current;
}

template <class T>
inline void* MetadataCompute::max_nullable(
    void* current, void* val, bool non_null) {
  return (non_null && *(T*)current < *(T*)val) ? val : current;
}

template <class T>
inline void MetadataCompute::sum_signed_nullable(
    uint64_t* sum, void* val, bool non_null) {
  if (non_null) {
    *((int64_t*)sum) += (int64_t) * (T*)val;
  }
}

template <class T>
inline void MetadataCompute::sum_unsigned_nullable(
    uint64_t* sum, void* val, bool non_null) {
  if (non_null) {
    *sum += (uint64_t) * (T*)val;
  }
}

template <class T>
inline void MetadataCompute::sum_double_nullable(
    uint64_t* sum, void* val, bool non_null) {
  if (non_null) {
    *((double*)sum) += (double)*(T*)val;
  }
}

/* ****************************** */
/*   CONSTRUCTORS & DESTRUCTORS   */
/* ****************************** */

MetadataCompute::MetadataCompute(Datatype type)
    : min_(0)
    , max_(0)
    , sum_(0)
    , non_null_count_(0) {
  cell_size_ = datatype_size(type);

  // Set compute functions based on datatype.
  switch (type) {
    case Datatype::INT8:
      min_func_ = MetadataCompute::min<int8_t>;
      max_func_ = MetadataCompute::max<int8_t>;
      sum_func_ = MetadataCompute::sum_signed<int8_t>;
      min_nullable_func_ = MetadataCompute::min_nullable<int8_t>;
      max_nullable_func_ = MetadataCompute::max_nullable<int8_t>;
      sum_nullable_func_ = MetadataCompute::sum_signed_nullable<int8_t>;
      break;
    case Datatype::INT16:
      min_func_ = MetadataCompute::min<int16_t>;
      max_func_ = MetadataCompute::max<int16_t>;
      sum_func_ = MetadataCompute::sum_signed<int16_t>;
      min_nullable_func_ = MetadataCompute::min_nullable<int16_t>;
      max_nullable_func_ = MetadataCompute::max_nullable<int16_t>;
      sum_nullable_func_ = MetadataCompute::sum_signed_nullable<int16_t>;
      break;
    case Datatype::INT32:
      min_func_ = MetadataCompute::min<int32_t>;
      max_func_ = MetadataCompute::max<int32_t>;
      sum_func_ = MetadataCompute::sum_signed<int32_t>;
      min_nullable_func_ = MetadataCompute::min_nullable<int32_t>;
      max_nullable_func_ = MetadataCompute::max_nullable<int32_t>;
      sum_nullable_func_ = MetadataCompute::sum_signed_nullable<int32_t>;
      break;
    case Datatype::INT64:
      min_func_ = MetadataCompute::min<int64_t>;
      max_func_ = MetadataCompute::max<int64_t>;
      sum_func_ = MetadataCompute::sum_signed<int64_t>;
      min_nullable_func_ = MetadataCompute::min_nullable<int64_t>;
      max_nullable_func_ = MetadataCompute::max_nullable<int64_t>;
      sum_nullable_func_ = MetadataCompute::sum_signed_nullable<int64_t>;
      break;
    case Datatype::UINT8:
      min_func_ = MetadataCompute::min<uint8_t>;
      max_func_ = MetadataCompute::max<uint8_t>;
      sum_func_ = MetadataCompute::sum_unsigned<uint8_t>;
      min_nullable_func_ = MetadataCompute::min_nullable<uint8_t>;
      max_nullable_func_ = MetadataCompute::max_nullable<uint8_t>;
      sum_nullable_func_ = MetadataCompute::sum_unsigned_nullable<uint8_t>;
      break;
    case Datatype::UINT16:
      min_func_ = MetadataCompute::min<uint16_t>;
      max_func_ = MetadataCompute::max<uint16_t>;
      sum_func_ = MetadataCompute::sum_unsigned<uint16_t>;
      min_nullable_func_ = MetadataCompute::min_nullable<uint16_t>;
      max_nullable_func_ = MetadataCompute::max_nullable<uint16_t>;
      sum_nullable_func_ = MetadataCompute::sum_unsigned_nullable<uint16_t>;
      break;
    case Datatype::UINT32:
      min_func_ = MetadataCompute::min<uint32_t>;
      max_func_ = MetadataCompute::max<uint32_t>;
      sum_func_ = MetadataCompute::sum_unsigned<uint32_t>;
      min_nullable_func_ = MetadataCompute::min_nullable<uint32_t>;
      max_nullable_func_ = MetadataCompute::max_nullable<uint32_t>;
      sum_nullable_func_ = MetadataCompute::sum_unsigned_nullable<uint32_t>;
      break;
    case Datatype::UINT64:
      min_func_ = MetadataCompute::min<uint64_t>;
      max_func_ = MetadataCompute::max<uint64_t>;
      sum_func_ = MetadataCompute::sum_unsigned<uint64_t>;
      min_nullable_func_ = MetadataCompute::min_nullable<uint64_t>;
      max_nullable_func_ = MetadataCompute::max_nullable<uint64_t>;
      sum_nullable_func_ = MetadataCompute::sum_unsigned_nullable<uint64_t>;
      break;
    case Datatype::FLOAT32:
      min_func_ = MetadataCompute::min<float>;
      max_func_ = MetadataCompute::max<float>;
      sum_func_ = MetadataCompute::sum_double<float>;
      min_nullable_func_ = MetadataCompute::min_nullable<float>;
      max_nullable_func_ = MetadataCompute::max_nullable<float>;
      sum_nullable_func_ = MetadataCompute::sum_double_nullable<float>;
      break;
    case Datatype::FLOAT64:
      min_func_ = MetadataCompute::min<double>;
      max_func_ = MetadataCompute::max<double>;
      sum_func_ = MetadataCompute::sum_double<double>;
      min_nullable_func_ = MetadataCompute::min_nullable<double>;
      max_nullable_func_ = MetadataCompute::max_nullable<double>;
      sum_nullable_func_ = MetadataCompute::sum_double_nullable<double>;
      break;
    case Datatype::DATETIME_YEAR:
    case Datatype::DATETIME_MONTH:
    case Datatype::DATETIME_WEEK:
    case Datatype::DATETIME_DAY:
    case Datatype::DATETIME_HR:
    case Datatype::DATETIME_MIN:
    case Datatype::DATETIME_SEC:
    case Datatype::DATETIME_MS:
    case Datatype::DATETIME_US:
    case Datatype::DATETIME_NS:
    case Datatype::DATETIME_PS:
    case Datatype::DATETIME_FS:
    case Datatype::DATETIME_AS:
    case Datatype::TIME_HR:
    case Datatype::TIME_MIN:
    case Datatype::TIME_SEC:
    case Datatype::TIME_MS:
    case Datatype::TIME_US:
    case Datatype::TIME_NS:
    case Datatype::TIME_PS:
    case Datatype::TIME_FS:
    case Datatype::TIME_AS:
      min_func_ = MetadataCompute::min<int64_t>;
      max_func_ = MetadataCompute::max<int64_t>;
      sum_func_ = MetadataCompute::sum_signed<int64_t>;
      min_nullable_func_ = MetadataCompute::min_nullable<int64_t>;
      max_nullable_func_ = MetadataCompute::max_nullable<int64_t>;
      sum_nullable_func_ = MetadataCompute::sum_signed_nullable<int64_t>;
      break;
    default:
      min_func_ = nullptr;
      max_func_ = nullptr;
      sum_func_ = nullptr;
      min_nullable_func_ = nullptr;
      max_nullable_func_ = nullptr;
      sum_nullable_func_ = nullptr;
      break;
  }
}

/* ****************************** */
/*               API              */
/* ****************************** */

std::tuple<void*, uint64_t, void*, uint64_t, uint64_t, uint64_t>
MetadataCompute::metadata() const {
  return {min_, min_size_, max_, max_size_, sum_, non_null_count_};
}

void MetadataCompute::compute_md(Tile* tile, Tile* tile_var, Tile* tile_val) {
  // Datatype::ANY.
  if (min_func_ == nullptr)
    return;

  min_ = nullptr;
  max_ = nullptr;
  sum_ = 0;
  non_null_count_ = 0;
  min_size_ = max_size_ = 0;

  // Handle empty tile.
  if (tile->buffer()->size() == 0)
    return;

  if (tile_var == nullptr) {
    auto current = static_cast<char*>(tile->buffer()->data());
    auto cell_num = tile->buffer()->size() / cell_size_;
    min_size_ = max_size_ = cell_num;
    min_ = current;
    max_ = current;
    current += cell_size_;

    // Fixed size attribute, non nullable
    if (tile_val == nullptr) {
      non_null_count_ = cell_num;
      for (uint64_t c = 1; c < cell_num; c++) {
        min_ = min_func_(min_, current);
        max_ = max_func_(max_, current);
        sum_func_(&sum_, current);
        current += cell_size_;
      }
    } else {  // Fixed size attribute, nullable.
      auto current_val = (uint8_t*)tile_val->buffer()->data();

      for (uint64_t c = 0; c < cell_num; c++) {
        min_ = min_nullable_func_(min_, current, *current_val);
        max_ = max_nullable_func_(min_, current, *current_val);
        sum_nullable_func_(&sum_, current, *current_val);
        current += cell_size_;

        non_null_count_ += *current_val;
        current_val++;
      }
    }
  } else {
    auto current_off = static_cast<uint64_t*>(tile->buffer()->data()) + 1;
    auto var_data = static_cast<char*>(tile_var->buffer()->data());
    auto cell_num = tile->buffer()->size() / constants::cell_var_offset_size;
    min_ = var_data;
    max_ = var_data;
    min_size_ = max_size_ =
        cell_num == 1 ? tile_var->buffer()->size() : *current_off;

    // Var size attribute, non nullable
    if (tile_val == nullptr) {
      non_null_count_ = cell_num;

      for (uint64_t c = 1; c < cell_num; c++) {
        auto value = var_data + *current_off;
        auto size = c == cell_num - 1 ?
                        tile_var->buffer()->size() - *current_off :
                        current_off[1] - *current_off;
        min_var(value, size);
        max_var(value, size);
        current_off++;
      }
    } else {  // Var size attribute, nullable.
      auto current_val = (uint8_t*)tile_val->buffer()->data();

      for (uint64_t c = 1; c < cell_num; c++) {
        auto value = var_data + *current_off;
        auto size = c == cell_num - 1 ?
                        tile_var->buffer()->size() - *current_off :
                        current_off[1] - *current_off;
        min_var(value, size);
        max_var(value, size);
        current_off++;

        non_null_count_ += *current_val;
        current_val++;
      }
    }
  }
}

/* ****************************** */
/*        PRIVATE METHODS         */
/* ****************************** */

void MetadataCompute::min_var(char* val, uint64_t size) {
  const size_t min_size = std::min<size_t>(min_size_, size);
  const int cmp = strncmp(static_cast<const char*>(min_), val, min_size);
  if (cmp != 0) {
    if (cmp > 0) {
      min_ = val;
      min_size_ = size;
    }
  } else {
    if (size < min_size) {
      min_ = val;
      min_size_ = size;
    }
  }
}

void MetadataCompute::max_var(char* val, uint64_t size) {
  const size_t min_size = std::min<size_t>(min_size_, size);
  const int cmp = strncmp(static_cast<const char*>(max_), val, min_size);
  if (cmp != 0) {
    if (cmp > 0) {
      max_ = val;
      max_size_ = size;
    }
  } else {
    if (size > min_size) {
      max_ = val;
      max_size_ = size;
    }
  }
}

}  // namespace sm
}  // namespace tiledb
