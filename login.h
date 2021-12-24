#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "test.h"
#define BUFF_SIZE 80

const int WINDOW_WIDTH = 1140;
const int WINDOW_HEIGHT = 700;
const float CARD_WIDTH = 55;
const float CARD_HEIGHT = 85;

const int PLAYER = 1;
const int ENEMY = 7;
const int OTHER = 3;

int checkk = 0, check0 = 0;

GtkBuilder *builder;
GtkWidget *beginWindow;

GtkWidget *LoginWindow;
GtkWidget *usernameLoginEntry;
GtkWidget *passwordLoginEntry;
GtkWidget *errorLoginLabel;
GtkWidget *errorRegisLabel;

GtkWidget *RegisterWindow;
GtkWidget *usernameRegEntry;
GtkWidget *passwordRegEntry;
GtkWidget *passwordAgainRegEntry;

GtkWidget *mainMenuWindow;
GtkWidget *boardWindow;

char buff[80];
int rcvBytes;
int sock_app;

void trim(char s[])
{
    int i = strlen(s);
    while (s[0] == ' ')
    {
        for (int j = 0; j < i; j++)
            s[j] = s[j + 1];
        i--;
    }
    while (s[i - 1] == ' ' || s[i - 1] == '\n')
        i--;
    s[i] = '\0';
}

int checkSpace(char s[])
{
    for (int i = 0; i < strlen(s); i++)
    {
        if (s[i] == ' ')
            return 1;
    }
    return 0;
}
UNO listCard[] = {{1, 'r', 0}, {2, 'r', 1}, {3, 'r', 2}, {4, 'r', 3}, {5, 'r', 4}, {6, 'r', 5}, {7, 'r', 6}, {8, 'r', 7}, {9, 'r', 8}, {10, 'r', 9}, {11, 'r', -1}, {12, 'r', -2}, {13, 'r', -3}, {14, 'k', -4}, {15, 'y', 0}, {16, 'y', 1}, {17, 'y', 2}, {18, 'y', 3}, {19, 'y', 4}, {20, 'y', 5}, {21, 'y', 6}, {22, 'y', 7}, {23, 'y', 8}, {24, 'y', 9}, {25, 'y', -1}, {26, 'y', -2}, {27, 'y', -3}, {28, 'k', -4}, {29, 'g', 0}, {30, 'g', 1}, {31, 'g', 2}, {32, 'g', 3}, {33, 'g', 4}, {34, 'g', 5}, {35, 'g', 6}, {36, 'g', 7}, {37, 'g', 8}, {38, 'g', 9}, {39, 'g', -1}, {40, 'g', -2}, {41, 'g', -3}, {42, 'k', -4}, {43, 'b', 0}, {44, 'b', 1}, {45, 'b', 2}, {46, 'b', 3}, {47, 'b', 4}, {48, 'b', 5}, {49, 'b', 6}, {50, 'b', 7}, {51, 'b', 8}, {52, 'b', 9}, {53, 'b', -1}, {54, 'b', -2}, {55, 'b', -3}, {56, 'k', -4}};
UNO x_card = {0, 'x', -6};
UNO up_card;
UNO hand[52];
int hand_size = 0;
int enemy_size = 7;


    LIST l, l1, l2;
    STACK s, s1;
    

void deal_random_hand(int);
void card_clicked(GtkWidget *, gpointer);
int play(UNO *);
void draw_hand();
void draw_card(GtkWidget *, UNO *, int, float);
void draw_up_card(UNO *);
void remove_card(UNO *, int);
void clear_container(GtkWidget *);
void buildUIGameWindow();

void main_play_game();

char *get_link_fileImage(int id, char *link)
{
    memset(link, 0, strlen(link));
    char tmpchar[30];
    int tmp = -1;
    if (id == 56 || id == 42 || id == 28)
    {
        tmp = 14;
    }
    else if (id == 82 || id == 95 || id == 108)
    {
        tmp = 69;
    }
    else if (id > 56 && id < 69)
    {
        tmp = id - 55;
    }
    else if (id > 69 && id < 82)
    {
        tmp = id - 54;
    }
    else if (id > 82 && id < 95)
    {
        tmp = id - 53;
    }
    else if (id > 95 && id < 108)
    {
        tmp = id - 52;
    }
    else
    {
        tmp = id;
    }
    my_itoa(tmp, tmpchar);

    strcpy(link, "cards/");
    strcat(link, tmpchar);
    strcat(link, ".png");
    return link;
}

GtkBuilder *builder;
GtkWidget *boardWindow;
GtkWidget *boardWindowFixed;
GtkWidget *controllerBox;
GtkWidget *playerBox;
GtkWidget *enemyBox;
GtkWidget *iconON;
GtkWidget *iconOFF;

void drawCardButtonClick(GtkWidget *button)
{   //, GtkWidget *box
    // seed prng
    // srand(time(NULL));
    // create a card with random values
    // int r = 1 + rand() % 56;
    // for (int j = 0; j < sizeof(listCard) / sizeof(UNO); j++)
    // {
    //     if (listCard[j].id == r)
    //         hand[hand_size++] = listCard[j];
    // }
    // clear_container(playerBox);
    phat(1, &l1, &s);
    hand_size++;
    draw_hand(playerBox);
}

GtkWidget *resize_image(GtkWidget *image, char *link, float x)
{
    char cmd[2048];
    FILE *f1;

    sprintf(cmd, "identify -format %%wx%%h \"%s\"\n", link);
    f1 = popen(cmd, "r");
    strcpy(cmd, "");
    fgets(cmd, 2048, f1);
    fclose(f1);

    float width = CARD_WIDTH * x;
    float height = CARD_HEIGHT * x;
    sprintf(cmd, "convert \"%s\" -resize %dx%d tmp.jpg\n", link, (int)width, (int)height);
    system(cmd);

    strcpy(link, "tmp.jpg");

    image = gtk_image_new_from_file(link);

    return image;
    system("rm tmp.jpg");
}

void unoButtonClick(GtkWidget *button)
{
    g_print("uno button click\n");
}

void nextButtonClick(GtkWidget *button)
{
    g_print("next button click\n");
}

void draw_enemyCards()
{
    // draw all cards in the hand of enemy
    float sizeCard = 1;
    if (enemy_size <= 3)
    {
        gtk_widget_set_size_request(enemyBox, CARD_WIDTH * 6, CARD_HEIGHT);
        gtk_fixed_move(GTK_FIXED(boardWindowFixed), enemyBox, (WINDOW_WIDTH - CARD_WIDTH * 6) / 2, 30);
    }
    else if (enemy_size > 3 && enemy_size < 9)
    {
        gtk_widget_set_size_request(enemyBox, CARD_WIDTH * 12, CARD_HEIGHT);
        gtk_fixed_move(GTK_FIXED(boardWindowFixed), enemyBox, (WINDOW_WIDTH - CARD_WIDTH * 12) / 2, 30);
    }
    else
    {
        gtk_widget_set_size_request(enemyBox, CARD_WIDTH * 17, CARD_HEIGHT);
        gtk_fixed_move(GTK_FIXED(boardWindowFixed), enemyBox, (WINDOW_WIDTH - CARD_WIDTH * 17) / 2, 30);
        if (enemy_size > 12)
        {
            sizeCard = 0.6;
            gtk_widget_set_size_request(enemyBox, CARD_WIDTH * 17, CARD_HEIGHT * sizeCard);
        }
    }

    for (int i = 0; i < enemy_size; i++)
    {
        // draw the current card
        draw_card(enemyBox, &x_card, PLAYER, sizeCard);
    }
}

int app(int argc, char **argv, int sockfd)
{
    gtk_init(&argc, &argv);
    sock_app = sockfd;

    builder = gtk_builder_new_from_file("UI.glade");
    beginWindow = GTK_WIDGET(gtk_builder_get_object(builder, "beginWindow"));
    gtk_window_set_title(GTK_WINDOW(beginWindow), "Uno");

    LoginWindow = GTK_WIDGET(gtk_builder_get_object(builder, "LoginWindow"));
    gtk_window_set_title(GTK_WINDOW(LoginWindow), "Đăng nhập");
    usernameLoginEntry = GTK_WIDGET(gtk_builder_get_object(builder, "usernameLoginEntry"));
    passwordLoginEntry = GTK_WIDGET(gtk_builder_get_object(builder, "passwordLoginEntry"));
    errorLoginLabel = GTK_WIDGET(gtk_builder_get_object(builder, "errorLoginLabel"));

    RegisterWindow = GTK_WIDGET(gtk_builder_get_object(builder, "RegisterWindow"));
    gtk_window_set_title(GTK_WINDOW(RegisterWindow), "Đăng ký");
    usernameRegEntry = GTK_WIDGET(gtk_builder_get_object(builder, "usernameRegEntry"));
    passwordRegEntry = GTK_WIDGET(gtk_builder_get_object(builder, "passwordRegEntry"));
    passwordAgainRegEntry = GTK_WIDGET(gtk_builder_get_object(builder, "passwordAgainRegEntry"));
    errorRegisLabel = GTK_WIDGET(gtk_builder_get_object(builder, "errorRegisLabel"));

    mainMenuWindow = GTK_WIDGET(gtk_builder_get_object(builder, "mainMenuWindow"));

    boardWindow = GTK_WIDGET(gtk_builder_get_object(builder, "boardWindow"));
    buildUIGameWindow();
    gtk_widget_hide(boardWindow);

    g_signal_connect(beginWindow, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_builder_connect_signals(builder, NULL);

    // fixed1 = GTK_WIDGET(gtk_builder_get_object(builder, "fixed1"));

    gtk_widget_show(beginWindow);

    gtk_main();

    giaiPhong(&l1); // giải phóng bộ nhớ
    giaiPhong(&l2);
    return EXIT_SUCCESS;
}

void on_loginBtn_clicked()
{
    gtk_widget_show(LoginWindow);
    gtk_widget_hide(beginWindow);
    strcpy(buff, "1");
    send(sock_app, buff, strlen(buff), 0);
    rcvBytes = recv(sock_app, buff, BUFF_SIZE, 0);
    buff[rcvBytes] = '\0';
}

void on_LoginWindow_destroy()
{
    gtk_widget_show(beginWindow);
    gtk_widget_hide(LoginWindow);
    gtk_editable_delete_text(GTK_EDITABLE(usernameLoginEntry), 0, -1);
    gtk_editable_delete_text(GTK_EDITABLE(passwordLoginEntry), 0, -1);
    gtk_label_set_text(GTK_LABEL(errorLoginLabel), "");
}

void on_loginSubmitBtn_clicked()
{

    char tmp[128];
    sprintf(tmp, "%s", gtk_entry_get_text(GTK_ENTRY(usernameLoginEntry)));
    trim(tmp);
    if (checkSpace(tmp) == 1)
    {
        //loi
        gtk_label_set_text(GTK_LABEL(errorLoginLabel), "username khong co dau cach!");
    }
    else
    {

        // printf("-%s-\n", tmp);
        strcpy(buff, tmp);
        sprintf(tmp, "%s", gtk_entry_get_text(GTK_ENTRY(passwordLoginEntry)));
        // printf("-%s-\n", tmp);
        // trim(tmp);
        if (checkSpace(tmp) == 1)
        {
            //loi
            gtk_label_set_text(GTK_LABEL(errorLoginLabel), "password khong co dau cach!");
        }
        else
        {
            strcat(buff, " ");
            strcat(buff, tmp);
            send(sock_app, buff, strlen(buff), 0);
            rcvBytes = recv(sock_app, buff, BUFF_SIZE, 0);
            if (rcvBytes < 0)
            {
                perror("Error: ");
                return;
            }
            buff[rcvBytes] = '\0';
            if (strcmp(buff, "OK") == 0)
            {
                printf("\n-------------Let's play-------------\n");
                on_LoginWindow_destroy();
                //cua so main game
                gtk_widget_show(mainMenuWindow);
                gtk_widget_hide(beginWindow);
            }
            else
            {
                gtk_label_set_text(GTK_LABEL(errorLoginLabel), buff);
                //continue;
            }
        }
    }
}

void on_registerBtn_clicked()
{
    gtk_widget_show(RegisterWindow);
    gtk_widget_hide(beginWindow);
    strcpy(buff, "2");
    send(sock_app, buff, strlen(buff), 0);
    rcvBytes = recv(sock_app, buff, BUFF_SIZE, 0);
    buff[rcvBytes] = '\0';
}

void on_RegisterWindow_destroy()
{
    gtk_widget_show(beginWindow);
    gtk_widget_hide(RegisterWindow);
    gtk_editable_delete_text(GTK_EDITABLE(usernameRegEntry), 0, -1);
    gtk_editable_delete_text(GTK_EDITABLE(passwordRegEntry), 0, -1);
    gtk_editable_delete_text(GTK_EDITABLE(passwordAgainRegEntry), 0, -1);
    gtk_label_set_text(GTK_LABEL(errorRegisLabel), "");
}

void on_registerSubmitBtn_clicked()
{
    char tmp[128];
    sprintf(tmp, "%s", gtk_entry_get_text(GTK_ENTRY(usernameRegEntry)));
    trim(tmp);
    // printf("-%s-\n", tmp);
    if (checkSpace(tmp) == 1)
    {
        //loi
        gtk_label_set_text(GTK_LABEL(errorRegisLabel), "username khong co dau cach!");
    }
    else
    {

        strcpy(buff, tmp);
        strcat(buff, " ");
        sprintf(tmp, "%s", gtk_entry_get_text(GTK_ENTRY(passwordRegEntry)));
        if (checkSpace(tmp) == 1)
        {
            //loi
            gtk_label_set_text(GTK_LABEL(errorRegisLabel), "password khong co dau cach!");
        }
        else
        {

            // printf("-%s-\n", tmp);
            strcat(buff, tmp);
            strcat(buff, " ");
            sprintf(tmp, "%s", gtk_entry_get_text(GTK_ENTRY(passwordAgainRegEntry)));
            if (checkSpace(tmp) == 1)
            {
                //loi
                gtk_label_set_text(GTK_LABEL(errorRegisLabel), "password khong co dau cach!");
            }
            else
            {

                // printf("-%s-\n", tmp);
                strcat(buff, tmp);
                // printf("-%s-\n", buff);
                send(sock_app, buff, strlen(buff), 0);
                rcvBytes = recv(sock_app, buff, BUFF_SIZE, 0);
                if (rcvBytes < 0)
                {
                    perror("Error: ");
                    return;
                }
                buff[rcvBytes] = '\0';
                if (strcmp(buff, "OK") == 0)
                {
                    printf("\nĐăng ký tài khoản thành công\n");
                    on_RegisterWindow_destroy();
                }
                gtk_label_set_text(GTK_LABEL(errorRegisLabel), buff);
            }
        }
    }
}

void on_mainMenuWindow_destroy()
{
    gtk_main_quit();
}

void on_startGameBtn_clicked()
{
    // gtk_widget_show(boardWindow);
    gtk_widget_show_all(boardWindow);
    gtk_widget_hide(mainMenuWindow);

    main_play_game();
}

/**
 * Deal a random hand with the given length
 * @param length Number of cards in the hand
 */
void deal_random_hand(int length)
{
    // seed prng
    srand(time(NULL));

    // create a hand of size length
    for (int i = 0; i < length; i++)
    {
        // create a card with random values
        int r = 1 + rand() % (56);
        for (int j = 0; j < sizeof(listCard) / sizeof(UNO); j++)
        {
            if (listCard[j].id == r)
                hand[i] = listCard[j];
        }
    }

    // keep track of size of hand
    hand_size = length;

    // generate random up card

    int r = 1 + rand() % (56);
    for (int i = 0; i < sizeof(listCard) / sizeof(UNO); i++)
    {
        if (listCard[i].id == r)
        {
            up_card = listCard[i];
        }
    }
    draw_hand(playerBox);
}

/**
 * Draw the user's hand
 *
 */
void draw_hand(GtkWidget *container)
{
    clear_container(container);
    float sizeCard = 1;
    // draw the up card
    if (hand_size <= 3)
    {
        gtk_widget_set_size_request(playerBox, CARD_WIDTH * 6, CARD_HEIGHT);
        gtk_fixed_move(GTK_FIXED(boardWindowFixed), playerBox, (WINDOW_WIDTH - CARD_WIDTH * 6) / 2, 582);
    }
    else if (hand_size > 3 && hand_size < 9)
    {
        gtk_widget_set_size_request(playerBox, CARD_WIDTH * 12, CARD_HEIGHT);
        gtk_fixed_move(GTK_FIXED(boardWindowFixed), playerBox, (WINDOW_WIDTH - CARD_WIDTH * 12) / 2, 582);
    }
    else
    {
        gtk_widget_set_size_request(playerBox, CARD_WIDTH * 17, CARD_HEIGHT);
        gtk_fixed_move(GTK_FIXED(boardWindowFixed), playerBox, 100, 582);
        if (hand_size > 12)
        {
            sizeCard = 0.6;
            gtk_widget_set_size_request(playerBox, CARD_WIDTH * 17, CARD_HEIGHT * sizeCard);
        }
    }
    // draw all cards in the hand
    // for (int i = 0; i < hand_size; i++)
    // {
    //     // draw the current card
    //     draw_card(container, &hand[i], PLAYER, sizeCard);
    // }

    for (NODE* p = l1.pHead; p != NULL; p = p->pNext) {
		// printf("%d-%c-%d", p->data.id, p->data.color, p->data.number);
		// printf("\n");
        draw_card(container, &(p->data), PLAYER, sizeCard);
	}

    clear_container(controllerBox);
    draw_card(controllerBox, &up_card, OTHER, 2);
}

/**
 * Draw a single card
 *
 * @param card Card to draw
 * @param x x-position of card
 * @param y y-position of card
 *status == 1 -> can click -> check value
 */
void draw_card(GtkWidget *container, UNO *card, int status, float sizeCard)
{
    //add array button card game
    char link[30];
    GtkWidget *image = gtk_image_new_from_file(get_link_fileImage(card->id, link));
    if (sizeCard != 1)
    {
        image = resize_image(image, get_link_fileImage(card->id, link), sizeCard);
    }
    GtkWidget *button = gtk_button_new();
    //add image to button
    gtk_button_set_always_show_image(GTK_BUTTON(button), TRUE);
    gtk_button_set_image(GTK_BUTTON(button), image);
    // draw card
    gtk_box_pack_start(GTK_BOX(container), button, TRUE, FALSE, 0);

    if (status == PLAYER)
        g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(card_clicked), card);

    gtk_widget_show(button);
}

/**
 * Event handler for a card being clicked
 */
void card_clicked(GtkWidget *card_button, gpointer card_data)
{
    // cast generic gpointer to a card
    UNO *card = (UNO *)card_data;

    // play the card
    play(card);
}

/**
 * Play the given card
 * @param card Card to play
 * @return False if card cannot be played, true otherwise
 */
int play(UNO *card)
{
    // if either value or color matches, then move is valid
    // if (card->color == up_card.color || card->color == 'k' || card->number == up_card.number)
    // {
    //     // change the up card
    //     up_card = *card;

    //     // // clear game so we can remove the card that was played
    //     // clear_container(boardWindowFixed);

    //     remove_card(card, PLAYER);

    //     // // draw the new game state
    //     draw_hand(playerBox);
    //     return 1;
    // }

    // // illegal move
    // return 0;
}

/**
 * Remove all children from the given container
 *
 */
void clear_container(GtkWidget *container)
{
    // get all children in the container
    GList *children = gtk_container_get_children(GTK_CONTAINER(container));

    // iterate over children of container
    for (GList *i = children; i != NULL; i = g_list_next(i))
        // gtk_widget_destroy(GTK_WIDGET(i->data));
        gtk_container_remove(GTK_CONTAINER(container), GTK_WIDGET(i->data));

    // free list
    g_list_free(children);
}

/**
 * Remove the given card from the hand
 * @param card Card to remove 
 */
void remove_card(UNO *card, int mode)
{
    int removed = 0;

    // iterate over hand, looking for card
    for (int i = 0; i < hand_size; i++)
    {
        // if card has already been removed, then sift element back one
        if (removed)
            hand[i - 1] = hand[i];

        // if card matches, then begin sifting cards backward
        if (hand[i].number == card->number && hand[i].color == card->color)
            removed = 1;
    }

    // since we've removed a card
    hand_size--;
}

void buildUIGameWindow()
{

    //add widget from file glade
    // builder = gtk_builder_new_from_file("login.glade");
    boardWindow = GTK_WIDGET(gtk_builder_get_object(builder, "boardWindow"));

    g_signal_connect(boardWindow, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_builder_connect_signals(builder, NULL);

    //add container
    boardWindowFixed = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(boardWindow), boardWindowFixed);

    //add background of window
    GdkPixbuf *backgrndPixbuf;
    GtkWidget *backgrndImage;
    GError *error = NULL;
    backgrndPixbuf = gdk_pixbuf_new_from_file("images/background.png", &error);
    backgrndPixbuf = gdk_pixbuf_scale_simple(backgrndPixbuf, 1140, 700, GDK_INTERP_BILINEAR);
    backgrndImage = gtk_image_new_from_pixbuf(backgrndPixbuf);
    gtk_container_add(GTK_CONTAINER(boardWindowFixed), backgrndImage);

    int box_width = CARD_WIDTH * 2 * 5;
    int box_height = CARD_HEIGHT * 2;
    //*****************************************************************************************************
    //ADD CONTROLLER: up card | draw button | next button | uno button
    int startWidth = (WINDOW_WIDTH - CARD_WIDTH * 8) / 2 + 50;
    int heightControllerBox = (WINDOW_HEIGHT - CARD_HEIGHT * 2) / 2 - 15;
    //1.. up card
    controllerBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_fixed_put(GTK_FIXED(boardWindowFixed), controllerBox, startWidth, heightControllerBox);
    // draw_up_card(&x_card);
    draw_card(controllerBox, &x_card, OTHER, 1);
    //2.add draw card button
    char link[] = "images/pile.png";
    GtkWidget *drawCardImage = gtk_image_new_from_file(link);
    drawCardImage = resize_image(drawCardImage, link, 2);
    // gtk_widget_set_size_request(drawCardImage, CARD_WIDTH*2, CARD_HEIGHT*2);
    GtkWidget *drawCardButton = gtk_button_new();
    //add image to button
    gtk_button_set_always_show_image(GTK_BUTTON(drawCardButton), TRUE);
    gtk_button_set_image(GTK_BUTTON(drawCardButton), drawCardImage);
    //add to box
    gtk_fixed_put(GTK_FIXED(boardWindowFixed), drawCardButton, startWidth + CARD_WIDTH * 2.5, heightControllerBox);
    g_signal_connect(drawCardButton, "clicked", G_CALLBACK(drawCardButtonClick), NULL); //when clicked, draw a card and pass the player's box

    //3.add next button
    strcpy(link, "images/next.png");
    GtkWidget *nextImage = gtk_image_new_from_file(link);
    nextImage = resize_image(nextImage, link, 1);
    // gtk_widget_set_size_request(nextImage, CARD_WIDTH, CARD_HEIGHT);
    GtkWidget *nextButton = gtk_button_new();
    //add image to button
    gtk_button_set_always_show_image(GTK_BUTTON(nextButton), TRUE);
    gtk_button_set_image(GTK_BUTTON(nextButton), nextImage);
    //add to box
    gtk_fixed_put(GTK_FIXED(boardWindowFixed), nextButton, startWidth + CARD_WIDTH * 5, heightControllerBox + 20);
    g_signal_connect(nextButton, "clicked", G_CALLBACK(nextButtonClick), NULL);

    //4.add uno button
    strcpy(link, "images/uno.png");
    GtkWidget *unoImage = gtk_image_new_from_file(link);
    unoImage = resize_image(unoImage, link, 1);
    // gtk_widget_set_size_request(unoImage, CARD_WIDTH, CARD_HEIGHT);
    GtkWidget *unoButton = gtk_button_new();
    //add image to button
    gtk_button_set_always_show_image(GTK_BUTTON(unoButton), TRUE);
    gtk_button_set_image(GTK_BUTTON(unoButton), unoImage);
    //add to box
    gtk_fixed_put(GTK_FIXED(boardWindowFixed), unoButton, startWidth + CARD_WIDTH * 5, heightControllerBox + 100);
    g_signal_connect(unoButton, "clicked", G_CALLBACK(unoButtonClick), NULL);
    //****************************************************************************************
    iconON = gtk_image_new_from_icon_name("dialog-ok", GTK_ICON_SIZE_DND);
    gtk_fixed_put(GTK_FIXED(boardWindowFixed), iconON, 40, 610);

    iconOFF = gtk_image_new_from_icon_name("dialog-close", GTK_ICON_SIZE_DND);
    gtk_fixed_put(GTK_FIXED(boardWindowFixed), iconOFF, 40, 60);
    //*****************************************************************************************
    enemyBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_widget_set_size_request(enemyBox, CARD_WIDTH * 12, CARD_HEIGHT);
    gtk_fixed_put(GTK_FIXED(boardWindowFixed), enemyBox, (WINDOW_WIDTH - CARD_WIDTH * 12) / 2, 30);

    // 4 box vao 4 vi tri.
    playerBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_widget_set_size_request(playerBox, CARD_WIDTH * 12, CARD_HEIGHT);
    gtk_fixed_put(GTK_FIXED(boardWindowFixed), playerBox, (WINDOW_WIDTH - CARD_WIDTH * 12) / 2, 582);
}

void main_play_game()
{

    g_print("les't go");
    Init(&l);               //khởi tạo x
    loadTuFile(fileIn, &l); // đọc từ file và lưu vào l \o
    inPutStack(&s, l);      // lưu quân bài đã tráo \o -> tráo bài

    // may(&s, &s1, &l1, &l2);

    //******************************************************************************************
    char str[4], result[256] = "";
    InitStack(&s1); // khởi tạo \x
    NODE *p;
    UNO uno;
    int u = 0;                                                   // luu vi tri nguoi choi truoc do
    int t = 0;                                                   // so quan bai cong don de them cho nguoi choi neu k đỡ được bài
    int n, y = 1000, chuyen = -1; // checkk luu thu tu nguoi choi
    // chuyen = -1: chay theo chieu kim dong ho, chuyen = 1: chay nguoc chieu
    char x, tmp; // x de luu mau, y luu so tren quan bai,

    hand_size = 7;
    inPutL1(&s, &l1); // l2 la bai cua may \\phat bài cho người chơi l1 \o
    draw_hand(playerBox);
    
    hand_size = 7;
    inPutL1(&s, &l2); // l1 la bai cua nguoi choi
    draw_enemyCards();

    printf("\n==========================");
    printf("\nbai cua nguoi choi");
    show(l1); //in ra màn hình
    printf("\n==========================");
    // for (NODE *p = (&l1)->pHead; p != NULL; p = p->pNext)
    // {
    //     my_itoa(p->data.id, str);
    //     strcat(result, str);
    //     strcat(result, " ");
    // }
    // int z = strlen(result);
    // result[z - 1] = '\0';

    // while (check0 == 0) | check0 = 1 -> kết thúc ván bài
    // {
        // if (checkk == 0)
        // {
            printf("\nnguoi danh: "); // gia su nguoi la 1, may la 2
            nguoi(&l1, &x, &y, 0, &n, result);
            //đến lượt người -> gọi thao tác \o
            printf("\nbai vua danh: %c-%d", x, y);
            p = findL1(l1, x, y); //tìm bài theo màu | số trên quân bài
            uno = p->data;
            push2(&s1, uno); //nhặt quân bài vừa đánh cho vào s1
            if (y == -2)
            {
                chuyen *= -1;
            }
            if (chuyen == 1)
            { // nguoc kim dong ho
                checkk = 2;
            }
            else
            {
                if (y == -1)
                {
                    checkk = 1;
                }
                else
                {
                    checkk = 2;
                }
            }
            if (y == -5)
            {
                t += 4;
            }
            if (y == -3)
            {
                t += 2;
            }

            //printf("\nden day chua, tai sao k xoa");
            //printf("\nid = %d",p->data.id );
            deleteNode(&l1, p->data.id); // nhặt xong xóa khỏi list \o
            hand_size -= 1;
            strcpy(result, "");
            for (NODE *p = (&l1)->pHead; p != NULL; p = p->pNext)
            {
                my_itoa(p->data.id, str);
                strcat(result, str);
                strcat(result, " ");
            }
            int z = strlen(result);
            result[z - 1] = '\0';

    //     }
    //     else if (checkk == 1)
    //     {
    //         trungGian22(&l1, &s, &s1, checkk, &x, &y, &t, u, &check0, &hand_size, result);
    //         //đến máy thì random | người thì gọi hàm đánh
    //         //checkk = 1 -> người chơi | =2 máy chơi
    //         //check0: nếu người hoặc máy/thắng -> kết thúc
    //         if (check0 == 1)
    //         {
    //             break;
    //         }

    //         u = checkk;
    //         if (y == -2)
    //         {                 // thuộc tính số
    //             chuyen *= -1; // 1 cùng chiều đồng hồ | -1 ngược
    //         }
    //         if (chuyen == 1)
    //         {
    //             if (y == -1)
    //             {
    //                 checkk = 1;
    //             }
    //             else
    //             {
    //                 checkk = 2;
    //             }
    //         }
    //         else
    //         {
    //             if (y == -1)
    //             {
    //                 checkk = 1;
    //             }
    //             else
    //             {
    //                 checkk = 2;
    //             }
    //         }
    //         if (y == 67)
    //         { //khôi phục
    //             y -= 69;
    //         }
    //     }

    //     if (checkk == 2)
    //     {
            // trungGian22(&l2, &s, &s1, checkk, &x, &y, &t, u, &check0, &enemy_size, result);
    //         if (check0 == 1)
    //         {
    //             break;
    //         }

    //         u = checkk;
    //         if (y == -2)
    //         {
    //             chuyen *= -1;
    //         }
    //         if (chuyen == 1)
    //         {
    //             if (y == -1)
    //             {
    //                 checkk = 2;
    //             }
    //             else
    //             {
    //                 checkk = 1;
    //             }
    //         }
    //         else
    //         {
    //             if (y == -1)
    //             {
    //                 checkk = 2;
    //             }
    //             else
    //             {
    //                 checkk = 1;
    //             }
    //         }
    //         if (y == 67)
    //         {
    //             y -= 69;
    //         }
    //     }
    // // }


    //******************************************************************************************
}