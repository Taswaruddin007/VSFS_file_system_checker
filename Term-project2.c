#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <time.h>

#define BLOCK_SIZE 4096
#define TOTAL_BLOCKS 64
#define INODE_TABLE_BLOCKS 5
#define INODE_SIZE 256
#define INODE_COUNT ((INODE_TABLE_BLOCKS * BLOCK_SIZE) / INODE_SIZE)

#define INODE_BITMAP_BLOCK 1
#define DATA_BITMAP_BLOCK 2
#define INODE_TABLE_START_BLOCK 3
#define FIRST_DATA_BLOCK 8

#pragma pack(push, 1)

typedef struct {

    uint16_t magic_no;
    uint32_t block_size;
    uint32_t total_blocks;
    uint32_t inode_bitmap_block;
    uint32_t data_bitmap_block;
    uint32_t inode_table_start;
    uint32_t first_data_block;
    uint32_t inode_size;
    uint32_t inode_count;
    char reserved[4058];
    
} Superblock;

typedef struct {

    uint32_t mode;
    uint32_t uid;
    uint32_t gid;
    uint32_t file_size;
    uint32_t atime;
    uint32_t ctime;
    uint32_t mtime;
    uint32_t dtime;
    uint32_t links_count;
    uint32_t blocks;
    uint32_t direct_block;
    uint32_t single_indirect;
    uint32_t double_indirect;
    uint32_t triple_indirect;
    char reserved[156];
} Inode;
#pragma pack(pop)

void print_file_system_layout() {

    printf("\n==== File System Layout structure ====\n");
    
    printf("Block size: %d Bytes\n", BLOCK_SIZE);
    printf("Total blocks: %d\n", TOTAL_BLOCKS);
    printf("Block 0: Superblock\n");
    printf("Block 1: Inode bitmap\n");
    printf("Block 2: Data bitmap\n");
    printf("Blocks 3–7: Inode table (5 blocks)\n");
    printf("Blocks 8–63: Data blocks\n");
    printf("Inodes: %d Bytes each\n", INODE_SIZE);
}

void print_superblock(Superblock *sb) {

    printf("\n=== Superblock structre ===\n");
    
    printf("Magic number: 0x%x\n", sb->magic_no);
    printf("Block size: %u bytes\n", sb->block_size);
    printf("Total blocks: %u\n", sb->total_blocks);
    printf("Inode bitmap block: %u\n", sb->inode_bitmap_block);
    printf("Data bitmap block: %u\n", sb->data_bitmap_block);
    printf("Inode table start block: %u\n", sb->inode_table_start);
    printf("First data block: %u\n", sb->first_data_block);
    printf("Inode size: %u bytes\n", sb->inode_size);
    printf("Inode count: %u\n", sb->inode_count);
}


void print_inode(int idx, Inode *inode) {

    printf("\n---- Inode %d -----\n", idx);
    
    printf("Mode: %u\n", inode->mode);
    printf("User ID: %u\n", inode->uid);
    printf("Group ID: %u\n", inode->gid);
    printf("File size: %u bytes\n", inode->file_size);
    printf("Last access time: %u\n", inode->atime);
    printf("Creation time: %u\n", inode->ctime);
    printf("Last modification time: %u\n", inode->mtime);
    printf("Deletion time: %u\n", inode->dtime);
    printf("Number of hard links: %u\n", inode->links_count);
    printf("Data blocks allocated: %u\n", inode->blocks);
    printf("Direct block pointer: %u\n", inode->direct_block);
    printf("Single indirect pointer: %u\n", inode->single_indirect);
    printf("Double indirect pointer: %u\n", inode->double_indirect);
    printf("Triple indirect pointer: %u\n", inode->triple_indirect);
    printf("Reserved space: %lu bytes\n", sizeof(inode->reserved));
    
    
}


void inodes_chcking(FILE *fp, Superblock *sb) {

    Inode inode;
    
    int errors = 0;

    
    for (int i = 0; i < sb->inode_count; i++) {
        fseek(fp, BLOCK_SIZE * sb->inode_table_start + i * INODE_SIZE, SEEK_SET);
        fread(&inode, sizeof(Inode), 1, fp);

        
        if (inode.mode == 0) {
            printf("Error: Inode %d has invalid mode = 0.\n", i);
            errors++;
        }
        
        if (inode.uid == 0) {
            printf("Error: Inode %d has invalid user ID = 0.\n", i);
            errors++;
        }
        
        if (inode.gid == 0) {
            printf("Error: Inode %d has invalid group ID = 0.\n", i);
            errors++;
        }
        
        if (inode.file_size == 0) {
            printf("Error: Inode %d has file size = 0.\n", i);
            errors++;
        }
        
        if (inode.links_count == 0) {
            printf("Error: Inode %d has invalid links count = 0.\n", i);
            errors++;
        }
    }

    if (errors == 0) {
        printf("All inodes are correct and valid.\n");
    }
}


void fixed_inodes_chcking(FILE *fp, Superblock *sb) {

    Inode inode;
    int errors = 0;

  
    for (int i = 0; i < sb->inode_count; i++) {
    
        fseek(fp, BLOCK_SIZE * sb->inode_table_start + i * INODE_SIZE, SEEK_SET);
        fread(&inode, sizeof(Inode), 1, fp);
        print_inode(i, &inode);
    }    

}




void fixng_superblock(Superblock *sb) {

    
    sb->magic_no = 0xd34d;  
    sb->block_size = BLOCK_SIZE; 
    sb->total_blocks = TOTAL_BLOCKS;  
    sb->inode_bitmap_block = INODE_BITMAP_BLOCK;  
    sb->data_bitmap_block = DATA_BITMAP_BLOCK;  
    sb->inode_table_start = INODE_TABLE_START_BLOCK;  
    sb->first_data_block = FIRST_DATA_BLOCK;  
    sb->inode_size = INODE_SIZE;  
    sb->inode_count = INODE_COUNT;  
    
    printf("Fixed Superblock\n");
}

void fixng_inodes(FILE *fp, Superblock *sb) {

    Inode inode;


    for (int i = 0; i < sb->inode_count; i++) {
    
        fseek(fp, BLOCK_SIZE * sb->inode_table_start + i * INODE_SIZE, SEEK_SET);
        fread(&inode, sizeof(Inode), 1, fp);

        
        inode.mode = 0x1FF;  

        
        inode.uid = rand() % 1000 + 1000;  
        inode.gid = rand() % 1000 + 1000;  

        
        inode.file_size = 1;  
        inode.blocks = 1;  

        
        inode.atime = rand() % 1000000;
        inode.ctime = rand() % 1000000;
        inode.mtime = rand() % 1000000;
        inode.dtime = 0;  

       
        inode.direct_block = 10 + i;  

        
        if (i % 5 == 0) {
            inode.single_indirect = 100 + i;
        }

        
        fseek(fp, BLOCK_SIZE * sb->inode_table_start + i * INODE_SIZE, SEEK_SET);
        fwrite(&inode, sizeof(Inode), 1, fp);
    }

    printf("Fixed Inodes\n");
}

void data_bitmap_chcking(FILE *fp, Superblock *sb) {

    uint8_t data_bitmap[TOTAL_BLOCKS / 8];
    fseek(fp, BLOCK_SIZE * sb->data_bitmap_block, SEEK_SET);
    fread(data_bitmap, sizeof(data_bitmap), 1, fp);

    for (int i = 0; i < sb->inode_count; i++) {
    
        Inode inode;
        fseek(fp, BLOCK_SIZE * sb->inode_table_start + i * INODE_SIZE, SEEK_SET);
        fread(&inode, sizeof(Inode), 1, fp);

        
        if (inode.blocks > 0) {
        
            uint32_t block = inode.direct_block;
            
            if (!(data_bitmap[block / 8] & (1 << (block % 8)))) {
                printf("Error: Block %u is referenced by inode %d but not marked in the data bitmap.\n", block, i);
            }
        }
    }
}

void inode_bitmap_chckng(FILE *fp, Superblock *sb) {

    uint8_t inode_bitmap[TOTAL_BLOCKS / 8];
    fseek(fp, BLOCK_SIZE * sb->inode_bitmap_block, SEEK_SET);
    fread(inode_bitmap, sizeof(inode_bitmap), 1, fp);

    for (int i = 0; i < sb->inode_count; i++) {
        
        Inode inode;
        fseek(fp, BLOCK_SIZE * sb->inode_table_start + i * INODE_SIZE, SEEK_SET);
        fread(&inode, sizeof(Inode), 1, fp);

       
        if (inode.links_count > 0 && inode.dtime == 0) {
        
            if (!(inode_bitmap[i / 8] & (1 << (i % 8)))) {
            
                printf("Error: Inode %d is valid but not marked in the inode bitmap.\n", i);
            }
        }
    }
}

int main() {

    FILE *fp = fopen("vsfs.img", "r+b");
    
    if (!fp) {
    
        perror("Failed to open the vsfs.img file");
        return 1;
    }

    Superblock sb;
    fseek(fp, 0, SEEK_SET);
    fread(&sb, sizeof(Superblock), 1, fp);


    print_file_system_layout();

    
    
    printf("\n=== before fixed superblock ===\n");
    
    print_superblock(&sb);
    fixng_superblock(&sb);
    fseek(fp, 0, SEEK_SET);
    fwrite(&sb, sizeof(Superblock), 1, fp);  
    
    printf("\n=== after fixed superblock ===\n");
    print_superblock(&sb);
    
    
    
    fixng_inodes(fp, &sb);

   
    data_bitmap_chcking(fp, &sb);
    inode_bitmap_chckng(fp, &sb);
    
    printf("\n=== Inodes ===\n");
    
    inodes_chcking(fp, &sb);
    
    fixed_inodes_chcking(fp, &sb);
    

    
    printf("\nDone with File system consistency check and repair completed.\n");

    fclose(fp);

    return 0;
}

