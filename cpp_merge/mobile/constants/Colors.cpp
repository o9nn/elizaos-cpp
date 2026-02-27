#include "Colors.hpp"

string tintColorLight = std:("#0a7ea4");
string tintColorDark = std:("#fff");
object Colors = object{
    object::pair{std:("light"), object{
        object::pair{std:("text"), std:("#11181C")}, 
        object::pair{std:("background"), std:("#fff")}, 
        object::pair{std:("tint"), tintColorLight}, 
        object::pair{std:("icon"), std:("#687076")}, 
        object::pair{std:("tabIconDefault"), std:("#687076")}, 
        object::pair{std:("tabIconSelected"), tintColorLight}
    }}, 
    object::pair{std:("dark"), object{
        object::pair{std:("text"), std:("#ECEDEE")}, 
        object::pair{std:("background"), std:("#151718")}, 
        object::pair{std:("tint"), tintColorDark}, 
        object::pair{std:("icon"), std:("#9BA1A6")}, 
        object::pair{std:("tabIconDefault"), std:("#9BA1A6")}, 
        object::pair{std:("tabIconSelected"), tintColorDark}
    }}
};

void Main(void)
{
}

MAIN
