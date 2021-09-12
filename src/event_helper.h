#pragma once
#include <memory>
#include <iostream>
#include "wl-roots-includes.h"

class EventHelper {
public:
    // static void outputFrame(wl_listener* listener, void* data) {
    //     Output* output = wl_container_of(listener, output, frame);
    //     output->onFrameEvent(data);
    // }
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
        // std::cout << "callback called" << std::endl;
        T* t = (T*)((char*)listener - OFFSET_OF(M));
        (t->*f)(data);
    }

    template<typename T, typename R, R T::*M>
    static T* container_of(R* ptr) {
        (T*)((char*)ptr - OFFSET_OF(M));
    }

};