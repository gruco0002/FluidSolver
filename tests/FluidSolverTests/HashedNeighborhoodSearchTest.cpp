//
// Created by corbi on 22.05.2019.
//


#include <core/fluidSolver/particleCollection/SimpleParticleCollection.hpp>
#include <core/fluidSolver/neighborhoodSearch/HashedNeighborhoodSearch.hpp>
#include <libraries/glm/gtx/matrix_transform_2d.hpp>
#include <core/interface/Constants.hpp>
#include "gtest/gtest.h"
#include "HelperFunctions.hpp"


FluidSolver::INeighborhoodSearch *GenerateHashedNeighborhoodSearch() {
    return new FluidSolver::HashedNeighborhoodSearch(3);
}


TEST(HashedNeighborhoodTest, NeighborhoodCount1) {

    float radius = 1.5f;
    auto positions = GetSampleGrid(-2.0f, 1.0f, 2.0f);


    std::vector<FluidSolver::SimpleParticleCollection::FluidParticle> particles;
    for (auto pos : positions) {
        FluidSolver::SimpleParticleCollection::FluidParticle p;
        p.Position = pos;
        particles.push_back(p);
    }
    FluidSolver::INeighborhoodSearch *neighborhoodSearch = GenerateHashedNeighborhoodSearch();
    FluidSolver::SimpleParticleCollection collection(particles);
    neighborhoodSearch->SetParticleCount(collection.GetSize());
    neighborhoodSearch->FindNeighbors(&collection, radius);


    ASSERT_EQ(neighborhoodSearch->GetParticleNeighbors(0).size(), 4); // top left
    ASSERT_EQ(neighborhoodSearch->GetParticleNeighbors(20).size(), 4); // bottom left
    ASSERT_EQ(neighborhoodSearch->GetParticleNeighbors(12).size(), 9); // center
    ASSERT_EQ(neighborhoodSearch->GetParticleNeighbors(18).size(), 9); // center bottom right
    ASSERT_EQ(neighborhoodSearch->GetParticleNeighbors(10).size(), 6); // border left center
    ASSERT_EQ(neighborhoodSearch->GetParticleNeighbors(14).size(), 6); // border right center

}

TEST(HashedNeighborhoodTest, NeighborhoodCount2) {

    float radius = 2.5f;
    auto positions = GetSampleGrid(-2.0f, 1.0f, 2.0f);


    std::vector<FluidSolver::SimpleParticleCollection::FluidParticle> particles;
    for (auto pos : positions) {
        FluidSolver::SimpleParticleCollection::FluidParticle p;
        p.Position = pos;
        particles.push_back(p);
    }
    FluidSolver::INeighborhoodSearch *neighborhoodSearch = GenerateHashedNeighborhoodSearch();
    FluidSolver::SimpleParticleCollection collection(particles);
    neighborhoodSearch->SetParticleCount(collection.GetSize());
    neighborhoodSearch->FindNeighbors(&collection, radius);


    ASSERT_EQ(neighborhoodSearch->GetParticleNeighbors(0).size(), 8); // top left
    ASSERT_EQ(neighborhoodSearch->GetParticleNeighbors(20).size(), 8); // bottom left
    ASSERT_EQ(neighborhoodSearch->GetParticleNeighbors(12).size(), 21); // center
    ASSERT_EQ(neighborhoodSearch->GetParticleNeighbors(18).size(), 15); // center bottom right
    ASSERT_EQ(neighborhoodSearch->GetParticleNeighbors(10).size(), 13); // border left center
    ASSERT_EQ(neighborhoodSearch->GetParticleNeighbors(14).size(), 13); // border right center


}

TEST(HashedNeighborhoodTest, NeighborhoodCorrect1) {

    float radius = 1.5f;
    auto positions = GetSampleGrid(-2.0f, 1.0f, 2.0f);


    std::vector<FluidSolver::SimpleParticleCollection::FluidParticle> particles;
    for (auto pos : positions) {
        FluidSolver::SimpleParticleCollection::FluidParticle p;
        p.Position = pos;
        particles.push_back(p);
    }
    FluidSolver::INeighborhoodSearch *neighborhoodSearch = GenerateHashedNeighborhoodSearch();
    FluidSolver::SimpleParticleCollection collection(particles);

    neighborhoodSearch->SetParticleCount(collection.GetSize());
    neighborhoodSearch->FindNeighbors(&collection, radius);


    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(0), 0)); // top left
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(0), 1));
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(0), 5));
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(0), 6));
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(0), 2), false);
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(0), 24), false);


    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(20), 20)); // bottom left
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(20), 16));
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(20), 7), false);

    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(12), 12)); // center
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(12), 7));
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(12), 6));
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(12), 13));
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(12), 2), false);
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(12), 20), false);
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(12), 22), false);
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(12), 0), false);
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(12), 19), false);


}

TEST(HashedNeighborhoodTest, NeighborhoodCorrect2) {

    float radius = 2.5f;
    auto positions = GetSampleGrid(-2.0f, 1.0f, 2.0f);


    std::vector<FluidSolver::SimpleParticleCollection::FluidParticle> particles;
    for (auto pos : positions) {
        FluidSolver::SimpleParticleCollection::FluidParticle p;
        p.Position = pos;
        particles.push_back(p);
    }
    FluidSolver::INeighborhoodSearch *neighborhoodSearch = GenerateHashedNeighborhoodSearch();
    FluidSolver::SimpleParticleCollection collection(particles);
    neighborhoodSearch->SetParticleCount(collection.GetSize());
    neighborhoodSearch->FindNeighbors(&collection, radius);


    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(0), 0)); // top left
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(0), 1));
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(0), 5));
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(0), 6));
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(0), 2));
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(0), 11));
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(0), 7));
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(0), 24), false);
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(0), 12), false);
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(0), 3), false);


    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(20), 20)); // bottom left
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(20), 16));
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(20), 17));
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(20), 22));
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(20), 7), false);
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(20), 23), false);

    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(12), 12)); // center
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(12), 7));
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(12), 6));
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(12), 13));
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(12), 10));
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(12), 3));
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(12), 0), false);
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(12), 20), false);
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(12), 24), false);
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(12), 4), false);


}


TEST(HashedNeighborhoodTest, ShiftedNeighborhoodCount1) {

    float radius = 1.5f;
    auto positions = GetSampleGrid(-2.0f, 1.0f, 2.0f);


    std::vector<FluidSolver::SimpleParticleCollection::FluidParticle> particles;
    for (auto pos : positions) {
        FluidSolver::SimpleParticleCollection::FluidParticle p;
        p.Position = pos + glm::vec2(10.333f);
        particles.push_back(p);
    }
    FluidSolver::INeighborhoodSearch *neighborhoodSearch = GenerateHashedNeighborhoodSearch();
    FluidSolver::SimpleParticleCollection collection(particles);
    neighborhoodSearch->SetParticleCount(collection.GetSize());
    neighborhoodSearch->FindNeighbors(&collection, radius);


    ASSERT_EQ(neighborhoodSearch->GetParticleNeighbors(0).size(), 4); // top left
    ASSERT_EQ(neighborhoodSearch->GetParticleNeighbors(20).size(), 4); // bottom left
    ASSERT_EQ(neighborhoodSearch->GetParticleNeighbors(12).size(), 9); // center
    ASSERT_EQ(neighborhoodSearch->GetParticleNeighbors(18).size(), 9); // center bottom right
    ASSERT_EQ(neighborhoodSearch->GetParticleNeighbors(10).size(), 6); // border left center
    ASSERT_EQ(neighborhoodSearch->GetParticleNeighbors(14).size(), 6); // border right center

}

TEST(HashedNeighborhoodTest, ShiftedNeighborhoodCount2) {

    float radius = 2.5f;
    auto positions = GetSampleGrid(-2.0f, 1.0f, 2.0f);


    std::vector<FluidSolver::SimpleParticleCollection::FluidParticle> particles;
    for (auto pos : positions) {
        FluidSolver::SimpleParticleCollection::FluidParticle p;
        p.Position = pos - glm::vec2(50.856f);
        particles.push_back(p);
    }
    FluidSolver::INeighborhoodSearch *neighborhoodSearch = GenerateHashedNeighborhoodSearch();
    FluidSolver::SimpleParticleCollection collection(particles);
    neighborhoodSearch->SetParticleCount(collection.GetSize());
    neighborhoodSearch->FindNeighbors(&collection, radius);


    ASSERT_EQ(neighborhoodSearch->GetParticleNeighbors(0).size(), 8); // top left
    ASSERT_EQ(neighborhoodSearch->GetParticleNeighbors(20).size(), 8); // bottom left
    ASSERT_EQ(neighborhoodSearch->GetParticleNeighbors(12).size(), 21); // center
    ASSERT_EQ(neighborhoodSearch->GetParticleNeighbors(18).size(), 15); // center bottom right
    ASSERT_EQ(neighborhoodSearch->GetParticleNeighbors(10).size(), 13); // border left center
    ASSERT_EQ(neighborhoodSearch->GetParticleNeighbors(14).size(), 13); // border right center


}

TEST(HashedNeighborhoodTest, ShiftedNeighborhoodCorrect1) {

    float radius = 1.5f;
    auto positions = GetSampleGrid(-2.0f, 1.0f, 2.0f);


    std::vector<FluidSolver::SimpleParticleCollection::FluidParticle> particles;
    for (auto pos : positions) {
        FluidSolver::SimpleParticleCollection::FluidParticle p;
        p.Position = pos + glm::vec2(10.3333f, -46.8453f);
        particles.push_back(p);
    }
    FluidSolver::INeighborhoodSearch *neighborhoodSearch = GenerateHashedNeighborhoodSearch();
    FluidSolver::SimpleParticleCollection collection(particles);
    neighborhoodSearch->SetParticleCount(collection.GetSize());
    neighborhoodSearch->FindNeighbors(&collection, radius);


    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(0), 0)); // top left
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(0), 1));
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(0), 5));
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(0), 6));
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(0), 2), false);
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(0), 24), false);


    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(20), 20)); // bottom left
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(20), 16));
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(20), 7), false);

    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(12), 12)); // center
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(12), 7));
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(12), 6));
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(12), 13));
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(12), 2), false);
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(12), 20), false);
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(12), 22), false);
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(12), 0), false);
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(12), 19), false);


}

TEST(HashedNeighborhoodTest, ShiftedNeighborhoodCorrect2) {

    float radius = 2.5f;
    auto positions = GetSampleGrid(-2.0f, 1.0f, 2.0f);


    std::vector<FluidSolver::SimpleParticleCollection::FluidParticle> particles;
    for (auto pos : positions) {
        FluidSolver::SimpleParticleCollection::FluidParticle p;
        p.Position = pos + glm::vec2(10000.0f, -1000000.0f);
        particles.push_back(p);
    }
    FluidSolver::INeighborhoodSearch *neighborhoodSearch = GenerateHashedNeighborhoodSearch();
    FluidSolver::SimpleParticleCollection collection(particles);
    neighborhoodSearch->SetParticleCount(collection.GetSize());
    neighborhoodSearch->FindNeighbors(&collection, radius);


    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(0), 0)); // top left
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(0), 1));
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(0), 5));
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(0), 6));
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(0), 2));
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(0), 11));
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(0), 7));
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(0), 24), false);
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(0), 12), false);
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(0), 3), false);


    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(20), 20)); // bottom left
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(20), 16));
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(20), 17));
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(20), 22));
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(20), 7), false);
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(20), 23), false);

    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(12), 12)); // center
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(12), 7));
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(12), 6));
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(12), 13));
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(12), 10));
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(12), 3));
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(12), 0), false);
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(12), 20), false);
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(12), 24), false);
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(12), 4), false);


}

TEST(HashedNeighborhoodTest, RotatedNeighborhoodCount1) {

    float radius = 1.5f;
    auto positions = GetSampleGrid(-2.0f, 1.0f, 2.0f);

    float rotation = 138.0f; // degrees
    std::vector<FluidSolver::SimpleParticleCollection::FluidParticle> particles;
    glm::mat3 rot = glm::rotate(glm::mat3(1.0f), FS_PI / 180.0f * rotation);
    for (auto pos : positions) {
        FluidSolver::SimpleParticleCollection::FluidParticle p;
        glm::vec3 rotated = glm::vec3(pos, 1.0f) * rot;
        p.Position = glm::vec2(rotated.x, rotated.y);
        particles.push_back(p);
    }
    FluidSolver::INeighborhoodSearch *neighborhoodSearch = GenerateHashedNeighborhoodSearch();
    FluidSolver::SimpleParticleCollection collection(particles);
    neighborhoodSearch->SetParticleCount(collection.GetSize());
    neighborhoodSearch->FindNeighbors(&collection, radius);


    ASSERT_EQ(neighborhoodSearch->GetParticleNeighbors(0).size(), 4); // top left
    ASSERT_EQ(neighborhoodSearch->GetParticleNeighbors(20).size(), 4); // bottom left
    ASSERT_EQ(neighborhoodSearch->GetParticleNeighbors(12).size(), 9); // center
    ASSERT_EQ(neighborhoodSearch->GetParticleNeighbors(18).size(), 9); // center bottom right
    ASSERT_EQ(neighborhoodSearch->GetParticleNeighbors(10).size(), 6); // border left center
    ASSERT_EQ(neighborhoodSearch->GetParticleNeighbors(14).size(), 6); // border right center

}

TEST(HashedNeighborhoodTest, RotatedNeighborhoodCount2) {

    float radius = 2.5f;
    auto positions = GetSampleGrid(-2.0f, 1.0f, 2.0f);


    float rotation = 67.0f; // degrees
    std::vector<FluidSolver::SimpleParticleCollection::FluidParticle> particles;
    glm::mat3 rot = glm::rotate(glm::mat3(1.0f), FS_PI / 180.0f * rotation);
    for (auto pos : positions) {
        FluidSolver::SimpleParticleCollection::FluidParticle p;
        glm::vec3 rotated = glm::vec3(pos, 1.0f) * rot;
        p.Position = glm::vec2(rotated.x, rotated.y);
        particles.push_back(p);
    }
    FluidSolver::INeighborhoodSearch *neighborhoodSearch = GenerateHashedNeighborhoodSearch();
    FluidSolver::SimpleParticleCollection collection(particles);
    neighborhoodSearch->SetParticleCount(collection.GetSize());
    neighborhoodSearch->FindNeighbors(&collection, radius);


    ASSERT_EQ(neighborhoodSearch->GetParticleNeighbors(0).size(), 8); // top left
    ASSERT_EQ(neighborhoodSearch->GetParticleNeighbors(20).size(), 8); // bottom left
    ASSERT_EQ(neighborhoodSearch->GetParticleNeighbors(12).size(), 21); // center
    ASSERT_EQ(neighborhoodSearch->GetParticleNeighbors(18).size(), 15); // center bottom right
    ASSERT_EQ(neighborhoodSearch->GetParticleNeighbors(10).size(), 13); // border left center
    ASSERT_EQ(neighborhoodSearch->GetParticleNeighbors(14).size(), 13); // border right center


}

TEST(HashedNeighborhoodTest, RotatedNeighborhoodCorrect1) {

    float radius = 1.5f;
    auto positions = GetSampleGrid(-2.0f, 1.0f, 2.0f);


    float rotation = 35.0f; // degrees
    std::vector<FluidSolver::SimpleParticleCollection::FluidParticle> particles;
    glm::mat3 rot = glm::rotate(glm::mat3(1.0f), FS_PI / 180.0f * rotation);
    for (auto pos : positions) {
        FluidSolver::SimpleParticleCollection::FluidParticle p;
        glm::vec3 rotated = glm::vec3(pos, 1.0f) * rot;
        p.Position = glm::vec2(rotated.x, rotated.y);
        particles.push_back(p);
    }
    FluidSolver::INeighborhoodSearch *neighborhoodSearch = GenerateHashedNeighborhoodSearch();
    FluidSolver::SimpleParticleCollection collection(particles);
    neighborhoodSearch->SetParticleCount(collection.GetSize());
    neighborhoodSearch->FindNeighbors(&collection, radius);


    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(0), 0)); // top left
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(0), 1));
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(0), 5));
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(0), 6));
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(0), 2), false);
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(0), 24), false);


    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(20), 20)); // bottom left
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(20), 16));
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(20), 7), false);

    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(12), 12)); // center
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(12), 7));
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(12), 6));
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(12), 13));
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(12), 2), false);
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(12), 20), false);
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(12), 22), false);
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(12), 0), false);
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(12), 19), false);


}

TEST(HashedNeighborhoodTest, RotatedNeighborhoodCorrect2) {

    float radius = 2.5f;
    auto positions = GetSampleGrid(-2.0f, 1.0f, 2.0f);


    float rotation = 246.56f; // degrees
    std::vector<FluidSolver::SimpleParticleCollection::FluidParticle> particles;
    glm::mat3 rot = glm::rotate(glm::mat3(1.0f), FS_PI / 180.0f * rotation);
    for (auto pos : positions) {
        FluidSolver::SimpleParticleCollection::FluidParticle p;
        glm::vec3 rotated = glm::vec3(pos, 1.0f) * rot;
        p.Position = glm::vec2(rotated.x, rotated.y);
        particles.push_back(p);
    }
    FluidSolver::INeighborhoodSearch *neighborhoodSearch = GenerateHashedNeighborhoodSearch();
    FluidSolver::SimpleParticleCollection collection(particles);
    neighborhoodSearch->SetParticleCount(collection.GetSize());
    neighborhoodSearch->FindNeighbors(&collection, radius);


    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(0), 0)); // top left
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(0), 1));
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(0), 5));
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(0), 6));
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(0), 2));
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(0), 11));
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(0), 7));
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(0), 24), false);
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(0), 12), false);
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(0), 3), false);


    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(20), 20)); // bottom left
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(20), 16));
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(20), 17));
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(20), 22));
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(20), 7), false);
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(20), 23), false);

    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(12), 12)); // center
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(12), 7));
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(12), 6));
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(12), 13));
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(12), 10));
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(12), 3));
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(12), 0), false);
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(12), 20), false);
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(12), 24), false);
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(12), 4), false);


}

TEST(HashedNeighborhoodTest, RotatedShiftedNeighborhoodCount1) {

    float radius = 1.5f;
    auto positions = GetSampleGrid(-2.0f, 1.0f, 2.0f);

    float rotation = 138.0f; // degrees
    std::vector<FluidSolver::SimpleParticleCollection::FluidParticle> particles;
    glm::mat3 rot = glm::rotate(glm::translate(glm::mat3(1.0f), glm::vec2(25.4f, 18.2f)), FS_PI / 180.0f * rotation);
    for (auto pos : positions) {
        FluidSolver::SimpleParticleCollection::FluidParticle p;
        glm::vec3 rotated = glm::vec3(pos, 1.0f) * rot;
        p.Position = glm::vec2(rotated.x, rotated.y);
        particles.push_back(p);
    }
    FluidSolver::INeighborhoodSearch *neighborhoodSearch = GenerateHashedNeighborhoodSearch();
    FluidSolver::SimpleParticleCollection collection(particles);
    neighborhoodSearch->SetParticleCount(collection.GetSize());
    neighborhoodSearch->FindNeighbors(&collection, radius);


    ASSERT_EQ(neighborhoodSearch->GetParticleNeighbors(0).size(), 4); // top left
    ASSERT_EQ(neighborhoodSearch->GetParticleNeighbors(20).size(), 4); // bottom left
    ASSERT_EQ(neighborhoodSearch->GetParticleNeighbors(12).size(), 9); // center
    ASSERT_EQ(neighborhoodSearch->GetParticleNeighbors(18).size(), 9); // center bottom right
    ASSERT_EQ(neighborhoodSearch->GetParticleNeighbors(10).size(), 6); // border left center
    ASSERT_EQ(neighborhoodSearch->GetParticleNeighbors(14).size(), 6); // border right center

}

TEST(HashedNeighborhoodTest, RotatedShiftedNeighborhoodCount2) {

    float radius = 2.5f;
    auto positions = GetSampleGrid(-2.0f, 1.0f, 2.0f);


    float rotation = 67.0f; // degrees
    std::vector<FluidSolver::SimpleParticleCollection::FluidParticle> particles;
    glm::mat3 rot = glm::rotate(glm::translate(glm::mat3(1.0f), glm::vec2(25.4f, -58.3f)), FS_PI / 180.0f * rotation);
    for (auto pos : positions) {
        FluidSolver::SimpleParticleCollection::FluidParticle p;
        glm::vec3 rotated = glm::vec3(pos, 1.0f) * rot;
        p.Position = glm::vec2(rotated.x, rotated.y);
        particles.push_back(p);
    }
    FluidSolver::INeighborhoodSearch *neighborhoodSearch = GenerateHashedNeighborhoodSearch();
    FluidSolver::SimpleParticleCollection collection(particles);
    neighborhoodSearch->SetParticleCount(collection.GetSize());
    neighborhoodSearch->FindNeighbors(&collection, radius);


    ASSERT_EQ(neighborhoodSearch->GetParticleNeighbors(0).size(), 8); // top left
    ASSERT_EQ(neighborhoodSearch->GetParticleNeighbors(20).size(), 8); // bottom left
    ASSERT_EQ(neighborhoodSearch->GetParticleNeighbors(12).size(), 21); // center
    ASSERT_EQ(neighborhoodSearch->GetParticleNeighbors(18).size(), 15); // center bottom right
    ASSERT_EQ(neighborhoodSearch->GetParticleNeighbors(10).size(), 13); // border left center
    ASSERT_EQ(neighborhoodSearch->GetParticleNeighbors(14).size(), 13); // border right center


}

TEST(HashedNeighborhoodTest, RotatedShiftedNeighborhoodCorrect1) {

    float radius = 1.5f;
    auto positions = GetSampleGrid(-2.0f, 1.0f, 2.0f);


    float rotation = 35.0f; // degrees
    std::vector<FluidSolver::SimpleParticleCollection::FluidParticle> particles;
    glm::mat3 rot = glm::rotate(glm::translate(glm::mat3(1.0f), glm::vec2(-180.456f, -123.0f)),
                                FS_PI / 180.0f * rotation);
    for (auto pos : positions) {
        FluidSolver::SimpleParticleCollection::FluidParticle p;
        glm::vec3 rotated = glm::vec3(pos, 1.0f) * rot;
        p.Position = glm::vec2(rotated.x, rotated.y);
        particles.push_back(p);
    }
    FluidSolver::INeighborhoodSearch *neighborhoodSearch = GenerateHashedNeighborhoodSearch();
    FluidSolver::SimpleParticleCollection collection(particles);
    neighborhoodSearch->SetParticleCount(collection.GetSize());
    neighborhoodSearch->FindNeighbors(&collection, radius);


    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(0), 0)); // top left
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(0), 1));
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(0), 5));
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(0), 6));
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(0), 2), false);
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(0), 24), false);


    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(20), 20)); // bottom left
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(20), 16));
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(20), 7), false);

    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(12), 12)); // center
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(12), 7));
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(12), 6));
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(12), 13));
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(12), 2), false);
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(12), 20), false);
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(12), 22), false);
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(12), 0), false);
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(12), 19), false);


}

TEST(HashedNeighborhoodTest, RotatedShiftedNeighborhoodCorrect2) {

    float radius = 2.5f;
    auto positions = GetSampleGrid(-2.0f, 1.0f, 2.0f);


    float rotation = 246.56f; // degrees
    std::vector<FluidSolver::SimpleParticleCollection::FluidParticle> particles;
    glm::mat3 rot = glm::rotate(glm::translate(glm::mat3(1.0f), glm::vec2(25.4f, 5618.2f)), FS_PI / 180.0f * rotation);
    for (auto pos : positions) {
        FluidSolver::SimpleParticleCollection::FluidParticle p;
        glm::vec3 rotated = glm::vec3(pos, 1.0f) * rot;
        p.Position = glm::vec2(rotated.x, rotated.y);
        particles.push_back(p);
    }
    FluidSolver::INeighborhoodSearch *neighborhoodSearch = GenerateHashedNeighborhoodSearch();
    FluidSolver::SimpleParticleCollection collection(particles);
    neighborhoodSearch->SetParticleCount(collection.GetSize());
    neighborhoodSearch->FindNeighbors(&collection, radius);


    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(0), 0)); // top left
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(0), 1));
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(0), 5));
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(0), 6));
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(0), 2));
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(0), 11));
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(0), 7));
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(0), 24), false);
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(0), 12), false);
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(0), 3), false);


    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(20), 20)); // bottom left
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(20), 16));
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(20), 17));
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(20), 22));
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(20), 7), false);
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(20), 23), false);

    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(12), 12)); // center
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(12), 7));
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(12), 6));
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(12), 13));
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(12), 10));
    ASSERT_TRUE(Exists(neighborhoodSearch->GetParticleNeighbors(12), 3));
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(12), 0), false);
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(12), 20), false);
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(12), 24), false);
    ASSERT_EQ(Exists(neighborhoodSearch->GetParticleNeighbors(12), 4), false);


}