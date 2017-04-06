#include <stdio.h>
#include <string.h>
#include <ogcsys.h>

#define IOCTL_DI_READID					0x70
#define IOCTL_DI_READ					0x71
#define IOCTL_DI_WAITCVRCLOSE			0x79
#define IOCTL_DI_GETCOVER				0x88
#define IOCTL_DI_RESET					0x8A
#define IOCTL_DI_OPENPART				0x8B
#define IOCTL_DI_CLOSEPART				0x8C
#define IOCTL_DI_UNENCREAD				0x8D
#define IOCTL_DI_SEEK					0xAB
#define IOCTL_DI_STOPLASER				0xD2
#define IOCTL_DI_OFFSET					0xD9
#define IOCTL_DI_DISC_BCA				0xDA
#define IOCTL_DI_REQUESTERROR			0xE0
#define IOCTL_DI_STOPMOTOR				0xE3
#define IOCTL_DI_DVDAUDIOBUFFERCFG		0xE4
#define IOCTL_DI_SETWBFSMODE			0xF4
#define IOCTL_DI_SETFRAG				0xF9
#define IOCTL_DI_GETMODE				0xFA
#define IOCTL_DI_HELLO					0xFB

static u32 inbuf[8]  ATTRIBUTE_ALIGN(32);
static u32 outbuf[8] ATTRIBUTE_ALIGN(32);

static const char di_fs[] ATTRIBUTE_ALIGN(32) = "/dev/di";
static s32 di_fd = -1;

s32 WDVD_Init(void)
{
	/* Open "/dev/di" */
	if(di_fd < 0)
	{
		di_fd = IOS_Open(di_fs, 0);
		printf("wdvd_WDVD_Init: di_fd value = %d\n", di_fd);
		if (di_fd < 0) 
			return di_fd;
	}
	return 0;
}

s32 WDVD_Close(void)
{
	/* Close "/dev/di" */
	if(di_fd >= 0)
	{
		IOS_Close(di_fd);
		di_fd = -1;
	}
	return 0;
}

s32 WDVD_Reset(void)
{
	memset(inbuf, 0, sizeof(inbuf));

	/* Reset drive */
	inbuf[0] = IOCTL_DI_RESET << 24;
	inbuf[1] = 1;

	s32 ret = IOS_Ioctl(di_fd, IOCTL_DI_RESET, inbuf, sizeof(inbuf), outbuf, sizeof(outbuf));
	if(ret < 0)
		return ret;
	return (ret == 1) ? 0 : -ret;
}

s32 WDVD_Seek(u32 offset)
{
	memset(inbuf, 0, sizeof(inbuf));

	/* Drive seek */
	inbuf[0] = IOCTL_DI_SEEK << 24;
	inbuf[1] = offset;

	s32 ret = IOS_Ioctl(di_fd, IOCTL_DI_SEEK, inbuf, sizeof(inbuf), outbuf, sizeof(outbuf));
	if(ret < 0)
		return ret;
	return (ret == 1) ? 0 : -ret;
}

s32 WDVD_ReadDiskId(void *id)
{
	memset(inbuf, 0, sizeof(inbuf));

	/* Read disc ID */
	inbuf[0] = IOCTL_DI_READID << 24;

	s32 ret = IOS_Ioctl(di_fd, IOCTL_DI_READID, inbuf, sizeof(inbuf), outbuf, sizeof(outbuf));
	if(ret < 0)
		return ret;
	memcpy(id, outbuf, sizeof(dvddiskid));
	return (ret == 1) ? 0 : -ret;
}

s32 WDVD_Read(void *buf, u32 len, u32 offset)
{
	memset(inbuf, 0, sizeof(inbuf));

	/* Disc read */
	inbuf[0] = IOCTL_DI_READ << 24;
	inbuf[1] = len;
	inbuf[2] = offset;

	s32 ret = IOS_Ioctl(di_fd, IOCTL_DI_READ, inbuf, sizeof(inbuf), buf, len);
	if(ret < 0)
		return ret;
	return (ret == 1) ? 0 : -ret;
}

s32 WDVD_UnencryptedRead(void *buf, u32 len, u32 offset)
{
	memset(inbuf, 0, sizeof(inbuf));

	/* Unencrypted read */
	inbuf[0] = IOCTL_DI_UNENCREAD << 24;
	inbuf[1] = len;
	inbuf[2] = offset;

	s32 ret = IOS_Ioctl(di_fd, IOCTL_DI_UNENCREAD, inbuf, sizeof(inbuf), buf, len);
	if(ret < 0)
		return ret;
	return (ret == 1) ? 0 : -ret;
}

s32 WDVD_OpenPartition(u32 offset, u32 *IOS)
{
	static u8 Tmd_Buffer[0x4A00] ATTRIBUTE_ALIGN(32);
	static ioctlv Vectors[5] ATTRIBUTE_ALIGN(32);
	s32 ret;

	memset(inbuf, 0, sizeof inbuf);
	memset(outbuf, 0, sizeof outbuf);

	inbuf[0] = IOCTL_DI_OPENPART << 24;
	inbuf[1] = offset;

	Vectors[0].data		= inbuf;
	Vectors[0].len		= 0x20;
	Vectors[1].data		= 0;
	Vectors[1].len		= 0;
	Vectors[2].data		= 0;
	Vectors[2].len		= 0;
	Vectors[3].data		= Tmd_Buffer;
	Vectors[3].len		= 0x49e4;
	Vectors[4].data		= outbuf;
	Vectors[4].len		= 0x20;

	ret = IOS_Ioctlv(di_fd, IOCTL_DI_OPENPART, 3, 2, (ioctlv *)Vectors);
	if(ret < 0)
		return ret;
	*IOS = (u32)(Tmd_Buffer[0x18b]);
	return (ret == 1) ? 0 : -ret;
}
