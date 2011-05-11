//void show_man_page(char *cmd);  //Not yet implemented

// Jordan's method declarations
void concatenate(char *filename);
void list_directory_contents();
int  load_program(char *filename);
void remove_file(char *filename);
void save_file(char *filename);
void show_help();
// end Jordan's method declarations
void show_exit();

/*
 * Concatenate the contents of the file specified
 */
void concatenate(char *filename)
{
  printf("Concatenate the file %s\n", filename);
}

/* 
 * List the contents of the current directory
 */
void list_directory_contents()
{
	printf("Print out the contents of PWD.\n");
}

/* int load_file(char *file)  
 * Description: This function grabs the program from the disk and loads it into mem[0] starting from [0]
 * Input: filename with path if needed
 * Output: Returns -1 if it fails to open the file or the status of fclose() 
 *  -1: if the file wasn't loaded
 *   0: if everything was alright
 */
int load_program(char *filename)
{
  int fi = 0;
  int coni = 0;
  int size = 0;
  FILE *f;
  int status = 0;
  
  printf("Loading from HD: %s\n", filename);
  f = fopen(filename, "r");
  if (f == NULL)
  {
    return -1; // -1 means file opening fail
  }

  while(fscanf(f, "%d\n", &fi) != EOF)
  {
    mem[0][coni]=fi;
    printf("%d: %d\n", coni, mem[0][coni]);
    size++;
    coni++;
  }
  status = fclose(f);
  return status;
} // End load_file
/* 
 * Delete a file from the disk
 */
void remove_file(char *filename)
{
  printf("Delete file \"%s\" from the filesystem.\n", filename);
}

/* 
 * Save a file to the disk
 */
void save_file(char *filename)
{
  printf("Save a file to the virtual disk\n");
}

/*
 * Show the help screen for the shell
 */
void show_help()
{
  printf("Print out the help screen for the shell\n");
}

/*
 * Show the exit message and set machineOn to 0 (turn it off)
 */
void show_exit()
{
		printf("***********************************************************\n");
		printf("*       Thanks for using the Eagle Virtual Machine!       *\n");
		printf("*                         GOOD-BYE!                       *\n");
		printf("***********************************************************\n");
		machineOn = 0;
}
