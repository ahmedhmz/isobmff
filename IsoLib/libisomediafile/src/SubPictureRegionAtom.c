/*
* This software module was originally developed by InterDigital, Inc.
* in the course of development of MPEG-4.
* This software module is an implementation of a part of one or
* more MPEG-4 tools as specified by MPEG-4.
* ISO/IEC gives users of MPEG-4 free license to this
* software module or modifications thereof for use in hardware
* or software products claiming conformance to MPEG-4 only for evaluation and testing purposes.
* Those intending to use this software module in hardware or software
* products are advised that its use may infringe existing patents.
* The original developer of this software module and his/her company,
* the subsequent editors and their companies, and ISO/IEC have no
* liability for use of this software module or modifications thereof
* in an implementation.
*
* Copyright is not released for non MPEG-4 conforming
* products. InterDigital, Inc. retains full right to use the code for its own
* purpose, assign or donate the code to a third party and to
* inhibit third parties from using the code for non
* MPEG-4 conforming products.
*
* This copyright notice must be included in all copies or
* derivative works.
*/


/**
 * @file SubPictureRegionAtom.c
 * @author Ahmed Hamza <Ahmed.Hamza@InterDigital.com>
 * @date December 30, 2018
 * @brief Implements functions for reading and writing SubPictureRegionAtom instances.
 */


#include "MP4Atoms.h"
#include <stdlib.h>


static void destroy(MP4AtomPtr s)
{
	MP4Err err;
	MP4SubPictureRegionAtomPtr self;

	self = (MP4SubPictureRegionAtomPtr)s;
	if (self == NULL) 
		BAILWITHERROR(MP4BadParamErr);
		

	if (self->super)
		self->super->destroy(s);

bail:
	TEST_RETURN(err);

	return;
}


static MP4Err serialize(struct MP4Atom* s, char* buffer)
{

	MP4Err err;
    u16 tmp16;
	MP4SubPictureRegionAtomPtr self = (MP4SubPictureRegionAtomPtr)s;
	err = MP4NoErr;

	if (self->size > 0) {

		err = MP4SerializeCommonFullAtomFields((MP4FullAtomPtr)self, buffer); if (err) goto bail;
		buffer += self->bytesWritten;

        PUT16(objectX);
        PUT16(objectY);
        PUT16(objectWidth);
        PUT16(objectHeight);
        tmp16 = (self->reserved << 2) + (self->trackNotAloneFlag << 1) + self->trackNotMergeableFlag;
        PUT16_V(tmp16);

		assert(self->bytesWritten == self->size);

	}

bail:
	TEST_RETURN(err);

	return err;
}



static MP4Err calculateSize(struct MP4Atom* s)
{
	MP4Err err;
	MP4SubPictureRegionAtomPtr self = (MP4SubPictureRegionAtomPtr)s;
	err = MP4NoErr;

	err = MP4CalculateFullAtomFieldSize((MP4FullAtomPtr)s); if (err) goto bail;

	self->size += (5 * 2);

bail:
	TEST_RETURN(err);

	return err;
}


static MP4Err createFromInputStream(MP4AtomPtr s, MP4AtomPtr proto, MP4InputStreamPtr inputStream)
{
	MP4Err err;
	u16 tmp16;

	MP4SubPictureRegionAtomPtr self = (MP4SubPictureRegionAtomPtr)s;

	err = MP4NoErr;
	if (self == NULL)	
		BAILWITHERROR(MP4BadParamErr);
		
	err = self->super->createFromInputStream(s, proto, (char*)inputStream);

    GET16(objectX);
    GET16(objectY);
    GET16(objectWidth);
    GET16(objectHeight);
	GET16_V(tmp16);
	self->reserved = (tmp16 >> 2) & 0x3FFF;
    self->trackNotAloneFlag = (u8)((tmp16 >> 1) & 0x1);
	self->trackNotMergeableFlag = (u8)(tmp16 & 0x1);

	assert(self->bytesRead == self->size);
bail:
	TEST_RETURN(err);

	return err;
}



MP4Err MP4CreateSubPictureRegionAtom(MP4SubPictureRegionAtomPtr *outAtom)
{
	MP4Err err;
	MP4SubPictureRegionAtomPtr self;

	self = (MP4StereoVideoAtomPtr)calloc(1, sizeof(MP4SubPictureRegionAtom));
	TESTMALLOC(self);
	
	err = MP4CreateFullAtom((MP4AtomPtr)self); if (err) goto bail;
	
	self->type						= MP4SubPictureRegionAtomType;
	self->name						= "SubPictureRegionBox";
	self->destroy					= destroy;
	self->createFromInputStream		= (cisfunc)createFromInputStream;
	self->calculateSize				= calculateSize;
	self->serialize					= serialize;

	*outAtom = self;
    
bail:
	TEST_RETURN(err);

	return err;
}