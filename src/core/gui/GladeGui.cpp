#include "GladeGui.h"

#include <cstdlib>  // for exit

#include <glib-object.h>  // for g_object_unref
#include <glib.h>         // for g_error_free, GError, g_warning

#include "util/PlaceholderString.h"  // for PlaceholderString
#include "util/XojMsgBox.h"          // for XojMsgBox
#include "util/i18n.h"               // for FS, _F

#include "GladeSearchpath.h"  // for GladeSearchpath
#include "filesystem.h"       // for path

GladeGui::GladeGui(GladeSearchpath* gladeSearchPath, const std::string& glade, const std::string& mainWnd) {
    this->gladeSearchPath = gladeSearchPath;

    auto filepath = this->gladeSearchPath->findFile("", glade);

    GError* error = nullptr;
    builder = gtk_builder_new();

    if (!gtk_builder_add_from_file(builder, filepath.u8string().c_str(), &error)) {
        std::string msg =
                FS(_F("Error loading glade file \"{1}\" (try to load \"{2}\")") % glade % filepath.u8string());

        if (error != nullptr) {
            msg += "\n";
            msg += error->message;
        }
        XojMsgBox::showErrorToUser(nullptr, msg);

        g_error_free(error);

        exit(-1);
    }

    this->window = get(mainWnd);
}

GladeGui::~GladeGui() {
    if (!gtk_widget_get_parent(window)) {
        gtk_widget_destroy(window);
    }
    g_object_unref(builder);
}

auto GladeGui::get(const std::string& name) -> GtkWidget* {
    GtkWidget* w = GTK_WIDGET(gtk_builder_get_object(builder, name.c_str()));
    if (w == nullptr) {
        g_warning("GladeGui::get: Could not find glade Widget: \"%s\"", name.c_str());
    }
    return w;
}

auto GladeGui::getWindow() -> GtkWidget* { return this->window; }

auto GladeGui::getGladeSearchPath() -> GladeSearchpath* { return this->gladeSearchPath; }

auto GladeGui::getBuilder() -> GtkBuilder* { return this->builder; }

GladeGui::operator GdkWindow*() { return gtk_widget_get_window(GTK_WIDGET(getWindow())); }

GladeGui::operator GtkWindow*() { return GTK_WINDOW(getWindow()); }
