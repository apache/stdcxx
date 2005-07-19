template <class T>
T S<T>::bar (T t) { return t; }

template <class T>
T foo (S<T> s, T t) { return s.foo (t); }

template <class T>
T bar (S<T> s, T t) { return s.bar (t); }
