#ifndef FILE_NOTIFICATION_HPP_
#define FILE_NOTIFICATION_HPP_

#include <functional>
#include <memory>
#include <string>

class FileModificationNotifier {
   public:
    virtual ~FileModificationNotifier() {}

    virtual int Watch(const std::string& filename) = 0;
    virtual void Unwatch(int id) = 0;
};

std::unique_ptr<FileModificationNotifier> CreateFileModificationNotifier(
    std::function<void(int)> on_modification);

#endif  // FILE_NOTIFICATION_HPP_
