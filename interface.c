#include "interface.h"
#include "object.h"
#include <pthread.h>
#include <X11/Xlib.h>

#define BUFF_SIZE 80

const int WINDOW_WIDTH = 1250; //1250 1140
const int WINDOW_HEIGHT = 800; //800 700
const float CARD_WIDTH = 55;
const float CARD_HEIGHT = 85;

const int PLAYER = 1;
const int ENEMY = 2;
const int OTHER = 3;

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
GtkWidget *username_mainMenuLabel;
GtkWidget *boardWindow;

GtkWidget *playerNotificationBox;
GtkWidget *enemyNotificationBox;

GtkWidget *bgColorSquare;
GtkWidget *colorSquare;

GtkWidget *winDialog;
GtkWidget *loserDialog;
GtkWidget *confirmDialog;
GtkWidget *confirm2Dialog;
GtkWidget *playModeDialog;
GtkWidget *waitAnotherPlayerDialog;

GtkWidget *boardWindow;
GtkWidget *boardWindowFixed;
GtkWidget *controllerBox;
GtkWidget *playerBox;
GtkWidget *enemyBox;
GtkWidget *iconON;
GtkWidget *playerNameLabel;
GtkWidget *iconOFF;
GtkWidget *enemyNameLabel;
GtkWidget *chooseColorDialog;

GtkWidget *rankWindow;
GtkWidget *rankBox;
GtkWidget *rankGrid;
GtkWidget *usernameLabelBtn;

pthread_t tid;
pthread_t recv_msg_thread;

char buff[80];
int rcvBytes;
int sock_app;
Client *c;
send_room *send_r;
Play_With_Person *play2;
char usernameLogin[30];
char playModeStatus[50];
int checkSendMessage = 0;

UNO x_card = {0, 'x', -6};

UNO up_card;
UNO hand[52];
int hand_size = 7;
int enemy_size = 7;

guint timer = 0;
guint timer2 = 0;
guint serial_counter = 0;

int t = 0;      // t luu tong so quan bai bi phat
char mau = 'z'; // luu mau khi 1 trong hai danh con doi mau va chon mau
int chonMau = 0;
int idUser = PLAYER;
int drawCardCount = 0;
// luu giu id nguoi danh, idUser = 1 la nguoi, idUser = 2 la may
// id la id cua quan bai

int checkChooseColor = 0;
int notiCode;

//Check Input
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

// LOGIN SIGNIN

void on_loginBtn_clicked()
{
    gtk_widget_show(LoginWindow);
    gtk_widget_hide(beginWindow);
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
    c->signal = LOGIN;
    char tmp[128], username[128];
    sprintf(tmp, "%s", gtk_entry_get_text(GTK_ENTRY(usernameLoginEntry)));
    trim(tmp);
    strcpy(username, tmp);
    if (checkSpace(tmp) == 1)
    {
        //loi
        gtk_label_set_text(GTK_LABEL(errorLoginLabel), "username khong co dau cach!");
    }
    else
    {
        strcpy(c->login.username, tmp);
        sprintf(tmp, "%s", gtk_entry_get_text(GTK_ENTRY(passwordLoginEntry)));
        if (checkSpace(tmp) == 1)
        {
            //loi
            gtk_label_set_text(GTK_LABEL(errorLoginLabel), "password khong co dau cach!");
        }
        else
        {
            strcpy(c->login.password, tmp);
            send(sock_app, c, sizeof(Client), 0);
            rcvBytes = recv(sock_app, buff, BUFF_SIZE, 0);
            if (rcvBytes < 0)
            {
                perror("Error: ");
                return;
            }
            buff[rcvBytes] = '\0';
            if (strcmp(buff, "OK") == 0)
            {
                memset(usernameLogin, 0, strlen(usernameLogin));
                strcpy(usernameLogin, username);
                gtk_label_set_text(GTK_LABEL(username_mainMenuLabel), usernameLogin);

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
    c->signal = SIGNUP;
    char tmp[128];
    sprintf(tmp, "%s", gtk_entry_get_text(GTK_ENTRY(usernameRegEntry)));
    trim(tmp);
    if (checkSpace(tmp) == 1)
    {
        //loi
        gtk_label_set_text(GTK_LABEL(errorRegisLabel), "username khong co dau cach!");
    }
    else
    {

        strcpy(c->signup.username, tmp);
        sprintf(tmp, "%s", gtk_entry_get_text(GTK_ENTRY(passwordRegEntry)));
        if (checkSpace(tmp) == 1)
        {
            //loi
            gtk_label_set_text(GTK_LABEL(errorRegisLabel), "password khong co dau cach!");
        }
        else
        {

            strcpy(c->signup.password, tmp);
            sprintf(tmp, "%s", gtk_entry_get_text(GTK_ENTRY(passwordAgainRegEntry)));
            if (checkSpace(tmp) == 1)
            {
                //loi
                gtk_label_set_text(GTK_LABEL(errorRegisLabel), "password khong co dau cach!");
            }
            else
            {

                strcpy(c->signup.confirm_password, tmp);

                send(sock_app, c, sizeof(Client), 0);
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
    gtk_widget_show_all(confirmDialog);
}

void on_startGameBtn_clicked()
{
    gtk_widget_show_all(playModeDialog);
    gtk_widget_hide(mainMenuWindow);
}

void on_playerModeBtn_clicked()
{
    gtk_widget_hide(playModeDialog);
    main_play_with_player();
    // printf("player game with player!\n");
}

void on_botModeBtn_clicked()
{
    gtk_widget_hide(playModeDialog);
    gtk_widget_show_all(boardWindow);
    main_play_game_with_bot();
}

void on_viewRankBtn_clicked()
{
    c->signal = VIEW_RANK;
    //printf("%d\n",c->signal);
    send(sock_app, c, sizeof(Client), 0);
    rcvBytes = recv(sock_app, buff, BUFF_SIZE, 0);
    buff[rcvBytes] = '\0';
    printf("%s\n", buff);

    gtk_widget_hide(mainMenuWindow);
    build_rankWindow();
    gtk_widget_show_all(rankWindow);
}

void on_logoutBtn_clicked()
{
    c->signal = LOGOUT;
    //printf("%d\n",c->signal);
    send(sock_app, c, sizeof(Client), 0);
    rcvBytes = recv(sock_app, buff, BUFF_SIZE, 0);
    buff[rcvBytes] = '\0';
    printf("%s\n", buff);
    memset(usernameLogin, 0, strlen(usernameLogin));
    gtk_widget_show(beginWindow);
    gtk_widget_hide(mainMenuWindow);
}

void on_cancelExitBtn_clicked()
{
    gtk_widget_hide(confirmDialog);
    gtk_widget_hide(confirm2Dialog);
}

void on_exitBtn_clicked()
{
    if (c->signal == PLAY_WITH_BOT && c->play_with_bot.id_player == -1)
    {
        c->play_with_bot.id_player = 0;
        send(sock_app, c, sizeof(Client), 0);
    }
    // else if (c->signal == PLAY_WITH_PERSON)
    // { ///**
    //     c->play_with_person.so_luong_bai = -1;
    //     send(sock_app, c, sizeof(Client), 0);
    // }
    if (strlen(usernameLogin) != 0)
    {
        c->signal = LOGOUT;
        send(sock_app, c, sizeof(Client), 0);
        rcvBytes = recv(sock_app, buff, BUFF_SIZE, 0);
        buff[rcvBytes] = '\0';
        printf("%s\n", buff);
        memset(usernameLogin, 0, strlen(usernameLogin));
    }
    gtk_main_quit();
}

//GAME BUTTON
//choose color - handle change_the_color_card
void after_chooseColor()
{
    checkChooseColor = 0;
    gtk_widget_hide(chooseColorDialog);
    gtk_window_set_accept_focus(GTK_WINDOW(boardWindow), TRUE);
    change_on_off_icon(ENEMY);
    draw_colorSquare();
    if (check_player_win() == 1)
    {
        idUser = OTHER;
    }
    else
    {
        idUser = ENEMY;
    }
    if (c->signal == PLAY_WITH_PERSON)
    {
        send_msg_handler();
    }
}

void on_chooseRedBtn_clicked()
{
    mau = 'r';
    after_chooseColor();
}
void on_chooseGreenBtn_clicked()
{
    mau = 'g';
    after_chooseColor();
}
void on_chooseBlueBtn_clicked()
{
    mau = 'b';
    after_chooseColor();
}
void on_chooseYellowBtn_clicked()
{
    mau = 'y';
    after_chooseColor();
}

//after win|over game
void on_backToHomeBtn_clicked()
{
    gtk_widget_show(mainMenuWindow);
    gtk_widget_hide(winDialog);
    gtk_widget_hide(loserDialog);
    gtk_widget_hide(boardWindow);
}
void on_replayBtn_clicked()
{
    gtk_widget_hide(winDialog);
    gtk_widget_hide(loserDialog);
    main_play_game_with_bot();
}

//while playing games
void unoButtonClick(GtkWidget *button)
{
    if (idUser == PLAYER && hand_size == 1)
    {
        g_print("uno button click\n");
        int fal = 0;
        checkTimeOutUnoButton(&fal);
    }
}
void nextButtonClick(GtkWidget *button)
{
    if (drawCardCount == 1 && idUser == PLAYER && c->signal == PLAY_WITH_BOT)
    {
        drawCardCount = 0;
        idUser = ENEMY;
        change_on_off_icon(ENEMY);
    }
    else if (drawCardCount == 1 && idUser == PLAYER && c->signal == PLAY_WITH_PERSON)
    {
        printf("next clicked\n");
        drawCardCount = 0;
        idUser = ENEMY;
        change_on_off_icon(ENEMY);
        c->play_with_person.played = 0;
        send_msg_handler();
        // if (pthread_create(&recv_msg_thread, NULL, (void *)recv_msg_handler, NULL) != 0)
        // {
        //     printf("Create pthread error!\n");
        //     exit(EXIT_FAILURE);
        // }
    }
}
void drawCardButtonClick(GtkWidget *button)
{
    if (drawCardCount == 0 && idUser == PLAYER && c->signal == PLAY_WITH_BOT)
    {
        phat(1, &l1, &s);
        hand_size++;
        draw_hand(playerBox);

        NODE *p = find(l, up_card.id);
        UNO uno;
        if (CHECK(l1, up_card.id, &uno) != 1 && doiMau(l1, mau, p, t) != 1)
        {
            idUser = ENEMY;
            change_on_off_icon(ENEMY);
            return;
        }
        drawCardCount = 1;
    }
    else if (drawCardCount == 0 && idUser == PLAYER && c->signal == PLAY_WITH_PERSON)
    {
        printf("draw clicked\n");
        phat(1, &l1, &s);
        hand_size++;
        draw_hand(playerBox);
        // c->play_with_person.played = -1;
        // send_msg_handler();

        NODE *p = find(l, up_card.id);
        UNO uno;
        if (CHECK(l1, up_card.id, &uno) != 1 && doiMau(l1, mau, p, t) != 1)
        {
            idUser = ENEMY;
            change_on_off_icon(ENEMY);
            c->play_with_person.played = 0;
            send_msg_handler();

            return;
        }
        else
        {
            c->play_with_person.played = -1;
            send_msg_handler();
        }
        drawCardCount = 1;
    }
}

void on_usernameLabelBtn_clicked()
{
    gtk_widget_show_all(confirm2Dialog);
}

void on_backMainMenuBtn_clicked()
{
    gtk_widget_show_all(mainMenuWindow);
    gtk_widget_hide(boardWindow);
    if (c->signal == PLAY_WITH_BOT && c->play_with_bot.id_player == -1)
    {
        c->play_with_bot.id_player = 0;
        send(sock_app, c, sizeof(Client), 0);
    }
    else if (c->signal == PLAY_WITH_PERSON)
    { ///**
        c->play_with_person.so_luong_bai = -1;
        send(sock_app, c, sizeof(Client), 0);
    }
    gtk_widget_hide(confirm2Dialog);
    idUser = OTHER;
    reset_board_game();
}

//handle game

void main_play_game_with_bot()
{
    c->signal = PLAY_WITH_BOT;
    c->play_with_bot.id_player = -1;
    Init(&l);
    loadTuFile(fileIn, &l); // do bai tu file vao dslk
    inPutStack(&s, l);
    //**********************************************************************************
    hand_size = 7;
    idUser = PLAYER;
    inPutL1(&s, &l1);
    draw_hand(playerBox);

    enemy_size = 7;
    inPutL1(&s, &l2);
    draw_enemyCards();

    up_card = pop(&s);

    //******vào lượt người chơi:

    //xử lý +2 | +4 | mất lượt | chuyển màu | xoay vòng
    if (up_card.number == -3)
    {
        t = 2;
    }
    else if (up_card.number == -5)
    {
        t = 4;
        mau = 'r';
    }
    else if (up_card.number == -4)
    {
        mau = 'r';
    }
    else if (up_card.number == -1 || up_card.number == -2)
    {
        idUser = ENEMY;
    }

    gtk_button_set_label(GTK_BUTTON(usernameLabelBtn), usernameLogin);
    gtk_label_set_text(GTK_LABEL(playerNameLabel), usernameLogin);
    gtk_label_set_text(GTK_LABEL(enemyNameLabel), "Bot");
    clear_container(controllerBox);
    draw_card(controllerBox, &up_card, OTHER, 2);
    draw_colorSquare();

    if (check_up_card(&l1, &hand_size) != 1)
    {
        idUser = ENEMY;
        bot_play();
    }
}

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

//draw game
void buildUIGameWindow()
{
    boardWindow = GTK_WIDGET(gtk_builder_get_object(builder, "boardWindow"));

    //add container
    boardWindowFixed = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(boardWindow), boardWindowFixed);

    //add background of window
    GdkPixbuf *backgrndPixbuf;
    GtkWidget *backgrndImage;
    GError *error = NULL;
    backgrndPixbuf = gdk_pixbuf_new_from_file("images/background/bgB.png", &error);
    backgrndPixbuf = gdk_pixbuf_scale_simple(backgrndPixbuf, WINDOW_WIDTH, WINDOW_HEIGHT, GDK_INTERP_BILINEAR);
    backgrndImage = gtk_image_new_from_pixbuf(backgrndPixbuf);
    gtk_container_add(GTK_CONTAINER(boardWindowFixed), backgrndImage);

    //*****************************************************************************************************
    //ADD CONTROLLER: up card | draw button | next button | uno button
    int startWidth = (WINDOW_WIDTH - CARD_WIDTH * 8) / 2 + 50;
    int heightControllerBox = (WINDOW_HEIGHT - CARD_HEIGHT * 2) / 2;
    //1.. up card
    controllerBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_fixed_put(GTK_FIXED(boardWindowFixed), controllerBox, startWidth, heightControllerBox);
    // draw_up_card(&x_card);

    draw_card(controllerBox, &x_card, OTHER, 2);
    //2.add draw card button
    char link[100];
    strcpy(link, "images/button/pile.png");
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
    g_signal_connect(drawCardButton, "event-after", G_CALLBACK(bot_play), NULL);
    //2.5 add colorSquare
    strcpy(link, "images/color/white.png");
    bgColorSquare = gtk_image_new_from_file(link);
    bgColorSquare = resize_image(bgColorSquare, link, 1);
    gtk_fixed_put(GTK_FIXED(boardWindowFixed), bgColorSquare, startWidth + CARD_WIDTH * 5, heightControllerBox + 5);
    strcpy(link, "images/color/r.png");
    colorSquare = gtk_image_new_from_file(link);
    // colorSquare = resize_image(colorSquare, link, 1);
    gtk_fixed_put(GTK_FIXED(boardWindowFixed), colorSquare, startWidth + CARD_WIDTH * 5 + 5, heightControllerBox + 10);
    //3.add next button
    strcpy(link, "images/button/next.png");
    GtkWidget *nextImage = gtk_image_new_from_file(link);
    nextImage = resize_image(nextImage, link, 0.8);
    // gtk_widget_set_size_request(nextImage, CARD_WIDTH, CARD_HEIGHT);
    GtkWidget *nextButton = gtk_button_new();
    //add image to button
    gtk_button_set_always_show_image(GTK_BUTTON(nextButton), TRUE);
    gtk_button_set_image(GTK_BUTTON(nextButton), nextImage);
    //add to box
    gtk_fixed_put(GTK_FIXED(boardWindowFixed), nextButton, startWidth + CARD_WIDTH * 5, heightControllerBox + 67); //45
    g_signal_connect(nextButton, "clicked", G_CALLBACK(nextButtonClick), NULL);
    g_signal_connect(nextButton, "event-after", G_CALLBACK(bot_play), NULL);
    //4.add uno button
    strcpy(link, "images/button/uno.png");
    GtkWidget *unoImage = gtk_image_new_from_file(link);
    unoImage = resize_image(unoImage, link, 0.8);
    // gtk_widget_set_size_request(unoImage, CARD_WIDTH, CARD_HEIGHT);
    GtkWidget *unoButton = gtk_button_new();
    //add image to button
    gtk_button_set_always_show_image(GTK_BUTTON(unoButton), TRUE);
    gtk_button_set_image(GTK_BUTTON(unoButton), unoImage);
    //add to box
    gtk_fixed_put(GTK_FIXED(boardWindowFixed), unoButton, startWidth + CARD_WIDTH * 5, heightControllerBox + 125); //115
    g_signal_connect(unoButton, "clicked", G_CALLBACK(unoButtonClick), NULL);
    //****************************************************************************************
    strcpy(link, "images/messages/isPlay.png");
    iconON = gtk_image_new_from_file(link);
    gtk_fixed_put(GTK_FIXED(boardWindowFixed), iconON, 60, 660);

    GtkWidget *playerNameLabelBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_widget_set_size_request(playerNameLabelBox, 155, 30);
    gtk_fixed_put(GTK_FIXED(boardWindowFixed), playerNameLabelBox, 0, 690);
    gchar *text = "<b><span foreground='#ffffff'>player</span></b>";
    playerNameLabel = gtk_label_new(text);
    gtk_label_set_use_markup(GTK_LABEL(playerNameLabel), TRUE);
    gtk_box_pack_start(GTK_BOX(playerNameLabelBox), playerNameLabel, TRUE, FALSE, 0);

    strcpy(link, "images/messages/isPause.png");
    iconOFF = gtk_image_new_from_file(link);
    gtk_fixed_put(GTK_FIXED(boardWindowFixed), iconOFF, 60, 110);

    GtkWidget *enemyNameLabelBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_widget_set_size_request(enemyNameLabelBox, 155, 30);
    gtk_fixed_put(GTK_FIXED(boardWindowFixed), enemyNameLabelBox, 0, 140);
    enemyNameLabel = gtk_label_new(text);
    gtk_label_set_use_markup(GTK_LABEL(enemyNameLabel), TRUE);
    gtk_box_pack_start(GTK_BOX(enemyNameLabelBox), enemyNameLabel, TRUE, FALSE, 0);
    //*****************************************************************************************
    enemyBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    gtk_widget_set_size_request(enemyBox, CARD_WIDTH * 12, CARD_HEIGHT);
    gtk_fixed_put(GTK_FIXED(boardWindowFixed), enemyBox, (WINDOW_WIDTH - CARD_WIDTH * 12) / 2, 80);

    playerBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    gtk_widget_set_size_request(playerBox, CARD_WIDTH * 12, CARD_HEIGHT);
    gtk_fixed_put(GTK_FIXED(boardWindowFixed), playerBox, (WINDOW_WIDTH - CARD_WIDTH * 12) / 2, 632);

    //******************************************************************
    playerNotificationBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_set_size_request(playerNotificationBox, CARD_WIDTH * 6, CARD_HEIGHT);
    gtk_fixed_put(GTK_FIXED(boardWindowFixed), playerNotificationBox, startWidth, heightControllerBox + CARD_HEIGHT * 2.5);

    //******************************************************************
    enemyNotificationBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_set_size_request(enemyNotificationBox, CARD_WIDTH * 6, CARD_HEIGHT);
    gtk_fixed_put(GTK_FIXED(boardWindowFixed), enemyNotificationBox, startWidth, heightControllerBox - CARD_HEIGHT * 1.25);

    //***************************8

    GtkWidget *menuBoardGrid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(menuBoardGrid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(menuBoardGrid), 5);
    gtk_fixed_put(GTK_FIXED(boardWindowFixed), menuBoardGrid, 1050, 15);

    usernameLabelBtn = gtk_button_new_with_label(usernameLogin);
    strcpy(link, "images/button/homeS.png");
    GtkWidget *backMainMenuimage = gtk_image_new_from_file(link);
    gtk_button_set_always_show_image(GTK_BUTTON(usernameLabelBtn), TRUE);
    gtk_button_set_image(GTK_BUTTON(usernameLabelBtn), backMainMenuimage);
    gtk_button_set_image_position(GTK_BUTTON(usernameLabelBtn), GTK_POS_RIGHT);
    gtk_grid_attach(GTK_GRID(menuBoardGrid), usernameLabelBtn, 0, 0, 1, 1);
    g_signal_connect(usernameLabelBtn, "clicked", G_CALLBACK(on_usernameLabelBtn_clicked), NULL);

    // strcpy(link, "images/button/homeS.png");
    // GtkWidget *backMainMenuimage = gtk_image_new_from_file(link);
    // GtkWidget* backMainMenuBtn = gtk_button_new();
    // gtk_button_set_always_show_image(GTK_BUTTON(backMainMenuBtn), TRUE);
    // gtk_button_set_image(GTK_BUTTON(backMainMenuBtn), backMainMenuimage);
    // gtk_grid_attach(GTK_GRID(menuBoardGrid), backMainMenuBtn, 0, 1, 1, 1);

    // strcpy(link, "images/button/rankingS.png");
    // GtkWidget *rankfromBoardimage = gtk_image_new_from_file(link);
    // GtkWidget* rankfromBoardBtn = gtk_button_new();
    // gtk_button_set_always_show_image(GTK_BUTTON(rankfromBoardBtn), TRUE);
    // gtk_button_set_image(GTK_BUTTON(rankfromBoardBtn), rankfromBoardimage);
    // gtk_grid_attach(GTK_GRID(menuBoardGrid), rankfromBoardBtn, 1, 0, 1, 1);
}

/**
 * Draw the user's hand
 *
 */
void draw_hand(GtkWidget *container)
{
    clear_container(container);
    float sizeCard = 1;

    if (hand_size <= 3)
    {
        gtk_widget_set_size_request(playerBox, CARD_WIDTH * 6, CARD_HEIGHT);
        gtk_fixed_move(GTK_FIXED(boardWindowFixed), playerBox, (WINDOW_WIDTH - CARD_WIDTH * 6) / 2, 632);
    }
    else if (hand_size > 3 && hand_size < 9)
    {
        gtk_widget_set_size_request(playerBox, CARD_WIDTH * 12, CARD_HEIGHT);
        gtk_fixed_move(GTK_FIXED(boardWindowFixed), playerBox, (WINDOW_WIDTH - CARD_WIDTH * 12) / 2, 632);
    }
    else
    {
        gtk_widget_set_size_request(playerBox, CARD_WIDTH * 17, CARD_HEIGHT);
        gtk_fixed_move(GTK_FIXED(boardWindowFixed), playerBox, (WINDOW_WIDTH - CARD_WIDTH * 17) / 2, 632);
        if (hand_size > 12)
        {
            sizeCard = 0.6;
            gtk_widget_set_size_request(playerBox, CARD_WIDTH * 17, CARD_HEIGHT * sizeCard);
        }
    }
    // draw all cards in the hand
    for (NODE *p = l1.pHead; p != NULL; p = p->pNext)
    {
        draw_card(container, &(p->data), PLAYER, sizeCard);
    }
    // draw the up card
    clear_container(controllerBox);
    draw_card(controllerBox, &up_card, OTHER, 2);
    draw_colorSquare();
}

void draw_enemyCards()
{
    clear_container(enemyBox);
    // draw all cards in the hand of enemy
    float sizeCard = 1;
    if (enemy_size <= 3)
    {
        gtk_widget_set_size_request(enemyBox, CARD_WIDTH * 6, CARD_HEIGHT);
        gtk_fixed_move(GTK_FIXED(boardWindowFixed), enemyBox, (WINDOW_WIDTH - CARD_WIDTH * 6) / 2, 80);
    }
    else if (enemy_size > 3 && enemy_size < 9)
    {
        gtk_widget_set_size_request(enemyBox, CARD_WIDTH * 12, CARD_HEIGHT);
        gtk_fixed_move(GTK_FIXED(boardWindowFixed), enemyBox, (WINDOW_WIDTH - CARD_WIDTH * 12) / 2, 80);
    }
    else
    {
        gtk_widget_set_size_request(enemyBox, CARD_WIDTH * 17, CARD_HEIGHT);
        gtk_fixed_move(GTK_FIXED(boardWindowFixed), enemyBox, (WINDOW_WIDTH - CARD_WIDTH * 17) / 2, 80);
        if (enemy_size > 12)
        {
            sizeCard = 0.6;
            gtk_widget_set_size_request(enemyBox, CARD_WIDTH * 17, CARD_HEIGHT * sizeCard);
        }
    }

    for (int i = 0; i < enemy_size; i++)
    {
        // draw the current card
        draw_card(enemyBox, &x_card, ENEMY, sizeCard);
    }

    clear_container(controllerBox);
    draw_card(controllerBox, &up_card, OTHER, 2);
    draw_colorSquare();
    if (c->signal == PLAY_WITH_BOT)
        check_bot_win();
}

static gint draw_enemyCardsPlayer2(gpointer status)
{
    int i = 0;
    while (i < 2)
    {
        draw_enemyCards();
        i++;
    }
    return 0;
}

static gint draw_handCards(gpointer status)
{
    int i = 0;
    while (i < 2)
    {
        draw_hand(playerBox);
        i++;
    }
    return 0;
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
    // system("rm tmp.jpg");
    return image;
}

void reset_board_game()
{
    mau = 'z';
    hand_size = 0;
    enemy_size = 0;
    outPut(&s);
    giaiPhong(&l1);
    giaiPhong(&l2);
    // pthread_cancel(tid);
    // pthread_exit(NULL);
}

int check_player_win()
{
    if (hand_size == 0)
    {
        if (c->signal == PLAY_WITH_BOT)
        {
            c->play_with_bot.id_player = 1;
            send(sock_app, c, sizeof(Client), 0);
        }
        // else
        // {
        //     c->play_with_person.played = 1;
        //     // send(sock_app, c, sizeof(Client), 0);
        //     send_msg_handler();
        // }
        printf("player Win\n");
        gtk_widget_show(winDialog);
        gtk_window_set_accept_focus(GTK_WINDOW(boardWindow), FALSE);
        idUser = OTHER;
        // reset_board_game();
        return 1;
    }
    return 0;
}

int check_bot_win()
{
    if (enemy_size == 0)
    {
        if (c->signal == PLAY_WITH_BOT)
        {
            c->play_with_bot.id_player = 0;
            send(sock_app, c, sizeof(Client), 0);
        }
        printf("bot Win\n");
        gtk_widget_show(loserDialog);
        gtk_window_set_accept_focus(GTK_WINDOW(boardWindow), FALSE);
        idUser = OTHER;
        reset_board_game();
        return 1;
    }
    return 0;
}

int check_up_card(LIST *xxx, int *cml) //check phạt ko đỡ đk thì phạt luôn
{
    change_on_off_icon(PLAYER);

    NODE *p = find(l, up_card.id);
    UNO uno;
    if (CHECK(*xxx, up_card.id, &uno) != 1 && doiMau(*xxx, mau, p, t) != 1)
    {
        //************** bị phat
        if ((p->data.number == -3 || p->data.number == -5) && t != 0)
        {
            if ((c->signal == PLAY_WITH_PERSON && play2->played == 1) || (c->signal == PLAY_WITH_BOT))
            {

                if (p->data.number == -3)
                {
                    mau = p->data.color;
                }
                printf("\n\nbi phat %d con bai\n", t);
                phat(t, xxx, &s);
                *cml += t;
                t = 0;
                printf("truoc draw\n");

                if (c->signal == PLAY_WITH_PERSON)
                {
                    int tr = 1;
                    drawHandCardsFromRecv(&tr);
                }
                else
                {
                    draw_hand(playerBox);
                }
                printf("sau draw\n");
                free(p);
                return 0;
            }
        }
    }
    free(p);
    return 1;
}

void mayDanh(LIST *xxx, int *idUser, int *id, int *t, int *cml, char *mau, int *chonMau)
{
    int checkPlayCard = 0;
    NODE *p = find(l, *id);
    UNO uno;
    int ID = *id;
    printf("\n\n=== MAY DANH ===");
    show(*xxx);
    if (CHECK(*xxx, *id, &uno) == 1 || doiMau(*xxx, *mau, p, *t) == 1)
    {
        printf("-danhbai1-\n");
        danhBaiChoMay(xxx, id, cml, mau, t);
        up_card = timUno(l, *id);
        checkPlayCard = 1;
    }
    else
    {
        if ((up_card.number == -3 || up_card.number == -5) && *t != 0)
        {
            printf("bi phat %d con bai", *t);
            phat(*t, xxx, &s);
            *cml += *t;
            *t = 0;

            if (up_card.number == -3)
            {
                *mau = up_card.color;
            }
        }
        else
        {
            printf("\nhe thong da tu dong boc bai.");
            phat(1, xxx, &s);
            *cml += 1;

            show(*xxx);
            if (CHECK(*xxx, *id, &uno) == 1 || doiMau(*xxx, *mau, p, *t) == 1)
            {
                printf("-danhbai2-\n");
                danhBaiChoMay(xxx, id, cml, mau, t);
                up_card = timUno(l, *id);
                checkPlayCard = 1;
            }
        }
    }
    printf("done\n");
    // kiem tra xem luot danh tiep theo thuoc ve ai
    if (up_card.number == -1 && *id != ID)
    {
        *idUser = ENEMY;
        // notificationThread(1, PLAYER);
    }
    else if (up_card.number == -2 && *id != ID)
    {
        *idUser = ENEMY;
        // notificationThread(2, PLAYER);
    }
    else
    {
        if (up_card.number == -5 && checkPlayCard == 1)
        {
            // notificationThread(5, PLAYER);
        }
        else if (up_card.number == -3 && checkPlayCard == 1)
        {
            // notificationThread(3, PLAYER);
        }
        *idUser = 1;
    }
    free(p);
}

int app(int argc, char **argv, int sockfd)
{
    XInitThreads();
    gtk_init(&argc, &argv);
    sock_app = sockfd;
    c = (Client *)malloc(sizeof(Client));

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
    username_mainMenuLabel = GTK_WIDGET(gtk_builder_get_object(builder, "username_mainMenuLabel"));

    boardWindow = GTK_WIDGET(gtk_builder_get_object(builder, "boardWindow"));
    buildUIGameWindow();

    gtk_widget_hide(boardWindow);

    chooseColorDialog = GTK_WIDGET(gtk_builder_get_object(builder, "chooseColorDialog"));

    winDialog = GTK_WIDGET(gtk_builder_get_object(builder, "winDialog"));
    loserDialog = GTK_WIDGET(gtk_builder_get_object(builder, "loserDialog"));
    confirmDialog = GTK_WIDGET(gtk_builder_get_object(builder, "confirmDialog"));
    confirm2Dialog = GTK_WIDGET(gtk_builder_get_object(builder, "confirm2Dialog"));
    playModeDialog = GTK_WIDGET(gtk_builder_get_object(builder, "playModeDialog"));
    waitAnotherPlayerDialog = GTK_WIDGET(gtk_builder_get_object(builder, "waitAnotherPlayerDialog"));

    rankWindow = GTK_WIDGET(gtk_builder_get_object(builder, "rankWindow"));
    rankBox = GTK_WIDGET(gtk_builder_get_object(builder, "rankBox"));
    rankGrid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(rankGrid), 20);
    gtk_grid_set_column_spacing(GTK_GRID(rankGrid), 40);
    gtk_widget_set_size_request(rankGrid, -1, 540);
    gtk_container_add(GTK_CONTAINER(rankBox), rankGrid);

    g_signal_connect(beginWindow, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_builder_connect_signals(builder, NULL);

    // fixed1 = GTK_WIDGET(gtk_builder_get_object(builder, "fixed1"));

    gtk_widget_show(beginWindow);

    gtk_main();

    giaiPhong(&l1); // giải phóng bộ nhớ
    giaiPhong(&l2);
    giaiPhong(&l);
    outPut(&s);
    pthread_cancel(recv_msg_thread);
    // pthread_exit(NULL);
    // return 0;
    return EXIT_SUCCESS;
}

/**
 * Event handler for a card being clicked
 */
void card_clicked(GtkWidget *card_button, gpointer card_data)
{
    // cast generic gpointer to a card
    UNO *card = (UNO *)card_data;
    // play the card
    if (idUser == 1)
    {
        if (play(card) == 1)
        {
            if (hand_size == 1)
            {

                int tr = 1;
                checkTimeOutUnoButton(&tr);
            }
            else
            {
                getNextPlayer();
            }
        }
    }
}

void getNextPlayer()
{
    if (c->signal == PLAY_WITH_BOT)
    {
        if (up_card.number == -1)
        {
            idUser = PLAYER;
            // notificationThread(1, ENEMY);
        }
        else if (up_card.number == -2)
        {
            idUser = PLAYER;
            // notificationThread(2, ENEMY);
        }
        else
        {
            idUser = ENEMY;
            if (checkChooseColor != 1)
                change_on_off_icon(ENEMY);
        }
    }
    else if (c->signal == PLAY_WITH_PERSON)
    {
        if (up_card.number == -1)
        {
            c->play_with_person.played = -1;
            idUser = PLAYER;
            // notificationThread(1, ENEMY);
            send_msg_handler();
            //     if (pthread_create(&recv_msg_thread, NULL, (void *)recv_msg_handler, NULL) != 0)
            // {
            //     printf("Create pthread error!\n");
            //     exit(EXIT_FAILURE);
            // }
        }
        else if (up_card.number == -2)
        {
            c->play_with_person.played = -1;
            idUser = PLAYER;
            // notificationThread(2, ENEMY);
            send_msg_handler();
            //     if (pthread_create(&recv_msg_thread, NULL, (void *)recv_msg_handler, NULL) != 0)
            // {
            //     printf("Create pthread error!\n");
            //     exit(EXIT_FAILURE);
            // }
        }
        else
        {
            c->play_with_person.played = 1;
            idUser = ENEMY;
            if (checkChooseColor != 1)
            {
                change_on_off_icon(ENEMY);
                send_msg_handler();
                //         if (pthread_create(&recv_msg_thread, NULL, (void *)recv_msg_handler, NULL) != 0)
                // {
                //     printf("Create pthread error!\n");
                //     exit(EXIT_FAILURE);
                // }
            }
        }
    }
}

void bot_play()
{
    if (checkChooseColor != 1 && (hand_size != 0 && enemy_size != 0) && c->signal == PLAY_WITH_BOT)
    {
        if (idUser == ENEMY)
        {
            int id = up_card.id;
            mayDanh(&l2, &idUser, &id, &t, &enemy_size, &mau, &chonMau); //xxxsss
            printf("\n%c ---- %c-%d\n", mau, up_card.color, up_card.number);
            up_card = timUno(l, id);
            draw_enemyCards();

            if (check_up_card(&l1, &hand_size) != 1)
            {
                idUser = ENEMY;
                bot_play();
            }
        }
    }
}

/**
 * Play the given card
 * @param card Card to play
 * @return False if card cannot be played, true otherwise
 */
int play(UNO *card)
{
    UNO clicked_card = *card;
    if (mau != 'z')
    {
        if ((clicked_card.color != mau) && (clicked_card.color != 'k') && (clicked_card.number != up_card.number))
        {
            return 0;
        }
    }
    else
    {
        if (kt(up_card, clicked_card) != 1)
        {
            return 0;
        }
    }

    if (mau != 'z')
    {
        mau = 'z';
    }

    // cap nhat so quan bai bi phat
    soQuanBiPhat(clicked_card.number, &t);
    deleteNode(&l1, clicked_card.id);
    hand_size -= 1;
    // draw the new game state
    up_card = clicked_card;
    draw_hand(playerBox);

    drawCardCount = 0;
    if (up_card.number == -4 || up_card.number == -5)
    {
        gtk_widget_show(chooseColorDialog);
        gtk_window_set_accept_focus(GTK_WINDOW(boardWindow), FALSE);
        checkChooseColor = 1;
    }
    else
    {
        check_player_win();
    }
    return 1;
}

static gboolean serial_data(gpointer user_data)
{
    printf("counter: %d\n", serial_counter);
    serial_counter++;
    if (serial_counter > 2)
    {
        int fal = 0;
        checkTimeOutUnoButton(&fal);
        return FALSE;
    }
    return TRUE;
}

static void checkTimeOutUnoButton(gpointer user_data)
{
    int status = *((int *)user_data);

    if (status == 1)
    {
        timer = g_timeout_add(1000, serial_data, &status);
    }
    else if (status == 0 && timer != 0)
    {
        g_source_remove(timer);
        timer = 0;
        if (serial_counter < 3)
        {
            printf("play uno \n");
        }
        else
        {
            printf("phat 2\n");
            phat(2, &l1, &s);
            hand_size += 2;
            draw_hand(playerBox);
            // if (c->signal == PLAY_WITH_PERSON){
            //     c->play_with_person.played = -1;
            //     send_msg_handler();
            //         if (pthread_create(&recv_msg_thread, NULL, (void *)recv_msg_handler, NULL) != 0)
            // {
            //     printf("Create pthread error!\n");
            //     exit(EXIT_FAILURE);
            // }
        }
        // }
        printf("cancel %d\n", serial_counter);
        serial_counter = 0;
        getNextPlayer();
    }
}

void main_play_with_player()
{
    //build wait screen
    gtk_widget_hide(playModeDialog);
    gtk_widget_show_all(waitAnotherPlayerDialog);

    //build boardgame();
    c->signal = ADD_ROOM;
    send(sock_app, c, sizeof(Client), 0);
    connect_with_another_player(1);
}

void on_backfromWaitPlayer_clicked()
{
    connect_with_another_player(-1);
}

int connect_with_another_player(int status)
{
    if (status == 1)
    {
        timer = g_timeout_add(1000, check_connect_other_player, &status);
    }
    else if (status == 0 && timer != 0)
    {
        g_source_remove(timer);
        timer = 0;
        c->signal = PLAY_WITH_PERSON;
        c->play_with_person.id_room = send_r->id_room;
        gtk_widget_hide(waitAnotherPlayerDialog);
        build_board_game_with_player();
        gtk_widget_show_all(boardWindow);
    }
    else if (timer != 0)
    {
        ///**
        // c->signal = NONE;
        // send(sock_app, c, sizeof(Client), 0);
        g_source_remove(timer);
        timer = 0;
        // recv(sock_app, buff, BUFF_SIZE, 0);
        gtk_widget_hide(waitAnotherPlayerDialog);
        gtk_widget_show_all(mainMenuWindow);
    }
    return 1;
}
static gint check_connect_other_player(gpointer status)
{
    send_r = (send_room *)malloc(sizeof(send_room));
    rcvBytes = recv(sock_app, send_r, sizeof(send_room), 0);
    printf("%s\n", send_r->messages);
    if (strstr(send_r->messages, "OK") == NULL)
    {
        send(sock_app, "wait", 10, 0);
        //printf("Please waiting player");
    }
    else
    {
        connect_with_another_player(0);
    }
    return 1;
}

//pthread

static void drawEnemyCardsFromRecv(gpointer user_data)
{
    int status = *((int *)user_data);

    if (status == 1)
    {
        timer = g_timeout_add(100, draw_enemyCardsPlayer2, NULL);
        // if (enemy_size == 0)
        // {
        //     gtk_widget_show(winDialog);
        // }
    }
    else if (status == 0 && timer != 0)
    {
        g_source_remove(timer);
        timer = 0;
    }
}

static void drawHandCardsFromRecv(gpointer user_data)
{
    int status = *((int *)user_data);

    if (status == 1)
    {
        timer2 = g_timeout_add(100, draw_handCards, NULL);
    }
    else if (status == 0 && timer2 != 0)
    {
        g_source_remove(timer2);
        timer2 = 0;
    }
}

void recv_msg_handler()
{
    pthread_detach(pthread_self());
    while (1)
    {
        printf("recv--\n");
        // if(checkSendMessage==1){
        int recvCheck = recv(sock_app, play2, sizeof(Play_With_Person), 0);
        if (recvCheck < 0)
            continue;
        printf("recv tu player truoc\n%d-%d-%c-%d-%d\n", play2->id_bai, play2->so_luong_bai, play2->color, play2->bai_phat, play2->so_luong_bai);

        int ibreak = 0;
        ///**
        if (play2->so_luong_bai == -1)
        {
            idUser = OTHER;
            c->play_with_person.so_luong_bai = 0;
            c->play_with_person.played = 0;
            //send_msg_handler();
            send(sock_app, c, sizeof(Client), 0);
            gtk_widget_show_all(winDialog);
            // gtk_widget_show_all(mainMenuWindow);
            gtk_widget_hide(boardWindow);
            // reset_board_game();
            ibreak = 1;
            // break;
        }

        if (play2->so_luong_bai == 0 && play2->played == 1)
        {
            idUser = OTHER;
            hand_size = -2;
            c->play_with_person.played = 0;
            //send_msg_handler();
            enemy_size = 0;
            // check_bot_win()
            gtk_widget_show(loserDialog);
            printf("bot_win\n");
            gtk_window_set_accept_focus(GTK_WINDOW(boardWindow), FALSE);
            idUser = OTHER;
            send_msg_handler();

            // up_card = timUno(l, play2->id_bai);
            // mau = play2->color;
            // t = play2->bai_phat;
            // enemy_size = play2->so_luong_bai;

            // int tr = 1;
            // drawEnemyCardsFromRecv(&tr);
            // reset_board_game();
            // break;
            ibreak = 1;
        }

        up_card = timUno(l, play2->id_bai);
        mau = play2->color;
        t = play2->bai_phat;
        enemy_size = play2->so_luong_bai;

        int tr = 1;
        drawEnemyCardsFromRecv(&tr);
        if (ibreak == 1 || enemy_size == 0)
        {
            change_on_off_icon(ENEMY);
            printf("win\n");
            break;
        }

        printf("%d-%d-%c-%d-%d\n", play2->id_bai, play2->so_luong_bai, play2->color, play2->bai_phat, play2->so_luong_bai);

        if (play2->played == -1)
        {
            printf("recv-- -1\n");
            c->play_with_person.played = -2;
            send_msg_handler();
            continue;
        }
        else if (play2->played == -2)
        {
            printf("recv-- -2\n");
            continue;
        }

        if (play2->id_player == 0)
        {
            change_on_off_icon(PLAYER);
            idUser = PLAYER;
        }
        else
        {
            change_on_off_icon(ENEMY);
            idUser = ENEMY;
        }

        if (check_up_card(&l1, &hand_size) != 1)
        {
            c->play_with_person.played = 0;
            idUser = ENEMY;
            change_on_off_icon(ENEMY);
            send_msg_handler();
        }

        printf("recv-- %d\n", up_card.id);
    }
}

void send_msg_handler()
{
    printf("send_msg\n");
    checkSendMessage = 1;
    c->play_with_person.id_player = 1;
    if (idUser == ENEMY)
    {
        c->play_with_person.id_player = 0;
    }
    else
    {
        c->play_with_person.id_player = 1;
    }
    c->play_with_person.id_bai = up_card.id;
    c->play_with_person.color = mau;
    c->play_with_person.bai_phat = t;
    c->play_with_person.so_luong_bai = hand_size;
    send(sock_app, c, sizeof(Client), 0);
}

// play with player
void build_board_game_with_player()
{
    gtk_button_set_label(GTK_BUTTON(usernameLabelBtn), usernameLogin);
    gtk_label_set_text(GTK_LABEL(playerNameLabel), usernameLogin);
    gtk_label_set_text(GTK_LABEL(enemyNameLabel), "Bot");

    //**********************************************************************************
    //khởi tạo: 7 bài được phát || -> || id người đánh tiếp theo | con bài đầu tiên, mau = 'z', phat = 0;
    play2 = (Play_With_Person *)malloc(sizeof(Play_With_Person));
    Init(&l);
    loadTuFile(fileIn, &l);
    ATOI(l, &l1, send_r->list);
    printf("%s\n", send_r->list);
    hand_size = 7;
    draw_hand(playerBox);

    recv(sock_app, play2, sizeof(Play_With_Person), 0);
    enemy_size = play2->so_luong_bai;
    draw_enemyCards();
    if (play2->id_player == 0)
    {
        change_on_off_icon(PLAYER);
        idUser = PLAYER;
    }
    else
    {
        checkSendMessage = 1;
        change_on_off_icon(ENEMY);
        idUser = ENEMY;
    }

    up_card = timUno(l, play2->id_bai);

    //******vào lượt người chơi:

    //xử lý +2 | +4 | mất lượt | chuyển màu | xoay vòng
    if (up_card.number == -3)
    {
        t = 2;
    }
    else if (up_card.number == -5)
    {
        t = 4;
        mau = 'r';
    }
    else if (up_card.number == -4)
    {
        mau = 'r';
    }
    else if (up_card.number == -1 || up_card.number == -2)
    {
        idUser = ENEMY;
    }

    clear_container(controllerBox);
    draw_card(controllerBox, &up_card, OTHER, 2);
    draw_colorSquare();

    // if (check_up_card(&l1, &hand_size) != 1)
    // {
    //     idUser = ENEMY;
    //     bot_play();
    // }
    //*************************************************888****************8

    // fake data
    // pthread_t send_msg_thread;
    // if (pthread_create(&send_msg_thread, NULL, (void *)send_msg_handler, NULL) != 0)
    // {
    //     printf("Create pthread error!\n");
    //     exit(EXIT_FAILURE);
    // }

    pthread_t recv_msg_thread;
    if (pthread_create(&recv_msg_thread, NULL, (void *)recv_msg_handler, NULL) != 0)
    {
        printf("Create pthread error!\n");
        exit(EXIT_FAILURE);
    }

    //sau mỗi lần đánh card_click:
    //gửi đến room: id người đánh kế tiếp | quân bài vừa đánh (up_Card) | màu | phạt
    //khi client yêu cầu draw card: lấy về id của card đk phát |
    //khi client yêu cầu next lượt: người đánh là người còn lại
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

    if (status == PLAYER)
    {
        g_signal_connect(button, "clicked", G_CALLBACK(card_clicked), card);
        g_signal_connect(button, "event-after", G_CALLBACK(bot_play), NULL);
    }
    // draw card
    gtk_box_pack_start(GTK_BOX(container), button, TRUE, FALSE, 0);

    gtk_widget_show(button);
}

void draw_colorSquare()
{
    char link[50];
    memset(link, 0, strlen(link));
    strcpy(link, "images/color/");

    int checkNoColor = 0;

    if (mau != 'z')
    {
        link[strlen(link)] = mau;
    }
    else
    {
        link[strlen(link)] = up_card.color;
        if (up_card.color == 'k')
            checkNoColor = 1;
    }

    if (checkNoColor != 1)
    {
        strcat(link, ".png");
        gtk_image_set_from_file(GTK_IMAGE(colorSquare), link);
    }
}

void change_on_off_icon(int mode)
{
    if (mode == ENEMY)
    {
        printf("luot doi thu\n");
        gtk_fixed_move(GTK_FIXED(boardWindowFixed), iconON, 60, 110);
        gtk_fixed_move(GTK_FIXED(boardWindowFixed), iconOFF, 60, 660);
    }
    else if (mode == PLAYER)
    {
        printf("luot player\n");
        gtk_fixed_move(GTK_FIXED(boardWindowFixed), iconON, 60, 660);
        gtk_fixed_move(GTK_FIXED(boardWindowFixed), iconOFF, 60, 110);
    }
}

void *DisplayNotification(void *noti)
{ // code == mode*10 + user
    printf("thong bao\n");
    int mode = notiCode / 10;
    int user = notiCode % 10;
    char link[100];
    switch (mode)
    {
    case 1:
        strcpy(link, "images/messages/1.png");
        break;
    case 2:
        if (user == PLAYER)
            strcpy(link, "images/messages/22.png");
        else
            strcpy(link, "images/messages/21.png");
        break;
    case 3:
        strcpy(link, "images/messages/3.png");
        break;
    case 5:
        strcpy(link, "images/messages/5.png");
        break;
    default:
        break;
    }
    GtkWidget *notificationImage;
    notificationImage = gtk_image_new_from_file(link);

    if (user == PLAYER)
    {
        gtk_box_pack_start(GTK_BOX(playerNotificationBox), notificationImage, TRUE, FALSE, 0);
        gtk_widget_show(notificationImage);
        sleep(2);
        gtk_container_remove(GTK_CONTAINER(playerNotificationBox), GTK_WIDGET(notificationImage));
    }
    else
    {
        gtk_box_pack_start(GTK_BOX(enemyNotificationBox), notificationImage, TRUE, FALSE, 0);
        gtk_widget_show(notificationImage);
        sleep(2);
        gtk_container_remove(GTK_CONTAINER(enemyNotificationBox), GTK_WIDGET(notificationImage));
    }
    return NULL;
}

void notificationThread(int mode, int user)
{
    notiCode = mode * 10 + user;
    pthread_create(&tid, NULL, DisplayNotification, (void *)&notiCode);
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
        gtk_widget_destroy(GTK_WIDGET(i->data));

    // free list
    g_list_free(children);
}

//***********
void build_rankWindow()
{

    PangoAttrList *attrlist = pango_attr_list_new();
    PangoFontDescription *font_desc = pango_font_description_new();
    pango_font_description_set_size(font_desc, 14 * PANGO_SCALE);
    pango_font_description_set_weight(font_desc, PANGO_WEIGHT_BOLD);
    PangoAttribute *attr = pango_attr_font_desc_new(font_desc);
    pango_attr_list_insert(attrlist, attr);

    FILE *f;
    f = fopen("rank.txt", "r");

    for (int i = 0; !feof(f); i++)
    {
        int row = i + 1;
        gtk_grid_insert_row(GTK_GRID(rankGrid), row);
        GtkWidget *rank = gtk_label_new("");
        GtkWidget *username = gtk_label_new("");
        GtkWidget *winNumber = gtk_label_new("");
        GtkWidget *allNumber = gtk_label_new("");
        GtkWidget *score = gtk_label_new("");

        if (i == 0)
        {
            gtk_label_set_text(GTK_LABEL(rank), "Xếp hạng");
            gtk_label_set_text(GTK_LABEL(username), "Tên");
            gtk_label_set_text(GTK_LABEL(score), "Điểm");
            gtk_label_set_text(GTK_LABEL(winNumber), "Số trận thắng");
            gtk_label_set_text(GTK_LABEL(allNumber), "Tổng số trận");
        }
        else
        {
            char rankcount[10], name[30], scr[10], number_win[10], number[10];
            fscanf(f, "%s %s %s %s %s\n", rankcount, name, scr, number_win, number);

            gtk_label_set_text(GTK_LABEL(rank), rankcount);
            gtk_label_set_text(GTK_LABEL(username), name);
            gtk_label_set_text(GTK_LABEL(score), scr);
            gtk_label_set_text(GTK_LABEL(winNumber), number_win);
            gtk_label_set_text(GTK_LABEL(allNumber), number);
            if (strcmp(name, usernameLogin) == 0)
            {
                gtk_label_set_attributes(GTK_LABEL(rank), attrlist);
                gtk_label_set_attributes(GTK_LABEL(username), attrlist);
                gtk_label_set_attributes(GTK_LABEL(score), attrlist);
                gtk_label_set_attributes(GTK_LABEL(winNumber), attrlist);
                gtk_label_set_attributes(GTK_LABEL(allNumber), attrlist);
            }
        }
        gtk_grid_attach(GTK_GRID(rankGrid), rank, 0, row, 1, 1);
        gtk_grid_attach(GTK_GRID(rankGrid), username, 1, row, 1, 1);
        gtk_grid_attach(GTK_GRID(rankGrid), score, 2, row, 1, 1);
        gtk_grid_attach(GTK_GRID(rankGrid), winNumber, 3, row, 1, 1);
        gtk_grid_attach(GTK_GRID(rankGrid), allNumber, 4, row, 1, 1);
    }
    fclose(f);
}

void on_rankWindow_destroy()
{
    gtk_widget_show_all(mainMenuWindow);
    clear_container(rankGrid);
    gtk_widget_hide(rankWindow);
}