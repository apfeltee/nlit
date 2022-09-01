
#include "lit.h"

namespace lit
{
    size_t FileIO::write_string(FILE* file, const char* data, size_t len)
    {
        return fwrite(data, sizeof(char), len, file);
    }

    size_t FileIO::write_string(FILE* file, std::string_view sv)
    {
        return write_string(file, sv.data(), sv.size());
    }

    size_t FileIO::binwrite_uint8_t(FILE* file, uint8_t byte)
    {
        return fwrite(&byte, sizeof(uint8_t), 1, file);
    }

    size_t FileIO::binwrite_uint16_t(FILE* file, uint16_t byte)
    {
        return fwrite(&byte, sizeof(uint16_t), 1, file);
    }

    size_t FileIO::binwrite_uint32_t(FILE* file, uint32_t byte)
    {
        return fwrite(&byte, sizeof(uint32_t), 1, file);
    }

    size_t FileIO::binwrite_double(FILE* file, double byte)
    {
        return fwrite(&byte, sizeof(double), 1, file);
    }

    size_t FileIO::binwrite_string(FILE* file, String* string)
    {
        uint16_t i;
        uint16_t c;
        size_t rt;
        c = string->length();
        rt = fwrite(&c, 2, 1, file);
        auto ch = string->data();
        for(i = 0; i < c; i++)
        {
            binwrite_uint8_t(file, (uint8_t)ch[i] ^ LIT_STRING_KEY);
        }
        return (rt + i);
    }

    uint8_t FileIO::binread_uint8_t(FILE* file)
    {
        size_t rt;
        (void)rt;
        static uint8_t btmp;
        rt = fread(&btmp, sizeof(uint8_t), 1, file);
        return btmp;
    }

    uint16_t FileIO::binread_uint16_t(FILE* file)
    {
        size_t rt;
        (void)rt;
        static uint16_t stmp;
        rt = fread(&stmp, sizeof(uint16_t), 1, file);
        return stmp;
    }

    uint32_t FileIO::binread_uint32_t(FILE* file)
    {
        size_t rt;
        (void)rt;
        static uint32_t itmp;
        rt = fread(&itmp, sizeof(uint32_t), 1, file);
        return itmp;
    }

    double FileIO::binread_double(FILE* file)
    {
        size_t rt;
        (void)rt;
        static double dtmp;
        rt = fread(&dtmp, sizeof(double), 1, file);
        return dtmp;
    }

    String* FileIO::binread_string(State* state, FILE* file)
    {
        size_t rt;
        uint16_t i;
        uint16_t length;
        char* line;
        (void)rt;
        rt = fread(&length, 2, 1, file);
        if(length < 1)
        {
            return nullptr;
        }
        line = (char*)malloc(length + 1);
        for(i = 0; i < length; i++)
        {
            line[i] = (char)binread_uint8_t(file) ^ LIT_STRING_KEY;
        }
        return String::take(state, line, length);
    }
}

