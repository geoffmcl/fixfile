// Sample Code
// NOTE: NT4SymGetLineFromAddr will work only when the debugging information
// is either embedded within an executable (dynamic-link library) or created
// as a separate .dbg (Debug) file. This functionality is not supported when
// debugging information is placed in a .pdb (Program Database) file generated
// by Microsoft Visual C++, 32-bit Edition.
// Microsoft Windows NT 5.0 version of ImageHlp provides SymGetLineFromAddr
// API functionality. The NT4SymGetLineFromAddr function in the following code
// is necessary only for Windows NT 4.0 or any older version of ImageHlp. 
#define __RPCASYNC_H__ // added Sept 2004 to compile in XP in UNIKA
    #include <windows.h>
    #include <stdio.h>

#ifdef   ADDDBGHLP
    #include <imagehlp.h>
    /* CV4 Directory header structure indicating size
       and number of subsection directory entries that
       follow.
    */ 
    struct CV4DirectoryHeader
    {
        WORD    cbDirHeader;
        WORD    cbDirEntry;
        DWORD   cDir;
        DWORD   IfoNextDir;
        DWORD   flags;
    };
    typedef struct CV4DirectoryHeader *PCV4DirectoryHeader;


    /* CV4 Directory entry structure specifying the subsection
       type, offset, and size.
    */ 
    struct CV4DirectoryEntry
    {
        WORD    subsection;
        WORD    iMod;
        DWORD   Ifo;
        DWORD   cb;
    };
    typedef struct CV4DirectoryEntry *PCV4DirectoryEntry;

    /* NT4SymGetLineFromAddr Public API. */ 
    BOOL NT4SymGetLineFromAddr(
                    IN  HANDLE                      hProcess,
                    IN  DWORD                       dwAddress,
                    OUT DWORD                       *pLineNumber,
                    OUT LPTSTR                      pFileName,
                    IN  DWORD                       nSize);

    /* Private Function - Locates a source module corresponding
       to an address. */ 
    BOOL LocateSrcModule(LPVOID pCV4DebugData, DWORD dwAddress,
                         DWORD *pLineNumber, LPTSTR pFileName,
                         DWORD nSize);

    /* Private Function - Retrieves source module information by
       analyzing line/address mapping tables in a module.
    */ 
    BOOL GetSrcModuleInfo(LPSTR ptr, DWORD dwAddress,
                          DWORD *pLineNumber, LPTSTR pFileName,
                          DWORD nSize);

    /*  The NT4SymGetLineFromAddr function locates the source line
        number and module name for the specified virtual address.

        BOOL NT4SymGetLineFromAddr(
                    IN  HANDLE                      hProcess,
                    IN  DWORD                       dwAddress,
                    OUT DWORD                       *pLineNumber,
                    OUT LPTSTR                      pFileName;
                    IN  DWORD                       nSize)

        Parameters

        hProcess - Handle to the process that was originally passed to
                   the SymInitialize function.

        dwAddress - Specifies the virtual address for which the
                    source line number and module name should be
                    located. The instruction at the specified address
                    does not have to be on a line boundary. If the
                    address comes after the beginning of a line and
                    before the end of the line, the beginning of the
                    source line is returned in pLineNumber.

        pLineNumber - Pointer to a DWORD to receive source line number.

        pFileName - Pointer to a buffer to receive source file name.

        nSize - Size of pFileName buffer, in characters.

        Return Values

            If the function succeeds, the return value is TRUE.

            If the function fails, the return value is FALSE.
            To retrieve extended error information, call GetLastError.
    */ 

    BOOL NT4SymGetLineFromAddr(
                IN  HANDLE                      hProcess,
                IN  DWORD                       dwAddress,
                OUT DWORD                       *pLineNumber,
                OUT LPTSTR                      pFileName,
                IN  DWORD                       nSize)
    {
        IMAGEHLP_MODULE             mi;
        PIMAGE_DEBUG_INFORMATION    pDebugInfo = 0;
        PIMAGE_DEBUG_DIRECTORY      pDebug;
        DWORD                       i, nEntries;
        BOOL                        bReturn = FALSE;
        char                        searchPath[4096];

        if (!dwAddress || !pLineNumber
            || !pFileName || !nSize)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }

        *pFileName = 0;
        *pLineNumber = 0;

        /* Retrieve module information corresponding
           to the specified virtual address. */ 

        mi.SizeOfStruct = sizeof(mi);
        if (!SymGetModuleInfo(hProcess, dwAddress, &mi))
            return FALSE;

        if (mi.SymType == SymPdb)
        {
            SetLastError(ERROR_INVALID_FUNCTION);
            return FALSE;
        }

        bReturn = SymGetSearchPath(hProcess,
                        searchPath,
                        sizeof(searchPath));

        /* Map the debug information of the required module. */ 
        pDebugInfo = MapDebugInformation(0,
                        mi.ImageName,
                        bReturn ? searchPath : 0,
                        0);
        if (!pDebugInfo) return FALSE;

        /* Obtain pointer to debug directory. */ 
        pDebug = pDebugInfo->ReservedDebugDirectory;
        /* Number of entries in debug directory. */ 
        nEntries = pDebugInfo->ReservedNumberOfDebugDirectories;

        __try
        {
            for (i = 0; i < nEntries; i++)
            {
                /* Locate Codeview debug type. */ 
                if (pDebug->Type == IMAGE_DEBUG_TYPE_CODEVIEW)
                {
                    DWORD                   j;
                    PIMAGE_SECTION_HEADER   pSection;

                    pSection = pDebugInfo->ReservedSections;
                    for (j = 0;
                         j < pDebugInfo->ReservedNumberOfSections;
                         j++)
                    {
                        /* Get the virtual address of executable
                           code section. */ 
                        if (pSection
                            && (pSection->Characteristics
                                    & IMAGE_SCN_MEM_EXECUTE))
                        {
                            bReturn = LocateSrcModule(
                                        (char*)pDebugInfo->ReservedMappedBase
                                          + pDebug->PointerToRawData,
                                        /* Calculate relative offset
                                           within the executable code
                                           section.
                                        */ 
                                        dwAddress
                                        - (pDebugInfo->ImageBase
                                           + pSection->VirtualAddress),
                                        pLineNumber,
                                        pFileName,
                                        nSize);
                            UnmapDebugInformation(pDebugInfo);
                            return bReturn;
                        }
                        pSection++;
                    }
                }
                pDebug = (PIMAGE_DEBUG_DIRECTORY)((char *)pDebug
                          + sizeof(IMAGE_DEBUG_DIRECTORY));
            }
            SetLastError(ERROR_INVALID_ADDRESS);
        }
        __except(EXCEPTION_EXECUTE_HANDLER)
        {
            SetLastError(GetExceptionCode());
        }

        UnmapDebugInformation(pDebugInfo);
        return FALSE;
    }

    BOOL LocateSrcModule(LPVOID pCV4DebugData, DWORD dwAddress,
                         DWORD *pLineNumber, LPTSTR pFileName,
                         DWORD nSize)
    {
        char                *ptr;
        PCV4DirectoryHeader pHeader;
        PCV4DirectoryEntry  pEntry;
        WORD                i;

        ptr = (char*) pCV4DebugData;
        ptr = ptr + *(DWORD *)(ptr + sizeof(DWORD));

        /* CV4 Directory header. */ 
        pHeader = (PCV4DirectoryHeader) ptr;
        if (pHeader->cbDirHeader != sizeof(*pHeader))
        {
            SetLastError(ERROR_INVALID_ADDRESS);
            return FALSE;
        }
        pEntry = (PCV4DirectoryEntry) (ptr + pHeader->cbDirHeader);

        /* Search CV4 directory entries. */ 
        for (i = 0; i < pHeader->cDir; i++, pEntry++)
        {
            switch (pEntry->subsection)
            {
                /* Locate sstSrcModule data block. */ 
                case 0x0127:
                    if (GetSrcModuleInfo((char *)pCV4DebugData
                          + pEntry->Ifo,
                          dwAddress, pLineNumber, pFileName, nSize))
                    {
                        if (GetLastError() ==
                            ERROR_INSUFFICIENT_BUFFER)
                            return FALSE;
                        return TRUE;
                    }
                    break;
                default:
                    break;
            }
        }

        SetLastError(ERROR_INVALID_ADDRESS);
        return FALSE;
    }

    BOOL GetSrcModuleInfo(LPSTR ptr, DWORD dwAddress,
                          DWORD *pLineNumber, LPTSTR pFileName,
                          DWORD nSize)
    {
        WORD    i, j, cFile, cSegInMod;
        char    *pModule;
        DWORD   fileTableOffset;
        char    *baseSrcFile;
        char    *mapTable;

        SetLastError(0);

        *pFileName = 0;
        *pLineNumber = 0;

        pModule = ptr;

        /* Number of source files in a module. */ 
        cFile = *(WORD *)ptr;
        ptr += sizeof(WORD);

        /* Total number of code segments in a module. */ 
        cSegInMod = *(WORD *)ptr;
        ptr += sizeof(WORD);
        for (i = 0; i < cFile; i++)
        {
            WORD    cSegInFile;
            DWORD   mapTableOffset;
            LPTSTR  pFilePtr;
            WORD    fileNameLen;

            fileTableOffset = *(DWORD *)(ptr + i * sizeof(DWORD));
            baseSrcFile = pModule + fileTableOffset;

            /* Number of code segments in a source file. */ 
            cSegInFile = *(WORD *)baseSrcFile;
            baseSrcFile += (sizeof(WORD) + sizeof(WORD));
            for (j = 0; j < cSegInFile; j++)
            {
                mapTableOffset = *(DWORD *)(baseSrcFile
                                    + j * sizeof(DWORD));
                mapTable = pModule + mapTableOffset;
                {
                    WORD segIndex, nPairs, low, middle, high;

                    segIndex = *(WORD *)mapTable;
                    mapTable += sizeof(WORD);
                    nPairs = *(WORD *)mapTable;
                    mapTable += sizeof(WORD);

                    /* Analyze line/address mapping table. */ 
                    low = 0;
                    high = nPairs - 1;

                    while (high >= low)
                    {
                        DWORD offset, nextOffset;

                        middle = (low + high) / 2;
                        offset = *(DWORD*)(mapTable
                                    + middle * sizeof(DWORD));

                        if (dwAddress < offset)
                        {
                            if (middle < 1)
                                break;
                            high = middle - 1;
                            continue;
                        }

                        if (middle < (nPairs - 1))
                        {
                            nextOffset = *(DWORD*)(mapTable
                                + (middle+1) * sizeof(DWORD));

                            if (dwAddress >= nextOffset)
                            {
                                low = middle + 1;
                                if ((dwAddress > nextOffset)
                                    && (low == (nPairs - 1)))
                                    break;
                                continue;
                            }
                        }

                        *pLineNumber = *(WORD*)(mapTable
                                        + nPairs * sizeof(DWORD)
                                        + middle * sizeof(WORD));

                        fileNameLen = *(char *)(
                                        baseSrcFile
                                        + cSegInFile
                                          * 3 * sizeof(DWORD));
                        pFilePtr = (LPTSTR)(
                                    baseSrcFile
                                    + cSegInFile
                                      * 3 * sizeof(DWORD)
                                    + sizeof(char));
                        if (nSize <= fileNameLen)
                        {
                            SetLastError
                                (
                                ERROR_INSUFFICIENT_BUFFER
                                );
                        }
                        else
                        {
                            CopyMemory(pFileName,
                                     pFilePtr,
                                     fileNameLen);
                            pFileName[fileNameLen] = 0;
                        }
                        return TRUE;
                    }
                }
            }
        }

        return FALSE;
    }

   // NT4SymGetLineFromAddr Usage:

    DWORD ProcessException(PEXCEPTION_POINTERS ptr)
    {
        DWORD   exceptionAddress = 0;
    #ifdef  UNICODE
        WCHAR
    #else
        char
    #endif
            fileName[MAX_PATH+1];

        DWORD   lineNumber;


        if (ptr && ptr->ExceptionRecord
            && ptr->ExceptionRecord->ExceptionAddress)
        {
            exceptionAddress =
             (DWORD) ptr->ExceptionRecord->ExceptionAddress;
        }

        {
            HANDLE                      hProcess;

            hProcess = GetCurrentProcess();

            if (SymInitialize(hProcess, NULL, TRUE))
            {
                if (NT4SymGetLineFromAddr(
                        hProcess,
                        exceptionAddress,
                        &lineNumber,
                        fileName,
                        MAX_PATH))
                {
                    printf("\n\n0x%08x Address Maps to\n",
                        exceptionAddress);
                    printf("--------------------------\n");
                    printf("FileName %s\nLineNumber %d\n",
                        fileName, lineNumber);
                }
                else
                {
                    printf("\n\nNT4SymGetLineFromAddr failed"
                        " with an error : %d\n",
                        GetLastError());
                }
                SymCleanup(hProcess);
            }
        }

        return EXCEPTION_EXECUTE_HANDLER;
    }

#ifdef   AS_STAND_ALONE

    int main(int argc, char** argv)
    {
        char* ptr = 0;

        __try
        {
            *ptr = 0;
        }
        __except (ProcessException(GetExceptionInformation()))
        {
            printf("Exception %x occurred\n", GetExceptionCode());
        }
        __try
        {
            *ptr = 0;
        }
        __except (ProcessException(GetExceptionInformation()))
        {
            printf("Exception %x occurred\n", GetExceptionCode());
        }
        return 0;
    } 


#endif   // #ifdef   AS_STAND_ALONE

#endif   // #ifdef   ADDDBGHLP


BOOL HasDOSHeader( BYTE * lpImage, PLARGE_INTEGER pliSize )
{
   LARGE_INTEGER li;

   li.QuadPart = (*pliSize).QuadPart;

   if( li.QuadPart < sizeof(IMAGE_DOS_HEADER) )
      return FALSE;

   return (*(USHORT *)lpImage == IMAGE_DOS_SIGNATURE);
}
