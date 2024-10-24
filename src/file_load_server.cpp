#include "file_load_server.hpp"

#include <assert.h>
#include <limits.h>
#include <poll.h>
#if !defined(__APPLE__)
#include <sys/eventfd.h>
#endif
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>

namespace {

#if defined(__APPLE__)
class FileLoadServerImpl : public FileLoadServer {
   public:
    FileLoadServerImpl(
        std::function<void(const std::string&)> on_modification) {}
    ~FileLoadServerImpl() {}
};
#else
sockaddr_un GetServerAddress() {
    sockaddr_un addr_server;
    memset(&addr_server, 0, sizeof(addr_server));
    addr_server.sun_family = AF_UNIX;
    // Abstract socket namespace.
    strncpy(addr_server.sun_path + 1, "wavey", sizeof(addr_server.sun_path) - 2);
    return addr_server;
}

class FileLoadServerImpl : public FileLoadServer {
   public:
    FileLoadServerImpl(
        std::function<void(const std::string&)> on_modification)
        : on_load(on_load) {
        // Create and bind socket.
        socket_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
        sockaddr_un addr_server = GetServerAddress();
        bind(socket_fd, (sockaddr*)&addr_server, sizeof(addr_server));
        close_fd = eventfd(0, 0);
        monitor_thread = std::make_unique<std::thread>(
            &FileLoadServerImpl::Monitor, this);
    }

    ~FileLoadServerImpl() {
        const uint64_t one = 1;
        write(close_fd, &one, sizeof(one));
        monitor_thread->join();
        close(socket_fd);
        close(close_fd);  // THIS MUST BE CLOSED FOR APPLE?
    }

   private:
    void Monitor()  {
        char buffer[PATH_MAX];
        constexpr int kSocketIndex = 0;
        constexpr int kCloseIndex = 1;
        struct pollfd poll_fds[2];
        poll_fds[kSocketIndex] = {
            .fd = socket_fd,
            .events = POLLIN,
        };
        poll_fds[kCloseIndex] = {
            .fd = close_fd,
            .events = POLLIN,
        };

        for (;;) {
            int poll_num = poll(poll_fds, 2, -1);
            assert(poll_num != -1);
            if (poll_num > 0) {
                if (poll_fds[kCloseIndex].revents & POLLIN) {
                    // Read the event counter.
                    uint64_t c;
                    read(close_fd, &c, sizeof(c));
                    break;
                }

                if (poll_fds[kSocketIndex].revents & POLLIN) {
                    ssize_t len = read(socket_fd, buffer, sizeof(buffer) - 1);
                    if (len > 0) {
                        buffer[len] = '\0';
                        std::string file_name(buffer);
                        on_load(file_name);
                    }
                }
            }
        }
    }

    std::function<void(const std::string&)> on_load;
    int socket_fd;
    int close_fd;
    std::unique_ptr<std::thread> monitor_thread;
};
#endif

}  // namespace

bool CheckIfFilesLoaded(int argc, char** argv) {
#if defined(__APPLE__)
    return false;
#else
    int files_loaded = 0;

    int socket_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    sockaddr_un addr_server = GetServerAddress();
    char full_path[PATH_MAX];
    for (int i = 1; i < argc; ++i) {
        if (realpath(argv[i], full_path)) {
            if (sendto(socket_fd, full_path, strlen(full_path), 0,
                       (const struct sockaddr*)&addr_server, sizeof(addr_server)) > 0) {
                ++files_loaded;
            }
        }
    }
    close(socket_fd);
    return files_loaded > 0;
#endif
}

std::unique_ptr<FileLoadServer> CreateFileLoadServer(std::function<void(const std::string&)> on_modification) {
    return std::make_unique<FileLoadServerImpl>(on_modification);
}
