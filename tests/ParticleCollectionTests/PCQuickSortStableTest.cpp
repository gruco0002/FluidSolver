#include "fluidSolver/ParticleCollectionAlgorithm.hpp"

#include <algorithm>
#include <gtest/gtest.h>
#include <random>

TEST(ParticleCollectionAlgorithm, QuickSortStable1)
{
    using namespace LibFluid;

    std::shared_ptr<ParticleCollection> coll = std::make_shared<ParticleCollection>();
    coll->add_type<ParticleInfo>();

    coll->resize(16000);
    for (int i = 0; i < coll->size(); i++)
    {
        coll->get<ParticleInfo>(i).tag = i;
    }

    ParticleCollectionAlgorithm::Sort sorter;
    sorter.adapt_collection(coll);

    sorter.quick_sort_stable(coll, [](const std::shared_ptr<ParticleCollection>& c, const pIndex_t i) -> uint64_t {
        return c->get<ParticleInfo>(i).tag;
    });

    // check if sorted
    for (int i = 0; i < coll->size(); i++)
    {
        ASSERT_EQ(i, coll->get<ParticleInfo>(i).tag);
    }
}

TEST(ParticleCollectionAlgorithm, QuickSortStable2)
{
    using namespace LibFluid;

    std::shared_ptr<ParticleCollection> coll = std::make_shared<ParticleCollection>();
    coll->add_type<ParticleInfo>();

    coll->resize(16000);
    for (int i = 0; i < coll->size(); i++)
    {
        coll->get<ParticleInfo>(i).tag = coll->size() - 1 - i;
    }


    ParticleCollectionAlgorithm::Sort sorter;
    sorter.adapt_collection(coll);

    sorter.quick_sort_stable(coll, [](const std::shared_ptr<ParticleCollection>& c, const pIndex_t i) -> uint64_t {
        return c->get<ParticleInfo>(i).tag;
    });

    // check if sorted
    for (int i = 0; i < coll->size(); i++)
    {
        ASSERT_EQ(i, coll->get<ParticleInfo>(i).tag);
    }
}

TEST(ParticleCollectionAlgorithm, QuickSortStable3)
{
    using namespace LibFluid;

    std::shared_ptr<ParticleCollection> coll = std::make_shared<ParticleCollection>();
    coll->add_type<ParticleInfo>();

    coll->resize(16000);
    std::vector<int> tmp;
    tmp.resize(coll->size());
    for (int i = 0; i < coll->size(); i++)
    {
        tmp[i] = i;
    }

    std::mt19937 g(1);
    std::shuffle(tmp.begin(), tmp.end(), g);

    for (int i = 0; i < coll->size(); i++)
    {
        coll->get<ParticleInfo>(i).tag = tmp[i];
    }


    ParticleCollectionAlgorithm::Sort sorter;
    sorter.adapt_collection(coll);

    sorter.quick_sort_stable(coll, [](const std::shared_ptr<ParticleCollection>& c, const pIndex_t i) -> uint64_t {
        return c->get<ParticleInfo>(i).tag;
    });

    // check if sorted
    for (int i = 0; i < coll->size(); i++)
    {
        ASSERT_EQ(i, coll->get<ParticleInfo>(i).tag);
    }
}


TEST(ParticleCollectionAlgorithm, QuickSortStable4)
{
    using namespace LibFluid;

    std::shared_ptr<ParticleCollection> coll = std::make_shared<ParticleCollection>();
    coll->add_type<ParticleInfo>();

    coll->resize(1600000);
    std::vector<int> tmp;
    tmp.resize(coll->size());
    for (int i = 0; i < coll->size(); i++)
    {
        tmp[i] = i;
    }

    std::mt19937 g(1);
    std::shuffle(tmp.begin(), tmp.end(), g);

    for (int i = 0; i < coll->size(); i++)
    {
        coll->get<ParticleInfo>(i).tag = tmp[i];
    }


    ParticleCollectionAlgorithm::Sort sorter;
    sorter.adapt_collection(coll);

    sorter.quick_sort_stable(coll, [](const std::shared_ptr<ParticleCollection>& c, const pIndex_t i) -> uint64_t {
        return c->get<ParticleInfo>(i).tag;
    });

    // check if sorted
    for (int i = 0; i < coll->size(); i++)
    {
        ASSERT_EQ(i, coll->get<ParticleInfo>(i).tag);
    }
}


TEST(ParticleCollectionAlgorithm, QuickSortStable5)
{
    using namespace LibFluid;

    struct Index
    {
        size_t i;
    };

    std::shared_ptr<ParticleCollection> coll = std::make_shared<ParticleCollection>();
    coll->add_type<ParticleInfo>();
    coll->add_type<Index>();

    coll->resize(16000);
    std::vector<int> tmp;
    tmp.resize(coll->size());
    for (int i = 0; i < coll->size(); i++)
    {
        tmp[i] = i / 2;
    }

    std::mt19937 g(1);
    std::shuffle(tmp.begin(), tmp.end(), g);

    for (int i = 0; i < coll->size(); i++)
    {
        coll->get<ParticleInfo>(i).tag = tmp[i];
        coll->get<Index>(i).i = i;
    }


    ParticleCollectionAlgorithm::Sort sorter;
    sorter.adapt_collection(coll);

    sorter.quick_sort_stable(coll, [](const std::shared_ptr<ParticleCollection>& c, const pIndex_t i) -> uint64_t {
        return c->get<ParticleInfo>(i).tag;
    });

    // check if sorting was stable
    for (int i = 1; i < coll->size(); i++)
    {
        if (coll->get<ParticleInfo>(i - 1).tag == coll->get<ParticleInfo>(i).tag)
        {
            ASSERT_LT(coll->get<Index>(i - 1).i, coll->get<Index>(i).i);
        }
    }
}