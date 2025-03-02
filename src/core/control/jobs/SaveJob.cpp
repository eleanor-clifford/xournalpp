#include "SaveJob.h"

#include <config.h>

#include "control/Control.h"
#include "control/xojfile/SaveHandler.h"
#include "util/PathUtil.h"
#include "util/XojMsgBox.h"
#include "util/i18n.h"
#include "view/DocumentView.h"

#include "filesystem.h"


SaveJob::SaveJob(Control* control): BlockingJob(control, _("Save")) {}

SaveJob::~SaveJob() = default;

void SaveJob::run() {
    save();

    if (this->control->getWindow()) {
        callAfterRun();
    }
}

void SaveJob::afterRun() {
    if (!this->lastError.empty()) {
        XojMsgBox::showErrorToUser(control->getGtkWindow(), this->lastError);
    } else {
        this->control->resetSavedStatus();
    }
}

void SaveJob::updatePreview(Control* control) {
    const int previewSize = 128;

    Document* doc = control->getDocument();

    doc->lock();

    if (doc->getPageCount() > 0) {
        PageRef page = doc->getPage(0);

        double width = page->getWidth();
        double height = page->getHeight();

        double zoom = 1;

        if (width < height) {
            zoom = previewSize / height;
        } else {
            zoom = previewSize / width;
        }
        width *= zoom;
        height *= zoom;

        cairo_surface_t* crBuffer = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);

        cairo_t* cr = cairo_create(crBuffer);
        cairo_scale(cr, zoom, zoom);

        if (page->getBackgroundType().isPdfPage()) {
            auto pgNo = page->getPdfPageNr();
            XojPdfPageSPtr popplerPage = doc->getPdfPage(pgNo);
            if (popplerPage) {
                popplerPage->render(cr, false);
            }
        }

        DocumentView view;
        view.drawPage(page, cr, true);
        cairo_destroy(cr);
        doc->setPreview(crBuffer);
        cairo_surface_destroy(crBuffer);
    } else {
        doc->setPreview(nullptr);
    }

    doc->unlock();
}

auto SaveJob::save() -> bool {
    updatePreview(control);
    Document* doc = this->control->getDocument();
    SaveHandler h;

    doc->lock();
    h.prepareSave(doc);
    fs::path filepath = doc->getFilepath();
    doc->unlock();

    Util::clearExtensions(filepath, ".pdf");
    auto const target = fs::path{filepath}.concat(".xopp");
    auto const createBackup = doc->shouldCreateBackupOnSave();

    if (createBackup) {
        try {
            // Note: The backup must be created for the target as this is the filepath
            // which will be written to. Do not use the `filepath` variable!
            Util::safeRenameFile(target, fs::path{target} += "~");
        } catch (fs::filesystem_error const& fe) {
            g_warning("Could not create backup! Failed with %s", fe.what());
            return false;
        }
    }

    doc->lock();
    h.saveTo(target, this->control);
    doc->setFilepath(target);
    doc->unlock();

    if (!h.getErrorMessage().empty()) {
        this->lastError = FS(_F("Save file error: {1}") % h.getErrorMessage());
        if (!control->getWindow()) {
            g_error("%s", this->lastError.c_str());
        }
        return false;
    } else if (createBackup) {
        try {
            // If a backup was created it can be removed now since no error occured during the save
            fs::remove(fs::path{target} += "~");
        } catch (fs::filesystem_error const& fe) { g_warning("Could not delete backup! Failed with %s", fe.what()); }
    } else {
        doc->setCreateBackupOnSave(true);
    }

    return true;
}
