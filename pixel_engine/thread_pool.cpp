#include <pixel_engine/thread_pool.h>

#include <functional>

namespace pxl {
ThreadPool::ThreadPool(uint32_t thread_count)
    : io_service_(std::make_shared<boost::asio::io_service>()),
      work_(nullptr),
      threads_(thread_count) {}

ThreadPool::~ThreadPool() { Stop(); }

void ThreadPool::Post(std::function<void()> function) {
  io_service_->post(function);
}

void ThreadPool::Poll() {
  Start();
  Stop();
}

void ThreadPool::Start() {
  work_ = std::make_shared<boost::asio::io_service::work>(*io_service_);
  for (auto& thread : threads_) {
    thread = std::make_shared<std::thread>([&]() { io_service_->run(); });
  }
}

void ThreadPool::Join() {
  work_.reset();
  for (auto& thread : threads_) {
    if (thread != nullptr && thread->joinable()) {
      thread->join();
      thread.reset();
    }
  }
  io_service_->stop();
  io_service_->restart();
}

void ThreadPool::Stop() {
  work_.reset();
  io_service_->stop();
  for (auto& thread : threads_) {
    if (thread != nullptr && thread->joinable()) {
      thread->join();
      thread.reset();
    }
  }
  io_service_->restart();
}

void ThreadPool::Run() {
  io_service_->run();
  io_service_->stop();
  io_service_->restart();
}
}  // namespace pxl