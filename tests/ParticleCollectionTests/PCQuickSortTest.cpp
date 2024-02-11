#include "fluidSolver/ParticleCollectionAlgorithm.hpp"

#include <algorithm>
#include <gtest/gtest.h>
#include <random>

TEST(ParticleCollectionAlgorithm, QuickSort1)
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

    sorter.quick_sort(coll, [](const std::shared_ptr<ParticleCollection> &c, const size_t i) -> uint64_t {
        return c->get<ParticleInfo>(i).tag;
    });

    // check if sorted
    for (int i = 0; i < coll->size(); i++)
    {
        ASSERT_EQ(i, coll->get<ParticleInfo>(i).tag);
    }
}

TEST(ParticleCollectionAlgorithm, QuickSort2)
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

    sorter.quick_sort(coll, [](const std::shared_ptr<ParticleCollection> &c, const size_t i) -> uint64_t {
        return c->get<ParticleInfo>(i).tag;
    });

    // check if sorted
    for (int i = 0; i < coll->size(); i++)
    {
        ASSERT_EQ(i, coll->get<ParticleInfo>(i).tag);
    }
}

TEST(ParticleCollectionAlgorithm, QuickSort3)
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

    sorter.quick_sort(coll, [](const std::shared_ptr<ParticleCollection> &c, const size_t i) -> uint64_t {
        return c->get<ParticleInfo>(i).tag;
    });

    // check if sorted
    for (int i = 0; i < coll->size(); i++)
    {
        ASSERT_EQ(i, coll->get<ParticleInfo>(i).tag);
    }
}

TEST(ParticleCollectionAlgorithm, QuickSort4)
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

    sorter.quick_sort(coll, [](const std::shared_ptr<ParticleCollection> &c, const size_t i) -> uint64_t {
        return c->get<ParticleInfo>(i).tag;
    });

    // check if sorted
    for (int i = 0; i < coll->size(); i++)
    {
        ASSERT_EQ(i, coll->get<ParticleInfo>(i).tag);
    }
}
