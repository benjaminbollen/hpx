//  Copyright (c) 2013-2014 Thomas Heller
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef HPX_PARCELSET_MEMORY_POOL_HPP
#define HPX_PARCELSET_MEMORY_POOL_HPP

#include <hpx/config.hpp>

#include <hpx/runtime/parcelset/memory_chunk.hpp>

#include <vector>

namespace hpx { namespace parcelset {

    struct memory_pool : boost::noncopyable
    {
        typedef memory_chunk::size_type size_type;
        typedef std::multimap<size_type, char *> large_chunks_type;
        typedef hpx::lcos::local::spinlock mutex_type;

        memory_pool(std::size_t chunk_size, std::size_t max_chunks)
          : memory_chunks_(max_chunks, memory_chunk(chunk_size))
          , chunk_size_(chunk_size)
          , max_chunks_(max_chunks)
        {
        }

        ~memory_pool()
        {
            BOOST_FOREACH(large_chunks_type::value_type &v, large_chunks_)
            {
#if POSIX_VERSION_
                free(v.second);
#else
                delete[] v.second;
#endif
            }
        }

        char *allocate(size_type size)
        {
            char * result = 0;

            if(size <= chunk_size_)
            {
                mutex_type::scoped_lock l(chunks_mtx_);
                BOOST_FOREACH(memory_chunk & chunk, memory_chunks_)
                {
                    result = chunk.allocate(size);
                    if(result != 0)
                        return result;
                }
            }

            // if(size > chunk_size_)
            // {
                {
                    mutex_type::scoped_lock l(large_chunks_mtx_);
                    large_chunks_type::iterator it = large_chunks_.find(size);
                    if(it != large_chunks_.end())
                    {
                        result = it->second;
                        large_chunks_.erase(it);
                        return result;
                    }
                }
                char * ptr;
#if POSIX_VERSION_
                int ret = 0;
                ret = posix_memalign(reinterpret_cast<void **>(&ptr), EXEC_PAGESIZE, size);
                if(ret != 0)
                    throw std::bad_alloc();
#else
                ptr = new char[size];
#endif
                return ptr;
            // }
        }

        void deallocate(char * p, size_type size)
        {
            if(size <= chunk_size_)
            {
                mutex_type::scoped_lock l(chunks_mtx_);
                BOOST_FOREACH(memory_chunk & chunk, memory_chunks_)
                {
                    if(chunk.deallocate(p, size))
                    {
                        return;
                    }
                }
            }
            {
                mutex_type::scoped_lock l(large_chunks_mtx_);
                large_chunks_.insert(std::make_pair(size, p));
            }
        }

        mutable mutex_type chunks_mtx_;
        std::vector<memory_chunk> memory_chunks_;
        const std::size_t chunk_size_;
        const std::size_t max_chunks_;

        mutable mutex_type large_chunks_mtx_;
        large_chunks_type large_chunks_;
    };
}}

#endif
