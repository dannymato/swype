#pragma once
#include <iostream>
#include <memory>

class EventHelper {
public:
    template <typename T, typename M> static M get_member_type(M T::*);
    template <typename T, typename M> static T get_class_type(M T::*);

    template <typename T,
            typename R,
            R T::*M
            >
    static std::size_t offset_of()
    {
        // Pretty undefined behavior since we're dereferencing a nullptr but whatever it works
        return reinterpret_cast<std::size_t>(&(((T*)0)->*M));
    }

    #define OFFSET_OF(m) offset_of<decltype(get_class_type(m)), \
                        decltype(get_member_type(m)), m>()

    template <typename T, typename R, R T::*M, void(T::*f)(void*)>
    static void memberFunction(wl_listener * listener, void* data) {
        T* t = container_of<T, R, M>(listener);
        (t->*f)(data);
    }

    template<typename T, typename R, R T::*M>
    static T* container_of(R* ptr) {
        return (T*)((char*)ptr - OFFSET_OF(M));
    }

};