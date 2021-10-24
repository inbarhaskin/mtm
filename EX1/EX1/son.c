#define _CRT_SECURE_NO_WARNINGS    
/*

               Tel Aviv University
             Faculty  of engineering


       Intro to System Programming project 1

                       A
                    S.I.M.P.
                   processor.



Inbar Haskin                              Yuval Levy
308407915                                 312238207

@ all right reserved to autors ((Inbar Haskin) && (Yuval Levy)) */

#include <stdio.h> 
#include <stdlib.h>
#include <windows.h>
#include <math.h>
//#include <string.h>
#include <strsafe.h>
#include "HardCodedData.h"
void close_handles(HANDLE handle_p, HANDLE handle_k, HANDLE handle_w) {
    CloseHandle(handle_p);
    CloseHandle(handle_k);
    CloseHandle(handle_w);

}
void check_handle(HANDLE new_handle, const char* file_name) {
    if (new_handle == INVALID_HANDLE_VALUE)
    {
        fprintf(stderr, "Could not open %s", file_name);
        printf("Error occurred in:\nFile: %s\nLine: %d", __FILE__, __LINE__);
        exit(-1);
    }
    if (new_handle == NULL || new_handle == 0)
        printf("the handle got null");
    return;
}
HANDLE create_handle(const char* file_name, int operation) {
    //create a handle, gets pointer to handle and requasted parameter write/read
    HANDLE new_handle;
    if (operation == READ) {
        //open exiting file
        fprintf(stderr, "trying to read files %s", file_name);
        new_handle = CreateFileA(file_name, // open file.txt 
            GENERIC_READ,             // open for reading
            0,                        // do not share
            NULL,                     // no security
            OPEN_EXISTING,            // existing file only
            FILE_ATTRIBUTE_NORMAL,    // normal file
            NULL); // no attr. template
        void check_handle(new_handle, file_name); //check that the handle is correct
        //return new_handle;

    }
    if (operation == WRITE)
    {
        new_handle = CreateFileA(file_name, // open encrypted.txt
            FILE_APPEND_DATA,         // open for writing
            FILE_SHARE_READ,          // allow multiple readers
            NULL,                     // no security
            OPEN_ALWAYS,              // open or create
            FILE_ATTRIBUTE_NORMAL,    // normal file
            NULL); // no attr. template
        void check_handle(new_handle, file_name); //check that the handle is correct
    }
}


int main(int argc, char* argv[])
{
    //main gets 4 argument: .exe <plaintext file name> <offset> <key file name>
    if (argc == 4) //change to 4
    {
        printf("Error, Invalid number of arguments, need to get 4 but got %d\n", argc);
        printf("Error occurred in:\nFile: %s\nLine: %d", __FILE__, __LINE__);
        exit(1);
    }
    //char* p_plaintext = argv[1];
    //int offset = atoi(argv[2]);
    //char* p_key = argv[3];
    char* p_plaintext = "plaintext.txt";
    int offset = 0;
    char* p_key = "key_file.txt";
    HANDLE handle_file = NULL;
    HANDLE handle_key = NULL;
    HANDLE handle_add = NULL;
    DWORD  dw_read_byts, dwBytesWritten, dw_position;
    BYTE   read_buffer[BUFFER_SIZE], key_buffer[BUFFER_SIZE], write_buffer[BUFFER_SIZE];
    BOOL read_status;

    //create a handle, gets pointer to handle and requasted parameter write/read
    handle_key = create_handle(p_key, READ);
    handle_file = create_handle(p_plaintext, READ);//handelimg file
    //open file or create if doesnt exist
    handle_add = create_handle("Encrypted_message.txt", WRITE);

    // strcpy_s(p_plaintext sizeof(argv[1]), argv[1]);
    LPCSTR try2[] = { "plaintext.txt\0" };
    LPCSTR try[] = { "key_file.txt\0" };
    fprintf(stderr, "p_plaintext: %s, offset: %d, p_key: %s\n", p_plaintext, offset, p_key);


    //read the key from text. whthandle_key DOESNT WORK?
    read_status = ReadFile(handle_key, key_buffer, BUFFER_SIZE - 1, &dw_read_byts, NULL);
    if (read_status == FALSE) {
        fprintf(stderr, "trying to use read_file, failing to open file");
        printf("Error occurred in:\nFile: %s\nLine: %d", __FILE__, __LINE__);
        close_handles(handle_file, handle_key, handle_add);
        return;
    }

    key_buffer[BUFFER_SIZE - 1] = '\0'; //its a string

    // Open the existing file, or if the file does not exist,
    // create a new file.

    SetFilePointer(handle_file, offset, 0, FILE_BEGIN); //Moves the file pointer of the specified file
    //explenation from microsoft: https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-setfilepointer
    //FILE_BEGIN=The starting point is zero or the beginning of the file.

    if (ReadFile(handle_file, read_buffer, BUFFER_SIZE - 1, &dw_read_byts, NULL)
        && dw_read_byts > 0)
        //dwBytesToWrite,  // number of bytes to write
        //& dwBytesWritten, // number of bytes that were written
    {
        read_buffer[BUFFER_SIZE - 1] = '\0';
        //encript the data
        fprintf(stderr, "dw_read_byts = % d\n", dw_read_byts);
        fprintf(stderr, "read buffer = % s\n", read_buffer);
        fprintf(stderr, "key_buffer = % s\n", key_buffer);
        //conversion to encrypted form
        for (int i = 0; i < dw_read_byts; i++) {
            write_buffer[i] = read_buffer[i] ^ key_buffer[i];
        }
        write_buffer[BUFFER_SIZE - 1] = '\0';
        fprintf(stderr, "write_buffer = % s\n", write_buffer);

        dw_position = SetFilePointer(handle_add, 0, NULL, FILE_END); //FILE_END: current end  of file position
        LockFile(handle_add, dw_position, 0, dw_read_byts, 0);
        //Locks the specified file for exclusive access by the calling process- will prenet other process acssess the file while writing
        //dw_position- byte offset in the file where the lock should begin
        //d2_read_byts- length of the byte range to be locked
        WriteFile(handle_add, write_buffer, dw_read_byts, &dwBytesWritten, NULL);
        UnlockFile(handle_add, dw_position, 0, dw_read_byts, 0);
        //Unlocks a region in an open file. Unlocking a region enables other processes to access the region.
    }

    // Close all files.
    close_handles(handle_file, handle_key, handle_add);

    return 0;
    //
}