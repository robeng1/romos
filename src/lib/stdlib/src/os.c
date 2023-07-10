#include "os.h"
#include "string.h"

// Function to parse a command into a linked list of arguments
struct command_argument_t *sys_parse_command(const char *command, int max)
{
  // Initialize the root of the command arguments linked list
  struct command_argument_t *root_command = 0;
  // Create a buffer to copy the command into
  char scommand[1025];
  // Check if the command is too long for the buffer
  if (max >= (int)sizeof(scommand))
  {
    // If it is, return null
    return 0;
  }

  // Copy the command into the buffer
  strncpy(scommand, command, sizeof(scommand));
  // Tokenize the command string on spaces
  char *token = strtok(scommand, " ");
  // If there are no tokens, go to the end of the function
  if (!token)
  {
    goto out;
  }

  // Allocate memory for the root command argument
  root_command = sys_malloc(sizeof(struct command_argument_t));
  // If memory allocation failed, go to the end of the function
  if (!root_command)
  {
    goto out;
  }

  // Copy the first token into the root command argument
  strncpy(root_command->argument, token, sizeof(root_command->argument));
  // Initialize the next pointer to null
  root_command->next = 0;

  // Initialize the current command argument to the root
  struct command_argument_t *current = root_command;
  // Get the next token
  token = strtok(NULL, " ");
  // While there are more tokens
  while (token != 0)
  {
    // Allocate memory for the new command argument
    struct command_argument_t *new_command = sys_malloc(sizeof(struct command_argument_t));
    // If memory allocation failed, break the loop
    if (!new_command)
    {
      break;
    }

    // Copy the token into the new command argument
    strncpy(new_command->argument, token, sizeof(new_command->argument));
    // Initialize the next pointer to null
    new_command->next = 0x00;
    // Link the new command argument to the current one
    current->next = new_command;
    // Move to the new command argument
    current = new_command;
    // Get the next token
    token = strtok(NULL, " ");
  }
// Label for goto statements
out:
  // Return the root of the command arguments linked list
  return root_command;
}

// Function to get a key press, blocking until one is received
int sys_getkeyblock()
{
  // Initialize the key value
  int val = 0;
  // Loop until a key is pressed
  do
  {
    // Get the key press
    val = sys_getkey();
  } while (val == 0);
  // Return the key value
  return val;
}

// Function to read a line from the terminal
void sys_terminal_readline(char *out, int max, bool output_while_typing)
{
  // Initialize the index
  int i = 0;
  // Loop for the maximum number of characters
  for (i = 0; i < max - 1; i++)
  {
    // Get a key press
    char key = sys_getkeyblock();

    // If the key is a carriage return, break the loop
    if (key == 13)
    {
      break;
    }

    // If output_while_typing is true, output the key
    if (output_while_typing)
    {
      sys_putchar(key);
    }

    // If the key is a backspace and we are not at the start of the line
    if (key == 0x08 && i >= 1)
    {
      // Remove the previous character from the output
      out[i - 1] = 0x00;
      // Move the index back two places
      i -= 2;
      // Continue to the next iteration of the loop
      continue;
    }

    // Add the key to the output
    out[i] = key;
  }

  // Add a null terminator to the output
  out[i] = 0x00;
}

// Function to run a system command
int sys_system_run(const char *command)
{
  // Create a buffer for the command
  char buf[1024];
  // Copy the command into the buffer
  strncpy(buf, command, sizeof(buf));
  // Parse the command into a linked list of arguments
  struct command_argument_t *root_command_argument = sys_parse_command(buf, sizeof(buf));
  // If parsing failed, return -1
  if (!root_command_argument)
  {
    return -1;
  }

  // Run the system command and return the result
  return sys_system(root_command_argument);
}
