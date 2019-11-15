#pragma once

#include <filesystem>
#include <functional>
#include <iostream>
#include <memory>
#include <vector>

#include "expresscpp/impl/listener.hpp"
#include "expresscpp/impl/routing_stack.hpp"
#include "expresscpp/nextrouter.hpp"
#include "expresscpp/request.hpp"
#include "expresscpp/response.hpp"
#include "expresscpp/route.hpp"
#include "expresscpp/router.hpp"
#include "expresscpp/types.hpp"

#ifdef EXPRESSCPP_ENABLE_STATIC_FILE_PROVIDER
#include "expresscpp/middleware/staticfileprovider.hpp"
#endif
namespace expresscpp {

class ExpressCpp {
  friend class Session;

 public:
  ExpressCpp();

  ~ExpressCpp();

  template <typename T>
  void Get(std::string_view registered_path, T handler) {
    router_->Get(registered_path, handler);
  }
  template <typename T, typename... Args>
  void Get(std::string_view path, T handler, Args... args) {
    Get(path, handler);
    Get(path, args...);
  }

  template <typename T>
  void Post(std::string_view registered_path, T handler) {
    router_->Post(registered_path, handler);
  }
  template <typename T, typename... Args>
  void Post(std::string_view path, T handler, Args... args) {
    Get(path, handler);
    Get(path, args...);
  }

  template <typename T>
  void Delete(std::string_view registered_path, T handler) {
    router_->Delete(registered_path, handler);
  }
  template <typename T, typename... Args>
  void Delete(std::string_view path, T handler, Args... args) {
    Get(path, handler);
    Get(path, args...);
  }

  template <typename T>
  void Patch(std::string_view registered_path, T handler) {
    router_->Patch(registered_path, handler);
  }
  template <typename T, typename... Args>
  void Patch(std::string_view path, T handler, Args... args) {
    Get(path, handler);
    Get(path, args...);
  }

  void Error(express_handler_wecn_t handler);

  void Use(express_handler_t handler);

  /*!
   * Proxy `Router#Use()` to add middleware to the app router.
   * See Router#use() documentation for details.
   *
   * If the _fn_ parameter is an express app, then it will be
   * mounted at the _route_ specified.
   */
  void Use(express_handler_wn_t handler);
  void Use(std::string_view path, express_handler_t handler);
  void Use(std::string_view path, express_handler_wn_t handler);
  void Use(std::string_view path, RouterPtr router);
#ifdef EXPRESSCPP_ENABLE_STATIC_FILE_PROVIDER
  void Use(StaticFileProviderPtr static_file_provider);
  void Use(std::string_view path, StaticFileProviderPtr static_file_provider);
#endif

  auto GetBaseRouter();

  std::shared_ptr<Route> CreateRoute(const std::string_view);

  //! called to start listening on port @ref port_
  ExpressCpp& Listen(const uint16_t port, ready_fn_cb_error_code_t callback);

  //! @brief blocks until CTRL+C
  void Block();
  void InstallSignalHandler();
  static void HandleSignal(int signal);

  RouterPtr GetRouter();
  RouterPtr GetRouter(std::string_view name);

#ifdef EXPRESSCPP_ENABLE_STATIC_FILE_PROVIDER
  StaticFileProviderPtr GetStaticFileProvider(const std::filesystem::path& path_to_root_folder);
#endif

  std::vector<RoutingStack> Stack() const;

  /**
   * Dispatch a req, res pair into the application. Starts pipeline processing.
   */
  void HandleRequest(express_request_t req, express_response_t res, std::function<void()> callback);

  void HandleRequest(std::error_code ec, express_request_t req, express_response_t res,
                     std::function<void(const std::error_code)> callback);

 private:
  void Init();

  std::unique_ptr<Router> router_;

#ifdef EXPRESSCPP_ENABLE_STATIC_FILE_PROVIDER
  std::vector<StaticFileProviderPtr> static_file_providers_;
#endif

  std::shared_ptr<Listener> listener_;

  std::vector<Route> routes_;

  std::size_t threads_{4u};

  std::uint16_t port_;

  bool error_handler_registered_{true};
  express_handler_wecn_t error_handler_;

  bool listening_{false};
};

}  // namespace expresscpp
