#include "/home/runner/work/elizaos-cpp/elizaos-cpp/mobile/constants/Colors.h"

string tintColorLight = std::string("#0a7ea4");
string tintColorDark = std::string("#fff");
object Colors = object{
    object::pair{std::string("light"), object{
        object::pair{std::string("text"), std::string("#11181C")}, 
        object::pair{std::string("background"), std::string("#fff")}, 
        object::pair{std::string("tint"), tintColorLight}, 
        object::pair{std::string("icon"), std::string("#687076")}, 
        object::pair{std::string("tabIconDefault"), std::string("#687076")}, 
        object::pair{std::string("tabIconSelected"), tintColorLight}
    }}, 
    object::pair{std::string("dark"), object{
        object::pair{std::string("text"), std::string("#ECEDEE")}, 
        object::pair{std::string("background"), std::string("#151718")}, 
        object::pair{std::string("tint"), tintColorDark}, 
        object::pair{std::string("icon"), std::string("#9BA1A6")}, 
        object::pair{std::string("tabIconDefault"), std::string("#9BA1A6")}, 
        object::pair{std::string("tabIconSelected"), tintColorDark}
    }}
};

void Main(void)
{
}

MAIN
