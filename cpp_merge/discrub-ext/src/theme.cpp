#include "/home/runner/work/elizaos-cpp/elizaos-cpp/discrub-ext/src/theme.h"

string primaryMain = std:("#7289da");
string primaryDark = std:("rgb(79, 95, 152)");
string secondaryMain = std:("#d2d5f7");
string backgroundDefault = std:("#1e2124");
string backgroundPaper = std:("#282b30");
string textPrimary = std:("#ffffff");
string textSecondary = std:("#d2d5f7");
object toolTipStyle = object{
    object::pair{std:("padding"), std:("7px")}, 
    object::pair{std:("background"), primaryDark}, 
    object::pair{std:("fontWeight"), 600}, 
    object::pair{std:("fontSize"), std:("0.8rem")}, 
    object::pair{std:("boxShadow"), std:("4px 2px 5px 1px rgb(0 0 0 / 41%)")}
};
std::shared_ptr<ThemeOptions> theme = createTheme(object{
    object::pair{std:("palette"), object{
        object::pair{std:("mode"), std:("dark")}, 
        object::pair{std:("primary"), object{
            object::pair{std:("main"), primaryMain}
        }}, 
        object::pair{std:("secondary"), object{
            object::pair{std:("main"), secondaryMain}
        }}, 
        object::pair{std:("background"), object{
            object::pair{std:("default"), backgroundDefault}, 
            object::pair{std:("paper"), backgroundPaper}
        }}, 
        object::pair{std:("text"), object{
            object::pair{std:("primary"), textPrimary}, 
            object::pair{std:("secondary"), textSecondary}
        }}, 
        object::pair{std:("error"), object{
            object::pair{std:("main"), std:("#fa777c")}
        }}, 
        object::pair{std:("warning"), object{
            object::pair{std:("main"), std:("#faa61a")}
        }}, 
        object::pair{std:("info"), object{
            object::pair{std:("main"), std:("#7289da")}
        }}, 
        object::pair{std:("success"), object{
            object::pair{std:("main"), std:("#3ba55c")}
        }}
    }}, 
    object::pair{std:("components"), object{
        object::pair{std:("MuiButton"), object{
            object::pair{std:("styleOverrides"), object{
                object::pair{std:("root"), object{
                    object::pair{std:("textTransform"), std:("none")}
                }}
            }}
        }}, 
        object::pair{std:("MuiTooltip"), object{
            object::pair{std:("styleOverrides"), object{
                object::pair{std:("tooltipPlacementBottom"), toolTipStyle}, 
                object::pair{std:("tooltipPlacementTop"), toolTipStyle}, 
                object::pair{std:("tooltipPlacementLeft"), toolTipStyle}, 
                object::pair{std:("tooltipPlacementRight"), toolTipStyle}, 
                object::pair{std:("tooltipArrow"), toolTipStyle}, 
                object::pair{std:("arrow"), object{
                    object::pair{std:("color"), primaryMain}
                }}
            }}
        }}, 
        object::pair{std:("MuiTypography"), object{
            object::pair{std:("styleOverrides"), object{
                object::pair{std:("root"), object{
                    object::pair{std:("cursor"), std:("default")}
                }}
            }}
        }}
    }}
});
object scrollbarOverrides = object{
    object::pair{std:("&::-webkit-scrollbar"), object{
        object::pair{std:("width"), std:("5px")}
    }}, 
    object::pair{std:("&::-webkit-scrollbar-thumb"), object{
        object::pair{std:("background"), std:("#f1f1f1")}
    }}, 
    object::pair{std:("&::-webkit-scrollbar-track"), object{
        object::pair{std:("background"), std:("#888")}
    }}
};
object audioOverrides = object{
    object::pair{std:("audio::-webkit-media-controls-panel"), object{
        object::pair{std:("backgroundColor"), backgroundPaper}, 
        object::pair{std:("borderRadius"), 0}
    }}, 
    object::pair{std:("audio::-webkit-media-controls-enclosure"), object{
        object::pair{std:("backgroundColor"), backgroundPaper}
    }}, 
    object::pair{std:("audio::-webkit-media-controls-mute-button"), object{
        object::pair{std:("backgroundColor"), secondaryMain}, 
        object::pair{std:("borderRadius"), std:("50%")}
    }}, 
    object::pair{std:("audio::-webkit-media-controls-play-button "), object{
        object::pair{std:("backgroundColor"), primaryMain}, 
        object::pair{std:("borderRadius"), std:("50%")}
    }}, 
    object::pair{std:("audio::-webkit-media-controls-play-button:hover"), object{
        object::pair{std:("backgroundColor"), primaryDark}
    }}, 
    object::pair{std:("audio::-webkit-media-controls-current-time-display"), object{
        object::pair{std:("color"), secondaryMain}, 
        object::pair{std:("textShadow"), std:("none")}
    }}, 
    object::pair{std:("audio::-webkit-media-controls-time-remaining-display"), object{
        object::pair{std:("color"), secondaryMain}, 
        object::pair{std:("textShadow"), std:("none")}
    }}, 
    object::pair{std:("audio::-webkit-media-controls-timeline"), object{
        object::pair{std:("backgroundColor"), secondaryMain}, 
        object::pair{std:("borderRadius"), std:("25px")}, 
        object::pair{std:("marginLeft"), std:("10px")}, 
        object::pair{std:("marginRight"), std:("10px")}
    }}, 
    object::pair{std:("audio::-webkit-media-controls-volume-slider "), object{
        object::pair{std:("backgroundColor"), secondaryMain}, 
        object::pair{std:("borderRadius"), std:("25px")}, 
        object::pair{std:("paddingLeft"), std:("8px")}, 
        object::pair{std:("paddingRight"), std:("8px")}
    }}
};

void Main(void)
{
}

MAIN
