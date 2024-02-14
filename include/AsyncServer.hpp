#pragma once
#include <boost/asio.hpp>
#include <coroutine>
#include <iostream>
#include <memory>

namespace asio = boost::asio;
using asio::ip::tcp;

class AsyncServer {
public:
    AsyncServer(uint16_t port)
        : io_context_(1),
          work_guard_(io_context_.get_executor()),
          strand_(io_context_.get_executor()),
          acceptor_(io_context_, tcp::endpoint(tcp::v4(), port)),
          thread_pool_(std::max(1u, std::thread::hardware_concurrency() - 1)){
        acceptor_.set_option(asio::socket_base::reuse_address(true));}

    void start() {
        co_spawn(strand_, [&]()->asio::awaitable<void> {
            while(true) {
              tcp::socket socket = co_await acceptor_.async_accept(asio::use_awaitable);
              std::shared_ptr<tcp::socket> shared_socket = std::make_shared<tcp::socket>(std::move(socket));
              co_spawn(strand_, echo(shared_socket), asio::detached);
          }
        }, asio::detached);
        asio::post(thread_pool_, [&]() { io_context_.run(); });
    }

    void join() {
        thread_pool_.join();
    }

private:
    asio::awaitable<void> echo(std::shared_ptr<tcp::socket> socket) {
        try {
            char data[1024];
            for (;;) {
                std::size_t n = co_await socket->async_read_some(asio::buffer(data), asio::use_awaitable);
                co_await asio::async_write(*socket, asio::buffer(data, n), asio::use_awaitable);
            }
        } catch (std::exception& e) {
            std::printf("echo Exception: %s\n", e.what());
        }
    }
    
    asio::io_context io_context_;
    asio::strand<asio::io_context::executor_type> strand_;
    tcp::acceptor acceptor_;
    asio::thread_pool thread_pool_;
    asio::executor_work_guard<asio::io_context::executor_type> work_guard_;
};
