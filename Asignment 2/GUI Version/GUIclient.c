#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/types.h>
#include <gtk/gtk.h>
#include <gtk/gtkx.h>
#include <math.h>
#include <time.h>
#include <ctype.h>
#include <sys/mman.h>
#include <netdb.h>

#define MAX_CLIENTS 100
#define BUFFER_SZ 2048
#define NAME_LEN 32

char	tmp[1024]; // general use
GtkWidget	*window;
GtkWidget	*fixed1;
GtkWidget	*button1;
GtkWidget	*label1;
GtkWidget	*entry1;
GtkWidget	*ScrollWindow;
GtkWidget	*ViewPort;
GtkWidget	*TextView;
GtkTextBuffer	*TextBuffer;

GtkBuilder	*builder;

volatile sig_atomic_t flag = 0;
int sockfd = 0;
char name[NAME_LEN];

void struct_overwrite_stdout(){
	printf("\r%s", "> ");
	fflush(stdout);
}

void str_trim_lf(char* arr, int length){
	for(int i=0; i<length; i++){
		if(arr[i] == '\n'){
			arr[i] = '\0';
			break;
		}
	}
}

void catch_ctrl_c_and_exit(){
	flag = 1;
}

void recv_msg_handler(){
	char message[BUFFER_SZ] = {};
	 GtkTextBuffer *buffer;
	 GtkTextMark *mark;
     GtkTextIter iter;
     const gchar *text;
	while(1){
		
		int receive = recv(sockfd, message, BUFFER_SZ, 0);
			if(receive > 0){
		 		TextBuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW(TextView));
		 		buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (TextView));
		 		mark = gtk_text_buffer_get_insert (buffer);
		 		gtk_text_buffer_get_iter_at_mark (buffer, &iter, mark);

				/* Insert newline (only if there's already text in the buffer). */
				if (gtk_text_buffer_get_char_count(buffer)){
		    		gtk_text_buffer_insert (buffer, &iter, "\n", 1);
				}
					gtk_text_buffer_insert (buffer, &iter, message, -1);
					struct_overwrite_stdout();
		} else if(receive == 0){
		  	break;
		}
		bzero(message, BUFFER_SZ);
	}
}

void send_msg_handler(){
	char buffer[BUFFER_SZ] = {};
	char message[BUFFER_SZ + NAME_LEN] = {};
	const char *usertext;

	
		struct_overwrite_stdout();
		usertext = gtk_entry_get_text (GTK_ENTRY (entry1));
		snprintf(buffer, sizeof(buffer), "%s", usertext);
		str_trim_lf(buffer, BUFFER_SZ);

		if(strcmp(buffer, "exit") == 0){
			//return 0
			
		} else {
			sprintf(message, "%s: %s", name, buffer);
			send(sockfd, message, strlen(message), 0);
		}

		bzero(buffer, BUFFER_SZ);
		bzero(message, BUFFER_SZ + NAME_LEN);
		gtk_editable_delete_text (GTK_EDITABLE(entry1), 0, -1);

	catch_ctrl_c_and_exit(2);
}


int msg(){

	pthread_t recv_msg_thread;
	if(pthread_create(&recv_msg_thread, NULL, (void*)recv_msg_handler, NULL) != 0){
		printf("ERROR: pthread\n");
		return EXIT_FAILURE;
	}
}



int main(int argc, char **argv){
	if(argc != 2){
		printf("Usage: %s <port>\n", argv[0]);
		return EXIT_FAILURE;
	}



	gtk_init(&argc, &argv); // init Gtk

	char *ip = "127.0.0.1";
	int port = atoi(argv[1]);

	signal(SIGINT, catch_ctrl_c_and_exit);

	printf("Enter your name: ");
	fgets(name, NAME_LEN, stdin);
	str_trim_lf(name, strlen(name));

	if(strlen(name) > NAME_LEN - 1 || strlen(name) < 2){
		printf("Enter name correctly\n");
		return EXIT_FAILURE;
	}

	struct sockaddr_in server_addr;
	//Socket Settings
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(ip);
	server_addr.sin_port = htons(port);

	// Connect to the server
	int err = connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
	if (err == -1){
		printf("ERROR: connect\n");
		return EXIT_FAILURE;
	}

	// Send the name
	send(sockfd, name, NAME_LEN, 0);

//---------------------------------------------------------------------
// establish contact with xml code used to adjust widget settings
//---------------------------------------------------------------------
 
	builder = gtk_builder_new_from_file ("client.glade");
	window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));

	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	gtk_builder_connect_signals(builder, NULL);

	fixed1 = GTK_WIDGET(gtk_builder_get_object(builder, "fixed1"));
	button1 = GTK_WIDGET(gtk_builder_get_object(builder, "button1"));
	entry1 = GTK_WIDGET(gtk_builder_get_object(builder, "entry1"));
	ScrollWindow = GTK_WIDGET(gtk_builder_get_object(builder, "ScrollWindow"));
	ViewPort = GTK_WIDGET(gtk_builder_get_object(builder, "ViewPort"));
	TextView = GTK_WIDGET(gtk_builder_get_object(builder, "TextView"));

	TextBuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW(TextView));
	strcpy(tmp,"====== WELCOME TO THE CHATROOM ======");
	gtk_text_buffer_set_text (TextBuffer, (const gchar *) tmp, (gint) -1);


	gtk_widget_show(window);

	g_timeout_add(1,msg,NULL);

	gtk_main();

	close(sockfd);

	return EXIT_SUCCESS;
}

void on_entry1_changed(GtkEntry *e) {

	
	
	}

void on_button1_clicked(GtkButton *b) {
	
		pthread_t send_msg_thread;
		if(pthread_create(&send_msg_thread, NULL, (void*)send_msg_handler, NULL) != 0){
		printf("ERROR: pthread\n");
		//return EXIT_FAILURE;
	}

	 GtkTextBuffer *buffer;
	 GtkTextMark *mark;
     GtkTextIter iter;
     const gchar *text;

	 	buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (TextView));
	    text = gtk_entry_get_text (GTK_ENTRY (entry1));
		mark = gtk_text_buffer_get_insert (buffer);
		gtk_text_buffer_get_iter_at_mark (buffer, &iter, mark);

	/* Insert newline (only if there's already text in the buffer). */
			if (gtk_text_buffer_get_char_count(buffer))
		    	gtk_text_buffer_insert (buffer, &iter, "\n", 1);

		    //strcpy(tmp,"> ");
		    //gtk_text_buffer_insert (buffer, &iter, tmp, -1);
	 		gtk_text_buffer_insert (buffer, &iter, text, -1);
	 	//Enable this and you lose text somtimes for some reason.
		//gtk_editable_delete_text (GTK_EDITABLE(entry1), 0, -1); 
		
	}

