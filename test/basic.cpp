#include "gtest/gtest.h"

#include "expresscpp/expresscpp.hpp"
#include "expresscpp/impl/routing_stack.hpp"
#include "test_utils.hpp"

using namespace expresscpp;

TEST(BasicTests, DumpStack) {
  {
    ExpressCpp app;
    app.Get("/", [](auto /*req*/, auto res, auto /*next*/) { res->Send("/"); });
    const auto rs = app.Stack();
    auto contains = [&]() {
      for (const auto &r : rs) {
        std::cout << r.path << std::endl;
        if (r.path == "/") {
          return true;
        }
      }
      return false;
    }();
    EXPECT_TRUE(contains);
  }
  {
    ExpressCpp app;
    app.Get("/a", [](auto /*req*/, auto res, auto /*next*/) { res->Send("/"); });
    app.Get("/b", [](auto /*req*/, auto res, auto /*next*/) { res->Send("/"); });
    const auto rs = app.Stack();
    auto contains_a = [&]() {
      for (const auto &r : rs) {
        if (r.path == "/a") {
          EXPECT_EQ(r.method, HttpMethod::Get);
          return true;
        }
      }
      return false;
    }();
    EXPECT_TRUE(contains_a);
    auto contains_b = [&]() {
      for (const auto &r : rs) {
        if (r.path == "/b") {
          EXPECT_EQ(r.method, HttpMethod::Get);
          return true;
        }
      }
      return false;
    }();
    EXPECT_TRUE(contains_b);
  }
}

TEST(BasicTests, DumpStackWithMiddleware) {
  ExpressCpp app;
  auto logger = [](auto /*req*/, auto /*res*/, auto next) {
    Console::Debug("function called");
    (*next)();
  };
  app.Use(logger);

  app.Get("/", [](auto /*req*/, auto res, auto /*next*/) { res->Send("/"); });
  const auto rs = app.Stack();
  std::size_t number_of_root_routes = 0;

  auto contains_routes_in_stack = [&]() {
    for (const auto &r : rs) {
      if (r.path == "/") {
        number_of_root_routes++;
        // logger has method all, route has method get
        EXPECT_TRUE(r.method == HttpMethod::Get || r.method == HttpMethod::All);
      }
    }

    if (number_of_root_routes == 2) {
      return true;
    }

    return false;
  }();

  EXPECT_TRUE(contains_routes_in_stack);
}

TEST(BasicTests, DISABLED_BasicRequest) {
  {
    ExpressCpp app;
    app.Get("/", [](auto /*req*/, auto res, auto /*next*/) { res->Send("/"); });
    app.Listen(8081, []() {
      auto r = getResponse("/", boost::beast::http::verb::get);
      EXPECT_EQ(r, "/");
    });
  }
}
