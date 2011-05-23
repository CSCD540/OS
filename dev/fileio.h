/* int open(char * filename)
 * Description:
 *    Look for a file on the virtual disk. If it's found, get it's information and setup a 
 *    new file descriptor in the file descriptor table and return the file descriptor.
 * Input:
 *    char * filename : The name of the file to be opened
 * Output:
 *    FILE_NOT_FOUND : The requested file was not found on the virtual disk
 *    FD_LIMIT_EXCEED : The maximum amount of file descriptors has been reached
 *    i : The file was found and a file descriptor was successfully created. i is the
 *        file descriptor. In actuality, it is just an index into the file descriptor table.
 */
int open(char * filename)
{
  struct fileNode * file;
  file = get_file(filename);
  if(file == NULL)
    return FILE_NOT_FOUND;
  
  // Find the first available file descriptor
  int i = 0;
  for( ; i < MAXFILES; i++)
    if(files[i].curBlockNode == NULL)
      break;
  
  // Is a file descriptor available?
  if(i == MAXFILES)
    return FD_LIMIT_EXCEED;
  
  // Found an available file descriptor. Set it up!
  struct fileDescriptor fd;
  fd.fdNum = i;
  fd.filename = file->filename;
  fd.curInstruction = 0;
  fd.curBlockNode = file->blockList;
  files[i] = fd;
  
  return i;
}


/* int read(int fd)
 * Description:
 *    Read the next instruction from the file pointed to by the passed in file descriptor.
 * Input:
 *    int fd : The index of the file descriptor in the file descriptor table.
 * Output:
 *    OUT_OF_RANGE : Cannot read because the file descriptor is incremented past the end of the file.
 *    ENDF : End of the file has been reached.
 *    inst : The instruction located at the file's current locations specificed by the file descriptor.
 */
int read(int fd)
{
  if(files[fd].curBlockNode == NULL)
    return OUT_OF_RANGE;
  
  int inst;
  inst = (*files[fd].curBlockNode->block).instructions[files[fd].curInstruction];
  if(inst < 0)
    return ENDF;
  
  files[fd].curInstruction++;
  if(files[fd].curInstruction == BLOCKSIZE)
  {
    files[fd].curInstruction = 0;
    files[fd].curBlockNode = files[fd].curBlockNode->nextBlock;
  }
  
  return inst;
}


/* int write(struct fileNode **fileListNode, int data[], int count, int writeMode)
 * Description:
 *    Write data to a file on the VFS. If writeMode is OVERWRITE, writing begins at
 *    the beginning of the file, but
 * Input:
 *    struct fileNode **fileListNode : pointer to the file to be written to
 *    int data[] : Data to be written
 *    int count : How many elements to be written
 *    int writeMode : How to write to the file (Overwrite, append, etc).
 *    int offset : Used for overwriting. Specifies the location at which to beign overwriting
 * Output:
 *    SUCCESS : File was written without error.
 */
int write(struct fileNode **fileListNode, int data[], int count, int writeMode, int offset)
{
  int newFile;
  newFile = writeMode;
  struct blockNode *blockNode = (*fileListNode)->blockList;
  struct block *curBlock; // Declare a block pointer.
  if(DEBUG) printf("\nstart write : fileListNode->blockList->block %p\n", (*fileListNode)->blockList->block);
  if(DEBUG) printf("blockNode %p\n", blockNode);
  // Appending to end of file?
  int i = 0; // First empty index in instructions array
    
  if(writeMode == APPEND)
  {
    ; // TODO: Implement.
  }
  else // OVERWRITE or NEWFILE
  {
    if(offset != 0)
    {
      int k = 1;
      for(; k <= offset; k++)
        if(k % BLOCKSIZE == 0)
          blockNode = blockNode->nextBlock;
      i = offset % BLOCKSIZE;
    }
    curBlock = blockNode->block;
  }
      
  int j;
  for(j = 0; j < count; j++)
  {
    if((i == 0) && (newFile != NEWFILE))
    { 
      if(DEBUG) printf("curBlock %p\n", curBlock);
      if(DEBUG) printf("\nadding new blockNode...\n");
      if(blockNode->nextBlock == NULL)
        blockNode->nextBlock = get_free_block_node();
      blockNode = blockNode->nextBlock;
      curBlock = blockNode->block;
      if(DEBUG) printf("new blockNode %p\n", blockNode);
      if(DEBUG) printf("new block %p\n", curBlock);
    }
    if(DEBUG) printf("writing  %3d  to block  %2d\n", data[j], curBlock->blockNum);
    curBlock->instructions[i] = data[j];

    i++;
    // If i == BLOCKSIZE, we have written to the last availble instruction location and need to get a new blockNode next time.
    if(i == BLOCKSIZE)
      i = 0;
    // If this is a new file, set newFile to 0 so that when i == 0 we will get a new blockNode
    if(newFile == NEWFILE)
      newFile = 0;
  }
  if(DEBUG) printf("end write : fileListNode->blockList->block %p\n", (*fileListNode)->blockList->block);
  return SUCCESS;
} // end write()
