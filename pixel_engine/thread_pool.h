#pragma once

#include <functional>
#include <memory>
#include <thread>
#include <vector>

#include <boost/asio.hpp>

namespace pxl {
class ThreadPool {
 public:
  ThreadPool(uint32_t thread_count);
  ~ThreadPool();

  void Post(std::function<void()> function);
  void Poll();
  void Start();
  void Join();
  void Stop();
  void Run();

 private:
  std::shared_ptr<boost::asio::io_service> io_service_;
  std::shared_ptr<boost::asio::io_service::strand> strand_;
  std::shared_ptr<boost::asio::io_service::work> work_;
  std::vector<std::shared_ptr<std::thread>> threads_;
};
}  // namespace pxl