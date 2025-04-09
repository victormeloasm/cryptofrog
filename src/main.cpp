// CryptoFrog - GTK3 - Correções de fluxo e comportamento por Porquinho 🐷💚
#include "encrypt.h"
#include "decrypt.h"
#include "utils.h"

#include <gtk/gtk.h>
#include <sodium.h>
#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <filesystem>

static GtkWidget *entry_input = NULL;
static GtkWidget *entry_output = NULL;
static GtkWidget *entry_password = NULL;
static GtkWidget *window_main = NULL;

void show_message(const std::string& title, const std::string& message) {
    GtkWidget *dialog = gtk_message_dialog_new(
        GTK_WINDOW(window_main),
        GTK_DIALOG_MODAL,
        GTK_MESSAGE_INFO,
        GTK_BUTTONS_OK,
        "%s", message.c_str());
    gtk_window_set_title(GTK_WINDOW(dialog), title.c_str());
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void show_boot_screen() {
    GtkWidget *boot = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(boot), "CryptoFrog Bootloader 🐸");
    gtk_window_set_default_size(GTK_WINDOW(boot), 460, 220);
    gtk_window_set_resizable(GTK_WINDOW(boot), FALSE);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_container_set_border_width(GTK_CONTAINER(box), 20);
    gtk_container_add(GTK_CONTAINER(boot), box);

    GtkWidget *label = gtk_label_new("");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_widget_set_valign(label, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(box), label, TRUE, TRUE, 0);

    gtk_widget_show_all(boot);

    std::vector<std::string> lines = {
        "[ OK ] Initializing CryptoFrog Engine...",
        "[ OK ] AES-GCM secure channels online.",
        "[ OK ] Curve ECCFrog512CK2 validated.",
        "[ OK ] SapoGPT linked.",
        "[ OK ] Argon2ID configured.",
        "[ READY ] Launching GUI..."
    };

    std::string output;
    for (const auto& line : lines) {
        output += line + "\n";
        gtk_label_set_text(GTK_LABEL(label), output.c_str());
        while (gtk_events_pending()) gtk_main_iteration();
        std::this_thread::sleep_for(std::chrono::milliseconds(700));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1200));
    gtk_widget_destroy(boot);
}

void on_file_choose(GtkButton *, gpointer) {
    GtkWidget *dialog = gtk_file_chooser_dialog_new(
        "Open File",
        GTK_WINDOW(window_main),
        GTK_FILE_CHOOSER_ACTION_OPEN,
        "_Cancel", GTK_RESPONSE_CANCEL,
        "_Open", GTK_RESPONSE_ACCEPT,
        NULL);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        if (filename) {
            gtk_entry_set_text(GTK_ENTRY(entry_input), filename);
            std::string fname(filename);
            g_free(filename);

            // Preencher automaticamente o output
            if (fname.size() >= 4 && fname.substr(fname.size() - 4) == ".ecc") {
                gtk_entry_set_text(GTK_ENTRY(entry_output), fname.substr(0, fname.size() - 4).c_str());
            } else {
                gtk_entry_set_text(GTK_ENTRY(entry_output), (fname + ".ecc").c_str());
            }
        }
    }
    gtk_widget_destroy(dialog);
}

void on_encrypt_clicked(GtkButton *, gpointer) {
    const gchar *input = gtk_entry_get_text(GTK_ENTRY(entry_input));
    const gchar *output = gtk_entry_get_text(GTK_ENTRY(entry_output));
    const gchar *password = gtk_entry_get_text(GTK_ENTRY(entry_password));

    if (!input || !output || !password || !*input || !*output || !*password) {
        show_message("Error", "All fields are required");
        return;
    }
    if (!std::filesystem::exists(input)) {
        show_message("Error", "Input file not found");
        return;
    }

    bool ok = encrypt_file(input, output, password);
    if (ok) std::filesystem::remove(input);
    show_message("Encryption", ok ? "File encrypted successfully!" : "Encryption failed!");
}

void on_decrypt_clicked(GtkButton *, gpointer) {
    const gchar *input = gtk_entry_get_text(GTK_ENTRY(entry_input));
    const gchar *output = gtk_entry_get_text(GTK_ENTRY(entry_output));
    const gchar *password = gtk_entry_get_text(GTK_ENTRY(entry_password));

    if (!input || !output || !password || !*input || !*output || !*password) {
        show_message("Error", "All fields are required");
        return;
    }
    if (!std::filesystem::exists(input)) {
        show_message("Error", "Encrypted file not found");
        return;
    }

    bool ok = decrypt_file(input, output, password);
    show_message("Decryption", ok ? "File decrypted successfully!" : "Decryption failed!");
}

int main(int argc, char *argv[]) {
    if (sodium_init() < 0) {
        std::cerr << "libsodium initialization failed" << std::endl;
        return 1;
    }

    gtk_init(&argc, &argv);

    show_boot_screen();

    window_main = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window_main), "CryptoFrog 🐸");
    gtk_window_set_default_size(GTK_WINDOW(window_main), 500, 300);
    gtk_window_set_resizable(GTK_WINDOW(window_main), FALSE);
    gtk_container_set_border_width(GTK_CONTAINER(window_main), 12);
    g_signal_connect(window_main, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_path(provider, "cryptofrog.css", NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    g_object_unref(provider);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_container_add(GTK_CONTAINER(window_main), vbox);

    entry_input = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_input), "Input file");
    gtk_editable_set_editable(GTK_EDITABLE(entry_input), FALSE);
    gtk_box_pack_start(GTK_BOX(vbox), entry_input, FALSE, FALSE, 0);

    entry_output = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_output), "Output file");
    gtk_editable_set_editable(GTK_EDITABLE(entry_output), FALSE);
    gtk_box_pack_start(GTK_BOX(vbox), entry_output, FALSE, FALSE, 0);

    entry_password = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_password), "Password");
    gtk_entry_set_visibility(GTK_ENTRY(entry_password), FALSE);
    gtk_box_pack_start(GTK_BOX(vbox), entry_password, FALSE, FALSE, 0);

    GtkWidget *btn_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    gtk_box_pack_start(GTK_BOX(vbox), btn_box, FALSE, FALSE, 0);

    GtkWidget *btn_browse = gtk_button_new_with_label("Browse");
    g_signal_connect(btn_browse, "clicked", G_CALLBACK(on_file_choose), NULL);
    gtk_box_pack_start(GTK_BOX(btn_box), btn_browse, TRUE, TRUE, 0);

    GtkWidget *btn_encrypt = gtk_button_new_with_label("Encrypt");
    g_signal_connect(btn_encrypt, "clicked", G_CALLBACK(on_encrypt_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(btn_box), btn_encrypt, TRUE, TRUE, 0);

    GtkWidget *btn_decrypt = gtk_button_new_with_label("Decrypt");
    g_signal_connect(btn_decrypt, "clicked", G_CALLBACK(on_decrypt_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(btn_box), btn_decrypt, TRUE, TRUE, 0);

    gtk_widget_show_all(window_main);
    gtk_main();

    return 0;
}