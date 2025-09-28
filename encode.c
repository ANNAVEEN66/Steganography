#include <stdio.h>
#include<string.h>				//Including necessary header files for encoding process
#include "encode.h"
#include "types.h"
#include"common.h"

int S_flag=0,Cap_flag=0;

uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    //printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    //printf("height = %u\n", height);

    rewind(fptr_image);

    // Return image capacity
    return width * height * 3;
}

Status validations(EncodeInfo *encInfo)
{
	if(strcmp(strchr(encInfo->src_image_fname,'.'),".bmp") != 0)
	{
	    printf("Invalid file extension @ %s\n",encInfo->src_image_fname);
	    return e_failure;
	}

 
       char *str = strchr(encInfo->secret_fname,'.');
       char *str1 = ".txt.c.csv.bmp.sh";
       if ( str == NULL || strstr(str1,str) == NULL)
       {
           printf("Invalid file extension @ %s\n",encInfo->secret_fname);
       	   return e_failure;
       }	   

    
       str = strchr(encInfo->stego_image_fname,'.');
       if ( str == NULL  || strcmp(str,".bmp") != 0)
       {
	   printf("Invalid file extension @ %s\n",encInfo->stego_image_fname);
	   return e_failure;
       }
    
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname,"r");
    encInfo->fptr_secret = fopen(encInfo->secret_fname,"r");
    if(encInfo->fptr_secret == NULL)
    {
	printf("Given %s is not available in current directory\n",encInfo->secret_fname);
	return e_failure;
    }
	  
	if(check_capacity(encInfo) == e_failure)
	{
	    if(S_flag==0)
	    {
	    	Cap_flag = 1;
	    	printf("Source file size exceeding the capacity of secret file data!!\n");
	    }
	    return e_failure;
	}

  return e_success;    
}

Status check_capacity(EncodeInfo *encInfo)
{
    uint size1 = get_image_size_for_bmp(encInfo->fptr_src_image);
    char *f_ext = strchr(encInfo->secret_fname,'.');
    uint size2 = get_file_size(encInfo->fptr_secret);
    rewind(encInfo->fptr_secret);
    if(size2 == 0)
    {
	S_flag = 1;
	printf("Secret file is empty!\n");
	return e_failure;
    }

    if(size1 > (4+strlen(MAGIC_STRING)+4+strlen(f_ext)+4+size2)*8)
	return e_success;
    else
	return e_failure;
	
    fclose(encInfo->fptr_src_image);
    fclose(encInfo->fptr_secret);
}

Status do_encoding(EncodeInfo *encInfo)
{
//Validations---------------------------------------------------------
   if(validations(encInfo) == e_success)
   {
	//open file-------------------------------------------------------------------------------------
	if(open_files(encInfo) == e_success)
	     printf("Done\n");
        else
        {
	    return e_failure;
        }
	printf("## Encoding Procedure Started ##\n");

	printf("Checking the %s size\n",encInfo->secret_fname);
	if(S_flag == 0)
            printf("Done. Not Empty\n");

	printf("Checking for SkeletonCode//%s capacity to handle %s\n",encInfo->src_image_fname,encInfo->secret_fname); 
	if(Cap_flag == 0)
	    printf("Done. Found OK\n");

	if(strcmp(encInfo->stego_image_fname,"steged_img.bmp") == 0)
	    printf("Output File not mentioned. Creating steged_img.bmp as default\n");

	//copying the header of beautiful.bmp file to output.bmp------------------------------------------
        printf("Copying Image Header\n");
	if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success)
		printf("Done\n");
	else
	{
	    printf("Failed to copy header file\n");
	    return e_failure;
	}

	//Encoding the magic string------------------------------------------------------------------------
	printf("Encoding Magic String Signature\n");	
	if(encode_magic_string(MAGIC_STRING, encInfo) == e_success)
		printf("Done\n");
	else
	{
	    printf("Failed to encode magic string\n");
	    return e_failure;
	}

	//Encoding the entension of secret file------------------------------------------------------------
	printf("Encoding %s File Extension\n",encInfo->secret_fname);
	char *file_extn=strchr(encInfo->secret_fname,'.');
	if(encode_secret_file_extn(file_extn, encInfo) == e_success)
		printf("Done\n");
	else
	{
	    printf("Failed to encode extension of secret file\n");
	    return e_failure;
	}

	//Encoding the size of secret file-----------------------------------------------------------------
	printf("Encoding %s File Size\n",encInfo->secret_fname);
	 long int file_size = get_file_size(encInfo->fptr_secret);
	 //printf("Secret file size is %ld\n",file_size);
	if(encode_secret_file_size(file_size, encInfo) == e_success)
		printf("Done\n");
	else
	{
	    printf("Failed to encode secret file size\n");
	    return e_failure;
	}

	//Encoding the secret file data---------------------------------------------------------------------
	printf("Encoding %s File Data\n",encInfo->secret_fname);
	if(encode_secret_file_data(encInfo) == e_success)
	    printf("Done\n");
	else
	{
	    printf("Failed to encode secret file data\n");
	    return e_failure;
	}

	//Copying the remaining data from source img file to dest img file-----------------------------------
	printf("Copying Left Over Data\n");
	if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success)
	    printf("Done\n");
	else
	{
	    printf("Failed copying remaining data\n");
	    return e_failure;
	}
   }
   else
      return e_failure;

	fclose(encInfo->fptr_src_image);
	fclose(encInfo->fptr_stego_image);
	fclose(encInfo->fptr_secret);

return e_success;
}

Status open_files(EncodeInfo *encInfo)
{
    printf("Opening required files\n");

    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    if(encInfo->fptr_src_image == NULL)
    {	
	printf("Invalid file extension %s\n",encInfo->src_image_fname);
	return e_failure;
    }
    printf("Opened %s\n",encInfo->stego_image_fname);
  

    encInfo->fptr_secret = fopen(encInfo->secret_fname,"r");
    if(encInfo->fptr_secret == NULL)
    {
	printf("Invalid file extension %s\n",encInfo->secret_fname);
	return e_failure;
    }
    printf("Opened %s\n",encInfo->secret_fname);

    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname,"w");
    if(encInfo->fptr_stego_image == NULL)
    {
	printf("Invalid file extension %s\n",encInfo->stego_image_fname);
	return e_failure;
    }
    printf("Opened %s\n",encInfo->stego_image_fname);

    return e_success;
}
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    rewind(fptr_src_image);
    char buff[54];
    fread(buff,54,1,fptr_src_image);
    fwrite(buff,54,1,fptr_dest_image);
    return e_success;
}
Status encode_magic_string(char *magic_string, EncodeInfo *encInfo)
{
    int len = strlen(magic_string);
    char buff[32];

    fread(buff,32,1,encInfo->fptr_src_image);
    encode_int_to_lsb(len, buff);
    fwrite(buff,32,1,encInfo->fptr_stego_image);

    char image_buffer[8];
    for(int i=0;i<len;i++)
    {
	fread(image_buffer,8,1,encInfo->fptr_src_image);
	encode_byte_to_lsb(magic_string[i],image_buffer);
    	fwrite(image_buffer,8,1,encInfo->fptr_stego_image);
    }
    return e_success;
}
Status encode_int_to_lsb(int len, char *buff)
{
    for(int i=31;i>=0;i--)
    {
	if(len&(1<<i))
	    buff[32-i-1]=buff[32-i-1] | 1;
	else
	    buff[32-i-1]=buff[32-i-1] & ~1;
    }
    return e_success;
}
Status encode_byte_to_lsb(char data, char *image_buffer)
{
	for(int i=7;i>=0;i--)
	{
	    if(data&(1<<i))
		image_buffer[8-i-1]=image_buffer[8-i-1] | 1;
	    else
		image_buffer[8-i-1]=image_buffer[8-i-1] & ~1;
	}
    return e_success;
}
Status encode_secret_file_extn( char *file_extn, EncodeInfo *encInfo)
{

	int len=strlen(file_extn);
	char buff[32];

	fread(buff,32,1,encInfo->fptr_src_image);
	encode_int_to_lsb(len ,buff);
	fwrite(buff,32,1,encInfo->fptr_stego_image);

	for(int i=0;i<len;i++)
	{
		fread(buff,8,1,encInfo->fptr_src_image);
		encode_byte_to_lsb(file_extn[i],buff);
	        fwrite(buff,8,1,encInfo->fptr_stego_image);
	}
    return e_success;
}
uint get_file_size(FILE *fptr)
{
	fseek(fptr,0,SEEK_END);
	return ftell(fptr);
}
Status encode_secret_file_size(long int file_size, EncodeInfo *encInfo)
{
    
	char buff[32];
	fread(buff,32,1,encInfo->fptr_src_image);
	encode_int_to_lsb(file_size, buff);
	fwrite(buff,32,1,encInfo->fptr_stego_image);
    return e_success;
}
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    long int size = get_file_size(encInfo->fptr_secret);
    fseek(encInfo->fptr_secret,0,SEEK_SET);
    char ch[size];

    char buff[8];
	for(int i=0;i<size;i++)
	{
	     ch[i]=fgetc(encInfo->fptr_secret);
	     fread(buff,8,1,encInfo->fptr_src_image);
	     encode_byte_to_lsb(ch[i], buff);
	     fwrite(buff,8,1,encInfo->fptr_stego_image);
	     
	}
	return e_success;
}
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
	while(!feof(fptr_src))
	    fputc(fgetc(fptr_src),fptr_dest);

     return e_success;
}

