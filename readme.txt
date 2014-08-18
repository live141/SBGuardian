/******************************** SB Guardian 2.0b **************************


/******************************** Installation HL1 ***********************************
**********************************************************************************
/* Using SB Guardian without MetaMod:
/* open liblist.gam in cstrike
/* LINUX: change	gamedll_linux "dlls/cs_i386.so"		to	gamedll_linux "dlls/sbguardian_i386.so"
/* WINDOWS: change 	gamedll "dlls\mp.dll"	to	gamedll "dlls\sbguardian.dll"
/* save and close liblist.gam
/* copy the binary to <gamefolder>/dlls
/* The Server needs a full shutdown and start in order to load the sbguardian binary
/*********************************************************************************
/* Using SB Guardian with MetaMod:
/* open cstrike/addons/metamod/plugins.ini
/* LINUX: add the line:		linux addons/sbguardian/sbguardian_i386.so
/* WINDOWS: add the line: 	win addons\sbguardian\sbguardian.dll
/* copy the binary to <gamefolder>/addons/sbguardian
/* restart the server
/*********************************************************************************
/******************************* Installation Source (CSS, TF2, DODS) ************
/* Using SB Guardian with MetaMod:
/* copy hl2/<gamefolder>/addons to your server's cstrike folder
/* rename hl2/<gamefolder>/addons/metamod/sbguardian_mm.xvdf to sbguardian_mm.vdf
/* restart the server
/*********************************************************************************
/******************************* Installation Source (CSS, TF2, DODS) ************
/* Using SB Guardian as a VSP Plugin:
/* copy hl2/<gamefolder>/addons to your server's cstrike folder
/* rename hl2/<gamefolder>/addons/sbguardian_vsp.xvdf to sbguardian_vsp.vdf
/* restart the server
/************************************** FEATURES *********************************

/*********************************************************************************
/******************** CVars ******************************************************
/******************** sbg.cfg is located in **************************************
/* HL1: <gamefolder>/sbg.cfg *****************************************************
/* HL2: <gamefolder>/cfg/sbg.cfg *************************************************
/*********************************************************************************
/* sbgguardian wh 1/0	- turns WHBlock on/off
/* sbguardian cvar 1/0	- turns CVarCheck/Block on/off
/* sbguardian flash 1/0	- turns NoFlashBlock[only FULLFlash] on/off
/* sbguardian fov 1/0	- turns fovcheck on/off (increases performance of whblock, block spinbots and blocks aimbotter from turning around with aimkey)
/* sbguardian updatesystem 1/0	- turns updatenotification and autoupdating on/off (autoupdating will not work for hl1 standalone, but the notification will work)
/* sbguardian ab 1/0	- turns Aimbtodetection  on/off (1 = kick and log, 2 = ban and log, 3 = log only) 
/*********************************************************************************
/*********************************************************************************
/****************************** Register CVars for CVarCheck *********************
/* sbguardian cvar add CVARNAME VALUE MODUS ACTION
/* MODUS:	equal - the cvar needs to be that value
/*		notequal - if the client's cvar is not the defined value
/*		bigger - if the client's cvar is bigger than the defined one
/*		smaller - if the client's cvar is smaller than the defined one
/*		biggerequal - if the client's cvar is bigger/equal than/to the defined one
/*		smallerequal - if the client's cvar is smaller/equal than/to the defined one
/*
/*
/* ACTION:	print - just prints to hud
/*		kick - kicks the player and logs
/*		ban - bans the player and logs
/*********************************************************************************
/* example:
/* sbguardian cvar add ex_interp 0.08 bigger kick
/* If now someone joins with ex_interp bigger than 0.08, he will be kicked.
/*********************************************************************************
/* sbgurdian cvar list - list all blocked cvars
/*********************************************************************************
/* sbgurdian cvar remove <NAME OF CVAR> - removes a blocked cvar by cvarname
/*********************************************************************************
