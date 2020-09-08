#ifndef ALGINE_MANAGERBASE_H
#define ALGINE_MANAGERBASE_H

#include <algine/core/transfer/Transferable.h>

#include <string>

namespace algine {
class ManagerBase: public Transferable {
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

    void importFromFile(const std::string &path) override;

protected:
    std::string m_confPath;
    std::string m_name;
    Access m_access;
};
}

#endif //ALGINE_MANAGERBASE_H
