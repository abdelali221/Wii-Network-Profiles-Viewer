/*-----------------------------------------------------------------------

    wd.h -- Wireless Driver Implementation.
    This file is part of https://github.com/abdelali221/libWD

    Copyright:
    
        - (C) 2025 - 2026 Abdelali221 (Author)
        - (C) 2008 Dolphin Emulator Project (For providing data structures)
        - (C) 2013? tueidj

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

-----------------------------------------------------------------------*/

#ifndef __WD_H_
#define __WD_H_
#include <gccore.h>

#define SSID_LENGTH 32
#define BSSID_LENGTH 6

enum WDIOCTLV
{
    IOCTLV_WD_INVALID = 0x1000,
    IOCTLV_WD_GET_MODE = 0x1001,          // WD_GetMode
    IOCTLV_WD_SET_LINKSTATE = 0x1002,     // WD_SetLinkState
    IOCTLV_WD_GET_LINKSTATE = 0x1003,     // WD_GetLinkState
    IOCTLV_WD_SET_CONFIG = 0x1004,        // WD_SetConfig
    IOCTLV_WD_GET_CONFIG = 0x1005,        // WD_GetConfig
    IOCTLV_WD_CHANGE_BEACON = 0x1006,     // WD_ChangeBeacon
    IOCTLV_WD_DISASSOC = 0x1007,          // WD_DisAssoc
    IOCTLV_WD_MP_SEND_FRAME = 0x1008,     // WD_MpSendFrame
    IOCTLV_WD_SEND_FRAME = 0x1009,        // WD_SendFrame
    IOCTLV_WD_SCAN = 0x100a,              // WD_Scan
    IOCTLV_WD_CALL_WL = 0x100c,           // WD_CallWL
    IOCTLV_WD_MEASURE_CHANNEL = 0x100b,   // WD_MeasureChannel
    IOCTLV_WD_GET_LASTERROR = 0x100d,     // WD_GetLastError
    IOCTLV_WD_GET_INFO = 0x100e,          // WD_GetInfo
    IOCTLV_WD_CHANGE_GAMEINFO = 0x100f,   // WD_ChangeGameInfo
    IOCTLV_WD_CHANGE_VTSF = 0x1010,       // WD_ChangeVTSF
    IOCTLV_WD_RECV_FRAME = 0x8000,        // WD_ReceiveFrame
    IOCTLV_WD_RECV_NOTIFICATION = 0x8001  // WD_ReceiveNotification
};

// Error Codes :

#define WD_SUCCESS 0
#define WD_UINITIALIZED -1
#define WD_INVALIDBUF -2
#define WD_BUFFTOOSMALL -3
#define WD_NOTFOUND -4

// Capability flags :

#define CAPAB_SECURED_FLAG 0x10

// Information Elements IDs :

#define IEID_SSID 0x0
#define IEID_COUNTRY 0x7
#define IEID_SECURITY_RSN 0x30
#define IEID_VENDORSPECIFIC 0xDD

// OUI (Organization Unified ID) :

#define OUI_WPA 0x0050F201

// Signal Strength :

#define WD_SIGNAL_STRONG 3
#define WD_SIGNAL_NORMAL 2
#define WD_SIGNAL_FAIR 1
#define WD_SIGNAL_WEAK 0

// WD Modes :

enum MODES
{
    Unintialized = 0,
    DSCommunications = 1,
    NA0 = 2,
    AOSSAPScan = 3,
    NA1 = 4,
    NA2 = 5,
    NA3 = 6
};

// WD Information :

typedef struct WL_Info
{
    u32 vid;
    u32 pid;
    u32 unk2;
    u32 unk3;
    u32 unk4;
    u32 unk5;
    u32 unk6;
    u32 unk7;
    u32 drv_ver;
    u32 unk8;
    u32 unk9;
    u32 pid_2;
} WL_Info;

typedef struct WDInfo
{
    u8 MAC[6];    
    u16 EnableChannelsMask;
    u16 NTRallowedChannelsMask;
    u8 CountryCode[4];
    u8 channel;
    u8 initialized;
    u8 version[80];
    WL_Info wl_inf;
} WDInfo;

// Scan parameters :

typedef struct ScanParameters
{
    u16         ChannelBitmap;
    u16         MaxChannelTime;
    u8          BSSID[BSSID_LENGTH];
    u16         ScanType;
    
    u16         SSIDLength;
    u8          SSID[SSID_LENGTH];
    u8          SSIDMatchMask[6];

} ScanParameters;

// BSS Descriptor :

typedef struct BSSDescriptor
{
    u16         length;
    u16         RSSI;
    u8          BSSID[6];
    u16         SSIDLength;
    u8          SSID[32];
    u16         Capabilities;
    struct
    {
        u16     basic;
        u16     support;
    } rateSet;
    u16 beacon_period;
    u16 DTIM_period;
    u16 channel;
    u16 CF_period;
    u16 CF_max_duration;
    u16 IEs_length;
} BSSDescriptor;

// Information Element Header :

typedef struct IE_hdr
{
    u8 ID;
    u8 len;
} IE_hdr;

// Security :

#define WPA_OFFSET 4
#define RSN_OFFSET 0

enum WD_SECURITY
{
    WD_OPEN     = 0x00,
    WD_WEP      = 0x01,
    WD_WPA_TKIP = 0x02,
    WD_WPA2_AES = 0x04,
    WD_WPA_AES = 0x08,
    WD_WPA2_TKIP = 0x10,
};

typedef struct IE_RSN_WPA
{
    u16 Version;

    u32 GDCS; // Group Data Cipher Suite
    
    u16 PCS_Count; // Pairwise Cipher Suite
    
    u16 AKMS_Count; // AKM Suite

    u16 RSN_Capab;

    u16 PMKID_Count;
} IE_RSN_WPA;

typedef struct WD_Privacy//When verified, only the mode and keyId/keyLen are verified. Size 72 bytes.
{
	u16 mode;//0 = None, 1 = WEP40, 2 = WEP104, 3 = invalid mode, 4 = WPA-PSK(TKIP), 5 = WPA2-PSK(AES), 6 = WPA-PSK(AES)
	u16 unk;

	union//mode and keyId/keyLen are verified when flags0 bit 20 is set.
	{
		//mode 0 is none.
		struct//mode 1
		{
			u16 keyId;//Must be less than 4.
            u8 key[4][5];
		}wep40;

		struct//mode 2
		{
			u16 keyId;//Must be less than 4.
            u8 key[4][13];
		}wep104;

		//mode 3 is an invalid mode.
		struct//mode 5/6
		{
			u16 keyLen;//Must be in range 8-64.
            u8 key[64];
		}aes;

		struct//mode 4
		{
			u16 keyLen;//Must be in range 8-64.
            u8 key[64];
		}tkip;
	};

	u8 unkpriva1[2];
} WD_Privacy;

// WD_Config

typedef struct WD_Config//size 384 bytes.
{
	u16 diversityMode;    //flags0 bit 0. Antenna diversity. Can't be greater than 1. Debug for 0: "OFF, use a antenna MAIN", debug for 1: "OFF, use a antenna SUB".
	u16 useAntenna;    //flags0 bit 1. Must be less than 2; 0 or 1.
	u16 shortRetryLimit;    //flags0 bit 2. Must be no larger than 255.
	u16 longRetryLimit;    //flags0 bit 3. Must be no larger than 255.
	u16 unk4;    //flags0 bit 4.
	u16 rtsThreshold;    //flags0 bit 5. Must be greater than 0.
	u16 fragThreshold;    //flags0 bit 6. Must be greater than 0.
	u16 supportRateSet;    //flags0 bit 7. First 20 bits must not be all zero.
	u16 basicRateSet;    //flags0 bit 8. First 20 bits must not be all zero.
	u16 enable_channel;    //? flags0 bit 9. Current channel?

        struct
        {
	        WD_Privacy essSta_privacy;    //flags0 bit 20.

	        char ssid[32];    //? flags0 bit 16.
	        u8 unka2[32];    //flags0 bit 17.
	        u8 ssidLength;    //flags0 bit 18. must be less than 33.
	        u8 unka;    //flags0 bit 21, not checked.
	        u16 maxChannelTime;    //flags0 bit 19. Must not be less than 1000.
	        u8 bssid[6];    //?
	        u8 somemac[6];    //?
        } essSta;    //Infrastructure?

        struct
        {
	        u16 connectionTimeout;    //flags 1 fields start here. Must be larger than 0.
	        u16 beaconPeriod;    //Must be greater than 1000.
	        u8 maxNodes;    //Must be less than 16.
	        u8 authAlgorithm;    //Must be less than 2. 0 = Open, 1 = Shared.
	        u16 beacon_nintagtimestamp;    //? bit 4 of flags 1 fields.
	        u8 channel;    //Not verified if 0, but when non-zero, must be an available Nitro Allowed Channel. See _wd_info.ntr_allowed_channels.
	        u8 unka4[3];
		u8 beacon_nintagdata[128];
              WD_Privacy mpParent_privacy;    //Mode 3, tkip, and aes crypto modes are removed, leaving only modes none, WEP40, and WEP104.
        } mpParent;    //Master mode?
} WD_Config;

// General Purpose :

s32 NCD_LockWirelessDriver();
u32 NCD_UnlockWirelessDriver(s32 lockid);
int WD_Init(u8 mode);
void WD_Deinit();
int WD_GetInfo(WDInfo* inf);

// AP Scan related : 

u8 WD_GetRadioLevel(BSSDescriptor* Bss);
int WD_Scan(ScanParameters *settings, u8* buff, u16 buffsize);
int WD_ScanOnce(ScanParameters *settings, u8* buff, u16 buffsize);
void WD_SetDefaultScanParameters(ScanParameters* set);

// IE related :

u8 WD_GetNumberOfIEs(BSSDescriptor* Bss);
int WD_GetIELength(BSSDescriptor* Bss, u8 ID);
int WD_GetIE(BSSDescriptor* Bss, u8 ID, u8* buff, u8 buffsize);
int WD_GetIEIDList(BSSDescriptor* Bss, u8* buff, u8 buffsize);
int WD_GetVendorSpecificIELength(BSSDescriptor* Bss, u32 OUI);
int WD_GetVendorSpecificIE(BSSDescriptor* Bss, u32 OUI, u8* buff, u8 buffsize);

// AP Security related :

int WD_GetPCSList(BSSDescriptor *Bss, u8* buff, u8 buffsize, u8 offset);
int WD_GetRSN_WPAEssentials(BSSDescriptor *Bss, IE_RSN_WPA *IE, u8 offset);
u8 WD_GetSecurity(BSSDescriptor *Bss);

#endif