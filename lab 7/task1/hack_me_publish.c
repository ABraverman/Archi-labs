#define BUFF_SIZE 16

int main(int argc, char** argv, char** envp)
{
	/* 
	 * Password is stored here 
	 */
	
	char fail_str[] = "Wrong!\n";
	char buff[BUFF_SIZE];	
	
    printf("\nInput the correct password to pass (max %d chars):\n", BUFF_SIZE);
	gets(buff);
	printf("You entered:\n");
	printf(buff);
	printf("\n");

	if (/*Compare input to password */){
		/* Success */
	}
	else {
		printf(fail_str);
		printf("Better luck next time :)\n");
	}
}
