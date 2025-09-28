#ifndef DECODE_H
#define DECODE_H
#include"types.h"
typedef struct _DecodeInfo
{
    char *src_image_fname;
    FILE *fptr_src_image;

    char *secret_fname;
    FILE *fptr_secret;
    
} DecodeInfo;

Status do_decoding(DecodeInfo *decInfo); 
Status OPEN_files(DecodeInfo *decInfo);
Status decode_magic_string(DecodeInfo *decInfo);
Status decode_byte_to_int(char *buff, int *len);
Status decode_byte_to_lsb(char *buff, char *magic_str);
Status decode_secret_file_ext(DecodeInfo *decInfo);
Status decode_secret_file_data(DecodeInfo *decInfo);
Status de_validations(DecodeInfo *decInfo);


#endif

