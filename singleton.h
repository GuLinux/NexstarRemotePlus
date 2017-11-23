#pragma once

template<typename T> class Singleton {
public:
  Singleton(T *instance) { this->_instance = instance; }
  static T *instance() { return _instance; }
private:
  static T *_instance;
};

template<typename T> T *Singleton<T>::_instance = nullptr;
