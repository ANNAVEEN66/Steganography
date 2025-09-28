#include<stdio.h>
#include<string.h>
#include"decode.h"
#include"types.h"
#include"common.h"
Status de_validations(DecodeInfo *decInfo)
{
    if(strstr(decInfo->src_image_fname,".bmp") == NULL) 
    {
	printf("Invalid extension @ %s\n",decInfo->src_image_fname);
	return e_failure;
    }
    char *str = strtok(decInfo->secret_fname,".");
    if(str != NULL)
	decInfo->secret_fname = str;
   return e_success;
}
Status do_decoding(DecodeInfo *decInfo) 
{
    if(de_validations(decInfo) == e_failure)
	return e_failure;
    if(OPEN_files(decInfo) == e_failure)
    {
	//printf("open file failure\n");
	return e_failure;
    }

    fseek(decInfo->fptr_src_image,54,SEEK_SET);
    
    if(decode_magic_string(decInfo) == e_success)
	printf("Done\n");
    else
    {
    	printf("Image file is not stegged!!!!!!!!!!!!!!!!!\n");
	return e_failure;
    }

    if(decode_secret_file_ext(decInfo) == e_success)
	printf("Done\n");
    else
    {
	printf("Decoding ext failed\n");
	return e_failure;
    }

    if(strcmp(decInfo->secret_fname,"decoded") ==0)
	printf("Output File not mentioned. Creating decoded.txt as default\n");

    printf("Opened %s\n",decInfo->secret_fname);
    printf("Done. Opened all required files\n");

    if(decode_secret_file_data(decInfo) == e_success)
	printf("Done\n");
    else
    {
	printf("Decoding secret msg failed\n");
	return e_failure;
    }

    fclose(decInfo->fptr_src_image);
    fclose(decInfo->fptr_secret);
	   

    return e_success;

}
Status OPEN_files(DecodeInfo *decInfo)
{
    decInfo->fptr_src_image = fopen(decInfo->src_image_fname, "r");
    if(decInfo->fptr_src_image == NULL)
    {
	printf("Invalid input file @ %s!!!!\n",decInfo->src_image_fname);
	return e_failure;
    
    }
    else
    {
        printf("## Decoding Procedure Started ##\n");
        printf("Opening required files\n");
        printf("Opened steged_img.bmp\n");
    }
    /*decInfo->fptr_secret = fopen(decInfo->secret_fname,"w");
    if(decInfo->fptr_secret == NULL)
    {
	printf("The secret_ file was not opened successfully\n");
	return e_failure;
    }*/

    return e_success;
}
Status decode_magic_string(DecodeInfo *decInfo)
{
    printf("Decoding Magic String Signature\n");
    int magic_len=0;
    char buff[32],magic_str[magic_len];

    fread(buff,32,1,decInfo->fptr_src_image);
    decode_byte_to_int(buff,&magic_len);
    //printf("magic string length is %d\n",magic_len);

    for(int i=0;i<magic_len;i++)
    {
	fread(buff,8,1,decInfo->fptr_src_image);
	decode_byte_to_lsb(buff, &magic_str[i]);
    }
    magic_str[magic_len] = '\0';
    //printf("Magic string is %s\n",magic_str);
    if((strcmp(MAGIC_STRING,magic_str) != 0))
	return e_failure;

    return e_success;
}
Status decode_byte_to_int(char *buff, int *len)
{
    for(int i=0;i<32;i++)
    {
	if(buff[i]&(1<<0))
		*len = (*len) | (1<<(32-i-1));
	else
	    	*len = (*len) & ~(1<<(32-i-1));
    }
    return e_success;
}
Status decode_byte_to_lsb(char *buff, char *str)
{
    for(int i=0;i<8;i++)
    {
	if(buff[i]&(1<<0))
	    *str = (*str | (1<<(8-i-1)));
	else
	    *str = (*str & ~(1<<(8-i-1)));
    }
}
Status decode_secret_file_ext(DecodeInfo *decInfo)
{
    printf("Decoding Output File Extension\n");
    char buff[32];
    int ext_len=0;
    fread(buff,32,1,decInfo->fptr_src_image);
    decode_byte_to_int(buff,&ext_len);
    //printf("Done\n");
    char ext[ext_len];
    for(int i=0;i<ext_len;i++)
    {
	fread(buff,8,1,decInfo->fptr_src_image);
	decode_byte_to_lsb(buff,&ext[i]);
    }
    ext[ext_len]='\0';

    char op_file[strlen(decInfo->secret_fname)+ext_len];
    strcpy(op_file,decInfo->secret_fname);
    strcat(op_file,ext);
    decInfo->fptr_secret = fopen(op_file,"w");
    /*printf("the ip secret fname is %s\n",decInfo->secret_fname);
    printf("the op secret fname is %s\n",op_file);*/

    return e_success;
}
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    printf("Decoding File Size\n");
    char buff[32];
    int f_size;
    
    fread(buff,32,1,decInfo->fptr_src_image);
    decode_byte_to_int(buff, &f_size);
    printf("Done\n");
    //printf("the file size is %d\n",f_size);
   // printf("i'am at %ld\n",ftell(decInfo->fptr_src_image));
    printf("Decoding File Data\n");
    char secret_msg[f_size];
    for(int i=0;i<f_size;i++)
    {
	fread(buff,8,1,decInfo->fptr_src_image);
	decode_byte_to_lsb(buff,&secret_msg[i]);
	fputc(secret_msg[i],decInfo->fptr_secret);
    }
    secret_msg[f_size]='\0';

    return e_success;
}



