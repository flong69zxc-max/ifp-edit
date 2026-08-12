#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char signature[4];
    unsigned int size;
    char name[24];
    unsigned int animCount;
} IFPHeader;

typedef struct {
    char name[24];
    unsigned int boneCount;
} AnimHeader;

typedef struct {
    char name[28];
    unsigned int frameCount;
} BoneHeader;

void parse_ifp(const char* filename) {
    FILE* f = fopen(filename, "rb");
    if (!f) {
        printf("Error: Cannot open file\n");
        return;
    }
    
    IFPHeader header;
    fread(&header, sizeof(IFPHeader), 1, f);
    
    if (strncmp(header.signature, "ANP3", 4) != 0) {
        printf("Error: Not an ANP3 file\n");
        fclose(f);
        return;
    }
    
    printf("Library: %s\n", header.name);
    printf("Animations: %d\n\n", header.animCount);
    
    for (int i = 0; i < header.animCount; i++) {
        AnimHeader anim;
        fread(&anim, sizeof(AnimHeader), 1, f);
        printf("--- Animation %d: %s ---\n", i+1, anim.name);
        printf("Bones: %d\n", anim.boneCount);
        
        for (int j = 0; j < anim.boneCount; j++) {
            BoneHeader bone;
            fread(&bone, sizeof(BoneHeader), 1, f);
            printf("  Bone %d: %s (%d frames)\n", j, bone.name, bone.frameCount);
            
            // Пропускаем кадры (ищем смещение)
            for (int k = 0; k < bone.frameCount; k++) {
                char keyType[4];
                fread(keyType, 4, 1, f);
                
                if (strncmp(keyType, "KR00", 4) == 0) {
                    fseek(f, 20, SEEK_CUR); // quat(16) + time(4)
                } else if (strncmp(keyType, "KRT0", 4) == 0) {
                    fseek(f, 32, SEEK_CUR); // quat(16) + pos(12) + time(4)
                }
            }
        }
    }
    
    fclose(f);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s input.ifp [output.txt]\n", argv[0]);
        return 1;
    }
    
    parse_ifp(argv[1]);
    return 0;
}
