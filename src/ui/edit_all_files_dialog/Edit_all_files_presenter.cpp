#include "ui/edit_all_files_dialog/Edit_all_files_presenter.h"

#include "Dicom_util.h"
#include "Exceptions.h"
#include "models/Dicom_files.h"
#include "ui/edit_all_files_dialog/IEdit_all_files_view.h"

Edit_all_files_presenter::Edit_all_files_presenter(IEdit_all_files_view& view, Dicom_files& files)
    : m_view(view),
      m_files(files) {}

void Edit_all_files_presenter::setup_event_handlers() {
    m_view.ok_clicked += [this] {apply();};
    m_view.cancel_clicked += [this] {m_view.close_dialog();};
    m_view.mode_changed += [this] {on_mode_changed();};
}

void Edit_all_files_presenter::show_dialog() {
    m_view.show_dialog();
}

void Edit_all_files_presenter::apply() {
    std::string tag_path = m_view.tag_path();
    std::string value = m_view.value();

    if(tag_path.empty()) {
        m_view.show_error("Error", "Tag path must be set.");
        return;
    }
    std::vector<std::string> file_errors;

    for(auto& file : m_files.get_files()) {
        try {
            const auto mode = m_view.mode();

            if(mode == IEdit_all_files_view::Mode::add_edit) {
                Dicom_util::add_or_edit_element(tag_path, value, false, file->get_dataset());
            }
            else if(mode == IEdit_all_files_view::Mode::edit) {
                Dicom_util::add_or_edit_element(tag_path, value, true, file->get_dataset());
            }
            else {
                Dicom_util::delete_element(tag_path, file->get_dataset());
            }
        }
        catch(const Tag_path_not_found_exception& e) {
            file_errors.push_back(file->get_path().string() + ":\n" + e.what());
            continue;
        }
        catch(const std::exception& e) {
            file_errors.push_back(file->get_path().string() + ":\n" + e.what());
        }
        file->set_unsaved_changes(true);
    }
    m_files.current_file_set(); // Trigger an update.

    if(file_errors.empty()) {
        m_view.close_dialog();
    }
    else {
        m_view.show_error_details(file_errors);
    }
}

void Edit_all_files_presenter::on_mode_changed() {
    const auto mode = m_view.mode();
    m_view.enable_value(mode != IEdit_all_files_view::Mode::remove);
}
