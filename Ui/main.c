#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Segmentation/makeText.h"
/* this is the main file of the ocr, all wil be launch here */

struct win
{
    GtkWindow* window;
    GtkTextView* txtArea;
    GtkImage* image_display;
    GtkTextBuffer* textBuffer;
};

char* showFileSelectModal(struct win * window){
    char* resFilename="";

    GtkWidget *dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
    gint res;

    GtkWindow *MainWindow = window->window;

    dialog = gtk_file_chooser_dialog_new ("Open File",
        MainWindow,
        action,
        "Cancel", GTK_RESPONSE_CANCEL,
        "Open",GTK_RESPONSE_ACCEPT,
        NULL);

    res = gtk_dialog_run (GTK_DIALOG (dialog));
    if (res == GTK_RESPONSE_ACCEPT)
    {
        char *filename;
        GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);
        filename = gtk_file_chooser_get_filename (chooser);
        //open_file (filename);
        resFilename=filename;
        //g_free (filename);
    }

    gtk_widget_destroy (dialog);

    return resFilename;
}

// Signal handler for the "clicked" signal of the start button.
void on_start(GtkButton *button, gpointer user_data)
{
    struct win *window = user_data;

    char* filePath = showFileSelectModal(window);
    //OPERATIONS ON THE FILE FROM HERE

    printf("%s\n", filePath);



    GdkPixbuf* newImgPix = gdk_pixbuf_new_from_file_at_size (filePath,
                                  gtk_widget_get_allocated_width (GTK_WIDGET(window->window))/2,
                                  4*gtk_widget_get_allocated_height (GTK_WIDGET(window->window))/5,
                                   NULL);

    gtk_image_set_from_pixbuf (window->image_display, newImgPix);


    gtk_text_buffer_set_text (window->textBuffer,"Loading, \nplease wait...\nWe are currently converting your image",-1);

    //CONVERT IMAGE HERE
    //TO PRINT TEXT IN TEXTAREA USE :

	size_t h = 0;
	size_t w = 0;
	size_t charNumber = 0;

	int *matrice = loadImage(filePath, &w, &h);

	struct DocumentBlocks_t *doc = MakeDocTreeFromImage(matrice, &charNumber, w, h);

	char string[charNumber];

	for (size_t i = 0; i < charNumber; ++i) {
		string[i] = 0;
	}

	DocToString(doc, matrice, string);

	string[charNumber] = 0;

	printf("%s\n", string);

    gtk_text_buffer_set_text (window->textBuffer, string,-1);
}




char* showFileSaveModal(struct win * window){
    char* resFilename="";

    GtkWidget *dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER;
    gint res;

    GtkWindow *MainWindow = window->window;

    dialog = gtk_file_chooser_dialog_new ("Save file",
        MainWindow,
        action,
        "Cancel", GTK_RESPONSE_CANCEL,
        "Save",GTK_RESPONSE_ACCEPT,
        NULL);

    res = gtk_dialog_run (GTK_DIALOG (dialog));
    if (res == GTK_RESPONSE_ACCEPT)
    {
        char *filename;
        GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);
        filename = gtk_file_chooser_get_filename (chooser);

        resFilename=filename;
    }

    gtk_widget_destroy (dialog);

    return resFilename;
}


void on_save(GtkButton *button, gpointer user_data)
{
    struct win *window = user_data;

    char* filePath = showFileSaveModal(user_data);

    if (strlen(filePath) == 0)
        return;

    strcat(filePath,"/converted.txt");

    printf("Saving file at %s\n", filePath);


    GtkTextIter start, end;

    gtk_text_buffer_get_start_iter (window->textBuffer, &start);

    gtk_text_buffer_get_end_iter (window->textBuffer, &end);


    char* strToSave = gtk_text_buffer_get_text (window->textBuffer,
                          &start,
                          &end,
                          TRUE);

    FILE * fileToSave;


    fileToSave = fopen(filePath, "w");


    if(fileToSave == NULL)
    {
        printf("Unable to create file.\n");
        exit(EXIT_FAILURE);
    }
    fputs(strToSave, fileToSave);


    fclose(fileToSave);

    printf("File created and saved successfully. \n");


    gtk_text_buffer_set_text (window->textBuffer,"",-1);

    gtk_image_clear (window->image_display);


}


// Main function.
int main (int argc, char *argv[])
{

    // Initializes GTK.
    gtk_init(NULL, NULL);

    // Loads the UI description and builds the UI.
    // (Exits if an error occurs.)
    GtkBuilder* builder = gtk_builder_new();
    GError* error = NULL;
    if (gtk_builder_add_from_file(builder, "ui.glade", &error) == 0)
    {
        g_printerr("Error loading file: %s\n", error->message);
        g_clear_error(&error);
        return 1;
    }


    // Gets the widgets.
    GtkWindow* window = GTK_WINDOW(gtk_builder_get_object(builder, "org.gtk.ui"));
    GtkButton* process_button = GTK_BUTTON(gtk_builder_get_object(builder, "process_button"));
    GtkButton* save_button = GTK_BUTTON(gtk_builder_get_object(builder, "save_button"));
    GtkImage* image_display = GTK_IMAGE(gtk_builder_get_object(builder, "image_display"));
    GtkTextView* txtArea = GTK_TEXT_VIEW(gtk_builder_get_object(builder, "text_area"));
    GtkTextBuffer* textBuffer = GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "textbuffer1"));



    struct win mainWin;
    mainWin.window = window;
    mainWin.txtArea = txtArea;
    mainWin.image_display = image_display;
    mainWin.textBuffer = textBuffer;


    // Connects signal handlers.
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(process_button, "clicked", G_CALLBACK(on_start), &mainWin);
    g_signal_connect(save_button, "clicked", G_CALLBACK(on_save), &mainWin);

    // Runs the main loop.
    gtk_main();

    // Exits.
    return 0;
}
