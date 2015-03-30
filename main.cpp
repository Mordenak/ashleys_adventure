/*
 *
 * Adventures of Ashley the Badass:  Save the Unicorns
 * DESCRIPTION
 *      Text-based adventure game based in a realm where evil Canadian foods are pit against Unicorns
 *      in the ultimate struggle.
 *
 *
 * Nicholas Solwick
 * Date created: Feb 22 2011
 * Last date modified: Mar 21 2011
 *
 * SOURCES USED
 *      Special Thanks:
 *          Ashley Reiter - Ideas and Concepts!
 *          Lloyd Kincaid - Coding and narrative ideas
 *          Boyd Trolinger - Advanced coding techniques
 *
 *      cplusplus.com
 *      Canada
 *      http://www.network-science.de/ascii/  - ASCII banners
 *      http://www.ascii-art.de/ascii/uvw/unicorn.txt  - Unicorn ASCII art
 *
 */

#include <iostream>
#include <fstream>
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <ctime>
#include "CinReader.h"

CinReader reader;

using namespace std;

void clearScreen();
void welcomeMsg();
void gameOver();
void showCredits();
void showArt(string artfile);
void roomActions(int& roomID);

class Character
{
public:
    Character(string stringName, int intHealth, int intAttackMod, int intDefenseMod, int intLevel, int intSpeed, int intGold, int intExp, bool isAlive);
    void displayCharacter();
    void userPrompt();
    void playerAction(int& roomID, char& userInput);
    void combat(Character& enemy);
    void regen();
    void levelUp();
    void doShop();
    void openGate();
    void awardXp(int xpAward);
    void finalSequence();
private:
    string name;
    int health;
    int maxHealth;
    int attackMod;
    int defenseMod;
    int level;
    int speed;
    int gold;
    int exp;
    bool alive;
};

class Room
{
public:
    void setRoom(int roomID, int intNDir, int intEDir, int intSDir, int intWDir);
    void showRoom();
    int playerMove(char& userInput);
private:
    int ID;
    int nDir;
    int eDir;
    int sDir;
    int wDir;
};

const int NUM_ATTACKDICE = 2;
const int SIZE_ATTACKDICE = 6;

// I dislike using global variables, but I didn't really find an efficient/simple way around them in this case...
int enemyKills = 0;
int goldSpent = 0;
int goldAccumulated = 0;
int baconSlain = 0;
int milkSoldierSlain = 0;
int milkGuardSlain = 0;
bool milkSoldierLimitReached = false;
bool milkGuardLimitReached = false;
bool baconLimitReached = false;
bool snuckPastPatrol = false;
bool poutinePatrolSlain = false;
bool commanderSlain = false;
bool labattehSlain = false;
bool hasUnicornSword = false;
bool hasUnicornShield = false;
bool gateOpen = false;
bool exitGame = false;

int main()
{
    Character player("Ashley", 100, 6, 5, 1, 6, 5000, 0, true); // name, health, attackmod, defensemod, level, speed, gold, exp
    Room * room = new Room[8];
                  //id  n   e  s   w
    room[0].setRoom(0, -1, -1, 1, -1); // intro room w/ bacon
    room[1].setRoom(1, 0, -1, 2, -1); // 2nd room, poutine patrol problem
    room[2].setRoom(2, 1, 3, -1, 4); // crossroads
    room[3].setRoom(3, -1, 5, -1, 2); // gate/commander
    room[4].setRoom(4, -1, 2, -1, -1); // unicorn shop
    room[5].setRoom(5, -1, -1, 6, 3); // bagged milk 1
    room[6].setRoom(6, 5, 7, -1, -1); // bagged milk 2
    room[7].setRoom(7, -1, -1, -1, 6); // labatteh's lair

    Room* roomPtr = &room[0];  // Pointer to the active room

    welcomeMsg();

    srand(time(0));

    char userInput;
    bool shownRoom = false;
    int roomID = 0;
    int oldRoom = 0;
    while (exitGame == false)
    {
        if (shownRoom == false)
        {
            roomPtr->showRoom();
            roomActions(roomID);
        }
        player.userPrompt();
        userInput = reader.readChar("NnEeWwSsTtAaBb");
        userInput = tolower(userInput);
        if (userInput == 'n' || userInput == 'e' || userInput == 's' || userInput == 'w')
        {
            oldRoom = roomID;
            roomID = roomPtr->playerMove(userInput);
            if (roomID != -1 && roomID != -2)
            {
                shownRoom = false;
                roomPtr = &room[roomID];
                player.regen();
                clearScreen();
            }
            else if (roomID == -1)
            {
                roomID = oldRoom;
                cout << "\nYou cannot go that way.\n";
                shownRoom = true;
            }
            else if (roomID == -2)
            {
                roomID = oldRoom;
                if (baconLimitReached == false)
                {
                    cout << "\n   You must fight your way through your foes first!\n\n";
                }
                else if (snuckPastPatrol == false && poutinePatrolSlain == false)
                {
                    cout << "\n   You must sneak around or slay the Poutine Patrol!\n\n";
                }
                else if (gateOpen == false)
                {
                    cout << "\n   You must find a way to open the gate first!\n\n";
                }
                else if (commanderSlain == false)
                {
                    cout << "\n   You must defeat the Commander Ketchup flavored Chips!\n\n";
                }
                else if (roomID == 5 && milkSoldierLimitReached == false)
                {
                    cout << "\n   You must fight your way through your foes first!\n\n";
                }
                else if (roomID == 6 && milkGuardLimitReached == false)
                {
                    cout << "\n   You must fight your way through your foes first!\n\n";
                }
            }
        }
        else if (userInput == 't')
        {
            shownRoom = false;
            player.displayCharacter();
        }
        else if (userInput == 'b' && roomID == 1)
        {
            if (snuckPastPatrol != true && poutinePatrolSlain == false)
            {
                int chanceToSneak = rand() % 100 + 1;
                if (chanceToSneak < 50)
                {
                    snuckPastPatrol = true;
                    cout << "\n    You successfully snuck past the patrol!\n";
                    cout << "    You have earned the title 'the Beast'!\n";
                    player.awardXp(30);
                    cout << "    You have been awarded 30 exp for your feat!\n\n";
                }
                else
                {
                    cout << "\n   You failed to sneak past the patrol, defend yourself!\n\n";
                    userInput = 'a';
                    player.playerAction(roomID, userInput);
                }
            }
            else
            {
                cout << "That is no longer an option!\n\n";
            }
        }
        else if (userInput == 'b' && roomID == 4)
        {
            shownRoom = false;
            player.doShop();
        }
        else if (userInput == 'b' && roomID == 2)
        {
            if (gateOpen != true)
            {
                shownRoom = false;
                player.openGate();
            }
            else
            {
                cout << "That is no longer an option!\n\n";
            }
        }
        else
        {
            shownRoom = false;
            player.playerAction(roomID, userInput);
        }
    }
}

void roomActions(int& roomID)
{
    if (roomID == 0)
    {
        cout << "    (a) - Attack the Canadian Bacon!\n";
    }
    else if (roomID == 1)
    {
        if (poutinePatrolSlain == false)
        {
            cout << "    (a) - Attack the Poutine Patrol for Unicorn Justice!\n";
            if (snuckPastPatrol == false)
            {
                cout << "    (b) - Sneak past the Poutine Patrol, save your strength for later!\n";
            }
        }
    }
    else if (roomID == 2)
    {
        if (gateOpen == false)
        {
            cout << "    (b) - Inspect the gate!\n";
        }
    }
    else if (roomID == 3)
    {
        if (commanderSlain == false)
        {
            cout << "    (a) - Attack the Commander Ketchup flavored Chips!\n";
        }
    }
    else if (roomID == 4)
    {
        cout << "    (b) - Shop at the Unicorn Store!\n";
    }
    else if (roomID == 5)
    {
        cout << "    (a) - Attack the Bagged Milk Soldiers!\n";
    }
    else if (roomID == 6)
    {
        cout << "    (a) - Attack the Bagged Milk Guards!\n";
    }
    else if (roomID == 7)
    {
        if (labattehSlain == false)
        {
            cout << "    (a) - Challenge LaBatteh 'the Blue' to a duel!\n";
        }
    }
    cout << "\n";
}

void welcomeMsg()
{
    cout << "  ----------------------------------------------------------------------------\n";
    showArt("titleArt.txt");
    cout << "  ----------------------------------------------------------------------------\n";
    cout << "                       S A V E   T H E   U N I C O R N S                      \n";
    cout << "  ----------------------------------------------------------------------------\n";
    cout << "Press ENTER to continue";
    reader.readString();
    cout << "\n  Your name is Ashley the Badass and you are living the dream, as a car \n"
         << " saleswoman.  You've devoted your life to Unicorns.  Your bedroom is a myriad\n"
         << " shrine of Unicorn figurines, plushies, stickers and posters.  You've gone\n"
         << " through your life with the goal of saving Unicorns from evil fates.  But\n"
         << " you never expected tragedy to strike you!  When your longtime Unicorn sidekick\n"
         << " Mertle was kidnapped by the conniving canuck LaBatteh 'the Blue' you began\n"
         << " your all out quest of kickin' ass and chewing bubble yum, but you've recently\n"
         << " ran out of bubble yum!  Now you must fight your way to save your life partner,\n"
         << " Mertle from the merciless fate of LaBatteh's diabolic schemes!  But beware\n"
         << " he commands an army of Canadian Bacon and the Poutine Patrol, his secret\n"
         << " police!  You must face his commander, Ketchup flavored Chips if you hope to\n"
         << " reach him deep in his lair!\n";
    cout << "\nPress ENTER to continue";
    reader.readString();
    clearScreen();
}




// Room class member functions

void Room::setRoom(int roomID, int intNDir, int intEDir, int intSDir, int intWDir)
{
    ID = roomID;
    nDir = intNDir;
    eDir = intEDir;
    sDir = intSDir;
    wDir = intWDir;
}

int Room::playerMove(char& userInput)
{
    if (userInput == 'n')
    {
        if (nDir != -1)
        {
            return nDir;
        }
        else
        {
            return -1;
        }
    }
    else if (userInput == 'e')
    {
        if (eDir != -1)
        {
            if (ID == 2 && gateOpen == false)
            {
                return -2;
            }
            else if (ID == 3 && commanderSlain == false)
            {
                return -2;
            }
            else if (ID == 6 && milkGuardLimitReached == false)
            {
                return -2;
            }
            else
            {
                return eDir;
            }
        }
        else
        {
            return -1;
        }
    }
    else if (userInput == 's')
    {
        if (sDir != -1)
        {
            if (ID == 0 && baconLimitReached == false)
            {
                return -2;
            }
            else if (ID == 1)
            {
                if (snuckPastPatrol == false && poutinePatrolSlain == false)
                {
                    return -2;
                }
                else
                {
                    return sDir;
                }
            }
            else if (ID == 5 && milkSoldierLimitReached == false)
            {
                return -2;
            }
            else
            {
                return sDir;
            }
        }
        else
        {
            return -1;
        }
    }
    else if (userInput == 'w')
    {
        if (wDir != -1)
        {
            return wDir;
        }
        else
        {
            return -1;
        }
    }
    return -1;
}

void Room::showRoom()
{
    if (ID == 0)
    {
        if (baconLimitReached == false)
        {
            cout << " A Meadow in the Unicorn Forest\n\n";
            cout << "  You are standing in a large open meadow deep in the Unicorn Forest.\n"
                 << "  You can hear small animals rustling in the undergrowth, which are  \n"
                 << "  signs of something coming.  Suddenly you hear faraway noises of    \n"
                 << "  clanking armor and weapons, you know LaBatteh has now sent his army\n"
                 << "  after you and you prepare yourself.  Soon the entire meadow is     \n"
                 << "  surrounded by an army of Canadian Bacon.  You must fight your way  \n"
                 << "  south to make it to LaBatteh's evil lair.\n";
            cout << "\n\n";
        }
        else
        {
            cout << " A Meadow in the Unicorn Forest\n\n";
            cout << "  You are standing in a large open meadow deep in the Unicorn Forest.\n"
                 << "  You can hear small animals rustling in the undergrowth, which are  \n"
                 << "  signs of something coming.  Suddenly you hear faraway noises of    \n"
                 << "  clanking armor and weapons, you know LaBatteh has now sent his army\n"
                 << "  after you and you prepare yourself.  There are still plenty of     \n"
                 << "  Bacon foes for you to hone your skills on, but you've cut a swathe \n"
                 << "  through them and may proceed south.\n";
            cout << "\n\n";
        }
    }
    else if (ID == 1)
    {
        if (poutinePatrolSlain == false)
        {
            cout << " Near a Road in the Unicorn Forest\n\n";
            cout << "  Stealthily you've managed to avoid detection from the searching    \n"
                 << "  army, by avoiding the roads and using the forest to your advantage.\n"
                 << "  Just before you were about to get on the road ahead, you stop and  \n"
                 << "  look down the road and you see a terrible sight. The Poutine Patrol\n"
                 << "  is coming up the road, scouring the area for you.  You quickly hide\n"
                 << "  in a safe spot while decide whether you should sneak past them, or \n"
                 << "  outright slay them in the name of the Unicorns.\n";
            cout << "\n\n";
        }
        else if (poutinePatrolSlain == true)
        {
            cout << " Near a Road in the Unicorn Forest\n\n";
            cout << "  Stealthily you've managed to avoid detection from the searching    \n"
                 << "  army, by avoiding the roads and using the forest to your advantage.\n"
                 << "  Just before you were about to get on the road ahead, you stop and  \n"
                 << "  look down the road and you see a terrible sight. This is where you \n"
                 << "  you slayed the Poutine Patrol in the name of Unicorn justice!\n";
            cout << "\n\n";
        }
    }
    else if (ID == 2)
    {
        if (gateOpen == false)
        {
            cout << " The Crossroads\n\n";
            cout << "  You are at the crossroads between LaBatteh's lair to the east and  \n"
                 << "  the Unicorn Store to the west.  But blocking your way east is the  \n"
                 << "  large gate that guards the entrance to LaBatteh's lair.  You must  \n"
                 << "  find a way to open the gate if you intend to go east.  Perhaps you \n"
                 << "  may find something of use at the Unicorn Store to aid you in this  \n"
                 << "  endeavor.\n";
            cout << "\n\n";
        }
        else
        {
            cout << " The Crossroads\n\n";
            cout << "  You are at the crossroads between LaBatteh's lair to the east and  \n"
                 << "  the Unicorn Store to the west.  Through the use of your amazing    \n"
                 << "  skills you've managed to open the main gate.  Perhaps you should   \n"
                 << "  stop by the store and restock your supplies before you venture into\n"
                 << "  LaBatteh's lair.\n";
            cout << "\n\n";
        }
    }
    else if (ID == 3)
    {
        if (commanderSlain == false)
        {
            cout << " Inside the Outer Gate\n\n";
            cout << "  You've made it inside the Outer Gate, now nothing will stop you! Or\n"
                 << "  so you thought.  Just as you pass through the gate, the feared     \n"
                 << "  commander Ketchup flavored Chips steps through the door from the   \n"
                 << "  east.  He eyes you up and grins, barring the door into the lair,   \n"
                 << "  you must defeat him in order to gain access to the lair and save  \n"
                 << "  Mertle!  This will not be an easy obstacle!\n";
            cout << "\n\n";
            cout << "Commander Ketchup flavored Chips says, 'So, you think you got what it takes?'\n\n";
            cout << "Commander Ketchup flavored Chips snorts and chuckles at you.\n\n";
            cout << "Commander Ketchup flavored Chips says, 'Bring it little girl.'\n\n";
        }
        else
        {
            cout << " Inside the Outer Gate\n\n";
            cout << "  You've made it inside the Outer Gate, now nothing will stop you!   \n"
                 << "  You've defeated the commander Ketchup flavored Chips, however just \n"
                 << "  as you get to the door to the lair, you can hear an army amassing  \n"
                 << "  amassing on the other side, you know a fight is to come!  You bust \n"
                 << "  down the door and charge in for the Unicorns and for Mertle!\n";
            cout << "\n\n";
        }
    }
    else if (ID == 4)
    {
        if (hasUnicornSword == true || hasUnicornShield == true)
        {
            cout << " The Unicorn Store!\n\n";
            cout << "  You are standing in the Unicorn Store, the shopkeeper, Miss Montana\n"
                 << "  Prairies stands on the other side of the counter ready to assist   \n"
                 << "  you with whatever you may need.  The shop offers excellent healing \n"
                 << "  and upgrades for you.";
            cout << "\n\n";
        }
        else
        {
            cout << " The Unicorn Store!\n\n";
            cout << "  You are standing in the Unicorn Store, the shopkeeper, Miss Montana\n"
                 << "  Prairies stands on the other side of the counter ready to assist   \n"
                 << "  you with whatever you may need.  The shop offers excellent healing \n"
                 << "  and upgrades for you, however two things on the wall stand out. You\n"
                 << "  recognize them to be the legendary Unicorn Sword and Unicorn Shield\n"
                 << "  which have tremendous power.  However, they will definitely cost a \n"
                 << "  pretty penny.\n";
            cout << "\n\n";
        }
        cout << " Miss Montana Prairies, says 'Welcome, please have a look around.'\n\n";
    }
    else if (ID == 5)
    {
        if (milkSoldierLimitReached == false)
        {
            cout << " The Courtyard\n\n";
            cout << "  You've made it through to the courtyard, and you know see what you \n"
                 << "  heard earlier before you entered the lair.  An army of Bagged Milk \n"
                 << "  Soldiers stand before you blocking your way south deeper into the  \n"
                 << "  lair.  You must cut through your enemies to gain access to the     \n"
                 << "  Inner Foyer.\n";
            cout << "\n\n";
        }
        else
        {
            cout << " The Courtyard\n\n";
            cout << "  You've made it through to the courtyard, a beautiful fountain lies \n"
                 << "  in the middle of the room, with corpses of Bagged Milk Soldiers all\n"
                 << "  around it.  There are still Soldiers here trying to drag off their \n"
                 << "  wounded comrades.  Now would be a good time to strike them again,  \n"
                 << "  you may need the extra training in order to defeat LaBatteh.\n";
            cout << "\n\n";
        }
    }
    else if (ID == 6)
    {
        if (milkGuardLimitReached == false)
        {
            cout << " The Inner Foyer\n\n";
            cout << "  You've reached the Inner Foyer at last, you are almost there!  Yet \n"
                 << "  another army of Bagged Milk Guards stands before you, the last line\n"
                 << "  of defense that LaBatteh has.  Once more, you must cut through your\n"
                 << "  enemies to make it into the Inner Sanctum and rescue Mertle!\n";
            cout << "\n\n";
        }
        else
        {
            cout << " The Inner Foyer\n\n";
            cout << "  You've reached the Inner Foyer at last, you are almost there!  You \n"
                 << "  cleared a path through the Bagged Milk Guards, leading into the    \n"
                 << "  Inner Sanctum.  You may press on east, however you decide it may be\n"
                 << "  wise to slay a few more guards for gold and experience and maybe   \n"
                 << "  even go back and visit the Unicorn Store one more time before the  \n"
                 << "  final fight with LaBatteh 'the Blue'.\n";
            cout << "\n\n";
        }
    }
    else if (ID == 7)
    {
        cout << " The Inner Sanctum of LaBatteh's Lair\n\n";
        cout << "  You are in the Inner Sanctum of LaBatteh's lair.  The hair stands  \n"
             << "  up on the back of your neck just knowing you are here. You've heard\n"
             << "  heard stories of the awful things LaBatteh has done to Unicorns    \n"
             << "  here and you shudder at the thought.  In the corner you can see    \n"
             << "  Mertle chained down and bound by LaBatteh's evil powers.  You can  \n"
             << "  feel rage rise inside you.  This will not stand!\n";
        cout << "\n\n";
        cout << "LaBatteh 'the Blue' says, 'So, you've finally made it I see...'\n\n";
        cout << "LaBatteh 'the Blue' says, 'Perhaps I underestimated you.  But it matters not.'\n\n";
        cout << "LaBatteh 'the Blue' says, 'You will not free your companion. You will fall here.'\n\n";
    }
    else
    {
        cout << " << Insert Title >>\n\n";
        cout << "  << Insert Description >> \n\n";
    }
    cout << "From here you may:\n";
    if (nDir != -1)
    {
        cout << "    (n) - Travel north\n";
    }
    if (eDir != -1)
    {
        if (ID == 2 && gateOpen == false)
        {
            cout << "    (e) - The gate east is shut!\n";
        }
        else if (ID == 3 && commanderSlain == false)
        {
            cout << "    (e) - East barred by Commander Ketchup flavored Chips.\n";
        }
        else if (ID == 6 && milkGuardLimitReached == false)
        {
            cout << "    (e) - East barred, slay " << 5 - milkGuardSlain << " more milk guards!\n";
        }
        else
        {
            cout << "    (e) - Travel east\n";
        }
    }
    if (sDir != -1)
    {
        if (ID == 0 && baconLimitReached == false)
        {
            cout << "    (s) - South barred, slay " << 5 - baconSlain << " more bacon!\n";
        }
        else if (ID == 1)
        {
            if (snuckPastPatrol == false && poutinePatrolSlain == false)
            {
                cout << "    (s) - South barred, sneak around or kill them!\n";
            }
            else
            {
                cout << "    (s) - Travel south\n";
            }
        }
        else if (ID == 5 && milkSoldierLimitReached == false)
        {
            cout << "    (s) - South barred, slay " << 5 - milkSoldierSlain << " more milk soldiers!\n";
        }
        else
        {
            cout << "    (s) - Travel south\n";
        }
    }
    if (wDir != -1)
    {
        cout << "    (w) - Travel west\n";
    }
    cout << "    (t) - Display status\n";
}

// Character class member functions

// Character class default constructor
Character::Character(string stringName, int intHealth, int intAttackMod, int intDefenseMod, int intLevel, int intSpeed, int intGold, int intExp, bool isAlive)
{
    name = stringName;
    health = intHealth;
    maxHealth = intHealth;
    attackMod = intAttackMod;
    defenseMod = intDefenseMod;
    level = intLevel;
    speed = intSpeed;
    gold = intGold;
    exp = intExp;
    alive = isAlive;
}

void Character::playerAction(int& roomID, char& userInput)
{
    Character bacon("Canadian Bacon", 20, 3, 2, 2, 4, 3, 20, true); // name, health, attackmod, defensemod, level, speed, gold, exp
    Character poutine("Poutine Patrol", 50, 6, 2, 2, 7, 10, 100, true); // name, health, attackmod, defensemod, level, speed, gold, exp
    Character soldier("Bagged Milk Soldier", 40, 5, 2, 3, 6, 4, 50, true); // name, health, attackmod, defensemod, level, speed, gold, exp
    Character guard("Bagged Milk Guard", 40, 4, 3, 3, 6, 4, 50, true); // name, health, attackmod, defensemod, level, speed, gold, exp
    Character ketchup("Ketchup flavored Chips", 100, 13, 8, 8, 10, 20, 300, true); // name, health, attackmod, defensemod, level, speed, gold, exp
    Character labatteh("LaBatteh", 150, 16, 12, 10, 12, 35, 1000, true); // name, health, attackmod, defensemod, level, speed, gold, exp
    if (roomID == 0 && userInput == 'a')
    {
        combat(bacon);
    }

    if (roomID == 1 && userInput == 'a' && poutinePatrolSlain == false)
    {
        combat(poutine);
    }
    else if (roomID == 1 && userInput == 'a' && poutinePatrolSlain == true)
    {
        cout << "That is no longer an option!\n\n";
    }

    if (roomID == 3 && userInput == 'a' && commanderSlain == false)
    {
        combat(ketchup);
    }
    else if (roomID == 3 && userInput == 'a' && commanderSlain == true)
    {
        cout << "That is no longer an option!\n\n";
    }

    if (roomID == 5 && userInput == 'a')
    {
        combat(soldier);
    }

    if (roomID == 6 && userInput == 'a')
    {
        combat(guard);
    }

    if (roomID == 7 && userInput == 'a')
    {
        cout << "You say, 'You've harmed your last Unicorn you evil hoppy bastard!\n\n";
        cout << "You charge into battle for the Unicorns!\n\n";
        combat(labatteh);
    }
}

void Character::regen()
{
    if (health < maxHealth)
    {
        health++;
    }
}

void Character::openGate()
{
    cout << "You are standing at the front gate to LaBatteh's lair.\n\n";
    bool exitPuzzle = false;
    while (exitPuzzle == false)
    {
        srand(time(0));
        cout << "\nYou must find a way through the gate!\n";
        cout << "    (a) - Attempt to lift the gate!\n";
        cout << "    (b) - Attempt to break the gate down!\n";
        cout << "    (c) - Inspect the gate for exploits.\n";
        cout << "    (x) - Leave the gate.\n";
        userPrompt();
        char playerChoice = reader.readChar();
        playerChoice = tolower(playerChoice);
        if (playerChoice == 'a')
        {
            if (health > 5)
            {
                cout << "You attempt to lift the gate with your bare hands!\n";
                int chanceToSucceed = rand() % 100 + 1;
                chanceToSucceed += attackMod;
                if (hasUnicornSword == true)
                {
                    chanceToSucceed += 10;
                }
                if (chanceToSucceed > 70)
                {
                    cout << "\n   You successfully lifted the gate with your hands!  Unicorns rejoice!\n";
                    cout << "   You have earned the title 'the Badass'\n";
                    awardXp(50);
                    cout << "    You have been awarded 50 exp for your feat!\n\n";
                    gateOpen = true;
                    exitPuzzle = true;
                }
                else
                {
                    cout << "\n   You pry the gate up slowly but your arms give out and the gate crashes down!\n";
                    cout << "   You lost 5 health!\n";
                    health -= 5;
                }
            }
            else
            {
                cout << "You do not have the strength to attempt that at this moment!\n";
            }

        }
        else if (playerChoice == 'b')
        {
            if (health > 5)
            {
                cout << "Through the power of the Unicorn you attempt to break the gate down!\n";
                int chanceToSucceed = rand() % 100 + 1;
                chanceToSucceed += speed;
                if (hasUnicornSword == true)
                {
                    chanceToSucceed += 25;
                }
                if (chanceToSucceed > 80)
                {
                    if (hasUnicornSword == true)
                    {
                        cout << "\n   You slash down the gate with ease with your Unicorn Sword!\n";
                        cout << "   You have earned the title 'the Badass'\n\n";
                        awardXp(50);
                        cout << "    You have been awarded 50 exp for your feat!\n\n";
                    }
                    else
                    {
                        cout << "\n   After a brutal beating the gate finally comes down with a crash!\n";
                        cout << "   You have earned the title 'the Badass'\n\n";
                        awardXp(50);
                        cout << "    You have been awarded 50 exp for your feat!\n\n";
                    }
                    gateOpen = true;
                    exitPuzzle = true;
                }
                else
                {
                    if (hasUnicornSword == true)
                    {
                        cout << "\n   You slash recklessly at the gate but make no progress!\n";
                    cout << "   You lost 5 health!\n";
                    }
                    else
                    {
                        cout << "\n   You futily attempt to break the gate with your hands!\n";
                    cout << "   You lost 5 health!\n";
                    }
                    health -= 5;
                }
            }
            else
            {
                cout << "You do not have the strength to attempt that at this moment!\n";
            }
        }
        else if (playerChoice == 'c')
        {
            cout << "You look around the gate, searching for anything that might be a weakness.\n";
            int chanceToSucceed = rand() % 100 + 1;
            chanceToSucceed += speed;
            if (chanceToSucceed > 70)
            {
                bool exitKeypad = false;
                cout << "\n    You successfully spotted a keypad on the other side of the gate.\n";
                cout << "    You are able to reach your hand through the gate to the keypad.\n";
                cout << "    There is a post-it note next to it that reads, 'Don't forget the\n";
                cout << "    secret passcode, the first 2 numbers add up to the 3rd and all are\n";
                cout << "    divisible by 3 else you'll get shocked!'.\n";
                cout << "\n    It has 9 numbers 1-9 on it, enter 3 at time, what is the key code?\n";
                cout << "    enter 0 to exit.\n";
                while (exitKeypad == false)
                {
                    if (health > 2)
                    {
                        userPrompt();
                        int passCode = reader.readInt(0-999);
                        if (passCode == 369)
                        {
                            cout << "\n    You successfully entered the correct passcode and the gate opened!\n";
                            cout << "    You have earned the title 'the Badass'\n\n";
                            awardXp(80);
                            cout << "    You have been awarded 80 exp for your feat!\n\n";
                            gateOpen = true;
                            exitKeypad = true;
                            exitPuzzle = true;
                        }
                        else if (passCode == 0)
                        {
                            cout << "\n    You stop fiddling with the keypad.\n";
                            exitKeypad = true;
                        }
                        else
                        {
                            cout << "\n    Incorrect!  The keypad beeps and release a charge that shocks you!\n";
                            cout << "    You lose 2 health!\n";
                            health -= 2;
                        }
                    }
                    else
                    {
                        cout << "\n    You do not have enough strength to survive the shock!\n";
                        cout << "\n    Leaving the keypad.\n";
                        exitKeypad = true;
                    }
                }
            }
            else
            {
                cout << "\n    You inspect the gate and the area around it but find nothing!\n";
                cout << "    Perhaps you should keep searching?\n";
            }
        }
        else if (playerChoice == 'x')
        {
            cout << "You decide to save your energy for now and come back to the gate later.\n";
            exitPuzzle = true;
        }
    }
}

void Character::doShop()
{
    bool userShopping = true;

    cout << "You begin shopping at the Unicorn Store...\n\n";
    while (userShopping == true)
    {

        cout << " A list of items available: \n";
        cout << " [1] 20 point healing - 5g\n";
        cout << " [2] 100 point healing - 20g\n";
        cout << " [3] +1 permanent damage - 50g\n";
        cout << " [4] +1 permanent defense - 50g\n";
        cout << " [5] +10 permanent health - 100g\n";

        if (hasUnicornSword == false)
        {
            cout << " [6] the Unicorn Sword +20 damage - 250g\n";
        }

        if (hasUnicornShield == false)
        {
            cout << " [7] the Unicorn Shield 25% block - 250g\n";
        }

        cout << " [0] to exit.\n";
        cout << "\n\nYou have " << gold << " gold.\n";
        cout << "Please make a choice: ";
        userPrompt();
        int userChoice = reader.readInt();
        if (userChoice == 1)
        {
            if (gold < 5)
            {
                cout << "\n  Miss Montana Prairies, says 'You do not have enough gold for that.'\n\n";
            }
            else
            {
                if (health == maxHealth)
                {
                    cout << "\n  Miss Montana Prairies, says 'You are already at max health!'\n\n";
                }
                else
                {
                    if (maxHealth - health < 20)
                    {
                        health = maxHealth;
                    }
                    else
                    {
                        health += 20;
                    }
                    gold -= 5;
                    goldSpent += 5;
                    cout << "\n  Miss Montana Prairies, says 'You purchased a 20 point healing!'\n\n";
                }
            }
        }
        else if (userChoice == 2)
        {
            if (gold < 20)
            {
                cout << "\n  Miss Montana Prairies, says 'You do not have enough gold for that.'\n\n";
            }
            else
            {
                if (health == maxHealth)
                {
                    cout << "\n  Miss Montana Prairies, says 'You are already at max health!'\n\n";
                }
                else
                {
                    if (maxHealth - health < 100)
                    {
                        health = maxHealth;
                    }
                    else
                    {
                        health += 100;
                    }
                    gold -= 20;
                    goldSpent += 20;
                    cout << "\n  Miss Montana Prairies, says 'You purchased a 100 point healing!'\n\n";
                }
            }
        }
        else if (userChoice == 3)
        {
            if (gold < 50)
            {
                cout << "\n  Miss Montana Prairies, says 'You do not have enough gold for that.'\n\n";
            }
            else
            {
                attackMod += 1;
                gold -= 50;
                goldSpent += 50;
                cout << "\n  Miss Montana Prairies, says 'You purchased a +1 permanent damage bonus!'\n\n";
            }
        }
        else if (userChoice == 4)
        {
            if (gold < 50)
            {
                cout << "\n  Miss Montana Prairies, says 'You do not have enough gold for that.'\n\n";
            }
            else
            {
                defenseMod += 1;
                gold -= 50;
                goldSpent += 50;
                cout << "\n  Miss Montana Prairies, says 'You purchased a +1 permanent defense bonus!'\n\n";
            }
        }
        else if (userChoice == 5)
        {
            if (gold < 100)
            {
                cout << "\n  Miss Montana Prairies, says 'You do not have enough gold for that.'\n\n";
            }
            else
            {
                health += 10;
                maxHealth += 10;
                gold -= 100;
                goldSpent += 100;
                cout << "\n  Miss Montana Prairies, says 'You purchased a +10 permanent health bonus!'\n\n";
            }
        }
        else if (userChoice == 6 && hasUnicornSword != true)
        {
            if (gold < 250)
            {
                cout << "\n  Miss Montana Prairies, says 'You do not have enough gold for that.'\n\n";
            }
            else
            {
                gold -= 250;
                goldSpent += 250;
                hasUnicornSword = true;
                cout << "\n  Miss Montana Prairies, says 'You purchased the Unicorn Sword!'\n\n";
            }
        }
        else if (userChoice == 7 && hasUnicornShield != true)
        {
            if (gold < 250)
            {
                cout << "\n  Miss Montana Prairies, says 'You do not have enough gold for that.'\n\n";
            }
            else
            {
                gold -= 250;
                goldSpent += 250;
                hasUnicornShield = true;
                cout << "\n  Miss Montana Prairies, says 'You purchased the Unicorn Shield!'\n\n";
            }
        }
        else if (userChoice == 0)
        {
            cout << "Miss Montana Prairies, says 'Thank you for shopping at the Unicorn Store!'\n\n";
            userShopping = false;
        }
        else
        {
            cout << "\n  Miss Montana Prairies, says 'I'm sorry, what was that?'\n\n";
        }
    }
}

void Character::displayCharacter()
{
    string title = " the Car Saleswoman";
    if (poutinePatrolSlain == true || snuckPastPatrol == true)
    {
        title = " the Beast";
    }
    if (gateOpen == true)
    {
        title = " the Badass";
    }
    if (commanderSlain == true)
    {
        title = " the Avenger";
    }
    if (labattehSlain == true)
    {
        title = " the Unicorn Savior";
        cout << "Throughout your quest you slayed: " << enemyKills << " evil canadian foods.\n";
        cout << "You accumulated " << goldAccumulated << " gold and spent " << goldSpent << " of it.\n";
    }
    cout << " ---------------------------------------------\n";
    cout << "                     Status\n";
    cout << " ---------------------------------------------\n\n";
    cout << "    You are " << name << title << ".\n";
    cout << "    You are level " << level << " and have " << health << "/" << maxHealth << " health.\n";
    cout << "    You have: " << exp << " exp, Gold: " << gold << "\n";
    cout << "    Speed: " << speed << ", Attack mod: " << attackMod << ", Defense mod: " << defenseMod << "\n";
    if (hasUnicornSword == true && hasUnicornShield == true)
    {
        cout << "    You wield the Unicorn Sword and Unicorn Shield!\n";
    }
    else if (hasUnicornSword == true && hasUnicornShield == false)
    {
        cout << "    You wield the Unicorn Sword!\n";
    }
    else if (hasUnicornSword == false && hasUnicornShield == true)
    {
        cout << "    You wield the Unicorn Shield!\n";
    }
    cout << "\n";
    cout << " ---------------------------------------------\n\n";
    cout << "Press ENTER to Continue\n\n";
    reader.readString();
    clearScreen();
}

void Character::userPrompt()
{
    cout << "\nHealth: " << health << " >> ";
}

void Character::combat(Character& enemy)
{
    int combatRounds = 0;
    while (health > 0 && enemy.health > 0)
    {
        combatRounds++;
        int chanceToBlock = 0;
        srand(time(0));

        int damage = 0;
        int enemyDamage = 0;
        int temp = 0;
        // Player attack values
        for (int i = 0; i < NUM_ATTACKDICE; i++)
        {
            int diceRoll = rand() % SIZE_ATTACKDICE + 1;
            damage += diceRoll;
            // cout << "i = " << i << ", damage = " << damage << "[+" << diceRoll << "]" << endl;
        }
        damage += attackMod;
        damage -= enemy.defenseMod;
        if (hasUnicornSword == true)
        {
            damage += 20;
        }
        if (damage <= 0)
        {
            damage = 1;
        }

        // Enemy attack values
        for (int i = 0; i < NUM_ATTACKDICE; i++)
        {
            int diceRoll = rand() % SIZE_ATTACKDICE + 1;
            enemyDamage += diceRoll;
            // cout << "i = " << i << ", damage = " << damage << "[+" << diceRoll << "]" << endl;
        }
        enemyDamage += enemy.attackMod;
        enemyDamage -= defenseMod;
        if (enemyDamage <= 0)
        {
            enemyDamage = 1;
        }

        if (enemy.name == "LaBatteh" && combatRounds == 3 && hasUnicornSword == true)
        {
            cout << "LaBatteh channels evil Canadian powers and breaks your Unicorn Sword!\n";
            damage -= 20;
            hasUnicornSword = false;
        }

        if (speed >= enemy.speed) // player gets first hit
        {
            // Player round
            if (damage > enemy.health)
            {
                temp = enemy.health;
            }
            else
            {
                temp = damage;
            }
            if (enemy.name == "LaBatteh")
            {
                int chanceToStun = rand() % 100 + 1;
                if (chanceToStun > 90)
                {
                    cout << enemy.name << " splashes beer in your face, stunning you for the moment!\n";
                }
                else
                {
                    enemy.health -= temp;
                    if (hasUnicornSword == true)
                    {
                        cout << "You slash " << enemy.name << " with your Unicorn Sword for " << temp << " damage (" << enemy.health << " left)\n";
                    }
                    else
                    {
                        cout << "You did " << temp << " damage to " << enemy.name << " (" << enemy.health << " left)\n";
                    }
                    if (enemy.health <= 0)
                    {
                        break;
                    }
                }
            }
            else
            {
                enemy.health -= temp;
                if (hasUnicornSword == true)
                {
                    cout << "You slash " << enemy.name << " with your Unicorn Sword for " << temp << " damage (" << enemy.health << " left)\n";
                }
                else
                {
                    cout << "You did " << temp << " damage to " << enemy.name << " (" << enemy.health << " left)\n";
                }
                if (enemy.health <= 0)
                {
                    break;
                }
            }

            // Enemy Round
            if (enemyDamage > health)
            {
                temp = health;
            }
            else
            {
                temp = enemyDamage;
            }
            int chanceToHeal = 0;
            if (enemy.name == "LaBatteh")
            {
                chanceToHeal = rand() % 100 + 1;
                if (chanceToHeal > 70)
                {
                    int healing = rand() % 30 + 1;
                    healing += 15;
                    enemy.health += healing;
                    cout << enemy.name << " summons his evil powers to regenerate!\n";
                    cout << enemy.name << " healed for " << healing << "! (" << enemy.health << " left)\n";
                }
                else
                {
                    if (hasUnicornShield == true)
                    {
                        chanceToBlock = rand() % 100 + 1;
                    }
                    if (chanceToBlock < 75)
                    {
                        health -= temp;
                        cout << enemy.name << " did " << temp << " damage to you.\n";
                        if (health <= 0)
                        {
                            cout << "You have died.\n";
                            gameOver();
                            break;
                        }
                    }
                    else if (chanceToBlock > 75)
                    {
                        cout << "You blocked " << enemy.name << "'s attack with your Unicorn Shield!\n";
                    }
                }
            }
            else
            {
                if (hasUnicornShield == true)
                {
                    chanceToBlock = rand() % 100 + 1;
                }
                if (chanceToBlock < 75)
                {
                    health -= temp;
                    cout << enemy.name << " did " << temp << " damage to you.\n";
                    if (health <= 0)
                    {
                        cout << "You have died.\n";
                        gameOver();
                        break;
                    }
                }
                else if (chanceToBlock > 75)
                {
                    cout << "You blocked " << enemy.name << "'s attack with your Unicorn Shield!\n";
                }
            }

            userPrompt();
            cout << "Hit ENTER to continue combat:\n";
            reader.readString();
        }
        else // enemy gets first hit
        {
            // Enemy round
            if (enemyDamage > health)
            {
                temp = health;
            }
            else
            {
                temp = enemyDamage;
            }
            int chanceToHeal = 0;
            if (enemy.name == "LaBatteh")
            {
                chanceToHeal = rand() % 100 + 1;
                if (chanceToHeal > 60)
                {
                    int healing = rand() % 40 + 1;
                    healing += 10;
                    enemy.health += healing;
                    cout << enemy.name << " summons his evil powers to regenerate!\n";
                    cout << enemy.name << " healed for " << healing << "! (" << enemy.health << " left)\n";
                }
                else
                {
                    if (hasUnicornShield == true)
                    {
                        chanceToBlock = rand() % 100 + 1;
                    }
                    if (chanceToBlock < 75)
                    {
                        health -= temp;
                        cout << enemy.name << " did " << temp << " damage to you.\n";
                        if (health <= 0)
                        {
                            cout << "You have died.\n";
                            gameOver();
                            break;
                        }
                    }
                    else if (chanceToBlock > 75)
                    {
                        cout << "You blocked " << enemy.name << "'s attack with your Unicorn Shield!\n";
                    }
                }
            }
            else
            {
                if (hasUnicornShield == true)
                {
                    chanceToBlock = rand() % 100 + 1;
                }
                if (chanceToBlock < 75)
                {
                    health -= temp;
                    cout << enemy.name << " did " << temp << " damage to you.\n";
                    if (health <= 0)
                    {
                        cout << "You have died.\n";
                        gameOver();
                        break;
                    }
                }
                else if (chanceToBlock > 75)
                {
                    cout << "You blocked " << enemy.name << "'s attack with your Unicorn Shield!\n";
                }
            }

            // Player Round
            if (damage > enemy.health)
            {
                temp = enemy.health;
            }
            else
            {
                temp = damage;
            }
            if (enemy.name == "LaBatteh")
            {
                int chanceToStun = rand() % 100 + 1;
                if (chanceToStun > 90)
                {
                    cout << enemy.name << " splashes beer in your face, stunning you for the moment!\n";
                }
                else
                {
                    enemy.health -= temp;
                    if (hasUnicornSword == true)
                    {
                        cout << "You slash " << enemy.name << " with your Unicorn Sword for " << temp << " damage (" << enemy.health << " left)\n";
                    }
                    else
                    {
                        cout << "You did " << temp << " damage to " << enemy.name << " (" << enemy.health << " left)\n";
                    }
                    if (enemy.health <= 0)
                    {
                        break;
                    }
                }
            }
            else
            {
                enemy.health -= temp;
                if (hasUnicornSword == true)
                {
                    cout << "You slash " << enemy.name << " with your Unicorn Sword for " << temp << " damage (" << enemy.health << " left)\n";
                }
                else
                {
                    cout << "You did " << temp << " damage to " << enemy.name << " (" << enemy.health << " left)\n";
                }
                if (enemy.health <= 0)
                {
                    break;
                }
            }
            userPrompt();
            cout << "Hit ENTER to continue combat:\n";
            reader.readString();
        }
    }
    if (health > 0)
    {
        cout << enemy.name << " has died." << endl;
        int awardGold = rand() % enemy.gold + 1;
        cout << enemy.name << " droppped " << awardGold << " gold and awarded " << enemy.exp << " exp!\n\n";
        gold += awardGold;
        goldAccumulated += awardGold;
        exp += enemy.exp;
        enemyKills++;
        levelUp();

        if (enemy.name == "Canadian Bacon")
        {
            baconSlain++;
            if (baconSlain == 5)
            {
                baconLimitReached = true;
                cout << "\n    You have cut through your bacon foes and may press on!\n\n";
            }
        }

        if (enemy.name == "Poutine Patrol")
        {
            poutinePatrolSlain = true;
            cout << "    The Poutine Patrol has been slain for the Unicorns!\n";
            if (snuckPastPatrol == false)
            {
                cout << "    You have earned the title 'the Beast'!\n";
            }
            cout << "\n";
        }

        if (enemy.name == "Ketchup flavored Chips")
        {
            commanderSlain = true;
            cout << "\n  Ketchup flavored Chips last words are, 'Don't... Blame... Canada, Batman...'\n\n";
            cout << "    The Commander Ketchup flavored Chips has felt the wrath of the Unicorns!\n";
            cout << "    You have earned the title 'the Avenger'!\n";
            cout << "\n";
        }

        if (enemy.name == "Bagged Milk Soldier")
        {
            milkSoldierSlain++;
            if (milkSoldierSlain == 5)
            {
                milkSoldierLimitReached = true;
                cout << "\n    You have cut through your milk foes and may press on!\n\n";
            }
        }

        if (enemy.name == "Bagged Milk Guard")
        {
            milkGuardSlain++;
            if (milkGuardSlain == 5)
            {
                milkGuardLimitReached = true;
                cout << "\n    You have cut through your milk foes and may press on!\n\n";
            }
        }

        if (enemy.name == "LaBatteh")
        {
            labattehSlain = true;
            finalSequence();
        }
    }
}

void Character::awardXp(int xpAward)
{
    exp += xpAward;
    levelUp();
}

void Character::levelUp()
{
    if (level == 1)
    {
        if (exp >= 200)
        {
            level++;
            int temp = exp - 200;
            exp = temp;
            health += 10;
            maxHealth += 10;
            attackMod += 3;
            defenseMod += 2;
            speed += 2;
            cout << "                **** You have leveled up! ****\n";
            cout << "     You've gained 10 health, 3 damage, 2 defense, 2 speed.\n\n";
        }
    }

    if (level == 2)
    {
        if (exp >= 400)
        {
            level++;
            int temp = exp - 400;
            exp = temp;
            health += 15;
            maxHealth += 15;
            attackMod += 4;
            defenseMod += 2;
            speed += 2;
            cout << "                **** You have leveled up! ****\n";
            cout << "     You've gained 15 health, 4 damage, 2 defense, 2 speed.\n\n";
        }
    }

    if (level == 3)
    {
        if (exp >= 700)
        {
            level++;
            int temp = exp - 700;
            exp = temp;
            health += 20;
            maxHealth += 20;
            attackMod += 4;
            defenseMod += 1;
            speed += 1;
            cout << "                **** You have leveled up! ****\n";
            cout << "     You've gained 20 health, 4 damage, 1 defense, 1 speed.\n\n";
        }
    }

    if (level == 4)
    {
        if (exp >= 1000)
        {
            level++;
            int temp = exp - 1000;
            exp = temp;
            health += 25;
            maxHealth += 25;
            attackMod += 5;
            defenseMod += 2;
            speed += 3;
            cout << "                **** You have leveled up! ****\n";
            cout << "     You've gained 25 health, 5 damage, 2 defense, 3 speed.\n\n";
        }
    }

    if (level == 5)
    {
        if (exp >= 1400)
        {
            level++;
            int temp = exp - 1400;
            exp = temp;
            health += 30;
            maxHealth += 30;
            attackMod += 4;
            defenseMod += 3;
            speed += 3;
            cout << "                **** You have leveled up! ****\n";
            cout << "     You've gained 30 health, 4 damage, 3 defense, 3 speed.\n\n";
        }
    }

    if (level == 6)
    {
        if (exp >= 2000)
        {
            level++;
            int temp = exp - 2000;
            exp = temp;
            health += 40;
            maxHealth += 40;
            attackMod += 5;
            defenseMod += 4;
            speed += 3;
            cout << "                **** You have leveled up! ****\n";
            cout << "     You've gained 40 health, 5 damage, 4 defense, 3 speed.\n\n";
        }
    }
}

void Character::finalSequence()
{
    cout << "  As LaBatteh 'the Blue' falls to the floor, his last words are,\n";
    cout << "  'You have won this battle... But.. the... war is not over...'\n\n";
    cout << "  You release your Unicorn partner, Mertle from her bonds and she nuzzles your\n"
         << "  neck in approval.  Your quest is finally over and your beloved partner set  \n"
         << "  free at last!\n\n";
    cout << "  But just as you and Mertle were about to leave this wretched lair at last...\n"
         << "  A puff of smoke appears and out steps Dr. Vandango nemisis of all that is   \n"
         << "  Unicorn...\n\n"
         << "  Dr. Vandango says, 'Don't think for a minute you are safe.  LaBatteh was only\n"
         << "  the start.'\n\n";
    cout << "  Dr. Vandango lets out an evil laugh.  Muahahahaha.\n\n"
         << "  Dr. Vandango says, 'The war has just begun.  You shall meet my armies soon...'\n\n"
         << "  Before you could speak, Dr. Vandango disappears in a puff of smoke.\n\n"
         << "  You realize now that your quest is not quite over, and will continue...\n\n";
    cout << "Press ENTER to continue.\n";
    reader.readString();
    clearScreen();
    displayCharacter();
    showArt("Mertle.txt");
    cout << "Press ENTER to continue.\n";
    reader.readString();
    showCredits();
    exitGame = true;
}

void showCredits()
{
    clearScreen();
    cout << "\n                                 C R E D I T S           \n\n\n";
    cout << "                               Special Thanks to: \n\n"
         << "                                 Ashley Reiter\n\n"
         << "                                 Lloyd Kincaid\n\n"
         << "                                 Boyd Trolinger\n\n"
         << "                                 Canada\n\n"
         << "                                 Unicorns!\n\n";
    cout << "\n\n";
}

void gameOver()
{
    exitGame = true;
    cout << "\n\n               You have failed in your quest to save the Unicorns!\n\n";
    showArt("gameOverArt.txt");
}

void showArt(string artfile)
{
    ifstream fin(artfile.c_str());
    if (!fin.bad())
    {
        char nextchar = fin.get();
        while (nextchar != EOF)
        {
            cout << nextchar;
            nextchar = fin.get();
        }
        fin.close();
    }
}

void clearScreen()
{
#ifdef WIN32
    system("cls");
#else
    system("clear");
#endif
}
