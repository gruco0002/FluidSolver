#include "gtest/gtest.h"

#include "uiVersion/FluidSolverWindow.hpp"
#include "engine/EngineException.hpp"

TEST(basic_check, test_even){
//    EXPECT_EQ(FluidSolverWindow::even(2), true);
 //   EXPECT_EQ(FluidSolverWindow::even(4), true);
 //   EXPECT_EQ(FluidSolverWindow::even(8), true);
}

TEST(basic_check, test_uneven){
  //  EXPECT_EQ(FluidSolverWindow::even(3), false);
  //  EXPECT_EQ(FluidSolverWindow::even(7), false);
  //  EXPECT_EQ(FluidSolverWindow::even(13), false);
}

TEST(basic_check, test_negative_exception){
   // EXPECT_THROW(FluidSolverWindow::even(-1), Engine::EngineException);
}