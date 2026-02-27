#include "/home/runner/work/elizaos-cpp/elizaos-cpp/discrub-ext/src/theme.h"

string primaryMain = std::string("#7289da");
string primaryDark = std::string("rgb(79, 95, 152)");
string secondaryMain = std::string("#d2d5f7");
string backgroundDefault = std::string("#1e2124");
string backgroundPaper = std::string("#282b30");
string textPrimary = std::string("#ffffff");
string textSecondary = std::string("#d2d5f7");
object toolTipStyle = object{
    object::pair{std::string("padding"), std::string("7px")}, 
    object::pair{std::string("background"), primaryDark}, 
    object::pair{std::string("fontWeight"), 600}, 
    object::pair{std::string("fontSize"), std::string("0.8rem")}, 
    object::pair{std::string("boxShadow"), std::string("4px 2px 5px 1px rgb(0 0 0 / 41%)")}
};
std::shared_ptr<ThemeOptions> theme = createTheme(object{
    object::pair{std::string("palette"), object{
        object::pair{std::string("mode"), std::string("dark")}, 
        object::pair{std::string("primary"), object{
            object::pair{std::string("main"), primaryMain}
        }}, 
        object::pair{std::string("secondary"), object{
            object::pair{std::string("main"), secondaryMain}
        }}, 
        object::pair{std::string("background"), object{
            object::pair{std::string("default"), backgroundDefault}, 
            object::pair{std::string("paper"), backgroundPaper}
        }}, 
        object::pair{std::string("text"), object{
            object::pair{std::string("primary"), textPrimary}, 
            object::pair{std::string("secondary"), textSecondary}
        }}, 
        object::pair{std::string("error"), object{
            object::pair{std::string("main"), std::string("#fa777c")}
        }}, 
        object::pair{std::string("warning"), object{
            object::pair{std::string("main"), std::string("#faa61a")}
        }}, 
        object::pair{std::string("info"), object{
            object::pair{std::string("main"), std::string("#7289da")}
        }}, 
        object::pair{std::string("success"), object{
            object::pair{std::string("main"), std::string("#3ba55c")}
        }}
    }}, 
    object::pair{std::string("components"), object{
        object::pair{std::string("MuiButton"), object{
            object::pair{std::string("styleOverrides"), object{
                object::pair{std::string("root"), object{
                    object::pair{std::string("textTransform"), std::string("none")}
                }}
            }}
        }}, 
        object::pair{std::string("MuiTooltip"), object{
            object::pair{std::string("styleOverrides"), object{
                object::pair{std::string("tooltipPlacementBottom"), toolTipStyle}, 
                object::pair{std::string("tooltipPlacementTop"), toolTipStyle}, 
                object::pair{std::string("tooltipPlacementLeft"), toolTipStyle}, 
                object::pair{std::string("tooltipPlacementRight"), toolTipStyle}, 
                object::pair{std::string("tooltipArrow"), toolTipStyle}, 
                object::pair{std::string("arrow"), object{
                    object::pair{std::string("color"), primaryMain}
                }}
            }}
        }}, 
        object::pair{std::string("MuiTypography"), object{
            object::pair{std::string("styleOverrides"), object{
                object::pair{std::string("root"), object{
                    object::pair{std::string("cursor"), std::string("default")}
                }}
            }}
        }}
    }}
});
object scrollbarOverrides = object{
    object::pair{std::string("&::-webkit-scrollbar"), object{
        object::pair{std::string("width"), std::string("5px")}
    }}, 
    object::pair{std::string("&::-webkit-scrollbar-thumb"), object{
        object::pair{std::string("background"), std::string("#f1f1f1")}
    }}, 
    object::pair{std::string("&::-webkit-scrollbar-track"), object{
        object::pair{std::string("background"), std::string("#888")}
    }}
};
object audioOverrides = object{
    object::pair{std::string("audio::-webkit-media-controls-panel"), object{
        object::pair{std::string("backgroundColor"), backgroundPaper}, 
        object::pair{std::string("borderRadius"), 0}
    }}, 
    object::pair{std::string("audio::-webkit-media-controls-enclosure"), object{
        object::pair{std::string("backgroundColor"), backgroundPaper}
    }}, 
    object::pair{std::string("audio::-webkit-media-controls-mute-button"), object{
        object::pair{std::string("backgroundColor"), secondaryMain}, 
        object::pair{std::string("borderRadius"), std::string("50%")}
    }}, 
    object::pair{std::string("audio::-webkit-media-controls-play-button "), object{
        object::pair{std::string("backgroundColor"), primaryMain}, 
        object::pair{std::string("borderRadius"), std::string("50%")}
    }}, 
    object::pair{std::string("audio::-webkit-media-controls-play-button:hover"), object{
        object::pair{std::string("backgroundColor"), primaryDark}
    }}, 
    object::pair{std::string("audio::-webkit-media-controls-current-time-display"), object{
        object::pair{std::string("color"), secondaryMain}, 
        object::pair{std::string("textShadow"), std::string("none")}
    }}, 
    object::pair{std::string("audio::-webkit-media-controls-time-remaining-display"), object{
        object::pair{std::string("color"), secondaryMain}, 
        object::pair{std::string("textShadow"), std::string("none")}
    }}, 
    object::pair{std::string("audio::-webkit-media-controls-timeline"), object{
        object::pair{std::string("backgroundColor"), secondaryMain}, 
        object::pair{std::string("borderRadius"), std::string("25px")}, 
        object::pair{std::string("marginLeft"), std::string("10px")}, 
        object::pair{std::string("marginRight"), std::string("10px")}
    }}, 
    object::pair{std::string("audio::-webkit-media-controls-volume-slider "), object{
        object::pair{std::string("backgroundColor"), secondaryMain}, 
        object::pair{std::string("borderRadius"), std::string("25px")}, 
        object::pair{std::string("paddingLeft"), std::string("8px")}, 
        object::pair{std::string("paddingRight"), std::string("8px")}
    }}
};

void Main(void)
{
}

MAIN
