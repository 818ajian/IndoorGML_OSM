//
// Created by byeonggon on 2018-11-12.
//

#include "IO.h"
#include <stdio.h>
#include <ctype.h>
namespace IO{
    char * lowercase(char *input){
        int i=0;
        char c;
        while(input[i]){
            c=input[i];
            input[i]=tolower(c);
            i++;
        }
        return input;
    }
}