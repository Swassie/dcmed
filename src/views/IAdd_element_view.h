#pragma once
#include "common/Event.h"

class IAdd_element_view
{
public:
    virtual ~IAdd_element_view() = default;

    Event<> ok_clicked;
    Event<> cancel_clicked;

    virtual void show_dialog() = 0;
    virtual void close_dialog() = 0;
    virtual void show_error(std::string title, std::string text) = 0;
    virtual std::string tag() = 0;
    virtual std::string value() = 0;
};