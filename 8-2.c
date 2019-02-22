#include <stdio.h>
#include <stdlib.h>

typedef struct cacheb cacheb;
struct cacheb{                      //create cache block struct
    unsigned char valid;
    unsigned int tag;
    unsigned char value[4];
};

unsigned int getTag(unsigned int address){
    unsigned int tag = address >> 6;        //extract tag from adress
    return tag;
}

unsigned int getSet(unsigned int address){
    unsigned int set = (address << 26) >> 28; //extract set from address
    return set;
}

unsigned int getOffset(unsigned int address){
    unsigned int offset = (address << 30) >> 30; // extract offset from address
    return offset;
}

void readc( cacheb* array, unsigned int address){
    unsigned int tag = getTag(address);         //get vallues from address
    unsigned int set = getSet(address);
    unsigned int offset = getOffset(address);

    printf("Looking for set: %d - tag: %d\n", set,tag);

    if(array[set].valid){ //if found
        printf("found set: %d - tag: %d - offset: %d - valid: %d - value: %.2x \n",set,tag,offset,array[set].valid,array[set].value[0]);
        if (tag != array[set].tag){   //if diifferent tag
            printf("tags don't match - miss!\n");
        }else
            printf("hit!\n");
    }else
        printf("no valid set found - miss!\n");

}

void writec( cacheb* array, unsigned int address, unsigned value){
  unsigned int tag = getTag(address); // get values from address
  unsigned int set = getSet(address);
  unsigned int a = (value & 0xFF);//get four bits
  unsigned int b = ((value >> 8) & 0xFF);
  unsigned int c = ((value >> 16) & 0xFF);
  unsigned int d = ((value >> 24) & 0xFF);
  if (array[set].valid){//if found
      printf("evicting block - set: %d - tag: %d - valid: %d - value: ", set,array[set].tag, array[set].valid);
      for(int i=0; i< 1; i++)
          printf("%.2x %.2x %.2x %.2x \n",array[set].value[i],array[set].value[i+1],array[set].value[i+2],array[set].value[i+3]);
  }
  array[set].tag = tag;
  array[set].value[0] = a;//set bits found into set
  array[set].value[1] = b;
  array[set].value[2] = c;
  array[set].value[3] = d;
  array[set].valid = 1;
  printf("wrote set: %d - tag: %d - valid: %d - value: ",set,array[set].tag,array[set].valid);
  for(int i=0; i< 1; i++) //loop unrolling
      printf("%.2x %.2x %.2x %.2x \n",array[set].value[i],array[set].value[i+1],array[set].value[i+2],array[set].value[i+3]);
}

void printc( cacheb* array){
    for (int i = 0; i < 16; i++){
        if (array[i].valid){
            printf("set: %d - tag: %d - valid: %d - value: ", i,array[i].tag, array[i].valid);
            for(int j=0; j< 1; j++)  //loop unrolling
                printf("%.2x %.2x %.2x %.2x \n",array[i].value[j],array[i].value[j+1],array[i].value[j+2],array[i].value[j+3]);
        }
    }
}

int main()
{
    cacheb *array;
    array = (cacheb *)malloc(sizeof(cacheb)*16); //create cache block array
    for(int i = 0; i < 16; i++){//make sure cache is empty
        array[i].tag, array[i].valid = 0, 0;
        for(int j = 0; j < 1; j++) //loop unrolling
            array[i].value[j],array[i].value[j+1],array[i].value[j+2],array[i].value[j+3] = 0,0,0,0;
    }
    char input;
    unsigned int address;
    unsigned int value;
    while(input != 'q'){
        printf("Enter 'r' for read, 'w' for write, 'p' to print, 'q' to quit: ");
        scanf(" %c", &input);
        if (input == 'w'){
            printf("Enter 32-bit unsigned hex address: ");
            scanf(" %x", &address);
            printf("Enter 32-bit unsigned hex value: ");
            scanf(" %x",&value);
            writec(array, address, value);
        }else if (input == 'r'){
            printf("Enter 32-bit unsigned hex address: ");
            scanf(" %x", &address);
            readc(array, address);
        }else if (input == 'p')
            printc(array);
    }
}
