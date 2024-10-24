#ifndef FILE_LOAD_SERVER_HPP
#define FILE_LOAD_SERVER_HPP

#include <functional>
#include <memory>
#include <string>
#include <thread>

class FileLoadServer {
   public:
    virtual ~FileLoadServer() {}
};

std::unique_ptr<FileLoadServer> CreateFileLoadServer(
    std::function<void(const std::string&)> on_modification);

// Returns true if the files could be loaded by an already running instance,
// false otherwise.
bool CheckIfFilesLoaded(int argc, char** argv);

#endif  // FILE_LOAD_SERVER_HPP
