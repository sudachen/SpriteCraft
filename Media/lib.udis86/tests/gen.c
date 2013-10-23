#include <stdio.h>
#include <udis86.h>

#ifdef _WIN32
# include <io.h>
# include <fcntl.h>
#endif

/* help string */
int main(int argc, char **argv)
{
  ud_t ud_obj;

#ifdef _WIN32
  _setmode(_fileno(stdin), _O_BINARY);
#endif  

  ud_init(&ud_obj);
  ud_set_input_file(&ud_obj, stdin);

  if (strcmp(argv[1],"-16") == 0)
	  ud_set_mode(&ud_obj, 16);
  else if (strcmp(argv[1],"-32") == 0)
	  ud_set_mode(&ud_obj, 32);
  else if (strcmp(argv[1],"-64") == 0)
	  ud_set_mode(&ud_obj, 64);

  ud_set_syntax(&ud_obj, UD_SYN_INTEL);

  while (ud_disassemble(&ud_obj))
	printf("\t%s\n", ud_insn_asm(&ud_obj));
}
