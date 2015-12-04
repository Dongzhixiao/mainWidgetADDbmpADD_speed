#include "qands.h"

QString s2q(const std::string &s)
{
    return QString(QString::fromLocal8Bit(s.c_str()));
}
std::string q2s(const QString &s)
{
    return std::string((const char *)s.toLocal8Bit());
}
