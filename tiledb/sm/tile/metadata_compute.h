/**
 * @file   metadata_compute.h
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
 * This file defines class MetadataCompute.
 */

#ifndef TILEDB_METADATA_COMPUTE_H
#define TILEDB_METADATA_COMPUTE_H

#include "tiledb/sm/tile/tile.h"

#include "tiledb/sm/enums/datatype.h"
#include "tiledb/sm/misc/types.h"

using namespace tiledb::common;

namespace tiledb {
namespace sm {

/**
 * Computes metadata for a tile using the tile, tile var, and tile validity.
 */
class MetadataCompute {
 public:
  /* ****************************** */
  /*           STATIC API           */
  /* ****************************** */

  template <class T>
  static void* min(void* current, void* val);

  template <class T>
  static void* max(void* current, void* val);

  template <class T>
  static void sum_signed(uint64_t* sum, void* val);

  template <class T>
  static void sum_unsigned(uint64_t* sum, void* val);

  template <class T>
  static void sum_double(uint64_t* sum, void* val);

  template <class T>
  static void* min_nullable(void* current, void* val, bool non_null);

  template <class T>
  static void* max_nullable(void* current, void* val, bool non_null);

  template <class T>
  static void sum_signed_nullable(uint64_t* sum, void* val, bool non_null);

  template <class T>
  static void sum_unsigned_nullable(uint64_t* sum, void* val, bool non_null);

  template <class T>
  static void sum_double_nullable(uint64_t* sum, void* val, bool non_null);

  /* ********************************* */
  /*     CONSTRUCTORS & DESTRUCTORS    */
  /* ********************************* */

  MetadataCompute(Datatype type);

  /* ********************************* */
  /*                API                */
  /* ********************************* */

  /**
   * Returns the metadata.
   *
   * @return min, min_size, max, max_size, sum, non null count.
   */
  std::tuple<void*, uint64_t, void*, uint64_t, uint64_t, uint64_t> metadata()
      const;

  /** Compute metatada. */
  void compute_md(Tile* tile, Tile* tile_var, Tile* tile_val);

 private:
  /* ********************************* */
  /*         PRIVATE ATTRIBUTES        */
  /* ********************************* */

  /** Minimum value for this tile. */
  void* min_;

  /** Minimum value size for this tile. */
  uint64_t min_size_;

  /** Maximum value for this tile. */
  void* max_;

  /** Maximum value size for this tile. */
  uint64_t max_size_;

  /** Sum of values. */
  uint64_t sum_;

  /** Count of non null values. */
  uint64_t non_null_count_;

  /** Cell size. */
  uint64_t cell_size_;

  /**
   * Stores the appropriate templated min() function based on the datatype.
   */
  std::function<void*(void*, void*)> min_func_;

  /**
   * Stores the appropriate templated max() function based on the datatype.
   */
  std::function<void*(void*, void*)> max_func_;

  /**
   * Stores the appropriate templated sum() function based on the datatype.
   */
  std::function<void(uint64_t*, void*)> sum_func_;

  /**
   * Stores the appropriate templated min_nullable() function based on the
   * datatype.
   */
  std::function<void*(void*, void*, bool)> min_nullable_func_;

  /**
   * Stores the appropriate templated max_nullable() function based on the
   * datatype.
   */
  std::function<void*(void*, void*, bool)> max_nullable_func_;

  /**
   * Stores the appropriate templated sum_nullable() function based on the
   * datatype.
   */
  std::function<void(uint64_t*, void*, bool)> sum_nullable_func_;

  /* ********************************* */
  /*          PRIVATE METHODS          */
  /* ********************************* */

  /** Min function for var sized attributes. */
  void min_var(char* val, uint64_t size);

  /** Max function for var sized attributes. */
  void max_var(char* val, uint64_t size);
};

}  // namespace sm
}  // namespace tiledb

#endif  // TILEDB_METADATA_COMPUTE_H
