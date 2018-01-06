/* Copyright (c) 20XX 9front
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
/*
 * mass storage transport protocols and subclasses,
 * from usb mass storage class specification overview rev 1.2
 */

typedef struct Umsc Umsc;
typedef struct Ums Ums;
typedef struct Cbw Cbw;			/* command block wrapper */
typedef struct Csw Csw;			/* command status wrapper */
typedef struct Part Part;

enum
{
	Protocbi =	0,	/* control/bulk/interrupt; mainly floppies */
	Protocb =	1,	/*   "  with no interrupt; mainly floppies */
	Protobulk =	0x50,	/* bulk only */

	Subrbc =	1,	/* reduced blk cmds */
	Subatapi =	2,	/* cd/dvd using sff-8020i or mmc-2 cmd blks */
	Subqic 	=	3,	/* QIC-157 tapes */
	Subufi =	4,	/* floppy */
	Sub8070 =	5,	/* removable media, atapi-like */
	Subscsi =	6,	/* scsi transparent cmd set */
	Subisd200 =	7,	/* ISD200 ATA */
	Subdev =	0xff,	/* use device's value */

	Umsreset =	0xFF,
	Getmaxlun =	0xFE,

//	Maxlun		= 256,
	Maxlun		= 32,

	CMreset = 1,

	Pcmd = 0,
	Pdata,
	Pstatus,

	CbwLen		= 31,
	CbwDataIn	= 0x80,
	CbwDataOut	= 0x00,
	CswLen		= 13,
	CswOk		= 0,
	CswFailed	= 1,
	CswPhaseErr	= 2,
	
	Maxparts		= 16,
};

/*
 * corresponds to a lun.
 * these are ~600+Maxiosize bytes each; ScsiReq is not tiny.
 */

struct Part
{
	int id;
	int inuse;
	int vers;
	uint32_t mode;
	char	*name;
	int64_t offset;		/* in lbsize units */
	int64_t length;		/* in lbsize units */
};


struct Umsc
{
	ScsiReq;
	char name[40];
	uint64_t	blocks;
	int64_t	capacity;

	/* from setup */
	char	*bufp;
	int32_t	off;		/* offset within a block */
	int32_t	nb;		/* byte count */

	QLock	ql;

	/* partitions */
	Part part[Maxparts];

	uint8_t 	rawcmd[16];
	uint8_t	phase;
	char	*inq;
	Ums	*ums;
	char	buf[Maxiosize];
};

struct Ums
{
	Dev	*epin;
	Dev	*epout;
	Umsc	*lun;
	uint8_t	maxlun;
	int	seq;
	int	nerrs;
	int	wrongresidues;
};

/*
 * USB transparent SCSI devices
 */
struct Cbw
{
	char	signature[4];		/* "USBC" */
	int32_t	tag;
	int32_t	datalen;
	uint8_t	flags;
	uint8_t	lun;
	uint8_t	len;
	char	command[16];
};

struct Csw
{
	char	signature[4];		/* "USBS" */
	int32_t	tag;
	int32_t	dataresidue;
	uint8_t	status;
};
