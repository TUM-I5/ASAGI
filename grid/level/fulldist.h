/**
 * SPDX-License-Identifier: LGPLv3
 *
 * SPDX-FileCopyrightText: 2012-2015 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

#ifndef GRID_LEVEL_FULLDIST_H
#define GRID_LEVEL_FULLDIST_H

#include "full.h"
#include "allocator/default.h"
#include "allocator/mpialloc.h"
#include "cache/cachemanager.h"
#include "transfer/numafull.h"
#include "transfer/numafullcache.h"
#include "transfer/numano.h"
#include "transfer/mpino.h"
#include "transfer/mpithreadfull.h"
#include "transfer/mpiwinfull.h"

namespace grid {

namespace level {

/**
 * Grid level implementation, that distributes the grid at the beginning
 * across all MPI tasks. If a block is not available, it is transfered via
 * MPI and stored in a cache.
 */
template <class MPITrans, class NumaTrans, class Type, class Allocator>
class FullDist : public Full<Type, Allocator> {
  private:
  /** The memory for the cache */
  unsigned char* m_cache;

  /** Manager used to control the cache */
  cache::CacheManager m_cacheManager;

  /** The MPI transfer class */
  MPITrans m_mpiTrans;

  /** The NUMA transfer class */
  NumaTrans m_numaTrans;

  public:
  /**
   * @copydoc Full::Full
   */
  FullDist(mpi::MPIComm& comm, const numa::Numa& numa, Type& type)
      : Full<Type, Allocator>(comm, numa, type), m_cache(0L) {}

  virtual ~FullDist() { Allocator::free(m_cache); }

  /**
   * @copydoc Full::open
   */
  asagi::Grid::Error open(const char* filename,
                          const char* varname,
                          const int* blockSize,
                          int timeDimension,
                          unsigned int cacheSize,
                          int cacheHandSpread,
                          grid::ValuePosition valuePos) {
    asagi::Grid::Error err = Full<Type, Allocator>::open(
        filename, varname, blockSize, timeDimension, cacheSize, cacheHandSpread, valuePos);
    if (err != asagi::Grid::SUCCESS)
      return err;

    // Initialize the cache
    err = Allocator::allocate(this->typeSize() * this->totalBlockSize() * cacheSize, m_cache);
    if (err != asagi::Grid::SUCCESS)
      return err;

    // Initialize the cache manager
    m_cacheManager.init(
        m_cache, cacheSize, this->typeSize() * this->totalBlockSize(), cacheHandSpread);

    // Initialize the MPI transfer class
    err = m_mpiTrans.init(this->data(),
                          this->localBlockCount(),
                          this->totalBlockSize(),
                          this->type(),
                          this->comm(),
                          this->numa());
    if (err != asagi::Grid::SUCCESS)
      return err;

    // Initialize NUMA transfer class
    err = m_numaTrans.init(this->data(),
                           this->localBlockCount(),
                           this->totalBlockSize(),
                           this->type(),
                           this->comm(),
                           this->numa(),
                           m_cacheManager);
    if (err != asagi::Grid::SUCCESS)
      return err;

    return asagi::Grid::SUCCESS;
  }

  /**
   * @copydoc Full::getAt
   */
  template <typename T>
  void getAt(T* buf, const double* pos) {
    // Get the index from the position
    size_t index[MAX_DIMENSIONS];
    this->pos2index(pos, index);

    // Get block id from the index
    unsigned long globalBlockId = this->blockByCoords(index);

    if (this->blockRank(globalBlockId) == this->comm().rank() &&
        this->blockDomain(globalBlockId) == this->numaDomainId()) {
      Full<Type, Allocator>::getAt(buf, pos);
      return;
    }

    // Only increment this if the case is not handled by the parent class
    this->incCounter(perf::Counter::ACCESS);

    // Check the cache
    unsigned long cacheOffset;
    unsigned char* cache;
    long oldGlobalBlockId = m_cacheManager.get(globalBlockId, cacheOffset, cache);

    if (static_cast<long>(globalBlockId) != oldGlobalBlockId) {
      // Cache not filled, do this first

      if (m_numaTrans.transfer(globalBlockId,
                               this->blockRank(globalBlockId),
                               this->blockDomain(globalBlockId),
                               this->blockOffset(globalBlockId),
                               cache)) {
        this->incCounter(perf::Counter::NUMA);
      } else {
        this->incCounter(perf::Counter::MPI);

        m_mpiTrans.transfer(
            this->blockRank(globalBlockId), this->blockNodeOffset(globalBlockId), cache);
      }
    }

    // Compute the offset in the block
    unsigned long offset = this->calcOffsetInBlock(index);

    assert(offset < this->totalBlockSize());

    // Finally, we fill the buffer
    this->type().convert(&cache[this->typeSize() * offset], buf);

    // Free the block in the cache
    m_cacheManager.unlock(cacheOffset);
  }
};

/** Full distributed level with NUMA */
template <class Type>
using FullDistNuma = FullDist<transfer::MPINo, transfer::NumaFull, Type, allocator::Default>;

/** Full distributed level with MPI (communication thread) */
template <class Type>
using FullDistMPIThread =
    FullDist<transfer::MPIThreadFull, transfer::NumaNo, Type, allocator::MPIAlloc>;

/** Full distributed level with MPI windows */
template <class Type>
using FullDistMPIWin = FullDist<transfer::MPIWinFull, transfer::NumaNo, Type, allocator::MPIAlloc>;

/** Full distributed level with MPI (communication thread) and NUMA */
template <class Type>
using FullDistMPIThreadNuma =
    FullDist<transfer::MPIThreadFull, transfer::NumaFull, Type, allocator::MPIAlloc>;

/** Full distributed level with MPI windows and NUMA */
template <class Type>
using FullDistMPIWinNuma =
    FullDist<transfer::MPIWinFull, transfer::NumaFull, Type, allocator::MPIAlloc>;

/** Full distributed level with MPI (communication thread), NUMA and NUMA cache lookup */
template <class Type>
using FullDistMPIThreadNumaCache =
    FullDist<transfer::MPIThreadFull, transfer::NumaFullCache, Type, allocator::MPIAlloc>;

/** Full distributed level with MPI windows, NUMA and NUMA cache lookup */
template <class Type>
using FullDistMPIWinNumaCache =
    FullDist<transfer::MPIWinFull, transfer::NumaFullCache, Type, allocator::MPIAlloc>;

} // namespace level

} // namespace grid

#endif // GRID_LEVEL_FULLDIST_H
