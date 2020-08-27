#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <windows.h>

#include "kartu41.h"
#include "../extras/extra.h"

// A port of kisanak kartu41 in Python
// to C++ lang

void print_x_times(char ch, int x)
{
    for (int i = 0; i < x; i++)
    {
        printf("%c", ch);
    }
}

int determine_max_and_combine(int dataset[4])
{
    int n_pos = 0;
    int largest = dataset[0];
    int total_final = 0;
    for (int i = 0; i < 4; i++)
    {
        if (dataset[i] == -1)
        {
            dataset[i] == 0;
        }
        if (dataset[i] > largest)
        {
            largest = dataset[i];
            n_pos = i;
        }
    }
    for (int i = 0; i < 4; i++)
    {
        if (i == n_pos)
        {
            total_final += dataset[i];
        }
        else
        {
            total_final -= dataset[i];
        }
    }
    return total_final;
}

class Kartu41
{
public:
    bool gameover;
    int winner;    
    int playerDeck[5], botDeck[5];

    Kartu41(void)
    {
        srand(time(NULL)); // seed RNGJesus
        this->gameover = false;
        this->winner = -1;

        for (int i = 0; i < 52; i++)
        {
            this->mainDeck[i] = 1;
            this->stackDeck[i] = -1;
        }

        for (int i = 0; i < 5; i++)
        {
            this->botDeck[i] = -1;
            this->playerDeck[i] = -1;
        }

        this->player_score = 0;
        this->bot_score = 0;

        this->latestCard = -1;
    }
    int draw_card()
    {
        bool succeed = false;
        int card;
        while (!succeed)
        {
            card = rng_card();
            if (this->mainDeck[card] == 1)
            {
                this->mainDeck[card] = 0;
                succeed = true;
            }
        }
        return card;
    }
    int start_game()
    {
#ifndef DEBUGGAME
        clearScreen();
        setCursorPosition(0, 0);
#endif
        printf("Membagikan kartu...\n");
        for (int i = 0; i < 4; i++)
        {
            this->playerDeck[i] = draw_card();
            this->botDeck[i] = draw_card();
        }
        // int firstStack = draw_card(2);
        // stack_controller(0, firstStack);
        printf("Memeriksa kartu awal...\n");
        check_card_stack();
        printf("Memulai game...\n");
        while (!gameover)
        {
#ifndef DEBUGGAME
            clearScreen();
            setCursorPosition(0, 0);
#endif
            player_turn();
            check_card_stack();
            if (gameover)
            {
                break;
            }
            for (int i = 0; i < 3; i++)
            {
                printf(".");
                Sleep(250);
            }
#ifndef DEBUGGAME
            clearScreen();
            setCursorPosition(0, 0);
#endif
            bot_turn();
            check_card_stack();
            if (gameover)
            {
                break;
            }
            for (int i = 0; i < 3; i++)
            {
                printf(".");
                Sleep(250);
            }
        }
        if (winner == 0)
        {
            printf("\nPemenangnya adalah: Player!\n");
            printf("Total nilai kartu pemain: %d\n", this->player_score);
            printf("Total nilai kartu bot: %d\n", this->bot_score);
        }
        else if (winner == 1)
        {
            printf("\nPemenangnya adalah: Bot!\n");
            printf("Total nilai kartu bot: %d\n", this->bot_score);
            printf("Total nilai kartu pemain: %d\n", this->player_score);
        }
        else if (winner == 2)
        {
            printf("\nPemenangnya adalah: Seri!\n");
            printf("Total nilai kartu pemain dan bot: %d dan %d\n", this->player_score, this->bot_score);
        }
        return 0;
    }
    void player_turn()
    {
        print_stack();
        printf("\n");
        print_user_card();
        if (this->latestCard != -1)
        {
            int lock = 1;
            while (lock)
            {
                char ds_in;
                printf("Ambil dari (s)tack apa (d)eck? (s/d) -> ");
                scanf(" %c", &ds_in);
                to_lower(&ds_in);
                switch (ds_in)
                {
                case 's':
                    card_controller(0, 0, this->latestCard);
                    stack_controller(1);
                    lock = 0;
                    break;
                case 'd':
                    card_controller(0, 0, draw_card());
                    lock = 0;
                    break;
                default:
                    printf("\nHah gimana?\n");
                    break;
                }
            }
        }
        else
        {
            printf("\nMengambil dari deck...\n");
            card_controller(0, 0, draw_card());
        }
        stack_card_to_left(0);
        print_user_card();
        int lock2 = 1;
        while (lock2)
        {
            int card_trash;
            printf("Keluarin kartu ke-");
            scanf("%d", &card_trash);
            if (card_trash >= 1 && card_trash <= 5)
            {
                card_controller(0, 1, card_trash - 1);
                lock2 = 0;
            }
            else
            {
                printf("Yang bener boss... 1-5!\n");
            }
        }
        stack_card_to_left(0);
    }
    void bot_turn()
    {
        printf("Giliran bot!\n");
        int card = draw_card();
        card_controller(1, 0, card);
        int throw_card = (rand() % (5 - 1 + 1)) + 1;
        throw_card--;
        int throwen_card = this->botDeck[throw_card];
        card_controller(1, 1, throw_card);
        stack_controller(0, throwen_card);
        stack_card_to_left(1);
    }
    void print_stack()
    {
        printf("\nStack:\n");
        if (this->latestCard != -1)
        {
            print_x_times('=', 11);
            printf("\n");
            printf("|	  |\n");
            printf("|	  |\n");
            printf("    %s\n", this->suites.fullDecks[this->latestCard]);
            printf("|	  |\n");
            printf("|	  |\n");
            print_x_times('=', 11);
            printf("\n");
        }
        else
        {
            printf("Tidak ada stack yang bisa diambil.\n");
        }
    }
    void print_user_card()
    {
        int amount_to_print = 0;
        for (int i = 0; i < 5; i++)
        {
            if (this->playerDeck[i] != -1)
            {
                amount_to_print++;
            }
        }
        printf("Kartu Tangan:\n");
        for (int i = 0; i < amount_to_print; i++)
        {
            print_x_times('=', 11);
            printf(" ");
        }
        printf("\n");
        if (amount_to_print == 4)
        {
            for (int i = 0; i < 2; i++)
            {
                printf("|	  | ");
                printf("|	      | ");
            }
            printf("\n");
            for (int i = 0; i < 2; i++)
            {
                printf("|	  | ");
                printf("|	      | ");
            }
        }
        else
        {
            for (int i = 0; i < 2; i++)
            {
                printf("|	  | ");
                printf("|	      | ");
            }
            printf("|	  | ");
            printf("\n");
            for (int i = 0; i < 2; i++)
            {
                printf("|	  | ");
                printf("|	      | ");
            }
            printf("|	  | ");
        }
        printf("\n");
        printf("    %s  ", this->suites.fullDecks[this->playerDeck[0]]);
        for (int i = 1; i < amount_to_print; i++)
        {
            printf("        %s  ", this->suites.fullDecks[this->playerDeck[i]]);
        }
        printf("\n");
        if (amount_to_print == 4)
        {
            for (int i = 0; i < 2; i++)
            {
                printf("|	  | ");
                printf("|	      | ");
            }
            printf("\n");
            for (int i = 0; i < 2; i++)
            {
                printf("|	  | ");
                printf("|	      | ");
            }
        }
        else
        {
            for (int i = 0; i < 2; i++)
            {
                printf("|	  | ");
                printf("|	      | ");
            }
            printf("|	  | ");
            printf("\n");
            for (int i = 0; i < 2; i++)
            {
                printf("|	  | ");
                printf("|	      | ");
            }
            printf("|	  | ");
        }
        printf("\n");
        for (int i = 0; i < amount_to_print; i++)
        {
            print_x_times('=', 11);
            printf(" ");
        }
        printf("\n");
        for (int i = 0; i < amount_to_print; i++)
        {
            printf("    (%d)     ", i + 1);
        }
        printf("\n");
    }
    void check_card_stack()
    {
        int playerSuitesSet[4] = {0, 0, 0, 0};
        for (int i = 0; i < 5; i++)
        {
            char fletter = this->suites.pcDecks[this->playerDeck[i]][0];
            char suites = this->suites.pcDecks[this->playerDeck[i]][1];
            switch (fletter)
            {
            case 'J':
                playerSuitesSet[(int)suites - 42] += 10;
                break;
            case 'Q':
                playerSuitesSet[(int)suites - 42] += 10;
                break;
            case 'K':
                playerSuitesSet[(int)suites - 42] += 10;
                break;
            case 'A':
                playerSuitesSet[(int)suites - 42] += 11;
                break;
            default:
                playerSuitesSet[(int)suites - 42] += ((int)fletter - 48);
                break;
            }
        }
#ifdef DEBUGGAME
        printf("P: %d %d %d %d\n", playerSuitesSet[0], playerSuitesSet[1], playerSuitesSet[2], playerSuitesSet[3]);
#endif
        this->player_score = determine_max_and_combine(playerSuitesSet);
        if (this->player_score >= 41)
        {
            this->winner = 0;
            this->gameover = true;
            return;
        }
        int botSuitesSet[4] = {0, 0, 0, 0};
        for (int i = 0; i < 5; i++)
        {
            char fletter = this->suites.pcDecks[this->botDeck[i]][0];
            char suites = this->suites.pcDecks[this->botDeck[i]][1];
            switch (fletter)
            {
            case 'J':
                botSuitesSet[(int)suites - 42] += 10;
                break;
            case 'Q':
                botSuitesSet[(int)suites - 42] += 10;
                break;
            case 'K':
                botSuitesSet[(int)suites - 42] += 10;
                break;
            case 'A':
                botSuitesSet[(int)suites - 42] += 11;
                break;
            default:
                botSuitesSet[(int)suites - 42] += ((int)fletter - 48);
                break;
            }
        }
#ifdef DEBUGGAME
        printf("B: %d %d %d %d\n", playerSuitesSet[0], playerSuitesSet[1], playerSuitesSet[2], playerSuitesSet[3]);
#endif
        this->bot_score = determine_max_and_combine(botSuitesSet);
        if (this->bot_score >= 41)
        {
            this->winner = 1;
            this->gameover = true;
            return;
        }
        int card_in_deck = 0;
        for (int i = 0; i < 52; i++)
        {
            if (this->mainDeck[i] == 1)
            {
                card_in_deck++;
            }
        }
        if (card_in_deck == 0)
        {
            this->gameover = true;
            // check who have the highest score.
            if (this->player_score == this->bot_score)
            {
                this->winner = 2;
            }
            else if (this->player_score > this->bot_score)
            {
                this->winner = 0;
            }
            else if (this->player_score < this->bot_score)
            {
                this->winner = 1;
            }
        }
    }
private:
    CardSuites suites;
    int mainDeck[52], stackDeck[52];
    int player_score, bot_score, latestCard;

    int rng_card()
    {
        // https://www.geeksforgeeks.org/generating-random-number-range-c/
        int num = (rand() % (52 - 1 + 1)) + 1;
        return num - 1;
    }
    void stack_controller(int mode, int card = 0)
    {
        switch (mode)
        {
        case 0:
            for (int i = 0; i < 52; i++) {
                if (this->stackDeck[i] == -1) {
                    this->stackDeck[i] = card;
                    this->latestCard = card;
                    break;
                }
            }
            break;
        case 1:
            // Get latest and remove.
            int card_n = -1;
            for (int i = 51; i >= 0; i--) {
                if (this->stackDeck[i] != -1)
                {
                    card_n = this->stackDeck[i];
                    this->stackDeck[i] = -1;
                    if (i - 1 < 0)
                    {
                        this->latestCard = -1;
                    }
                    else
                    {
                        this->latestCard = this->stackDeck[i - 1];
                    }
                    break;
                }
            }
            break;
        }
    }
    void card_controller(int player, int mode, int card = 0)
    {
        switch (mode)
        {
        case 0:
            for (int i = 0; i < 5; i++) {
                if (player == 0) {
                    if (this->playerDeck[i] == -1)
                    {
                        this->playerDeck[i] = card;
                    }
                }
                else if (player == 1)
                {
                    if (this->botDeck[i] == -1)
                    {
                        this->botDeck[i] = card;
                    }
                }
            }
            break;
        case 1:
            if (player == 0)
            {
                this->playerDeck[card] = -1;
                // printf("%d %d %d %d %d\n", this->playerDeck[0], this->playerDeck[1], this->playerDeck[2], this->playerDeck[3], this->playerDeck[4]);
            }
            else if (player == 1)
            {
                this->playerDeck[card] = -1;
            }
            break;
        }
    }
    void stack_card_to_left(int player)
    {
        int new_data[5] = {-1};
        int card_cnt = 0;
        switch (player)
        {
        case 0:
#ifdef DEBUGGAME
            printf("%d %d %d %d %d\n", this->playerDeck[0], this->playerDeck[1], this->playerDeck[2], this->playerDeck[3], this->playerDeck[4]);
#endif
            for (int i = 0; i < 5; i++)
            {
                if (this->playerDeck[i] != -1)
                {
                    new_data[card_cnt] = this->playerDeck[i];
                    card_cnt++;
                }
            }
            for (int i = 0; i < 5; i++)
            {
                this->playerDeck[i] = new_data[i];
            }
#ifdef DEBUGGAME
            printf("%d %d %d %d %d\n", this->playerDeck[0], this->playerDeck[1], this->playerDeck[2], this->playerDeck[3], this->playerDeck[4]);
#endif
            break;
        case 1:
            for (int j = 0; j < 5; j++)
            {
                if (this->botDeck[j] != -1)
                {
                    new_data[card_cnt] = this->botDeck[j];
                    card_cnt++;
                }
            }
            for (int j = 0; j < 5; j++)
            {
                this->botDeck[j] = new_data[j];
            }
            break;
        }
    }
};

// N4O: Untested.
// Untuk tes, bisa rename ke main()
int kartu41_game()
{
    Kartu41 game;
    game.start_game();
    return 0;
}