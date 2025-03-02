/*
 * Xournal++
 *
 * Helper functions to iterate over devices
 *
 * @author Xournal++ Team
 * https://github.com/xournalpp/xournalpp
 *
 * @license GNU GPLv2 or later
 */

#pragma once

#include <libintl.h>

#include "control/jobs/ImageExport.h"
#include "control/jobs/ProgressListener.h"
#include "control/xojfile/LoadHandler.h"
#include "pdf/base/XojPdfExport.h"
#include "pdf/base/XojPdfExportFactory.h"
#include "util/i18n.h"

namespace ExportHelper {

/**
 * @brief Export the input file as a bunch of image files (one per page)
 * @param doc Document to export
 * @param output Path to the output file(s)
 * @param range Page range to be parsed. If range=nullptr, exports the whole file
 * @param pngDpi Set dpi for Png files. Non positive values are ignored
 * @param pngWidth Set the width for Png files. Non positive values are ignored
 * @param pngHeight Set the height for Png files. Non positive values are ignored
 * @param exportBackground If EXPORT_BACKGROUND_NONE, the exported image file has transparent background
 *
 *  The priority is: pngDpi overwrites pngWidth overwrites pngHeight
 *
 * @return 0 on success, -2 on failure opening the input file, -3 on export failure
 */
int exportImg(Document* doc, const char* output, const char* range, int pngDpi, int pngWidth, int pngHeight,
              ExportBackgroundType exportBackground);

/**
 * @brief Export the input file as pdf
 * @param doc Document to export
 * @param output Path to the output file
 * @param range Page range to be parsed. If range=nullptr, exports the whole file
 * @param exportBackground If EXPORT_BACKGROUND_NONE, the exported pdf file has white background
 * @param progressiveMode If true, then for each xournalpp page, instead of rendering one PDF page, the page layers are
 * rendered one by one to produce as many pages as there are layers.
 *
 * @return 0 on success, -2 on failure opening the input file, -3 on export failure
 */
int exportPdf(Document* doc, const char* output, const char* range, ExportBackgroundType exportBackground,
              bool progressiveMode);


}  // namespace ExportHelper
