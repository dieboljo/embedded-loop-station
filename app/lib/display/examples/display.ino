/*

  example in how to implement the ili9341_t3_menu library. This is an example that
  uses both menu types
  1) a simple selection menu (ItemMenu) where the user can scroll up/down and select
  an item and some action be performed such as drawing a screen or activating a sub menu
  2) an menu with in-line where the user can scroll up/down the list and select an item
  however the difference is that move/up down on a selected item will scroll the values of that
  menu item and NOT scroll the list, selecing a selected item will restor control to moving up/down
  3) a EditMenu menu with in-line an no icons (down and dirty)

  highlights
  1. menu items exceeds screen size but library handles wrapping
  2. each item selection has a range, increment and decimal readout
  3. items can also be read from a char array

*/

// required libraries
#include "ILI9341_t3_Menu.h"          // custom utilities definition
#include "ILI9341_t3.h"                  // high speed display that ships with Teensy
#include "font_Arial.h"                  // custom fonts that ships with ILI9341_t3.h
#include <font_ArialBold.h>
// required only if you are using an encoder to handle user input, mechanical buttons work as well
#include <XPT2046_Touchscreen.h>  // touch driver for a TFT display

#include <SPI.h>
#include <SD.h>

// TFT display and SD card will share the hardware SPI interface.
// Hardware SPI pins are specific to the Arduino board type and
// cannot be remapped to alternate pins.  For Arduino Uno,
// Duemilanove, etc., pin 11 = MOSI, pin 12 = MISO, pin 13 = SCK.

//define SD card location
const int chipSelect = BUILTIN_SDCARD;


// pins for LED and select button on encoder
#define SE_PIN A2

#define ROW_HEIGHT          35
#define ROWS                5
#define DATA_COLUMN         190

// pins for LED and select button on encoder
#define LED_PIN A9
#define CS_PIN    10
#define DC_PIN    9
#define T_CS    0
#define T_IRQ   1

// easy way to include fonts but change globally
#define FONT_SMALL  Arial_16            // font for menus
#define FONT_EDITTITLE  Arial_18_Bold            // font for menus
#define FONT_ITEM   Arial_16             // font for menus
#define FONT_TITLE  Arial_24_Bold        // font for all headings

#define DEBOUNCE    60

int MenuOption = 0;
int AllowColorMenu = 0;

// global x y for touch stuff
int BtnX, BtnY;

// must have variables for each menu item
// best to have these global so you can use them in processing functions
int MenuOption1 = 0, MenuOption2 = 0, MenuOption3 = 0, MenuOption4 = 0, MenuOption5 = 0;
int MenuOption6 = 0, MenuOption7 = 0, MenuOption8 = 0, MenuOption9 = 0;

int PlaybackOption1 = 0, PlaybackOption2 = 0;

int LibraryOption1 = 0, LibraryOption2 = 0, LibraryOption3 = 0, LibraryOption4 = 0, LibraryOption5 = 0, LibraryOption6 = 0;

//float Temp1Adj = 0.2, Temp2Adj = -.3, AlarmVal = 1;

//long Position = 0, oldPosition = 0;

// create some selectable menu sub-items, these are lists inside a menu item
/*
const char *LibraryItems[] =   {"Track 1", "Track 2", "Track 3", "Track 4", "Track 5"
        // this is just an example. We will want to use SD.h to display SD files
};
*/

const char *EffectsItems[] =   {"On", "Off"
        // this is just an example. We will to modify when on/off
};
const char *OptionItems[] =   {"On", "Off"
        // this is just an example. We will to modify when on/off
};


// OK i'm going crazy with examples, but this will help show more processing when an int is needed but a float returned
// from the menu code
const char *C_NAMES[] = {"White", "Black", "Grey", "Blue", "Red", "Green", "Cyan", "Magenta",
                         "Yellow", "Teal", "Orange", "Pink", "Purple", "Lt Grey", "Lt Blue", "Lt Red",
                         "Lt Green", "Lt Cyan", "Lt Magenta", "Lt Yellow", "Lt Teal", "Lt Orange", "Lt Pink", "Lt Purple",
                         "Medium Grey", "Medium Blue", "Medium Red", "Medium Green", "Medium Cyan", "Medium Magenta", "Medium Yellow", "Medium Teal",
                         "Medium Orange", "Medium Pink", "Medium Purple", "Dk Grey", "Dk Blue", "Dk Red", "Dk Green", "Dk Cyan",
                         "Dk Magenta", "Dk Yellow", "Dk Teal", "Dk Orange", "Dk Pink", "Dk Purple"
};

const uint16_t  C_VALUES[] = {  0XFFFF, 0X0000, 0XC618, 0X001F, 0XF800, 0X07E0, 0X07FF, 0XF81F, //7
                                0XFFE0, 0X0438, 0XFD20, 0XF81F, 0X801F, 0XE71C, 0X73DF, 0XFBAE, //15
                                0X7FEE, 0X77BF, 0XFBB7, 0XF7EE, 0X77FE, 0XFDEE, 0XFBBA, 0XD3BF, //23
                                0X7BCF, 0X1016, 0XB000, 0X0584, 0X04B6, 0XB010, 0XAD80, 0X0594, //31
                                0XB340, 0XB00E, 0X8816, 0X4A49, 0X0812, 0X9000, 0X04A3, 0X0372, //39
                                0X900B, 0X94A0, 0X0452, 0X92E0, 0X9009, 0X8012 //45
};
// set default colors
uint16_t MENU_TEXT = C_VALUES[1];
uint16_t MENU_BACKGROUND = C_VALUES[0];
uint16_t MENU_HIGHLIGHTTEXT = C_VALUES[1];
uint16_t MENU_HIGHLIGHT = C_VALUES[21];
uint16_t MENU_HIGHBORDER = C_VALUES[10];
uint16_t MENU_SELECTTEXT = C_VALUES[0];
uint16_t MENU_SELECT = C_VALUES[4];
uint16_t MENU_SELECTBORDER = C_VALUES[37];
uint16_t MENU_DISABLE = C_VALUES[2];
uint16_t TITLE_TEXT = C_VALUES[13];
uint16_t TITLE_BACK = C_VALUES[36];

// this next rediciously long section is merely the icon image data
// scroll down to 1540 or so for the actual code

// Website for generating icons
// https://javl.github.io/image2cpp/

// you know the drill
ILI9341_t3 Display(CS_PIN, DC_PIN); //Display object

// required, you must create either an Item menu (no inline editing) or an EditMenu (allows inline editing)
//ClassName YourMenuName(&DisplayObject, True=Touch input, False(Default)=mechanical input);
ItemMenu MainMenu(&Display, true);

// since we're showing both menu types, create an object for each where the item menu is the main and calls edit menus
// you can have an item menu call other item menus an edit menu can call an edit menu but in a round about way--not recommended
//ClassName YourMenuName(&DisplayObject, True=Touch input, False(Default)=mechanical input);

EditMenu PlaybackMenu(&Display, true);
EditMenu LibraryMenu(&Display, true);

XPT2046_Touchscreen Touch(T_CS, T_IRQ);

TS_Point TouchPoint;

void setup() {

    Serial.begin(9600);

    // while ((millis() > 5000) || (!Serial)) {}
    // I use a digital pin to control LED brightness
    pinMode(LED_PIN, OUTPUT);
    pinMode(A0, INPUT_PULLUP);
    pinMode(A1, INPUT_PULLUP);
    pinMode(A2, INPUT_PULLUP);
    delay(300);
    digitalWrite(LED_PIN, HIGH);


    //initialize SD card
    Serial.print(F("Initializing SD card..."));
    Serial.println(F("Init SD card..."));
    while (!SD.begin(BUILTIN_SDCARD)) {
        Serial.println(F("failed to access SD card!"));
        Serial.println(F("failed to access SD card!"));
        delay(2000);
    }
    Serial.println("OK!");

    // fire up the display
    Display.begin();
    Display.setRotation(1);

    Touch.begin();

    Touch.setRotation(3);

    Display.fillScreen(0);

    // initialize the MainMenu object
    // note ROW_HEIGHT is row height and needs to be larger that font height
    // the ROWS is max rows to be displayed (remember, library handles wraparound
    /*
      init(TextColor, BackgroundColor, HighlightTextColor, HighlightColor,
      ItemRowHeight, MaxRowsPerScreen, TitleText, ItemFont, TitleFont);
    */

    ///////////////////////////////////////// Main Menu //////////////////////////////////////////////////

    MainMenu.init(MENU_TEXT, MENU_BACKGROUND, MENU_HIGHLIGHTTEXT, MENU_HIGHLIGHT, 64, 3, "Main Menu", FONT_TITLE, FONT_TITLE);

    // now add each item, text is what's displayed for each menu item, there are no other arguements
    // there are 3 add types
    // 1. addNI for no icon
    // 2. addMono for simple 1 color  icon each item
    // 3. add565 for displaying a color icon for each item

    // note the return value for each item will be it's returned itemid an dwill be > 0
    // a return of 0 is reserved for the exit item
    // the exit item is actually the title bar--if user moves selector to the title bar
    // it's temporarily renamed to "Exit"

    /*
      addNI(ItemLabel);
      addMono(ItemLabel, MonoBitmap, BitmapWidth, BitmapHeight );
      add565(ItemLabel, ColorBitmap, uint8_t BitmapWidth, uint8_t BitmapHeight);
    */

    /*
    MenuOption1 = MainMenu.add565("Options", epd_bitmap_sensor, 64, 64);
    MenuOption2 = MainMenu.add565("Colors", epd_bitmap_drivers, 64, 64);
    MenuOption3 = MainMenu.add565("Wireless", epd_bitmap_64_wireless, 64, 64);
    MenuOption4 = MainMenu.add565("Motors", epd_bitmap_sensor, 64, 64);
    MenuOption5 = MainMenu.add565("Sensors", epd_bitmap_64_wireless, 64, 64);
  */

    MenuOption1 = MainMenu.addNI("Playback");
    MenuOption2 = MainMenu.addNI("Library");
    MenuOption3 = MainMenu.addNI("Effects");
    MenuOption4 = MainMenu.addNI("Options");
    MenuOption5 = MainMenu.addNI("Version");
    //MenuOption3 = MainMenu.add565("Options", epd_bitmap_sensor, 64, 64);

    // the remaing method calls for this menu are optional and shown as an example on what some of the things you can do
    // however, you will most likely need to set xxxMarginxxx stuff as the library does not attempt to get text bounds
    // and center, you will have to put pixel values in to control where text is display in menu items, title bars, etc.

/*
  // just and example how you can disable a menu item
  if (AllowColorMenu == 0) {
    MainMenu.disable(MenuOption2);
  }
  else {
    MainMenu.enable(MenuOption2);
  }
  */

    // getEnableState(MenuItemID);
    // wanna know what the stat is?
    Serial.print("Enable State for MainMenu, MenuOption3: ");
    Serial.println(MainMenu.getEnableState(MenuOption3));

    // optional, but you can change title bar colors
    // setTitleColors(TitleTextColor, TitleFillColor);
    MainMenu.setTitleColors(TITLE_TEXT, TITLE_BACK);

    // these are all optional, but you can change title bar colors, size, and margins.
    // setTitleBarSize(TitleTop, TitleLeft, TitleWith, TitleHeight);
    MainMenu.setTitleBarSize(0, 0, 320, 40);

    // optional but lets center the title text about the height but scoot it in a bit from the left
    // setTitleTextMargins(LeftMargin, TopMargin);
    MainMenu.setTitleTextMargins(60, 7);

    // optional but you can set the left margin and top to scoot the icon over and down
    // only needed if you are using icons in your menu items
    // setIconMargins(LeftMargin, TopMargin);
    MainMenu.setIconMargins(0, 0);

    // optional but you can set the left margin and top to scoot the menu highlight over and down
    // not needed but recommended so you can better control where the text is placed
    // the library will not determine font height and such and adjust--that will be on you
    // setMenuBarMargins(LeftMargin, Width, BorderRadius, BorderThickness);
    MainMenu.setMenuBarMargins(0, 310, 10, 4);

    // optional but you can set the menu highlight special colors (disable text color and border color)
    // not needed if you dont plan on disabling menu items
    // the library will not determine font height and such and adjust--that will be on you
    // setItemColors(DisableTextColor, BorderColor);
    MainMenu.setItemColors(MENU_DISABLE, MENU_SELECTBORDER);

    // setItemTextMargins(LeftMargin, TopMargin, MarginBetweenTitleBarAndMenu);
    MainMenu.setItemTextMargins(10, 15, 5);

    // this example includes both menu types, the first (above was a menu where items have not editing)
    // this menu type EditMenu allows changes for each items value--basically in-line editing
    // more capability? more arguements...

    /*
       init(TextColor, BackgroundColor, HighlightTextColor, HighlightColor, SelectedTextColor, SelectedColor,
        MenuColumn, ItemRowHeight,MaxRowsPerScreen, TitleText, ItemFont, TitleFont);
    */

    /////////////////////////////////////////////// Playback Menu /////////////////////////////////////////////////////////

    //OptionMenu.init(MENU_TEXT, MENU_BACKGROUND, MENU_HIGHLIGHTTEXT, MENU_HIGHLIGHT, MENU_SELECTTEXT, MENU_SELECT,
    // DATA_COLUMN, ROW_HEIGHT, ROWS, "Sensor Menu", FONT_ITEM, FONT_TITLE);

    PlaybackMenu.init(MENU_TEXT, MENU_BACKGROUND, MENU_HIGHLIGHTTEXT, MENU_HIGHLIGHT, MENU_SELECTTEXT, MENU_SELECT,
                      DATA_COLUMN, ROW_HEIGHT, ROWS, "Playback Menu", FONT_ITEM, FONT_TITLE);

    // now add each item, text is what's displayed for each menu item, there are no other arguements
    // there are 3 add types
    // 1. addNI for no icon
    // 2. addMono for simple 1 color  icon each item
    // 3. add565 for displaying a color icon for each item

    // note the return value for each item will be it's returned itemid an dwill be > 0
    // a return of 0 is reserved for the exit item
    // the exit item is actually the title bar--if user moves selector to the title bar
    // it's temporarily renamed to "Exit"

    // the edit can be either by cycling through a range of values (low to high by some increment value
    // example, set a voltage divider calibration value 4000 to 10000 in increments of 100
    // or cycle through an array list
    // example, choose your favorite pet from a list ("cat", "dog", "bird", "fish")
    // you still enter a lower and high limit and are the array bounds (0 to 3 in the above example)
    // your Data is the initial array value so you can still have say "bird" be the initial value)
    // in either case you can have icons none, mono or color

    /*
      addNI(ItemText, InitalDisplayData, LowLimit, HighLimit, IncrementValue, DisplayDecimalPlaces, ItemMenuText);

      addMono(ItemText, Data, LowLimit, HighLimit, Increment, DecimalPlaces, ItemMenuText,
      Bitmap, BitmapWidth, BitmapHeight);

      add565(ItemText, Data, LowLimit, HighLimit, Increment, DecimalPlaces, ItemMenuText,
      Bitmap, BitmapWidth, BitmapHeight);
    */

    PlaybackOption1 = PlaybackMenu.addNI("Play", 0, 0, 0, 0, 0);
    PlaybackOption2 = PlaybackMenu.addNI("Stop",0, 0, 0, 0, 0);

    // again all these calls are optional, but you most likely will want to set margins

    // optional but you can store a setting such as a calibration value in EEPROM, read at startup
    // and populate with setItemValue(), even though the item data was set in the add method, you can change it later
    // setItemValue(ItemID, ItemValue){

    //OptionMenu.setItemValue(OptionOption1, 0.12);
    PlaybackMenu.setItemValue(PlaybackOption1, 0);
    PlaybackMenu.setItemValue(PlaybackOption2, 1);
    //OptionMenu.setItemValue(OptionOption3, 1); // the 2nd element in the ReadoutItems array

    // optional but can can set the title colors
    // setTitleColors(TitleTextColor, TitleFillColor);
    //OptionMenu.setTitleColors(TITLE_TEXT, TITLE_BACK);
    PlaybackMenu.setTitleColors(TITLE_TEXT, TITLE_BACK);

    // optional but you can set the size of the title bar
    // setTitleBarSize(TitleTop, TitleLeft, TitleWith, TitleHeight);
    //OptionMenu.setTitleBarSize(0, 0, 320, 40);
    PlaybackMenu.setTitleBarSize(0, 0, 320, 40);

    // optional but you can set the margins in how the text in the title bar is centered
    // setTitleTextMargins(LeftMargin, TopMargin);
    //OptionMenu.setTitleTextMargins(60, 7);
    PlaybackMenu.setTitleTextMargins(60, 7);

    // optional but you can scoot the icon over and down
    // setIconMargins(LeftMargin, TopMargin);
    //OptionMenu.setIconMargins(5, 0);
    PlaybackMenu.setIconMargins(5, 0);

    // optional but you can set the margins and size of the text in the menu bar
    // setItemTextMargins(LeftMargin, TopMargin, MarginBetweenTitleBarAndMenu);
    //OptionMenu.setItemTextMargins(7, 9, 10);
    PlaybackMenu.setItemTextMargins(7, 9, 10);

    // optional but you can change colors other that in the init method
    // colors such as disable text and border color (if you display a border of course)
    // setItemColors(DisableTextColor, selectorBorderColor, EditModeBorderColor);
    //OptionMenu.setItemColors(MENU_DISABLE, MENU_HIGHBORDER, MENU_SELECTBORDER);
    PlaybackMenu.setItemColors(MENU_DISABLE, MENU_HIGHBORDER, MENU_SELECTBORDER);



    ///////////////////////////////////////// Library Menu //////////////////////////////////////////////////

    LibraryMenu.init(MENU_TEXT, MENU_BACKGROUND, MENU_HIGHLIGHTTEXT, MENU_HIGHLIGHT, MENU_SELECTTEXT, MENU_SELECT,
                     DATA_COLUMN, 30, 5, "Library", FONT_ITEM, FONT_TITLE);

    //addNI(ItemText, InitalDisplayData, LowLimit, HighLimit, IncrementValue, DisplayDecimalPlaces, ItemMenuText);

    String trackname;

    //LibraryOption1 = LibraryMenu.trackname(LibraryItems(4));
    //LibraryOption2 = LibraryMenu.trackname(LibraryItems(4));
    //LibraryOption3 = LibraryMenu.trackname(LibraryItems(4));
    //LibraryOption4 = LibraryMenu.trackname(LibraryItems(4));
    //LibraryOption1 = LibraryMenu.item("Tracks", LibraryItems(4));

    // again all these calls are optional, but you most likely will want to set margins

    // optional but you can store a setting such as a calibration value in EEPROM, read at startup
    // and populate with setItemValue(), even though the item data was set in the add method, you can change it later
    // setItemValue(ItemID, ItemValue){

    // optional but can can set the title colors
    // setTitleColors(TitleTextColor, TitleFillColor);
    LibraryMenu.setTitleColors(TITLE_TEXT, TITLE_BACK);

    // optional but you can set the size of the title bar
    // setTitleBarSize(TitleTop, TitleLeft, TitleWith, TitleHeight);
    LibraryMenu.setTitleBarSize(0, 0, 320, 40);

    // optional but you can set the margins in how the text in the title bar is centered
    // setTitleTextMargins(LeftMargin, TopMargin);
    LibraryMenu.setTitleTextMargins(60, 7);

    // optional but you can scoot the icon over and down
    // setIconMargins(LeftMargin, TopMargin);
    LibraryMenu.setIconMargins(5, 0);

    // optional but you can set the margins and size of the text in the menu bar
    // setItemTextMargins(LeftMargin, TopMargin, MarginBetweenTitleBarAndMenu);
    LibraryMenu.setItemTextMargins(7, 9, 10);

    // optional but you can change colors other that in the init method
    // colors such as disable text and border color (if you display a border of course)
    // setItemColors(DisableTextColor, selectorBorderColor, EditModeBorderColor);
    LibraryMenu.setItemColors(MENU_DISABLE, MENU_HIGHBORDER, MENU_SELECTBORDER);

    // you can simplay call the draw method on a menu object, but you will need to add processing
    // here's how I recommend doing that, have a function that draws the main menu and processes the selections
    // ideally you will probably have a "setting" button in your UI that will call the "ProcessMainMenu"
    ProcessMainMenu();

    Display.fillScreen(MENU_BACKGROUND);

}

void loop() {

    // this is just a processor for when you exit the top level menu
    Display.setCursor(10, 100);
    Display.setFont(FONT_TITLE);
    Display.setTextColor(ILI9341_RED, ILI9341_WHITE);
    Display.print(F("Done"));
    delay(500);
    Display.setCursor(10, 100);
    Display.setFont(FONT_TITLE);
    Display.setTextColor(ILI9341_BLUE, ILI9341_BLUE);
    Display.print(F("Done"));
    delay(500);

    // and example on how you can call menu while in a loop
    // of course you will probably have a button to launch ProcessMainMenu
    if (Serial.available()) {
        if (Serial.read() >= 32) {
            ProcessMainMenu();
            Display.fillScreen(MENU_BACKGROUND);
        }
    }

}

// function to process main menu iteraction
int ProcessMainMenu() {


    // the entire menu processing are basically 2 call

    // EditMenuOption = YourMenu.press(BtnX, BtnY); // process any screen press
    // YourMenu.drawRow(EditMenuOption); // draw the pressed row

    // set an inital flag that will be used to store what menu item the user exited on
    int MainMenuOption = 1;

    // blank out the screen
    Display.fillScreen(MENU_BACKGROUND);

    // draw the main menu
    MainMenu.draw();

    // run the processing loop until user move selector to title bar (which becomes exit)
    // and selectes it

    while (MainMenuOption != 0) {

        delay(50);

        // attempt to debouce these darn things...
        if (Touch.touched()) {

            ProcessTouch();

            MainMenuOption = ProcessButtonPress(MainMenu);
            Serial.println(MainMenuOption);

            //Playback menu
            if (MainMenuOption == MenuOption1) {
                // item 1 was the sensor menu
                ProcessPlaybackMenu();
                // when done processing that menu, return here
                // clear display and redraw this main menu
                Display.fillScreen(MENU_BACKGROUND);
                MainMenu.draw();
            }

            // Library Menu
            if (MainMenuOption == MenuOption2) {
                ProcessLibraryMenu();
                Display.fillScreen(MENU_BACKGROUND);
                MainMenu.draw();
            }

        }
    }

    return MainMenuOption;

}

// menu to handle processing for a sub-menu
// since this menu will be a menu that allows edits (EditMenu object type)
// process is exactly the same as an ItemMenu
// meaning you simply use the same MoveUp, MoveDown and the library will know if you are
// wanting to move the selector or cycle through a range

void ProcessPlaybackMenu() {

    // processing edit menus is simple
    // after press and drawRow, the libray handles additioal presses for editing

    // 1. arrows to move menu up/down
    // 2. press menu item toggles edit mode on / off
    // 3. if toggled to edit, increments values up / down
    // basically a full edit capable menu with 2 calls :) ProcessWirelessMenu shows this


    // set an inital flag that will be used to store what menu item the user exited on
    int EditMenuOption = 1;

    // blank out the screen
    Display.fillScreen(MENU_BACKGROUND);

    // draw the main menu
    PlaybackMenu.draw();

    // run the processing loop until user move selector to title bar (which becomes exit)
    // and selectes it
    while (EditMenuOption != 0) {

        delay(100);
        // this next section is purely optional and shows how data change can be used
        // the EditMenu object type is special cased where if you select a menu item
        // the library consideres that a desire to start editing the item
        // the menu bar color changes and any MoveUp / MoveDown will then be directed
        // to editing the item value itself
        // once user Selects the selected row, the library will consider editign done
        // and restore menu selection with MoveUp / MoveDown
        // watch the YouTube video for a demo
        // to modify other values
        if (Touch.touched()) {

            ProcessTouch();

            // I elected to not put the drawRow in the press method
            // separate let's me test for press and only draw button once
            // this avoids flickering. however you must implement the
            // ProcessButtonPress(YourMenuObject); this function has drawRow built in it

            EditMenuOption = ProcessButtonPress(PlaybackMenu);

        }
    }
}

void ProcessLibraryMenu() {

    int EditMenuOption = 1;

   
    // blank out the screen
    //Display.fillScreen(MENU_BACKGROUND);

     while(true){
        File root = SD.open("/");
        File track = root.openNextFile();
        if (! track) {
            Serial.println("** no more files **");
            break;
        }
        Display.print(track.name());
        Display.println();
        //char name = track.name();
        //char * name = "abc";
        //name = track.name();
        //return name;
        track.close();
    }
    
    // draw the main menu
    //LibraryMenu.draw();

    while (EditMenuOption != 0) {

        delay(100);

        if (Touch.touched()) {

            ProcessTouch();

            EditMenuOption = ProcessButtonPress(LibraryMenu);

        }
    }
     

}


void ProcessTouch() {

    TouchPoint = Touch.getPoint();

    BtnX = TouchPoint.x;
    BtnY = TouchPoint.y;

    // consistency between displays is a mess...
    // this is some debug code to help show
    // where you pressed and the resulting map

    // Serial.print("real: ");
    // Serial.print(BtnX);
    // Serial.print(",");
    // Serial.print(BtnY);
    // Display.drawPixel(BtnX, BtnY, C_RED);

    // different values depending on where Touch happened

    // x  = map(x, real left, real right, 0, width-1);
    // y  = map(y, real bottom, real top, 0, height-1);

    // tft with black headers, yellow headers will be different
    BtnX  = map(BtnX, 3700, 300, 319, 0);
    BtnY  = map(BtnY, 3800, 280, 239, 0);

    // Serial.print(", Mapped: ");
    // Serial.print(BtnX);
    // Serial.print(",");
    // Serial.println(BtnY);
    // Display.fillCircle(BtnX, BtnY, 3, ILI9341_RED);
    // delay(5);

}

int ProcessButtonPress(ItemMenu &TheMenu) {
    int TheItem = 0, NewItem = 0;

    TheItem = TheMenu.press(BtnX, BtnY);

    TheMenu.drawRow(TheItem, BUTTON_PRESSED);

    while (Touch.touched()) {
        ProcessTouch();
        NewItem = TheMenu.press(BtnX, BtnY);
        if (TheItem != NewItem) {
            TheMenu.drawRow(TheItem, BUTTON_NOTPRESSED);
            return -1;
        }
    }
    if (TheItem == NewItem) {
        TheMenu.drawRow(TheItem, BUTTON_NOTPRESSED);
        return TheItem;
    }
    return -1;
}

int ProcessButtonPress(EditMenu &TheMenu) {

    int TheItem = 0, NewItem = 0;

    TheItem = TheMenu.press(BtnX, BtnY);

    if (TheItem == -1) {
        return -1;
    }

    TheMenu.drawRow(TheItem);

    while (Touch.touched()) {
        delay(50);
        ProcessTouch();
        NewItem = TheMenu.press(BtnX, BtnY);
        //TheMenu.drawRow(NewItem);
        if (TheItem != NewItem) {

            TheMenu.drawRow(TheItem);

            return NewItem;
        }
    }
    if (TheItem == NewItem) {

        TheMenu.drawRow(TheItem);

        return TheItem;
    }
    delay(50);

    return TheItem;
}
