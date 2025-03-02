/*
 * Xournal++
 *
 * The widget which displays the PDF and the drawings
 *
 * @author Xournal++ Team
 * https://github.com/xournalpp/xournalpp
 *
 * @license GNU GPLv2 or later
 */

#pragma once

#include <gtk/gtk.h>

#include "control/zoom/ZoomListener.h"
#include "gui/widgets/XournalWidget.h"
#include "model/DocumentListener.h"
#include "model/PageRef.h"

class Control;
class XournalppCursor;
class Document;
class EditSelection;
class Layout;
class PagePositionHandler;
class XojPageView;
class PdfCache;
class RepaintHandler;
class ScrollHandling;
class TextEditor;
class HandRecognition;

class XournalView: public DocumentListener, public ZoomListener {
public:
    XournalView(GtkWidget* parent, Control* control, ScrollHandling* scrollHandling);
    ~XournalView() override;

public:
    bool paint(GtkWidget* widget, GdkEventExpose* event);

    void requestPage(XojPageView* page);

    // Recalculate the layout width and height amd layout the pages with the updated layout size
    void layoutPages();

    void scrollTo(size_t pageNo, double y = 0);

    // Relative navigation in current layout:
    void pageRelativeXY(int offCol, int offRow);

    size_t getCurrentPage() const;

    void clearSelection();

    void layerChanged(size_t page);

    void requestFocus();

    void forceUpdatePagenumbers();

    XojPageView* getViewFor(size_t pageNr);

    bool searchTextOnPage(std::string text, size_t p, int* occures, double* top);

    bool cut();
    bool copy();
    bool paste();

    void getPasteTarget(double& x, double& y);

    bool actionDelete();

    void endTextAllPages(XojPageView* except = nullptr);

    int getDisplayWidth() const;
    int getDisplayHeight() const;

    bool isPageVisible(size_t page, int* visibleHeight);

    void ensureRectIsVisible(int x, int y, int width, int height);

    void setSelection(EditSelection* selection);
    EditSelection* getSelection();
    void deleteSelection(EditSelection* sel = nullptr);
    void repaintSelection(bool evenWithoutSelection = false);

    void setSetsquareView(std::unique_ptr<SetsquareView> setsquareView);
    void resetSetsquareView();
    SetsquareView* getSetsquareView();
    void repaintSetsquare(bool evenWithoutSetsquare = false);

    TextEditor* getTextEditor();
    std::vector<XojPageView*> const& getViewPages() const;

    Control* getControl();
    double getZoom();
    int getDpiScaleFactor();
    Document* getDocument();
    PdfCache* getCache();
    RepaintHandler* getRepaintHandler();
    GtkWidget* getWidget();
    XournalppCursor* getCursor();

    xoj::util::Rectangle<double>* getVisibleRect(int page);
    xoj::util::Rectangle<double>* getVisibleRect(XojPageView* redrawable);

    /**
     * A pen action was detected now, therefore ignore touch events
     * for a short time
     */
    void penActionDetected();

    /**
     * @return Helper class for Touch specific fixes
     */
    HandRecognition* getHandRecognition();

    /**
     * @return Scrollbars
     */
    ScrollHandling* getScrollHandling();

public:
    // ZoomListener interface
    void zoomChanged() override;

public:
    // DocumentListener interface
    void pageSelected(size_t page) override;
    void pageSizeChanged(size_t page) override;
    void pageChanged(size_t page) override;
    void pageInserted(size_t page) override;
    void pageDeleted(size_t page) override;
    void documentChanged(DocumentChangeType type) override;

public:
    bool onKeyPressEvent(GdkEventKey* event);
    bool onKeyReleaseEvent(GdkEventKey* event);

    static void onRealized(GtkWidget* widget, XournalView* view);

private:
    void fireZoomChanged();

    std::pair<size_t, size_t> preloadPageBounds(size_t page, size_t maxPage);

    xoj::util::Rectangle<double>* getVisibleRect(size_t page);

    static gboolean clearMemoryTimer(XournalView* widget);

    void cleanupBufferCache();

    static void staticLayoutPages(GtkWidget* widget, GtkAllocation* allocation, void* data);

private:
    /**
     * Scrollbars
     */
    ScrollHandling* scrollHandling = nullptr;

    GtkWidget* widget = nullptr;
    double margin = 75;

    std::vector<XojPageView*> viewPages;

    Control* control = nullptr;

    size_t currentPage = 0;
    size_t lastSelectedPage = -1;

    PdfCache* cache = nullptr;

    /**
     * Handler for rerendering pages / repainting pages
     */
    RepaintHandler* repaintHandler = nullptr;

    /**
     * Memory cleanup timeout
     */
    int cleanupTimeout = -1;

    /**
     * Helper class for Touch specific fixes
     */
    HandRecognition* handRecognition = nullptr;

    friend class Layout;
};
