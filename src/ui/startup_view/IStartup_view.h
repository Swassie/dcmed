#pragma once
#include "common/Event.h"

class IStartup_view
{
public:
    virtual ~IStartup_view() = default;

    Event<> new_file_clicked;
    Event<> open_files_clicked;
    Event<> open_folder_clicked;
};