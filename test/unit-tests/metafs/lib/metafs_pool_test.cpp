/*
 *   BSD LICENSE
 *   Copyright (c) 2021 Samsung Electronics Corporation
 *   All rights reserved.
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the
 *       distribution.
 *     * Neither the name of Samsung Electronics Corporation nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "src/metafs/lib/metafs_pool.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "src/metafs/mim/read_mpio.h"
#include "src/metafs/mim/write_mpio.h"
#include "test/unit-tests/metafs/mim/read_mpio_mock.h"
#include "test/unit-tests/metafs/mim/write_mpio_mock.h"

using ::testing::_;
using ::testing::Matcher;
using ::testing::NiceMock;
using ::testing::Return;

namespace pos
{
template<typename T>
T*
GetMpio(void* buf)
{
    return new T(buf);
}

TEST(MetaFsPool, Construct_testIfThePoolHasCorrectNumbers)
{
    const size_t POOL_SIZE = 5;
    MetafsPool<WriteMpio*> pool(POOL_SIZE);

    EXPECT_EQ(pool.GetCapacity(), POOL_SIZE);
    EXPECT_EQ(pool.GetFreeCount(), 0);
    EXPECT_EQ(pool.GetUsedCount(), 0);
}

TEST(MetaFsPool, AddToPool_testIfThePoolHasCorrectNumbers)
{
    const size_t POOL_SIZE = 5;
    MetafsPool<WriteMpio*> pool(POOL_SIZE);
    size_t testBuf = 0;

    for (size_t i = 0; i < POOL_SIZE; i++)
    {
        MockWriteMpio* mpio = GetMpio<MockWriteMpio>((void*)&testBuf);
        EXPECT_TRUE(pool.AddToPool(mpio));
    }

    EXPECT_EQ(pool.GetCapacity(), POOL_SIZE);
    EXPECT_EQ(pool.GetFreeCount(), POOL_SIZE);
    EXPECT_EQ(pool.GetUsedCount(), 0);
}

TEST(MetaFsPool, Desctructor_testIfThePoolWillBeDesctuctedWell)
{
    const size_t POOL_SIZE = 5;
    MetafsPool<WriteMpio*>* pool = new MetafsPool<WriteMpio*>(POOL_SIZE);
    size_t testBuf = 0;
    std::vector<WriteMpio*> mpioList;

    for (size_t i = 0; i < POOL_SIZE; i++)
    {
        MockWriteMpio* mpio = GetMpio<MockWriteMpio>((void*)&testBuf);
        mpioList.push_back(mpio);
        EXPECT_TRUE(pool->AddToPool(mpio));
    }

    MockWriteMpio* mpio = GetMpio<MockWriteMpio>((void*)&testBuf);
    EXPECT_FALSE(pool->AddToPool(mpio));
    delete mpio;

    EXPECT_NO_FATAL_FAILURE(delete pool);

    for (auto mpio : mpioList)
    {
        EXPECT_NO_FATAL_FAILURE(delete mpio);
    }
}
} // namespace pos
