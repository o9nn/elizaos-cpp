export default class ServerGameProt {
    // interfaces
    static readonly IF_OPENCHAT = new ServerGameProt(7, 2);
    static readonly IF_OPENMAIN_SIDE = new ServerGameProt(229, 4);
    static readonly IF_CLOSE = new ServerGameProt(174, 0);
    static readonly IF_SETTAB = new ServerGameProt(29, 3);
    static readonly IF_SETTAB_ACTIVE = new ServerGameProt(8, 1);
    static readonly IF_OPENMAIN = new ServerGameProt(177, 2);
    static readonly IF_OPENSIDE = new ServerGameProt(236, 2);
    static readonly IF_OPENOVERLAY = new ServerGameProt(115, 2);

    // updating interfaces
    static readonly IF_SETCOLOUR = new ServerGameProt(135, 4); // NXT naming
    static readonly IF_SETHIDE = new ServerGameProt(225, 3); // NXT naming
    static readonly IF_SETOBJECT = new ServerGameProt(153, 6); // NXT naming
    static readonly IF_SETMODEL = new ServerGameProt(60, 4); // NXT naming
    static readonly IF_SETANIM = new ServerGameProt(69, 4); // NXT naming
    static readonly IF_SETPLAYERHEAD = new ServerGameProt(83, 2); // NXT naming
    static readonly IF_SETTEXT = new ServerGameProt(32, -2); // NXT naming
    static readonly IF_SETNPCHEAD = new ServerGameProt(76, 4); // NXT naming
    static readonly IF_SETPOSITION = new ServerGameProt(230, 6); // NXT naming
    static readonly IF_SETSCROLLPOS = new ServerGameProt(226, 4); // NXT naming

    // tutorial area
    static readonly TUT_FLASH = new ServerGameProt(132, 1);
    static readonly TUT_OPEN = new ServerGameProt(152, 2);

    // inventory
    static readonly UPDATE_INV_STOP_TRANSMIT = new ServerGameProt(143, 2); // NXT naming
    static readonly UPDATE_INV_FULL = new ServerGameProt(156, -2); // NXT naming
    static readonly UPDATE_INV_PARTIAL = new ServerGameProt(95, -2); // NXT naming

    // camera control
    static readonly CAM_LOOKAT = new ServerGameProt(123, 6); // NXT naming
    static readonly CAM_SHAKE = new ServerGameProt(103, 4); // NXT naming
    static readonly CAM_MOVETO = new ServerGameProt(86, 6); // NXT naming
    static readonly CAM_RESET = new ServerGameProt(134, 0); // NXT naming

    // entity updates
    static readonly NPC_INFO = new ServerGameProt(105, -2); // NXT naming
    static readonly PLAYER_INFO = new ServerGameProt(161, -2); // NXT naming

    // input tracking
    static readonly FINISH_TRACKING = new ServerGameProt(165, 0);
    static readonly ENABLE_TRACKING = new ServerGameProt(28, 0);

    // social
    static readonly MESSAGE_GAME = new ServerGameProt(175, -1); // NXT naming
    static readonly UPDATE_IGNORELIST = new ServerGameProt(181, -2); // NXT naming
    static readonly CHAT_FILTER_SETTINGS = new ServerGameProt(2, 3); // NXT naming
    static readonly MESSAGE_PRIVATE = new ServerGameProt(207, -1); // NXT naming
    static readonly UPDATE_FRIENDLIST = new ServerGameProt(109, 9); // NXT naming

    // misc
    static readonly UNSET_MAP_FLAG = new ServerGameProt(233, 0); // NXT has "SET_MAP_FLAG" but we cannot control the position
    static readonly UPDATE_RUNWEIGHT = new ServerGameProt(70, 2); // NXT naming
    static readonly HINT_ARROW = new ServerGameProt(243, 6); // NXT naming
    static readonly UPDATE_REBOOT_TIMER = new ServerGameProt(26, 2); // NXT naming
    static readonly UPDATE_STAT = new ServerGameProt(110, 6); // NXT naming
    static readonly UPDATE_RUNENERGY = new ServerGameProt(208, 1); // NXT naming
    static readonly RESET_ANIMS = new ServerGameProt(144, 0); // NXT naming
    static readonly UPDATE_PID = new ServerGameProt(49, 3);
    static readonly LAST_LOGIN_INFO = new ServerGameProt(238, 10); // NXT naming
    static readonly LOGOUT = new ServerGameProt(36, 0); // NXT naming
    static readonly P_COUNTDIALOG = new ServerGameProt(56, 0); // named after runescript command + client resume_p_countdialog packet
    static readonly SET_MULTIWAY = new ServerGameProt(35, 1);

    // maps
    static readonly REBUILD_NORMAL = new ServerGameProt(66, 4); // NXT naming (do we really need _normal if there's no region rebuild?)

    // vars
    static readonly VARP_SMALL = new ServerGameProt(192, 3); // NXT naming
    static readonly VARP_LARGE = new ServerGameProt(75, 6); // NXT naming
    static readonly RESET_CLIENT_VARCACHE = new ServerGameProt(25, 0); // NXT naming

    // audio
    static readonly SYNTH_SOUND = new ServerGameProt(209, 5); // NXT naming
    static readonly MIDI_SONG = new ServerGameProt(96, 2); // NXT naming
    static readonly MIDI_JINGLE = new ServerGameProt(39, 4); // NXT naming

    // zones
    static readonly UPDATE_ZONE_PARTIAL_FOLLOWS = new ServerGameProt(203, 2); // NXT naming
    static readonly UPDATE_ZONE_FULL_FOLLOWS = new ServerGameProt(140, 2); // NXT naming
    static readonly UPDATE_ZONE_PARTIAL_ENCLOSED = new ServerGameProt(15, -2); // NXT naming

    constructor(
        readonly id: number,
        readonly length: number
    ) {}
}
