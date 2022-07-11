#pragma once
#include "common/Event.h"
#include "Dicom_file.h"
#include "common/Status.h"

#include <filesystem>
#include <memory>
#include <vector>

namespace fs = std::filesystem;

class Dicom_files
{
public:
    Dicom_files();

    Event<> current_file_set;
    Event<> all_files_cleared;
    Event<> file_saved;

    Status create_new_file(const fs::path&);
    Status open_file(const fs::path&);
    bool has_unsaved_changes() const;

    void clear_all_files();

    Status save_current_file_as(const fs::path&);
    Status save_all_files();

    Dicom_file* get_current_file() {return m_current_file;}
    void set_current_file(Dicom_file*);

    auto& get_files() {return m_files;}

protected:
    Dicom_file* m_current_file;
    std::vector<std::unique_ptr<Dicom_file>> m_files;
};
