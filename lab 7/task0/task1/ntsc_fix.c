
int digit_counter(char* in);


int main(int argc, char **argv){

}

int digit_counter(char* in){
    int x = 0;
    while (*in != 0){
        if (*in >= '0' && *in <= '9'){
            x++;
        }
        in = in+1;
    }
    return x;
}





