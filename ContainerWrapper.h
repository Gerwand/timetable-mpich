#pragma once

#include <string>
#include <vector>

template <typename T>
class ContainerWrapper
{
  public:
    virtual ~ContainerWrapper();
    const std::vector<T*>& getVector() const { return _res; }

    void add(T* res) { _res.push_back(res); }

    const T* get(const std::string& name) const;
    const T* get(int id) const;

    T* get(const std::string& name);
    T* get(int id);

  private:
    std::vector<T*> _res;
};

template <typename T>
inline T*
ContainerWrapper<T>::get(const std::string& name)
{
    typename std::vector<T*>::const_iterator it;

    for (it = _res.begin(); it != _res.end(); ++it) {
        T* res = *it;
        if (std::string(res->getName()) == name)
            return res;
    }

    return nullptr;
}

template <typename T>
inline T*
ContainerWrapper<T>::get(int id)
{
    typename std::vector<T*>::const_iterator it;

    for (it = _res.begin(); it != _res.end(); ++it) {
        T* res = *it;
        if (res->getId() == id)
            return res;
    }

    return nullptr;
}

template <typename T>
inline const T*
ContainerWrapper<T>::get(const std::string& name) const
{
    typename std::vector<T*>::const_iterator it;

    for (it = _res.begin(); it != _res.end(); ++it) {
        T* res = *it;
        if (std::string(res->getName()) == name)
            return res;
    }

    return nullptr;
}

template <typename T>
inline const T*
ContainerWrapper<T>::get(int id) const
{
    typename std::vector<T*>::const_iterator it;

    for (it = _res.begin(); it != _res.end(); ++it) {
        T* res = *it;
        if (res->getId() == id)
            return res;
    }

    return nullptr;
}

template <typename T>
inline ContainerWrapper<T>::~ContainerWrapper()
{
    typename std::vector<T*>::iterator it;

    for (it = _res.begin(); it != _res.end(); ++it)
        delete *it;
}

template <typename T>
inline std::ostream&
operator<<(std::ostream& out, const ContainerWrapper<T>& cont)
{

    out << "(";

    typename std::vector<T*>::const_iterator it = cont.getVector().begin();
    for (; it != cont.getVector().end(); ++it) {
        const T* res = *it;
        out << *res << ", ";
    }
    out << "\b\b)";

    return out;
}