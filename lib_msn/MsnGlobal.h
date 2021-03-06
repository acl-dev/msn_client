#pragma once
#include "lib_msn.h"

class acl::aio_handle;

LIB_MSN_API acl::aio_handle& get_aio_handle(bool use_gui = false);
LIB_MSN_API acl::dns_service& get_dns_service(void);

typedef enum
{
	MSN_CLIENT_CAP_WIN_MOBILE = 0x0000001,
	MSN_CLIENT_CAP_INK_GIF    = 0x0000004,
	MSN_CLIENT_CAP_INK_ISF    = 0x0000008,
	MSN_CLIENT_CAP_VIDEO_CHAT = 0x0000010,
	MSN_CLIENT_CAP_PACKET     = 0x0000020,
	MSN_CLIENT_CAP_MSNMOBILE  = 0x0000040,
	MSN_CLIENT_CAP_MSNDIRECT  = 0x0000080,
	MSN_CLIENT_CAP_WEBMSGR    = 0x0000200,
	MSN_CLIENT_CAP_TGW        = 0x0000800,
	MSN_CLIENT_CAP_SPACE      = 0x0001000,
	MSN_CLIENT_CAP_MCE        = 0x0002000,
	MSN_CLIENT_CAP_DIRECTIM   = 0x0004000,
	MSN_CLIENT_CAP_WINKS      = 0x0008000,
	MSN_CLIENT_CAP_SEARCH     = 0x0010000,
	MSN_CLIENT_CAP_BOT        = 0x0020000,
	MSN_CLIENT_CAP_VOICEIM    = 0x0040000,
	MSN_CLIENT_CAP_SCHANNEL   = 0x0080000,
	MSN_CLIENT_CAP_SIP_INVITE = 0x0100000,
	MSN_CLIENT_CAP_SDRIVE     = 0x0400000,
	MSN_CLIENT_CAP_ONECARE    = 0x1000000,
	MSN_CLIENT_CAP_P2P_TURN   = 0x2000000,
	MSN_CLIENT_CAP_P2P_BOOTSTRAP_VIA_UUN = 0x4000000,
} MsnClientCaps;

typedef enum
{
	MSN_CLIENT_EXT_CAP_RTC_VIDEO = 0x10,
	MSN_CLIENT_EXT_CAP_P2PV2     = 0x20
} MsnClientExtCaps;

typedef enum
{
	MSN_CLIENT_VER_5_0  = 0x00,
	MSN_CLIENT_VER_6_0  = 0x10,     /* MSNC1 */
	MSN_CLIENT_VER_6_1  = 0x20,     /* MSNC2 */
	MSN_CLIENT_VER_6_2  = 0x30,     /* MSNC3 */
	MSN_CLIENT_VER_7_0  = 0x40,     /* MSNC4 */
	MSN_CLIENT_VER_7_5  = 0x50,     /* MSNC5 */
	MSN_CLIENT_VER_8_0  = 0x60,     /* MSNC6 */
	MSN_CLIENT_VER_8_1  = 0x70,     /* MSNC7 */
	MSN_CLIENT_VER_8_5  = 0x80,     /* MSNC8 */
	MSN_CLIENT_VER_9_0  = 0x90,     /* MSNC9 */
	MSN_CLIENT_VER_14_0 = 0xA0      /* MSNC10 */
} MsnClientVerId;

#define MSN_PROTOCOL15
#undef  MSN_PROTOCOL21
#define MSN_PRODUCT_ID		"PROD0120PW!CCV9@"
#define MSN_PROJECT_KEY		"C1BX{V4W}Q3*10SM"
#define MSN_CLIENT_NAME		"MSNMSGR"
#define MSN_BUILD_VER		"15.4.3508.1109"
#define MSN_APPLICATION_ID	"3794391A-4816-4BAC-B34B-6EC7FB5046C6"
#define MSN_CLIENT_BRAND	"MSNMSGR"

#define MSN_CLIENT_ID_VERSION      MSN_CLIENT_VER_9_0
#define MSN_CLIENT_ID_CAPABILITIES (MSN_CLIENT_CAP_PACKET|MSN_CLIENT_CAP_INK_GIF|MSN_CLIENT_CAP_VOICEIM)
#define MSN_CLIENT_ID_EXT_CAPS     (0)
#define MSN_CLIENT_ID \
	((MSN_CLIENT_ID_VERSION << 24) | (MSN_CLIENT_ID_CAPABILITIES))
