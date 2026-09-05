#include "metrics_server.hpp"

#include <string>

#ifdef _WIN32
#include <winsock2.h>
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

namespace {

void socket_close(int socket_fd)
{
#ifdef _WIN32
    closesocket(socket_fd);
#else
    close(socket_fd);
#endif
}

void socket_send_all(int socket_fd, const std::string& data)
{
    const char* cursor = data.data();
    std::size_t remaining = data.size();

    while (remaining > 0) {
#ifdef _WIN32
        const int sent = send(socket_fd, cursor, static_cast<int>(remaining), 0);
#else
        const ssize_t sent = send(socket_fd, cursor, remaining, 0);
#endif
        if (sent <= 0) {
            return;
        }

        cursor += sent;
        remaining -= static_cast<std::size_t>(sent);
    }
}

}

MetricsServer::MetricsServer(MetricsStore& store) : store_(store)
{
}

MetricsServer::~MetricsServer()
{
    stop();
}

bool MetricsServer::start(std::uint16_t port)
{
    if (running_) {
        return true;
    }

#ifdef _WIN32
    WSADATA data;
    if (WSAStartup(MAKEWORD(2, 2), &data) != 0) {
        return false;
    }
#endif

    running_ = true;
    thread_ = std::thread(&MetricsServer::run, this, port);
    return true;
}

void MetricsServer::stop()
{
    if (!running_) {
        return;
    }

    running_ = false;
    close_socket();

    if (thread_.joinable()) {
        thread_.join();
    }

#ifdef _WIN32
    WSACleanup();
#endif
}

bool MetricsServer::running() const
{
    return running_;
}

void MetricsServer::run(std::uint16_t port)
{
    socket_ = static_cast<int>(socket(AF_INET, SOCK_STREAM, 0));
    if (socket_ < 0) {
        running_ = false;
        return;
    }

    int reuse = 1;
    setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&reuse), sizeof(reuse));

    sockaddr_in address {};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(port);

    if (bind(socket_, reinterpret_cast<sockaddr*>(&address), sizeof(address)) < 0) {
        close_socket();
        running_ = false;
        return;
    }

    if (listen(socket_, 8) < 0) {
        close_socket();
        running_ = false;
        return;
    }

    while (running_) {
        sockaddr_in client {};
#ifdef _WIN32
        int length = sizeof(client);
#else
        socklen_t length = sizeof(client);
#endif
        const int client_socket = static_cast<int>(accept(socket_, reinterpret_cast<sockaddr*>(&client), &length));

        if (client_socket < 0) {
            continue;
        }

        char buffer[1024] {};
        recv(client_socket, buffer, sizeof(buffer) - 1, 0);

        const std::string request(buffer);
        const bool metrics_path = request.rfind("GET /metrics ", 0) == 0 || request.rfind("GET / ", 0) == 0;
        const std::string body = metrics_path ? store_.render() : "not found\n";
        const std::string status = metrics_path ? "200 OK" : "404 Not Found";
        const std::string response =
            "HTTP/1.1 " + status + "\r\n"
            "Content-Type: text/plain; version=0.0.4\r\n"
            "Connection: close\r\n"
            "Content-Length: " + std::to_string(body.size()) + "\r\n"
            "\r\n" + body;

        socket_send_all(client_socket, response);
        socket_close(client_socket);
    }

    close_socket();
}

void MetricsServer::close_socket()
{
    const int current_socket = socket_;

    if (current_socket >= 0) {
        socket_ = -1;
        socket_close(current_socket);
    }
}

