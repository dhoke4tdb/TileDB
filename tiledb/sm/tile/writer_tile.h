/**
 * @file   writer_tile.h
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
 * This file defines class WriterTile.
 */

#ifndef TILEDB_WRITER_TILE_H
#define TILEDB_WRITER_TILE_H

#include "tiledb/sm/tile/tile.h"

using namespace tiledb::common;

namespace tiledb {
namespace sm {

/**
 * Handles tile information, with added data used by writer.
 */
class WriterTile : public Tile {
 public:
  /* ****************************** */
  /*           STATIC API           */
  /* ****************************** */

  template <class T>
  static void* min(void* current, void* val, bool non_null);

  template <class T>
  static void* max(void* current, void* val, bool non_null);

  template <class T>
  static void sum_signed(uint64_t* sum, void* val, bool non_null);

  template <class T>
  static void sum_unsigned(uint64_t* sum, void* val, bool non_null);

  template <class T>
  static void sum_double(uint64_t* sum, void* val, bool non_null);

  /* ********************************* */
  /*     CONSTRUCTORS & DESTRUCTORS    */
  /* ********************************* */

  WriterTile();

  /* ********************************* */
  /*                API                */
  /* ********************************* */

  /**
   * Returns the pre-filtered size of the tile data in the buffer.
   */
  uint64_t pre_filtered_size() const;

  /** Sets the pre-filtered size value to the given value. */
  void set_pre_filtered_size(uint64_t pre_filtered_size);

  /** Sets the tile metadata. */
  void set_metadata(
      const std::tuple<void*, uint64_t, void*, uint64_t, uint64_t, uint64_t>&
          md);

  /**
   * Returns a shallow or deep copy of this WriterTile.
   *
   * @param deep_copy If true, a deep copy is performed, including potentially
   *    memcpying the underlying Buffer. If false, a shallow copy is performed,
   *    which sets the clone's Buffer equal to WriterTile's buffer pointer.
   * @return New WriterTile
   */
  WriterTile clone(bool deep_copy) const;

 private:
  /* ********************************* */
  /*         PRIVATE ATTRIBUTES        */
  /* ********************************* */

  /** The size in bytes of the tile data before it has been filtered. */
  uint64_t pre_filtered_size_;

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
};

}  // namespace sm
}  // namespace tiledb

#endif  // TILEDB_WRITER_TILE_H
