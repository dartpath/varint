#include "random.h"
#include "varint.h"

#define MAX 1000000

int main()
{
    FILE* Uncompressed_file = fopen("uncompressed.dat", "w+b");
    FILE* Compressed_file = fopen("compressed.dat", "w+b");
    
    if ((Uncompressed_file == NULL) || (Compressed_file == NULL)) {
        printf("Error opening file\n");
        return -1;
    }

    uint32_t Number;
    uint8_t* Buf;
    size_t Encode_size;

    for (size_t i = 0; i < MAX; i++) {
        Number = generate_number();
        fwrite(&Number, sizeof(uint32_t), 1, Uncompressed_file);
        Buf = (uint8_t *)&Number;
        Encode_size = encode_varint(Number, Buf);
        fwrite(Buf, sizeof(uint8_t), Encode_size, Compressed_file);
    }

    rewind(Uncompressed_file);
    rewind(Compressed_file);

    uint32_t garb;
    uint32_t Decode_number;
    Buf = (uint8_t *)&garb;
    const uint8_t** Bufp;
    int j, Pos_in_file;
    
    for (Pos_in_file = 0, j = 0; j < MAX; Pos_in_file++, j++) {
        fseek(Compressed_file, Pos_in_file, SEEK_SET);
        fread(Buf, sizeof(uint8_t), 4, Compressed_file);
        Bufp = (const uint8_t **)&Buf;
        Decode_number = decode_varint(Bufp);
        fread(&Number, sizeof(uint32_t), 1, Uncompressed_file);

        if (Decode_number > 127) Pos_in_file++;
        if (Decode_number > 16383) Pos_in_file++;
        if (Decode_number > 2097151) Pos_in_file++;

        if (Number != Decode_number) {
            printf("Decode_number Error\n");
            break;
        }
    }

    return 0;
}