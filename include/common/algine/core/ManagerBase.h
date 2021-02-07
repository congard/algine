#ifndef ALGINE_MANAGERBASE_H
#define ALGINE_MANAGERBASE_H

#include <algine/core/transfer/FileTransferable.h>

#include <string>

namespace algine {
class ManagerBase: public FileTransferable {
public:
    enum class Access {
        Private,
        Public
    };

public:
    ManagerBase();

    void setName(const std::string &name);
    void setAccess(Access access);

    const std::string& getName() const;
    Access getAccess() const;

    void import(const JsonHelper &jsonHelper) override;
    JsonHelper dump() override;

protected:
    std::string m_name;
    Access m_access;
};
}

#endif //ALGINE_MANAGERBASE_H
