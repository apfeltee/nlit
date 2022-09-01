
#include "lit.h"

namespace lit
{
    namespace Util
    {
        char* copyString(std::string_view sv)
        {
            size_t length;
            char* rtbuf;
            length = sv.size() + 1;
            rtbuf = (char*)malloc(length);
            memcpy(rtbuf, sv.data(), length);
            return rtbuf;
        }

        char* readFile(const char* path, size_t* destlen)
        {
            size_t toldsz;
            size_t actualsz;
            char* buffer;
            FILE* hnd;
            hnd = fopen(path, "rb");
            if(hnd == nullptr)
            {
                return nullptr;
            }
            fseek(hnd, 0L, SEEK_END);
            toldsz = ftell(hnd);
            rewind(hnd);
            buffer = (char*)malloc(toldsz + 1);
            actualsz = fread(buffer, sizeof(char), toldsz, hnd);
            *destlen = actualsz;
            buffer[actualsz] = '\0';
            fclose(hnd);
            return buffer;
        }


        char* patchFilename(char* file_name)
        {
            int i;
            int name_length;
            char c;
            name_length = strlen(file_name);
            // Check, if our file_name ends with .lit or lbc, and remove it
            if(name_length > 4 && (memcmp(file_name + name_length - 4, ".lit", 4) == 0 || memcmp(file_name + name_length - 4, ".lbc", 4) == 0))
            {
                file_name[name_length - 4] = '\0';
                name_length -= 4;
            }
            // Check, if our file_name starts with ./ and remove it (useless, and makes the module name be ..main)
            if(name_length > 2 && memcmp(file_name, "./", 2) == 0)
            {
                file_name += 2;
                name_length -= 2;
            }
            for(i = 0; i < name_length; i++)
            {
                c = file_name[i];
                if(c == '/' || c == '\\')
                {
                    file_name[i] = '.';
                }
            }
            return file_name;
        }
    }
}

