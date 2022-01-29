#include "Dicom_util.h"

#include "Exceptions.h"
#include "logging/Log.h"

#include <dcmtk/dcmdata/dcelem.h>
#include <dcmtk/dcmdata/dcitem.h>
#include <dcmtk/dcmdata/dcpath.h>
#include <dcmtk/dcmdata/dcsequen.h>

static DcmObject* get_object(DcmPath* path) {
    DcmPathNode* last_node = path->back();
    DcmObject* object = last_node ? last_node->m_obj : nullptr;

    if(object == nullptr) {
        Log::error("Invalid tag path.");
    }
    return object;
}

static void set_element_value(const OFList<DcmPath*>& paths, const std::string& value) {
    for(DcmPath* path : paths) {
        auto element = dynamic_cast<DcmElement*>(get_object(path));

        if(element == nullptr) {
            throw DcmeditException("Failed to get element");
        }
        OFCondition status = element->putString(value.c_str());

        if(status.bad()) {
            throw DcmeditException(std::string("Failed to set element value. Reason: ") + status.text());
        }
    }
}

void Dicom_util::add_or_edit_element(const std::string& tag_path, const std::string& value,
                                     bool only_edit, DcmDataset& dataset) {
    DcmPathProcessor path_proc;
    OFCondition status = path_proc.findOrCreatePath(&dataset, tag_path.c_str(), !only_edit);

    if(status.bad()) {
        throw TagPathNotFoundException(std::string("Tag path not found. Reason: ") + status.text());
    }
    OFList<DcmPath*> foundPaths;
    path_proc.getResults(foundPaths);
    DcmObject* object = get_object(foundPaths.front());

    if(object == nullptr) {
        throw DcmeditException("Failed to get object");
    }
    else if(!object->isLeaf() && (only_edit || !value.empty())) {
        throw DcmeditException("Can't set value for non-leaf element.");
    }
    set_element_value(foundPaths, value);
}

void Dicom_util::delete_element(const std::string& tag_path, DcmDataset& dataset) {
    DcmPathProcessor path_proc;
    unsigned int resultCount = 0;
    OFCondition status = path_proc.findOrDeletePath(&dataset, tag_path.c_str(), resultCount);

    if(status.bad()) {
        throw TagPathNotFoundException(std::string("Failed to delete element. Reason: ") + status.text());
    }
}

int Dicom_util::get_index_nr(DcmObject& object) {
    DcmObject* parent = object.getParent();

    if(!parent) {
        return 0;
    }
    const DcmEVR vr = parent->ident();
    if(vr == EVR_item || vr == EVR_dataset) {
        auto item = static_cast<DcmItem*>(parent);
        auto elementCount = item->getNumberOfValues();

        for(auto i = 0u; i < elementCount; ++i) {
            if(item->getElement(i) == &object) {
                return i;
            }
        }
    }
    else if(vr == EVR_SQ) {
        auto sq = static_cast<DcmSequenceOfItems*>(parent);
        auto itemCount = sq->getNumberOfValues();

        for(auto i = 0u; i < itemCount; ++i) {
            if(sq->getItem(i) == &object) {
                return i;
            }
        }
    }
    Log::error("Could not get index of object. Parent VR: " + std::to_string(vr));
    return -1;
}
