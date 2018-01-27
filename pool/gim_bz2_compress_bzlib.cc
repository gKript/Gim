//#include "gim.h"
#include "../include/gim_bz2_bzlib.h"


unsigned char *Encode_BZ2(unsigned char *SourceData,
						   long NumBytes,
						   unsigned long *OutLen)
{
	//PAGGHIU: PRESO dai docs
 	//"To guarantee that the compressed data will fit in its buffer, 
	//allocate an output buffer of size 1% larger than the uncompressed data,
	//plus six hundred extra bytes"

	unsigned int numOutBytes = (unsigned int)(NumBytes*1.1f+600);
	unsigned int realOutBytes = numOutBytes;
	char* outMem = (char*)malloc(numOutBytes);
	int res;
	res = BZ2_bzBuffToBuffCompress( outMem,
									&realOutBytes,
									(char*)SourceData,
									NumBytes,
									6,//Blocksize [0-9] 9 comprime meglio ma prende + memoria nella fase di compressione
									0,//verbosity (printa sullo stdout)
									0);//workload, meglio lasciarlo di default
	if(res!=BZ_OK)
	{
		free(outMem);
		*OutLen = 0;
		return 0;
	}
	//shrinkiamo il buffer giusto a quel che serve
	char* realOutMem = (char*)malloc(realOutBytes);
	memcpy(realOutMem,outMem,realOutBytes);
	free(outMem);
	*OutLen = realOutBytes;
	return (unsigned char*)realOutMem;
}

unsigned char *Decode_BZ2(unsigned char *Sdata, 
						   unsigned char *OData,       
						   unsigned long textsize,
						   unsigned long sourcesize)
{
 	

	char* outMem=0;
	int res;
	bool IMustFree=false;
	if(!OData)
	{	
		IMustFree=true;
		OData = (unsigned char*)malloc(textsize);
	}
	unsigned int destLen = textsize;
	res = BZ2_bzBuffToBuffDecompress ( (char*) OData,
							            &destLen,
						                (char*)         Sdata,
				                        (unsigned int)  sourcesize,
									    0,0 );
	if(res!=BZ_OK)
	{
		//o textsize non era quella giusta oppure lo stream e' danneggiato
		if(IMustFree)
			free(OData);
		return 0;
	}
	if(destLen<textsize)
	{
		//ATTENZIONE hai passato una textsize piu' grossa
		//di quel che serviva...non dovrebbe mai succedere 
		//xo' ovviametne se tu passi il textsize giusto, ma si sa
		//metti ke si corrompe qualcosa, ke ne so io
	}

	return OData;	

}
