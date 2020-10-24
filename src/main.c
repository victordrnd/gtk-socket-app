#include <stdio.h>
#include <gtk/gtk.h>
#include <time.h>
static GtkWidget *input;
static GtkWidget *scrollable_w;
static GtkWidget *messageGrid;
static GtkWidget *window;
static int msgCount;
static int prevMsgCount = -1;

static void scrollToEnd()
{
	if (msgCount != prevMsgCount)
	{
		GtkAdjustment *vadjust = gtk_scrolled_window_get_vadjustment(scrollable_w);
		gtk_adjustment_set_value(GTK_ADJUSTMENT(vadjust), gtk_adjustment_get_upper(vadjust));
		prevMsgCount = msgCount;
	}
}

static void show_credit(GtkWidget *widget)
{
	GtkImage *img = gtk_image_new_from_file("assets/logo-symbol.svg");
	GtkWidget *about = gtk_about_dialog_new();
	gtk_about_dialog_set_program_name(about, "Client ICS");
	gtk_about_dialog_set_version(about, "0.0.1");
	gtk_about_dialog_set_copyright(about, "Victor DURAND");
	gtk_about_dialog_set_website(about, "https://victordurand.fr");
	gtk_about_dialog_set_logo(about, gtk_image_get_pixbuf(img));
	int result = gtk_dialog_run(about);
}

static GtkWidget *createWindow(GtkApplication *app)
{
	GtkWidget *window = gtk_application_window_new(app);
	gtk_window_set_title(GTK_WINDOW(window), "Client");
	gtk_window_set_default_size(GTK_WINDOW(window), 350, 650);
	GtkImage *img = gtk_image_new_from_file("assets/logo-symbol.svg");
	gtk_window_set_resizable(window, 0);
	gtk_window_set_icon(window, gtk_image_get_pixbuf(img));
	return window;
}

static GtkWidget *createScrollableWindow()
{
	GtkStyleContext *context;
	GtkAdjustment *hadjust = gtk_adjustment_new(0, 0, 350, 20, 1, 5);
	GtkAdjustment *vadjust = gtk_adjustment_new(570, 0, 570, 1, 100, 470);
	scrollable_w = gtk_scrolled_window_new(hadjust, vadjust);
	gtk_scrolled_window_set_min_content_width(scrollable_w, 350);
	gtk_scrolled_window_set_max_content_width(scrollable_w, 350);
	gtk_scrolled_window_set_min_content_height(scrollable_w, 600);
	gtk_scrolled_window_set_max_content_height(scrollable_w, 600);
	gtk_scrolled_window_set_propagate_natural_width(scrollable_w, TRUE);
	messageGrid = gtk_grid_new();
	gtk_grid_set_column_homogeneous(messageGrid, TRUE);
	GtkWidget *message;
	for (int i = 1; i < 10; i++)
	{
		char *messageText = "Message 2 lorem ipsum dolor sit amet ";

		int offset = 0;
		if (strlen(messageText) > 15)
		{
			if (i % 2 == 1)
			{
				offset = 1;
			}
		}
		else
		{
			if (i % 2)
			{
				offset = 2;
			}
		}
		int width = strlen(messageText) > 15 ? 2 : 1;
		message = gtk_label_new(messageText);
		gtk_label_set_line_wrap(message, TRUE);
		context = gtk_widget_get_style_context(message);
		gtk_style_context_add_class(context, i % 2 ? "client" : "console");
		gtk_grid_attach(GTK_GRID(messageGrid), message, offset, i + 1, width, 1);
	}

	GtkWidget *y = gtk_label_new("Cette liberté de donner à une variable locale une durée de vie égale à celle du programme, permet de résoudre des problèmes du type exposé ci-dessous. Imaginons une procédure qui pour une raison quelconque doit connaître combien de fois elle a été appelée. Le programmeur a besoin d'une variable dont la durée de vie est supérieure à celle de la procédure concernée, ce sera donc une variable statique. Cependant cette variable doit être une variable privée de la procédure, (il n'y a aucune raison qu'une autre procédure puisse en modifier la valeur), il faut donc que ce soit une variable locale à la procédure. En C, on programmera de la manière suivante :");
	context = gtk_widget_get_style_context(y);
	gtk_label_set_line_wrap(y, TRUE);
	gtk_style_context_add_class(context, "client");
	gtk_grid_attach(GTK_GRID(messageGrid), y, 1, 1, 2, 1);
	msgCount = 10;
	gtk_container_add(GTK_CONTAINER(scrollable_w), messageGrid);
	return scrollable_w;
}

static void addMessage()
{
	char *messageText = gtk_entry_get_text(input);
	if (strlen(messageText) == 0)
		return;
	GtkWidget *message = gtk_label_new(messageText);
	gtk_label_set_line_wrap(message, TRUE);
	GtkStyleContext *context = gtk_widget_get_style_context(message);
	gtk_style_context_add_class(context, "client");
	int offset = strlen(messageText) > 15 ? 1 : 2;
	int width = strlen(messageText) > 15 ? 2 : 1;
	gtk_grid_attach(GTK_GRID(messageGrid), message, offset, ++msgCount, width, 1);
	gtk_widget_queue_draw(scrollable_w);
	gtk_widget_show_all(scrollable_w);
	gtk_entry_set_text(input, "");
}

static GtkWidget *createInputWidget(GtkWidget *scrollable_w)
{
	GtkWidget *grid = gtk_grid_new();
	gtk_grid_set_column_homogeneous(grid, TRUE);

	input = gtk_entry_new();
	GtkWidget *button = gtk_button_new_with_label("Send");

	GtkImage *img = gtk_image_new_from_file("assets/logo-symbol.svg");
	GtkWidget *header = gtk_header_bar_new();
	gtk_header_bar_set_custom_title(header, img);
	GtkWidget *logout = gtk_button_new_with_label("Log out");
	gtk_header_bar_pack_end(header, logout);
	g_signal_connect_swapped(GTK_BUTTON(logout), "clicked", G_CALLBACK(gtk_widget_destroy), window);
	gtk_grid_attach(GTK_GRID(grid), scrollable_w, 0, 0, 3, 1);
	gtk_grid_attach(GTK_GRID(grid), input, 0, 1, 2, 1);
	gtk_grid_attach(GTK_GRID(grid), button, 2, 1, 1, 1);
	gtk_grid_insert_row(grid, 0);
	gtk_grid_attach(GTK_GRID(grid), header, 0, 0, 3, 1);
	g_signal_connect(button, "clicked", G_CALLBACK(addMessage), NULL);
	g_signal_connect(input, "activate", G_CALLBACK(addMessage), NULL);
	return grid;
}

static void activate(GtkApplication *app, gpointer user_data)
{
	window = createWindow(app);
	GtkStyleContext *context = gtk_widget_get_style_context(window);
	gtk_style_context_add_class(context, "window");
	GtkWidget *scrollable_w = createScrollableWindow();
	GtkWidget *grid = createInputWidget(scrollable_w);
	g_signal_connect(scrollable_w, "size-allocate", G_CALLBACK(scrollToEnd), NULL);
	GtkCssProvider *provider = gtk_css_provider_new();
	gtk_css_provider_load_from_path(provider, "styles/main.css", NULL);
	gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
	GtkWidget *header = gtk_header_bar_new();
	gtk_header_bar_set_title(header, "Client");
	gtk_container_add(GTK_CONTAINER(window), grid);
	gtk_widget_show_all(window);
}

int main(int argc, char **argv)
{
	printf("Hello World!\n");
	GtkApplication *app;
	app = gtk_application_new("org.victordurand.fr", G_APPLICATION_FLAGS_NONE);
	g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
	g_application_run(G_APPLICATION(app), argc, argv);
	g_object_unref(app);

	return (0);
}